#include <pebble.h>
#include "mainWindow.h"

#define NUMBER_OF_DIGITS 4
static GBitmap *digits[NUMBER_OF_DIGITS];
static BitmapLayer *digit_layers[NUMBER_OF_DIGITS];

static TextLayer * time_layer;
static GFont time_font_16;

static Window *s_window;
static Layer *canvas;
static GBitmap *bg;
static BitmapLayer *bg_layer;


static void canvas_update_proc(Layer *layer, GContext *ctx)
{
}

// Get the time and update the time text layer.
static void update_time() {
    // Getting seconds since epoch
    time_t temp = time(NULL);
    // Getting time in local time
    struct tm * tick_time = localtime(&temp);
    // Formatting the time
    static char buffer[8]; // Should this be manually destroyed when the app closes?
    strftime(buffer, sizeof(buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

    // Change the text layer for time to reflect the current time.
    text_layer_set_text(time_layer, buffer);
}

// To be called when a tick happens. For this watchface, that'll be every minute.
// The tick_timer_subscriber function call will set this function as a callback.
static void tick_handler(struct tm * tick_time, TimeUnits units_changed) {
    update_time();
    // Set the day indicator
    // Set the battery indicator too?
}

// Get the layers setup and going on screen when this windows is opened.
static void window_load(Window *window)
{
    // Get the layer for this window to put stuff on it.
    Layer *window_layer = window_get_root_layer(s_window);
    GRect window_bounds = layer_get_bounds(window_layer);

    // Setting up the layer I'll draw on. Probably won't use it.
    canvas = layer_create(window_bounds);
    layer_set_update_proc(canvas, canvas_update_proc);
    layer_add_child(window_layer, canvas);

    // Setting up the background image
    bg = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_BG);
    bg_layer = bitmap_layer_create(window_bounds);
    bitmap_layer_set_compositing_mode(bg_layer, GCompOpSet);
    bitmap_layer_set_bitmap(bg_layer, bg);
    layer_add_child(window_layer, bitmap_layer_get_layer(bg_layer));

    // Time layer
    time_font_16 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_TIME_16));
    time_layer = text_layer_create(GRect(50, 112, 45, 20));
    text_layer_set_background_color(time_layer, GColorClear);
    text_layer_set_text_color(time_layer, GColorWhite);
    text_layer_set_font(time_layer, time_font_16);
    text_layer_set_text_alignment(time_layer, GTextAlignmentLeft);
    text_layer_set_text(time_layer, "00:00"); // temporary text until the tick handler runs
    layer_add_child(window_layer, text_layer_get_layer(time_layer));
}

void window_unload(Window *window)
{
    gbitmap_destroy(bg);
    bitmap_layer_destroy(bg_layer);

    text_layer_destroy(time_layer);
}

void main_window_create(void)
{
    s_window = window_create();
    // Tell it which functions to call for loading and unloading.
    window_set_window_handlers(s_window, (WindowHandlers){
        .load = window_load,
        .unload = window_unload,
    });
    window_stack_push(s_window, true); // true == animated

    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    //battery_state_service_subscribe(battery_state_handler);
    update_time();
}

void main_window_destroy(void)
{
    window_destroy(s_window);
}