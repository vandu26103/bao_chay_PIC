# MPLAB IDE generated this makefile for use with GNU make.
# Project: dacn.mcp
# Date: Fri Oct 04 14:36:21 2024

AS = MPASMWIN.exe
CC = mcc18.exe
LD = mplink.exe
AR = mplib.exe
RM = rm

dacn.cof : main.o
	$(LD) /l"C:\MCC18\lib" "C:\MCC18\lkr\18f4520.lkr" "main.o" /z__MPLAB_BUILD=1 /z__MPLAB_DEBUG=1 /o"dacn.cof" /M"dacn.map" /W

main.o : main.c C:/MCC18/h/delays.h C:/MCC18/h/adc.h C:/MCC18/h/stdio.h main.c C:/MCC18/h/P18f4520.h C:/MCC18/h/p18cxxx.h C:/MCC18/h/p18f4520.h C:/MCC18/h/stdarg.h C:/MCC18/h/stddef.h
	$(CC) -p=18F4520 "main.c" -fo="main.o" -D__DEBUG -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-

clean : 
	$(RM) "main.o" "dacn.cof" "dacn.hex"

