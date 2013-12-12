#include "pebble.h"

// Initializing Veriables
static Window *window;
static ScrollLayer *scroll_layer;
static TextLayer *title_layer;
static TextLayer *body_layer;
static AppSync sync;
static uint8_t sync_buffer[64];
enum Keys {
	TITLE_KEY = 0x0,	// TUPLE_CSTRING
	BODY_KEY = 0x1,		// TUPLE_CSTRING
	DUE_KEY = 0X2		// TUPLE_INTEGER
};

// Functions
static void app_no_update(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void app_updated(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
	switch (key) {
		case TITLE_KEY:
			text_layer_set_text(title_layer, new_tuple->value->cstring);
			break;

		case BODY_KEY:
			text_layer_set_text(body_layer, new_tuple->value->cstring);
			break;

		case DUE_KEY:
			text_layer_set_text(title_layer, new_tuple->value->cstring);
			break;
	}
	;
}

static void send_cmd(void) {
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);

	if (iter == NULL) {
		return;
	}
	
	Tuplet value = TupletInteger(1, 1);

	dict_write_tuplet(iter, &value);
	dict_write_end(iter);

	app_message_outbox_send();
}

static void window_load(Window *window) {	
	scroll_layer = scroll_layer_create(layer_get_frame(window_get_root_layer(window)));
	scroll_layer_set_click_config_onto_window(scroll_layer, window);

	title_layer = text_layer_create(GRect(0, 0, 144, 36));
	text_layer_set_text_color(title_layer, GColorBlack);
	text_layer_set_background_color(title_layer, GColorClear);
	text_layer_set_font(title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(title_layer, GTextAlignmentCenter);
	
	body_layer = text_layer_create(GRect(0, 36, 144, 2000));
	text_layer_set_text_color(body_layer, GColorBlack);
	text_layer_set_background_color(body_layer, GColorClear);
	text_layer_set_font(body_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text_alignment(body_layer, GTextAlignmentCenter);
	
	text_layer_set_text(title_layer, "Title");
	text_layer_set_text(body_layer, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Phasellus hendrerit. Pellentesque aliquet nibh nec urna. In nisi neque, aliquet vel, dapibus id, mattis vel, nisi. Sed pretium, ligula sollicitudin laoreet viverra, tortor libero sodales leo, eget blandit nunc tortor eu nibh. Nullam mollis. Ut justo. Suspendisse potenti.");
	
	scroll_layer_add_child(scroll_layer, text_layer_get_layer(title_layer));
	scroll_layer_add_child(scroll_layer, text_layer_get_layer(body_layer));
	layer_add_child(window_get_root_layer(window), scroll_layer_get_layer(scroll_layer));
	
	GSize content_size = text_layer_get_content_size(body_layer);
	text_layer_set_size(body_layer, content_size);	
	scroll_layer_set_content_size(scroll_layer, GSize(content_size.w, content_size.h+42));
	
	Tuplet initial_values[] = {
		TupletCString(TITLE_KEY, "Title"),
		TupletCString(BODY_KEY, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Phasellus hendrerit. Pellentesque aliquet nibh nec urna. In nisi neque, aliquet vel, dapibus id, mattis vel, nisi. Sed pretium, ligula sollicitudin laoreet viverra, tortor libero sodales leo, eget blandit nunc tortor eu nibh. Nullam mollis. Ut justo. Suspendisse potenti."),
		TupletInteger(DUE_KEY, (uint8_t) 0)
	};

	app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values), app_updated, app_no_update, NULL);

	send_cmd();
}

static void window_unload(Window *window) {
	app_sync_deinit(&sync);
	text_layer_destroy(title_layer);
	text_layer_destroy(body_layer);
	scroll_layer_destroy(scroll_layer);
}

static void init(void) {
	window = window_create();
	window_set_background_color(window, GColorWhite);
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