#include <pebble.h>
#include "mainWindow.h"
#include "Images.h"

#define NUMBER_OF_DIGITS 4
#define NUMBER_OF_IMAGES 1

static TextLayer * time_layer;
static GFont time_font_16;

static Window *s_window;
static Layer *canvas;

struct Images * image_holder;
struct Images * digit_image_holder;
struct Texts * text_holder;


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
    update_text(text_holder->text_array[0], buffer)
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

    // Setting images
    image_holder = init_images_struct(NUMBER_OF_IMAGES);
    if (image_holder == NULL){
        APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to create image holder");
        return;
    }
    add_image(image_holder, window_bounds, RESOURCE_ID_WEATHER_BG, window_layer);

    // Text
    text_holder = init_texts_struct((uint32_t) 1, GColorWhite, GColorClear, RESOURCE_ID_TIME_24, window_layer);
    if (text_holder == NULL){
        APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to create text holder");
        return;
    }
    add_text(text_holder, GRect(0,0,128,100), "00:00", window_layer);
}

void window_unload(Window *window)
{
    de_init_images_struct(image_holder);
    de_init_images_struct(digit_image_holder);
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