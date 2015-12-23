#pragma once

#include <pebble.h>

#define COORDS_LAT 10
#define COORDS_LONG 11
#define DIRECTIONS 12
#define DESTINATION 13

void init_message_handling();
void deinit_message_handling();
void start_message();
void add_to_message(int messageKey, int messageVal);
bool message_open();
void send_message();
void send_single_message(int messageKey, int messageVal);