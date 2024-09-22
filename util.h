#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

static inline void kb_clear_buffer();
#pragma aux kb_clear_buffer =   \
"mov ax, 0x0c00" \
"int 0x21" \
modify [ax];

static inline void putz(const char *str);
#pragma aux putz = \
"l:" \
"mov dl, [bx]" \
"test dl, dl" \
"jz end" \
"push bx" \
"mov ah, 02h" \
"int 21h" \
"pop bx" \
"add bx, 1" \
"jmp l" \
"end:" \
modify [ax dx] \
parm [bx];

static inline void set_text_cursor(uint8_t row, uint8_t col);
#pragma aux set_text_cursor = \
"mov ah, 2" \
"mov bh, 0" \
"xor al, al" \
"int 10h" \
modify [ax bh] \
parm [dh] [dl];

static inline void putz_pos(const char *str, uint8_t x, uint8_t y) {
    set_text_cursor(y, x);
    putz(str);
}

#endif
