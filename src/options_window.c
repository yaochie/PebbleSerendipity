#include <pebble.h>
#include "options_window.h"
#include "selection_layer.h"

static Window *s_options_window;
static Layer *s_selection;
static TextLayer *s_label;

static void options_load(Window *window) {
    Layer* window_layer = window_get_root_layer(window);
    Layer *s_selection = selection_layer_create(layer_get_bounds(window_layer));
    
    TextLayer *s_label = text_layer_create(GRect(0, 38, 144, 35));
    text_layer_set_text(s_label, "Max");
    text_layer_set_font(s_label, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_text_alignment(s_label, GTextAlignmentCenter);
    
    selection_layer_set_click_config_from_window(s_selection, window);
    
    layer_add_child(window_layer, s_selection);
    layer_add_child(window_layer, text_layer_get_layer(s_label));
}

static void options_unload(Window *window) {
    selection_layer_destroy(s_selection);
    text_layer_destroy(s_label);
}

void init_options_window() {
    s_options_window = window_create();
    
    window_set_window_handlers(s_options_window, (WindowHandlers) {
        .load = options_load,
        .unload = options_unload,
    });
    
    window_stack_push(s_options_window, true);
}
