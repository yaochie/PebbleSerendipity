#pragma once

#include <pebble.h>

#define COORDS_LAT 10
#define COORDS_LONG 11
#define DIRECTIONS 12
#define DESTINATION 13

void init_message_handling();
void deinit_message_handling();
void send_message(int messageVal);