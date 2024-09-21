#ifndef BIOS_H
#define BIOS_H

#include <stdint.h>

void set_video_mode(uint8_t mode);
#pragma aux set_video_mode = \
"xor ah, ah" \
"int 0x10" \
modify [ax] \
parm [al];

#endif
