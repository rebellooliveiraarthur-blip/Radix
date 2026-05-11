#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

#define WIDTH 320
#define HEIGHT 240
#define SCALE 2

typedef uint16_t Color;

int display_init();
int display_is_running();
void display_update(Color* buffer); 
void display_close();

#endif
