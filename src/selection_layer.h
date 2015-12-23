#pragma once
#include <pebble.h>

#define DEFAULT_CELL_WIDTH 50
#define DEFAULT_CELL_HEIGHT 40
#define DEFAULT_CELL_PADDING 10
#define DEFAULT_SELECTED_INDEX 0
#define DEFAULT_FONT FONT_KEY_GOTHIC_28_BOLD

#define NUM_CELLS 2
#define MAX_NUM_LENGTH 4

#define DEFAULT_MIN 0
#define DEFAULT_MAX 30

#define SET_RANGE 3

typedef struct SelectionLayerCallbacks {
    void *increment;
    void *decrement;
    void *forward;
    void *back;
} SelectionLayerCallbacks;

typedef struct SelectionBounds {
    GRect *bounds;
    uint8_t num_bounds;
} SelectionBounds;

typedef struct SelectionLayerData {
    int cell_width;
    int cell_height;
    int cell_padding;
    
    int selected_cell_idx;
    
    SelectionLayerCallbacks callbacks;
    
    GRect boxes[NUM_CELLS];
    void *context;
    int nums[NUM_CELLS];
    char text[NUM_CELLS][MAX_NUM_LENGTH];
    
    GFont font;
    GColor active_background_color;
    GColor inactive_background_color;
} SelectionLayerData;

void selection_layer_set_click_config_from_window(Layer *layer, Window *window);

Layer *selection_layer_create(GRect frame);
void selection_layer_destroy(Layer *layer);