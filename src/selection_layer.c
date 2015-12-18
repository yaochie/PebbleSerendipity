#include <pebble.h>
#include "selection_layer.h"

static void selection_layer_draw_rectangles(Layer *layer, GContext *ctx) {
    SelectionLayerData *data = layer_get_data(layer);
    for (int i=0; i<NUM_CELLS; i++) {
        if (i == data->selected_cell_idx) {
            graphics_context_set_fill_color(ctx, GColorClear);
            graphics_context_set_stroke_color(ctx, GColorBlack);
        } else {
            graphics_context_set_fill_color(ctx, GColorBlack);
            graphics_context_set_stroke_color(ctx, GColorClear);
        }
        
        GRect to_fill = data->boxes[i];
        graphics_fill_rect(ctx, to_fill, 0, GCornerNone);
        graphics_draw_rect(ctx, to_fill);
    }
}

static void selection_layer_draw_text(Layer *layer, GContext *ctx) {
    SelectionLayerData *data = layer_get_data(layer);
    for (int i=0; i<NUM_CELLS; i++) {
        if (i == data->selected_cell_idx) graphics_context_set_text_color(ctx, GColorBlack);
        else graphics_context_set_text_color(ctx, GColorClear);
        
        GRect box = data->boxes[i];
        graphics_draw_text(ctx, data->text[i], data->font, box, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    }
}

static void selection_layer_draw_arrows(Layer *layer, GContext *ctx) {
    
}

static void selection_layer_draw_labels(Layer *layer, GContext *ctx) {
    SelectionLayerData *data = layer_get_data(layer);
    (void)data;
}

static void selection_layer_draw(Layer *layer, GContext *ctx) {
    //Draw relative to bounds?
    selection_layer_draw_rectangles(layer, ctx);
    selection_layer_draw_text(layer, ctx);
    selection_layer_draw_arrows(layer, ctx);
    selection_layer_draw_labels(layer, ctx);
}

//--------------------------------------------------------------------
//Click Handlers

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
    //increase selected value by 5
    Layer *layer = (Layer*)context;
    SelectionLayerData *data = layer_get_data(layer);
    
    //data->callbacks.increment(data->selected_cell_idx, data->context);
    
    data->nums[data->selected_cell_idx] += 5;
    snprintf(data->text[data->selected_cell_idx], MAX_NUM_LENGTH, "%d", data->nums[data->selected_cell_idx]);
    layer_mark_dirty(layer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
    //increase selected value by 5
    Layer *layer = (Layer*)context;
    SelectionLayerData *data = layer_get_data(layer);
    
    //data->callbacks.decrement(data->selected_cell_idx, data->context);
    
    data->nums[data->selected_cell_idx] -= 5;
    if (data->nums[data->selected_cell_idx] < 0) data->nums[data->selected_cell_idx] = 0;
    snprintf(data->text[data->selected_cell_idx], MAX_NUM_LENGTH, "%d", data->nums[data->selected_cell_idx]);
    layer_mark_dirty(layer);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    //move to next field or go to navigation
    Layer *layer = (Layer*)context;
    SelectionLayerData *data = layer_get_data(layer);
    
    if (data->selected_cell_idx+1 < NUM_CELLS) {
        data->selected_cell_idx++;
        layer_mark_dirty(layer);
    }
}

static void back_click_handler(ClickRecognizerRef recognizer, void *context) {
    //move to previous field or close window
    Layer *layer = (Layer*)context;
    SelectionLayerData *data = layer_get_data(layer);
    
    if (data->selected_cell_idx > 0) {
        data->selected_cell_idx--;
        layer_mark_dirty(layer);
    } else {
        //close layer and window
        window_stack_pop(true);
    }
}

static void selection_click_config_provider(Layer *layer) {
    window_set_click_context(BUTTON_ID_UP, layer);
    window_set_click_context(BUTTON_ID_DOWN, layer);
    window_set_click_context(BUTTON_ID_SELECT, layer);
    window_set_click_context(BUTTON_ID_BACK, layer);
    
    window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
    window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler);
}

void selection_layer_set_click_config_from_window(Layer *layer, Window *window) {
    if (layer && window) {
        window_set_click_config_provider_with_context(window, (ClickConfigProvider)selection_click_config_provider, layer);
    }
}

//--------------------------------------------------------------------

static void selection_layer_init_boxes(Layer *layer) {
    GRect bounds = layer_get_bounds(layer);
    SelectionLayerData *data = layer_get_data(layer);
    
    int total_cell_width = NUM_CELLS * (data->cell_width + data->cell_padding) - data->cell_padding;
    
    for (int i=0; i<NUM_CELLS; i++) {
        int x = (bounds.size.w - total_cell_width)/2 + i*(data->cell_width + data->cell_padding);
        int y = (bounds.size.h - data->cell_height)/2;
        data->boxes[i] = GRect(x, y, data->cell_width, data->cell_height);
    }
}

static void selection_layer_init_data(Layer *layer) {
    SelectionLayerData *data = layer_get_data(layer);
    data->nums[0] = DEFAULT_MIN;
    data->nums[1] = DEFAULT_MAX;
    snprintf(data->text[0], MAX_NUM_LENGTH, "%d", data->nums[0]);
    snprintf(data->text[1], MAX_NUM_LENGTH, "%d", data->nums[1]);
}

/*
Create selection layer

Holds:
individual rectangles for each option
label on top
Arrows
Text of actual number
Different background color depending on whether it's selected
*/

Layer* selection_layer_create(GRect frame) {
    Layer *layer = layer_create_with_data(frame, sizeof(SelectionLayerData));
    
    SelectionLayerData *data = layer_get_data(layer);
    data->cell_width = DEFAULT_CELL_WIDTH;
    data->cell_height = DEFAULT_CELL_HEIGHT;
    data->cell_padding = DEFAULT_CELL_PADDING;
    data->selected_cell_idx = DEFAULT_SELECTED_INDEX;
    data->font = fonts_get_system_font(DEFAULT_FONT);
    
    selection_layer_init_boxes(layer);
    selection_layer_init_data(layer);
    
    //Set draw callbacks for underlying texts
    //Draw arrows above and below
    //return selection layer
    layer_set_update_proc(layer, (LayerUpdateProc)selection_layer_draw);
    
    return layer;
}

void selection_layer_destroy(Layer *layer) {
    layer_destroy(layer);
}
