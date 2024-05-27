#include <stdint.h>
#include <defs.h>
#include <string.h>
#include <keyboard.h>
#include <naiveRTC.h>
#include <naiveGraphicsConsole.h>
#include <time.h>
#include <speaker.h>
#include <interrupts.h>
#include <memory_manager.h>

extern uint64_t info[17];
extern uint8_t screenshot;

typedef int64_t (*syscallT) (uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

// Función para manejar la escritura en el sistema
static void sys_write_handler(uint64_t fd, uint64_t buffer, uint64_t bytes){
    if (fd == STDOUT) {
        for (uint64_t i = 0; i < bytes; i++){
            ngc_printChar(((char*)buffer)[i]);
        }
    }
}

// Función para manejar la lectura desde el sistema
static int64_t sys_read_handler(uint64_t fd, char * buffer, uint64_t bytes){
    if (fd != STDIN && fd != KBDIN) return -1;
    int64_t i = 0;
    char c;
    while(i < bytes && (c = getFirstChar()) != 0xFF) {
        buffer[i] = c;
        i++;
    }
    return i;
}

// Función para obtener la hora actual del sistema
static uint64_t sys_time_handler(){
    return (_NRTCGetHours()) | ((uint64_t)_NRTCGetMins() << 8) | ((uint64_t)_NRTCGetSeconds() << 16);
}

// Función para obtener información de los registros
static uint8_t sys_inforeg_handler(uint64_t regVec[17]){
    if (screenshot){
        for (int i = 0; i < 17; i++)
        {
            regVec[i] = info[i];
        }
        
    }
    return screenshot;
}

// Función para cambiar el tamaño de la fuente
static void sys_font_handler(uint64_t level){
    changeFontSize(level);
}

// Función para imprimir texto con un color específico
static void sys_printColor_handler(const char *buffer, uint64_t color){
    ngc_printColor(buffer, color);
}

// Función para limpiar la pantalla con un color específico
static void sys_clear_screen_handler(uint64_t color) {
    ngc_paint_screen(color);
}

// 2 por altura y ancho
// Función para obtener información sobre las dimensiones de la pantalla
static void sys_screenData_handler(uint16_t * data){
    data[0] = ngc_getWidth();
    data[1] = ngc_getHeight();
    return;
}

// Función para pintar un rectángulo en la pantalla
static void sys_paint_rect_handler(uint64_t fromX, uint64_t fromY, uint16_t width, uint16_t height, uint64_t color) {
    ngc_print_pixels(fromX, fromY, width, height, color);
}

// Función para obtener el número de "ticks" del sistema
static uint64_t sys_ticks_handler(){
    return ticks_elapsed();
}

// Función para activar un "beeper" con una frecuencia y duración específicas
static void sys_beeper_handler(uint64_t frequency, uint64_t interval) {
    beep(frequency);
    wait(interval);
    stopBeep();
}

static void sys_memstate_handler(){
    printMem();
}

static void * sys_alloc_handler(unsigned long size){
    return alloc(size);
}

static void sys_free_handler(void *memptr){
    free(memptr);
}

// Vector de punteros a funciones de llamada al sistema
static syscallT syscalls[]  = {
    (syscallT) sys_read_handler, 
    (syscallT) sys_write_handler, 
    (syscallT) sys_time_handler, 
    (syscallT) sys_inforeg_handler, 
    (syscallT) sys_font_handler, 
    (syscallT) sys_printColor_handler, 
    (syscallT) sys_clear_screen_handler, 
    (syscallT) sys_screenData_handler, 
    (syscallT) sys_paint_rect_handler, 
    (syscallT) sys_ticks_handler, 
    (syscallT) sys_beeper_handler,
    (syscallT) sys_memstate_handler,
    (syscallT) sys_alloc_handler,
    (syscallT) sys_free_handler
};

// Función para despachar llamadas al sistema
int64_t syscallDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t rax){
    return syscalls[rax](rdi, rsi, rdx, rcx, r8);   // rax tiene el id, rax = r9 y rcx = r10
}

