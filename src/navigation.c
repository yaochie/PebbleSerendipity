#include <pebble.h>
#include "navigation.h"
#include "message_handling.h"

static Window *s_nav_window;
static TextLayer *s_latitude;
static TextLayer *s_longitude;
static TextLayer *s_destination;
static TextLayer *s_current_instruction;
static TextLayer *s_loading_placeholder;

static char s_lat_buffer[32];
static char s_lon_buffer[32];
static char s_destination_buffer[64];
static char s_current_instruction_buffer[128];

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
    //get previous instruction
    send_message(PREV_INSTRUCTION);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
    //get next instruction
    send_message(NEXT_INSTRUCTION);
}

static void nav_click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void nav_load(Window *window) {
    Layer* window_layer = window_get_root_layer(window);
    
    s_latitude = text_layer_create(GRect(0, 120, 144, 20));
    s_longitude = text_layer_create(GRect(0, 135, 144, 28));
    text_layer_set_background_color(s_latitude, GColorClear);
    text_layer_set_background_color(s_longitude, GColorClear);
    text_layer_set_font(s_latitude, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_font(s_longitude, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    
    s_loading_placeholder = text_layer_create(GRect(0, 40, 144, 80));
    text_layer_set_background_color(s_loading_placeholder, GColorClear);
    text_layer_set_font(s_loading_placeholder, fonts_get_system_font(FONT_KEY_GOTHIC_28));
    text_layer_set_text(s_loading_placeholder, "Getting random location...");
    
    s_destination = text_layer_create(GRect(0, 0, 144, 40));
    s_current_instruction = text_layer_create(GRect(0, 40, 144, 70));
    text_layer_set_background_color(s_destination, GColorClear);
    text_layer_set_background_color(s_current_instruction, GColorClear);
    text_layer_set_font(s_destination, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_font(s_current_instruction, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    
    layer_add_child(window_layer, text_layer_get_layer(s_latitude));
    layer_add_child(window_layer, text_layer_get_layer(s_longitude));
    layer_add_child(window_layer, text_layer_get_layer(s_loading_placeholder));
    layer_add_child(window_layer, text_layer_get_layer(s_destination));
    layer_add_child(window_layer, text_layer_get_layer(s_current_instruction));
    
    send_message(INIT_DIRECTIONS);
}

static void nav_unload(Window *window) {
    text_layer_destroy(s_latitude);
    text_layer_destroy(s_longitude);
    text_layer_destroy(s_destination);
    text_layer_destroy(s_current_instruction);
    text_layer_destroy(s_loading_placeholder);
}

void init_navigation_window() {
    s_nav_window = window_create();
    window_set_window_handlers(s_nav_window, (WindowHandlers) {
        .load = nav_load,
        .unload = nav_unload,
    });
    window_set_click_config_provider(s_nav_window, nav_click_config_provider);
    
    window_stack_push(s_nav_window, true);
}

void deinit_navigation_window() {
    //?
}

void update_latitude(const char *new_lat) {
    snprintf(s_lat_buffer, sizeof(s_lat_buffer), "Lat: %s", new_lat);
    if (s_latitude) text_layer_set_text(s_latitude, s_lat_buffer);
}

void update_longitude(const char *new_long) {
    snprintf(s_lon_buffer, sizeof(s_lon_buffer), "Long: %s", new_long);
    if (s_longitude) text_layer_set_text(s_longitude, s_lon_buffer);
}

void update_destination(const char *new_des) {
    snprintf(s_destination_buffer, sizeof(s_destination_buffer), "%s", new_des);
    if (s_destination) text_layer_set_text(s_destination, s_destination_buffer);
}

void update_directions(const char *new_dir) {
    snprintf(s_current_instruction_buffer, sizeof(s_current_instruction_buffer), "%s", new_dir);
    if (s_current_instruction) text_layer_set_text(s_current_instruction, s_current_instruction_buffer);
}

void clear_loading_placeholder() {
    text_layer_set_text(s_loading_placeholder, "");
}

bool is_waiting_for_nav_loading() {
    return strncmp(text_layer_get_text(s_loading_placeholder), "", 1) != 0;
}
