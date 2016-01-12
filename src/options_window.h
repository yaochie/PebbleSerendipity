#pragma once

/*
Options window
Allows user to set minimum and maximum (rough) walking time to destination
Uses this to set the range of the panoramas found

Set minimum and maximum range
Two boxes with arrows, increment/decrement in 5 mins
next box with center button
prev box with prev button
*/

/*
#define NUM_CELLS 1
#define MAX_NUM_LENGTH 4

typedef struct OptionsData {
    int nums[NUM_CELLS];
    char text[NUM_CELLS][MAX_NUM_LENGTH];
} OptionsData;
*/

void init_options_window();
