#include <pebble.h>
#include "options_window.h"
#include "selection_layer.h"

static Window *s_options_window;
static Layer *s_selection;

static char *labels[] = {"Min", "Max"};

static void selection_increment(int index, void *context) {
    OptionsData *data = (OptionsData*)context;
    if (data) {
    }
}

static void options_load(Window *window) {
    Layer* window_layer = window_get_root_layer(window);
    Layer *s_selection = selection_layer_create(layer_get_bounds(window_layer));
    
    selection_layer_set_click_config_from_window(s_selection, window);
    
    layer_add_child(window_layer, s_selection);
}

static void options_unload(Window *window) {
    selection_layer_destroy(s_selection);
}

void init_options_window() {
    s_options_window = window_create();
    
    window_set_window_handlers(s_options_window, (WindowHandlers) {
        .load = options_load,
        .unload = options_unload,
    });
    
    window_stack_push(s_options_window, true);
}
