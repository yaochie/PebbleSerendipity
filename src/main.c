#include <pebble.h>
#include "navigation.h"
#include "message_handling.h"

static Window *s_main_window;
static SimpleMenuLayer *s_main_menu_layer;
static TextLayer *s_selected_text;

static SimpleMenuSection s_menu_list;
static SimpleMenuItem items[2];

static Window *s_popup_window;
static TextLayer *s_popup_text;

enum popupTypes {
    RANDOM,
    SELECTED,
};

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


static void init_popup_window(enum popupTypes type) {
    s_popup_window = window_create();
    
    if (type == SELECTED) {
        window_set_window_handlers(s_popup_window, (WindowHandlers) {
            .load = selected_load,
            .unload = selected_unload,
        });
    }
    
    window_stack_push(s_popup_window, true);
}


static void menu_select(int index, void *context) {
    //print title below!
    if ((uint32_t)index <= s_menu_list.num_items) {
        text_layer_set_text(s_selected_text, s_menu_list.items[index].title);
    }
    
    //open new window
    if (index == 0) {
        init_navigation_window();
    } else {
        init_popup_window(SELECTED);
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
    s_selected_text = text_layer_create(GRect(0, 90, 144, 40));
    text_layer_set_text(s_selected_text, "Selection goes here");
    
    init_menu();
    s_main_menu_layer = simple_menu_layer_create(GRect(0, 0, 144, 168), window, &s_menu_list, 2, NULL);
    if (s_main_menu_layer == NULL) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not create simple menu!");
    } else {
        layer_add_child(window_layer, simple_menu_layer_get_layer(s_main_menu_layer));
        layer_add_child(window_layer, text_layer_get_layer(s_selected_text));
    }
    
    init_message_handling();
}

static void main_window_unload(Window *window) {
    //destroy main menu
    simple_menu_layer_destroy(s_main_menu_layer);
    text_layer_destroy(s_selected_text);
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
