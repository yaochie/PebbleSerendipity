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

static AppSync s_sync;
static uint8_t s_sync_buffer[64];

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

static void random_load(Window *window) {
    Layer* window_layer = window_get_root_layer(window);
    
    s_latitude = text_layer_create(GRect(0, 0, 144, 50));
    s_longitude = text_layer_create(GRect(0, 50, 144, 50));
    text_layer_set_background_color(s_latitude, GColorClear);
    text_layer_set_background_color(s_longitude, GColorClear);
    
    layer_add_child(window_layer, text_layer_get_layer(s_latitude));
    layer_add_child(window_layer, text_layer_get_layer(s_longitude));
    
    get_coords();
}

static void random_unload(Window *window) {
    text_layer_destroy(s_latitude);
    text_layer_destroy(s_longitude);
}

static void selected_load(Window *window) {
    Layer* window_layer = window_get_root_layer(window);
    
    s_popup_text = text_layer_create(layer_get_bounds(window_layer));
    text_layer_set_background_color(s_popup_text, GColorClear);
    
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

static void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context) {
    static char s_lat_buffer[32];
    static char s_lon_buffer[32];
    if (key == COORDS_LAT) {
        snprintf(s_lat_buffer, sizeof(s_lat_buffer), "Lat: %s", new_tuple->value->cstring);
        //snprintf(s_lat_buffer, sizeof(s_lat_buffer), "Lat: %d", (int)new_tuple->value->uint32);
        if (s_latitude) text_layer_set_text(s_latitude, s_lat_buffer);
    } else {
        snprintf(s_lon_buffer, sizeof(s_lon_buffer), "Long: %s", new_tuple->value->cstring);
        //snprintf(s_lon_buffer, sizeof(s_lon_buffer), "Long: %d", (int)new_tuple->value->uint32);
        if (s_longitude) text_layer_set_text(s_longitude, s_lon_buffer);
    }
}

static void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Sync error!");
    APP_LOG(APP_LOG_LEVEL_ERROR, "Dict code: %d", (int)dict_error);
    APP_LOG(APP_LOG_LEVEL_ERROR, "AppMessage code: %d", (int)app_message_error);
}

static void init() {
    
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload,
    });
    
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
    
    Tuplet initial_values[] = {
        TupletCString(COORDS_LAT, "0"),
        TupletCString(COORDS_LONG, "0"),
    };
    
    window_stack_push(s_main_window, true);
    app_sync_init(&s_sync, s_sync_buffer, sizeof(s_sync_buffer), initial_values, ARRAY_LENGTH(initial_values), sync_changed_handler, sync_error_handler, NULL);
}

static void deinit() {
    window_destroy(s_main_window);
    app_sync_deinit(&s_sync);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
