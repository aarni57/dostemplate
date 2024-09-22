#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <conio.h>

#include "util.h"
#include "bios.h"
#include "keys.h"

//

static uint8_t quit = 0;
static uint64_t frame_time_us = 0;
static uint32_t delta_time_us = 0;
static uint16_t fps = 0;

//

#define KEY_BUFFER_SIZE 64
static volatile uint8_t key_buffer[KEY_BUFFER_SIZE];
static volatile uint8_t key_buffer_position = 0;
static volatile uint8_t extended_key = 0;

void keyb_key(uint8_t code) {
    // Called by the keyboard interrupt handler
    if (code != 0xe0) {
        uint8_t key = translate_key((code & 0x7f) | (extended_key ? 0x100 : 0));
        if (key != INVALID_KEY) {
            int up = (code & 0x80) == 0x80;
            key_buffer[key_buffer_position] = key | (up ? KEY_UP_FLAG : 0);
            key_buffer_position = (key_buffer_position + 1) & (KEY_BUFFER_SIZE - 1);
        }

        extended_key = 0;
    } else {
        extended_key = 1;
    }
}

static void update_input() {
    // Call once per frame
    static uint8_t i = 0;
    uint8_t current_position = key_buffer_position;

    while (i != current_position) {
        switch (key_buffer[i]) {
            case KEY_ESC:
                quit = 1;
                break;

            default:
                break;
        }

        i = (i + 1) & (KEY_BUFFER_SIZE - 1);
    }
}

//
// Timer divisor can be changed in timer.asm
// 1193180Hz / 4096 (0x1000) = 291.304Hz
#define TIMER_TICK_USEC 3433 // 1000000 / 291.304Hz
#define TIMER_TICK_USEC_COMP -2 // Compensation every 16th timer_tick

static volatile uint64_t timer_us = 0; // Microseconds since program start

static inline uint64_t read_timer() {
    uint64_t r;
    _disable();
    r = timer_us;
    _enable();
    return r;
}

void timer_tick(uint8_t counter) {
    // Called by the timer interrupt handler
    timer_us += TIMER_TICK_USEC;
    if (counter == 0) 
        timer_us += TIMER_TICK_USEC_COMP;
}

#define SEC_IN_US 1000000ULL
#define FPS_UPDATE_INTERVAL SEC_IN_US

static void update_time() {
    // Call in the beginning of every frame
    static uint64_t previous_frame_time_us = 0;
    static uint32_t fps_accum = 0;
    static uint16_t fps_frame_counter = 0;

    frame_time_us = read_timer();
    delta_time_us = frame_time_us - previous_frame_time_us;
    previous_frame_time_us = frame_time_us;

    fps_accum += delta_time_us;
    fps_frame_counter++;
    if (fps_accum >= FPS_UPDATE_INTERVAL) {
        fps = SEC_IN_US * fps_frame_counter / fps_accum;
        fps_frame_counter = 0;
        fps_accum -= FPS_UPDATE_INTERVAL;
    }
}

//

static void print_time_and_fps() {
    char buffer[32];
    uint16_t secs, millis;

    secs = frame_time_us / 1000000;
    millis = (frame_time_us - secs * 1000000) / 1000;

    snprintf(buffer, 32, "FPS: %u", fps);
    putz_pos(buffer, 1, 23);

    snprintf(buffer, 32, "Time: %u.%03u", secs, millis);
    putz_pos(buffer, 10, 23);
}

//

static uint8_t __far *vmem = (uint8_t __far*)0xa0000000;

#define GRAPHICS_MODE 0x13

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define SCREEN_NUM_PIXELS (SCREEN_WIDTH * SCREEN_HEIGHT)

#define SCREEN_CLEAR_COLOR 3

//

#define VGA_INPUT_STATUS 0x3da
#define VGA_VRETRACE 0x08

static inline void vga_vsync() {
    while (inp(VGA_INPUT_STATUS) & VGA_VRETRACE);
    while (!(inp(VGA_INPUT_STATUS) & VGA_VRETRACE));
}

//
// keyb.asm
void keyb_init();
void keyb_cleanup();
//
// timer.asm
void timer_init();
void timer_cleanup();
//

void main() {
    timer_init();
    keyb_init();

    set_video_mode(GRAPHICS_MODE);
    vga_vsync();
    // TODO: Set VGA palette here
    _fmemset(vmem, SCREEN_CLEAR_COLOR, SCREEN_NUM_PIXELS);

    while (!quit) {
        update_time();
        update_input();

        // TODO: Do game logic and simulation here

        vga_vsync();

        // TODO: Draw here

        print_time_and_fps();
    }

    set_video_mode(0x3);

    keyb_cleanup();
    timer_cleanup();

    kb_clear_buffer();
}
