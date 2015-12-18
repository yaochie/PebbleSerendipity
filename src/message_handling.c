#include <pebble.h>
#include "message_handling.h"
#include "navigation.h"

static AppSync s_sync;
static uint8_t s_sync_buffer[256];

void send_message(int messageVal) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    
    if (!iter) {
        return;
    }
    
    int value = 0; //not important, key value (messageVal) is the actual payload
    dict_write_int(iter, messageVal, &value, sizeof(int), true);
    dict_write_end(iter);
    
    app_message_outbox_send();
}

static void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Dictionary Error: %d", dict_error);
    APP_LOG(APP_LOG_LEVEL_ERROR, "App Message Error: %d", app_message_error);
    
    // if error obtaining directions, ask again
}

static void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context) {
    bool loadedCoords = false;
    
    switch(key) {
    case COORDS_LAT:
        if (strlen(new_tuple->value->cstring) > 0 && strncmp(new_tuple->value->cstring, old_tuple->value->cstring, 32) != 0) {
            update_latitude(new_tuple->value->cstring);
            loadedCoords = true;
        }
        break;
    case COORDS_LONG:
        if (strlen(new_tuple->value->cstring) > 0 && strncmp(new_tuple->value->cstring, old_tuple->value->cstring, 32) != 0) {
            update_longitude(new_tuple->value->cstring);
            loadedCoords = true;
        }
        break;
    case DIRECTIONS:
        if (strlen(new_tuple->value->cstring) > 0 && strncmp(new_tuple->value->cstring, old_tuple->value->cstring, 128) != 0) {
            update_directions(new_tuple->value->cstring);
        }
        break;
    case DESTINATION:
        if (strlen(new_tuple->value->cstring) > 0 && strncmp(new_tuple->value->cstring, old_tuple->value->cstring, 64) != 0) {
            update_destination(new_tuple->value->cstring);
        }
        break;
    default:
        break;
    }
    if (loadedCoords && is_waiting_for_nav_loading()) clear_loading_placeholder();
}

void init_message_handling() {
    Tuplet initial_values[] = {
        TupletCString(COORDS_LAT, ""),
        TupletCString(COORDS_LONG, ""),
        TupletCString(DIRECTIONS, ""),
        TupletCString(DESTINATION, ""),
    };
    
    app_sync_init(&s_sync, s_sync_buffer, sizeof(s_sync_buffer), initial_values, ARRAY_LENGTH(initial_values), sync_changed_handler, sync_error_handler, NULL);
}

void deinit_message_handling() {
    app_sync_deinit(&s_sync);
}