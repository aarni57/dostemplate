# Simple DOS COM game template with timer and keyboard interrupts

![screenshot](https://github.com/aarni57/dostemplate/blob/master/screenshot.png?raw=true)

Run `makecom.bat` in DOS to build `main.com`. Target CPU is set to 8086. The batch file can be easily changed to build a real mode EXE.

The main loop in `main.c` looks like this:

    while (!quit) {
        update_time();
        update_input();

        // TODO: Do game logic and simulation here

        vga_vsync();

        // TODO: Draw here

        print_time_and_fps();
    }

### Tools needed:
- Open Watcom 1.9: https://github.com/open-watcom/open-watcom-1.9/releases
- NASM: https://www.nasm.us/pub/nasm/releasebuilds/

### Useful:
- DOSBox-X: https://github.com/joncampbell123/dosbox-x/releases
