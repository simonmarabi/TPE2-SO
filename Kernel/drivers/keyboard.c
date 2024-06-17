// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <keyboard.h>
#include <interrupts.h>
#include <naiveGraphicsConsole.h>
#include <scheduler.h>

#define KEY_COUNT 105
#define BUFF_LEN 4096U

static const Key KeyTable[] = {
    0, KEY_ESC, KEY_1, KEY_2,
    KEY_3, KEY_4, KEY_5, KEY_6,
    KEY_7, KEY_8, KEY_9, KEY_0,
    KEY_APOSTROPHE, KEY_OPEN_QUESTION, KEY_BACKSPACE, KEY_TAB,
    KEY_Q, KEY_W, KEY_E, KEY_R,
    KEY_T, KEY_Y, KEY_U, KEY_I,
    KEY_O, KEY_P, KEY_ACUTE, KEY_PLUS,
    KEY_ENTER, KEY_LCTRL, KEY_A, KEY_S,
    KEY_D, KEY_F, KEY_G, KEY_H,
    KEY_J, KEY_K, KEY_L, KEY_TILDED_N,
    KEY_LBRACE, KEY_VERTICAL_LINE, KEY_LSHIFT, KEY_RBRACE,
    KEY_Z, KEY_X, KEY_C, KEY_V,
    KEY_B, KEY_N, KEY_M, KEY_COMMA,
    KEY_PERIOD, KEY_DASH, KEY_RSHIFT, KEY_NUM_ASTERISC,
    KEY_ALT, KEY_SPACE, KEY_LOCK_CAPS, KEY_F1,
    KEY_F2, KEY_F3, KEY_F4, KEY_F5,
    KEY_F6, KEY_F7, KEY_F8, KEY_F9,
    KEY_F10, KEY_NUM_LOCK, KEY_SCROLL_LOCK, KEY_NUM_7,
    KEY_NUM_8, KEY_NUM_9, KEY_NUM_DASH, KEY_NUM_4,
    KEY_NUM_5, KEY_NUM_6, KEY_NUM_PLUS, KEY_NUM_1,
    KEY_NUM_2, KEY_NUM_3, KEY_NUM_0, KEY_NUM_PERIOD,
    0, 0, KEY_LESS_THAN, KEY_F11, KEY_F12};


static const uint8_t AsciiTable[] = {
    0,-1,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    '|','1','2','3',
    '4','5','6','7',
    '8','9','0','\'',
    168,8,'\t','q',
    'w','e','r','t',
    'y','u','i','o',
    'p','\'','+','\n',
    0,'a','s','d',
    'f','g','h','j',
    'k','l',164,'{',
    '}',0,'<','z',
    'x','c','v','b',
    'n','m',',','.',
    '-',0,0,0,
    0,' ',0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    '/','*','-','7',
    '8','9','4','5',
    '6','1','2','3',
    '0','.','+','\n'};

static const uint8_t ShiftAsciiTable[] = {
    0,-1,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    248,'!','\"','#',
    '$','%','&','/',
    '(',')','=','?',
    173,8,'\t','q',
    'w','e','r','t',
    'y','u','i','o',
    'p',249,'*','\n',
    0,'a','s','d',
    'f','g','h','j',
    'k','l',165,'[',
    ']',0,'>','z',
    'x','c','v','b',
    'n','m',';',':',
    '_',0,0,0,
    0,' ',0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    '/','*','-','7',
    '8','9','4','5',
    '6','1','2','3',
    '0','.','+','\n'};


static uint8_t keyState[KEY_COUNT] = {0};
static uint64_t keysToRead = 0;
static uint8_t isSpecialKey = 0; 
static PID blockedpid;

static Key buffer[BUFF_LEN]; 
static uint32_t read_idx = 0;
static uint32_t write_idx = 0;

static uint8_t toUpper(uint8_t c) {
    if(c >= 'a' && c <= 'z') return c - 'a' + 'A';
    if(c == 164) return 165;
    return c;
}

uint8_t isKeyPressed(Key key) {
    return keyState[key];
}

Key readKey() {
    waitKey();

    Key k = buffer[read_idx++];
    if (read_idx == BUFF_LEN)
        read_idx = 0;
    return k;
}

uint8_t readAscii() {
    uint8_t c;
    Key k;
    while ((c = AsciiTable[(k = readKey())]) == 0);

    uint8_t capslock = isKeyPressed(KEY_LOCK_CAPS);
    uint8_t shift = isKeyPressed(KEY_LSHIFT) || isKeyPressed(KEY_RSHIFT);
    uint8_t caps = (capslock && !shift) || (shift && !capslock);
    uint8_t ctrl = isKeyPressed(KEY_LCTRL) || isKeyPressed(KEY_RCTRL);

    if (shift) c = ShiftAsciiTable[k];
    else c = AsciiTable[k];

    if (caps) c = toUpper(c);

    // Manejo de combinaciones con Ctrl
    if (ctrl) {
        if (k == KEY_C) return 3; // ASCII para Ctrl+C
        if (k == KEY_D) return 4; // ASCII para Ctrl+D
    }

    return c;
}

void waitKey() {
    blockedpid = getPID();
    while (!keysToRead) {
        write_idx = read_idx;
        blockProcess(blockedpid);
    }
    blockedpid = 0;
    keysToRead--;
}

Key handleSpecialKey(uint8_t scancode) {
    Key k;

    switch (scancode) {
        case 0x4D:
            k = KEY_ARROW_RIGHT;
            break;
        case 0x4B:
            k = KEY_ARROW_LEFT;
            break;
        case 0x50:
            k = KEY_ARROW_DOWN;
            break;
        case 0x48:
            k = KEY_ARROW_UP;
            break;
        case 0x1D:
            k = KEY_LCTRL;
            break;
        case 0xE0:
            k = KEY_RCTRL;
            break;
        case 0x38:
            k = KEY_ALTGR;
            break;
        case 0x53:
            k = KEY_DELETE;
            break;
        case 0x1C:
            k = KEY_NUM_ENTER;
            break;
        case 0x35:
            k = KEY_NUM_FORWARD_SLASH;
            break;
        default:
            return 0;
    }

    return k;
}

void handleKeyboardInterrupt() {
    uint8_t scancode = readKeyRaw();

    if (!isSpecialKey && scancode == 0xE0) {
        isSpecialKey = 1;
        return;
    }

    uint8_t pressed = 1;
    Key k;

    if (scancode >= 128) {
        pressed = 0;
        scancode -= 128;
    }

    if (scancode >= 89) return;

    if (isSpecialKey) {
        isSpecialKey = 0;
        k = handleSpecialKey(scancode);
    } else {
        k = KeyTable[scancode];
    }

    if (k == 0) return;

    if (k == KEY_LOCK_CAPS) {
        if (pressed)
            keyState[k] = !keyState[k];
    } else {
        keyState[k] = pressed;
    }

    if (pressed) {
        buffer[write_idx++] = k;
        keysToRead++;
    }

    if (write_idx == BUFF_LEN)
        write_idx = 0;

    if (keysToRead > BUFF_LEN)
        keysToRead = BUFF_LEN;

    if (blockedpid) unblockProcess(blockedpid);
}
