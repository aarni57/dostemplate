@echo off

wcc -0 -ms -onatxhl+ -ei -oe=40 main.c
IF ERRORLEVEL 1 GOTO exit

REM wdis -s -a -l main.obj
REM IF ERRORLEVEL 1 GOTO exit

nasm -f obj -o timer.obj timer.asm -l timer.lst
nasm -f obj -o keyb.obj keyb.asm -l keyb.lst

wlink name main.com system com option eliminate file { main.obj timer.obj keyb.obj }

:exit
