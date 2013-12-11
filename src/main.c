#include "pebble.h"

// Initializing Veriables
static Window *window;
static TextLayer *text_layer;
static AppSync sync;
static uint8_t sync_buffer[64];
static uint8_t counter = 0;
static char text = ' ';
enum Keys {
	TEST_KEY = 0x0         // TUPLE_CSTRING
};

// Functions
static void app_no_update(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void app_updated(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
	text_layer_set_text(text_layer, new_tuple->value->cstring);
}

static void send_cmd(void) {
	Tuplet value = TupletInteger(1, 1);

	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);

	if (iter == NULL) {
		return;
	}

	dict_write_tuplet(iter, &value);
	dict_write_end(iter);

	app_message_outbox_send();
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);

	text_layer = text_layer_create(GRect(0, 95, 144, 68));
	text_layer_set_text_color(text_layer, GColorWhite);
	text_layer_set_background_color(text_layer, GColorClear);
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(text_layer));
	
	Tuplet initial_values[] = {
		TupletCString(TEST_KEY, "St Pebblesburg")
	};

	app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values), app_updated, app_no_update, NULL);

	send_cmd();
}

static void window_unload(Window *window) {
	app_sync_deinit(&sync);
	text_layer_destroy(text_layer);
}

static void init(void) {
	window = window_create();
	window_set_background_color(window, GColorBlack);
	window_set_fullscreen(window, true);
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload
	});

	const int inbound_size = 64;
	const int outbound_size = 64;
	app_message_open(inbound_size, outbound_size);

	const bool animated = true;
	window_stack_push(window, animated);
}

static void deinit(void) {
	window_destroy(window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}