#ifndef BIOS_H
#define BIOS_H

#include <stdint.h>

uint8_t get_video_mode();
#pragma aux get_video_mode = \
"mov ax, 0x0f00" \
"int 0x10" \
modify [ax] \
value [al];

void set_video_mode(uint8_t mode);
#pragma aux set_video_mode = \
"xor ah, ah" \
"int 0x10" \
modify [ax] \
parm [al];

#endif
