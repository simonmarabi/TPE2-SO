#include <stdint.h>
#include <idtLoader.h>
#include <defs.h>
#include <interrupts.h>

#include <naiveGraphicsConsole.h>

#pragma pack(push)    /* Push de la alineación actual */
#pragma pack (1)      /* Alinear las siguientes estructuras a 1 byte */

/* Descriptor de interrupción */
typedef struct {
  uint16_t offset_l, selector;
  uint8_t cero, access;
  uint16_t offset_m;
  uint32_t offset_h, other_cero;
} DESCR_INT;

#pragma pack(pop)     /* Restablece la alineación actual */

DESCR_INT * idt = (DESCR_INT *) 0;  // IDT de 255 entradas

static void setup_IDT_entry (int index, uint64_t offset);

void load_idt() {
  _cli(); // Deshabilita interrupciones mientras se configuran las entradas de la IDT

  // Excepciones
  setup_IDT_entry(0x00, (uint64_t)&_divisionByZeroInterruption);
  setup_IDT_entry(0x06, (uint64_t)&_invalidOpCodeInterruption);

  // Interrupciones de Hardware
  setup_IDT_entry(0x20, (uint64_t)&_irq00Handler);
  setup_IDT_entry(0x21, (uint64_t)&_irq01Handler);

  // Interrupciones de Syscall
  setup_IDT_entry(0x80, (uint64_t)&_irq80Handler);

  // Configuración de máscaras para interrupciones
  picMasterMask(0xFC); // 0b11111100
  picSlaveMask(0xFF);  // 0b11111111

  _sti(); // Habilita interrupciones
}

static void setup_IDT_entry (int index, uint64_t offset) {
  idt[index].offset_l = offset & 0xFFFF;
  idt[index].selector = 0x08; // Selector de segmento de código en GDT
  idt[index].cero = 0;
  idt[index].access = ACS_INT; // Atributos de acceso
  idt[index].offset_m = (offset >> 16) & 0xFFFF;
  idt[index].offset_h = (offset >> 32) & 0xFFFFFFFF;
  idt[index].other_cero = 0;
}
