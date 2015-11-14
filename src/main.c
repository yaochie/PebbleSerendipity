#include <pebble.h>

#define COORDS_LAT 0
#define COORDS_LONG 1

static Window *s_main_window;
static SimpleMenuLayer *s_main_menu_layer;
static TextLayer *s_selected_text;

static SimpleMenuSection s_menu_list;
static SimpleMenuItem items[2];

static Window *s_popup_window;
static TextLayer *s_popup_text;
static TextLayer *s_latitude;
static TextLayer *s_longitude;
static TextLayer *s_random_placeholder;

static char s_lat_buffer[32];
static char s_lon_buffer[32];

enum popupTypes {
    RANDOM,
    SELECTED,
};

static void get_coords() {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    
    if (!iter) {
        return;
    }
    
    int value = 1;
    dict_write_int(iter, 1, &value, sizeof(int), true);
    dict_write_end(iter);
    
    app_message_outbox_send();
}

static void receive_pano_coords(DictionaryIterator *iterator, void *context) {
    Tuple *lat = dict_find(iterator, COORDS_LAT);
    Tuple *lon = dict_find(iterator, COORDS_LONG);
    bool loadedCoords = false;
    if (lat) {
        snprintf(s_lat_buffer, sizeof(s_lat_buffer), "Lat: %s", lat->value->cstring);
        if (s_latitude) text_layer_set_text(s_latitude, s_lat_buffer);
        loadedCoords = true;
    }
    if (lon) {
        snprintf(s_lon_buffer, sizeof(s_lon_buffer), "Long: %s", lon->value->cstring);
        if (s_longitude) text_layer_set_text(s_longitude, s_lon_buffer);
        loadedCoords = true;
    }
    if (loadedCoords && s_random_placeholder) text_layer_set_text(s_random_placeholder, "");
}

static void random_load(Window *window) {
    Layer* window_layer = window_get_root_layer(window);
    
    s_latitude = text_layer_create(GRect(0, 0, 144, 50));
    s_longitude = text_layer_create(GRect(0, 50, 144, 50));
    text_layer_set_background_color(s_latitude, GColorClear);
    text_layer_set_background_color(s_longitude, GColorClear);
    text_layer_set_font(s_latitude, fonts_get_system_font(FONT_KEY_GOTHIC_18));
    text_layer_set_font(s_longitude, fonts_get_system_font(FONT_KEY_GOTHIC_18));
    
    s_random_placeholder = text_layer_create(GRect(0, 40, 144, 80));
    text_layer_set_background_color(s_random_placeholder, GColorClear);
    text_layer_set_font(s_random_placeholder, fonts_get_system_font(FONT_KEY_GOTHIC_28));
    text_layer_set_text(s_random_placeholder, "Getting random location...");
    
    layer_add_child(window_layer, text_layer_get_layer(s_latitude));
    layer_add_child(window_layer, text_layer_get_layer(s_longitude));
    layer_add_child(window_layer, text_layer_get_layer(s_random_placeholder));
    
    app_message_register_inbox_received(&receive_pano_coords);
    get_coords();
}

static void random_unload(Window *window) {
    text_layer_destroy(s_latitude);
    text_layer_destroy(s_longitude);
    text_layer_destroy(s_random_placeholder);
    app_message_deregister_callbacks();
}

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
    if (type == RANDOM) {
        window_set_window_handlers(s_popup_window, (WindowHandlers) {
            .load = random_load,
            .unload = random_unload,
        });
    } else if (type == SELECTED) {
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
        init_popup_window(RANDOM);
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
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}