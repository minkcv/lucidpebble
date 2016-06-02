#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static GFont s_time_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void main_window_load(Window *window) {
    // Get info about the Window
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    // Create GBitmap
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);

    // Create BitmapLayer at new location
    GRect image_bounds = {{(144 / 2) - (120 / 2), 0}, {120, 120}};
    s_background_layer = bitmap_layer_create(image_bounds);

    // Set the bitmap onto the layer and add to the window
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
    layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));

    // Create the text layer with bounds
    s_time_layer = text_layer_create(
            GRect(0, PBL_IF_ROUND_ELSE(58, 120), bounds.size.w, 50));

    // Load font
    s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DEJAVU_SANS_MONO_32));

    // Improve layout
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorWhite);
    text_layer_set_font(s_time_layer, s_time_font);
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

    // Add as child layer to window
    layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
    // Destroy TextLayer
    text_layer_destroy(s_time_layer);

    // Unload font
    fonts_unload_custom_font(s_time_font);

    // Destroy GBitmap
    gbitmap_destroy(s_background_bitmap);

    // Destory BitmapLayer
    bitmap_layer_destroy(s_background_layer);
}

static void update_time() {
    // Get a tm structure
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    // Write the current hours and minutes into a buffer
    static char s_buffer[8];
    strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? 
            "%H:%M" : "%I:%M", tick_time);

    // Display this time on the TextLayer
    text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

static void init() {
    // Create main window element and assign to pointer
    s_main_window = window_create();

    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(s_main_window, (WindowHandlers) {
            .load = main_window_load,
            .unload = main_window_unload
    });

    window_set_background_color(s_main_window, GColorBlack);

    // Show the Window on the watch, with animated=true
    window_stack_push(s_main_window, true);

    // Register with TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

    update_time();
}

static void deinit() {
    // Destroy Window
    window_destroy(s_main_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
