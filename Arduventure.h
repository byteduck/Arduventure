#ifndef VGA_H
#define VGA_H

#include <avr/sleep.h>

//Macro for converting define to string
#define _STR(X) #X
#define STR(X) _STR(X)

#define VSYNC_PIN 11
#define HSYNC_PIN 9
#define RED_PIN1 24
#define RED_PIN2 25
#define GREEN_PIN1 26
#define GREEN_PIN2 27
#define BLUE_PIN1 28
#define BLUE_PIN2 29
#define DISPLAY_PIN 4

#define VERTICAL_SKIP 90 //Vertical lines to skip during blanking interval

#define RES_WIDTH 640
#define RES_HEIGHT 480

//How many pixels wide the display buffer is
#define PIXELS_WIDTH 120
//How many pixels high the display buffer is
#define PIXELS_HEIGHT 60

#define SUBLINES_PER_LINE 6

extern byte displayBuffer[PIXELS_WIDTH * PIXELS_HEIGHT];

#endif
