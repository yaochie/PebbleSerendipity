#include <pebble.h>
#include "navigation.h"
#include "options_window.h"
#include "message_handling.h"

static Window *s_main_window;
static SimpleMenuLayer *s_main_menu_layer;

static SimpleMenuSection s_menu_list;
static SimpleMenuItem items[2];

static void menu_select(int index, void *context) {
    //open new window
    if (index == 0) {
        init_navigation_window();
    } else {
        init_options_window();
    }
}

static void init_menu() {
    s_menu_list.title = NULL;
    s_menu_list.num_items = 2;
    items[0].title = "Random Place";
    items[0].callback = &menu_select;
    items[1].title = "Choose a Place";
    items[1].callback = &menu_select;
    s_menu_list.items = items;
}

static void main_window_load(Window *window) {
    //load main menu
    Layer* window_layer = window_get_root_layer(window);
    
    init_menu();
    s_main_menu_layer = simple_menu_layer_create(GRect(0, 0, 144, 168), window, &s_menu_list, 2, NULL);
    if (s_main_menu_layer == NULL) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not create simple menu!");
    } else {
        layer_add_child(window_layer, simple_menu_layer_get_layer(s_main_menu_layer));
    }
    
    init_message_handling();
}

static void main_window_unload(Window *window) {
    //destroy main menu
    simple_menu_layer_destroy(s_main_menu_layer);
}

static void init() {
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload,
    });
    
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
    window_stack_push(s_main_window, true);
}

static void deinit() {
    window_destroy(s_main_window);
    deinit_message_handling();
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
