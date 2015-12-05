#include <pebble.h>
#include "options_window.h"

static Window *s_options_window;
static TextLayer *s_popup_text;

static void selected_load(Window *window) {
    Layer* window_layer = window_get_root_layer(window);
    
    s_popup_text = text_layer_create(layer_get_bounds(window_layer));
    text_layer_set_background_color(s_popup_text, GColorClear);
    text_layer_set_font(s_popup_text, fonts_get_system_font(FONT_KEY_GOTHIC_28));
    
    text_layer_set_text(s_popup_text, "User choice has been selected!");
    
    layer_add_child(window_layer, text_layer_get_layer(s_popup_text));
}

static void selected_unload(Window *window) {
    text_layer_destroy(s_popup_text);
}

void init_options_window() {
    s_options_window = window_create();
    
    window_set_window_handlers(s_options_window, (WindowHandlers) {
        .load = selected_load,
        .unload = selected_unload,
    });
    
    window_stack_push(s_options_window, true);
}
