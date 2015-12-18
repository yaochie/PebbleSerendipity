#pragma once

#define INIT_DIRECTIONS 0
#define NEXT_INSTRUCTION 1
#define PREV_INSTRUCTION 2

void init_navigation_window();
void deinit_navigation_window();

void update_latitude(const char *new_lat);
void update_longitude(const char *new_long);
void update_destination(const char *new_des);
void update_directions(const char *new_dir);
void clear_loading_placeholder();

bool is_waiting_for_nav_loading();