#include "pebble.h"
	
// Constants
#define NUM_MENU_SECTIONS 1

// UI Variables
static Window *window;
static MenuLayer *menu_layer;

// Data Variables
static const char *section_title = "Test Items";
static uint16_t num_due = 4;

// Guts and Stuff Variables
/*static AppSync sync;
static uint8_t sync_buffer[512];
enum Keys {
	TITLE_KEY = 0x0,	// TUPLE_CSTRING
	BODY_KEY = 0x1,		// TUPLE_CSTRING
	DUE_KEY = 0X2		// TUPLE_INTEGER
};*/

// FUNCTIONS
/*static void app_no_update(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
	
}

static void app_updated(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context)
{
	switch (key)
	{
		case TITLE_KEY:
			section_title = new_tuple->value->cstring;
			//menu_section[0].title = new_tuple->value->cstring;
			break;

		case BODY_KEY:
			//\u00BB
			//menu_items[0].title = "Number of Tasks";
			//menu_items[0].subtitle = "Still a subtitle";
			//create_menu_items(body_layer, new_tuple->value->cstring);
			break;

		case DUE_KEY:
			num_due = new_tuple->value->uint8;
			//text_layer_set_text(title_layer, new_tuple->value->cstring);
			break;
	}
	;
}*/

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data)
{
	return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data)
{
	return num_due;
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data)
{
	return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data)
{
	menu_cell_basic_header_draw(ctx, cell_layer, section_title);
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data)
{
	menu_cell_basic_draw(ctx, cell_layer, "title", "subtitle", NULL);
}

void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data)
{
	layer_mark_dirty(menu_layer_get_layer(menu_layer));
}

/*static void send_cmd(void)
{
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);

	if (iter == NULL) {return;}
	
	Tuplet value = TupletInteger(1, 1);

	dict_write_tuplet(iter, &value);
	dict_write_end(iter);

	app_message_outbox_send();
}*/

static void window_load(Window *window)
{		
	// Initial Setup of the Menu
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
	
	menu_layer = menu_layer_create(bounds);
	
	menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks)
	{
		.get_num_sections = menu_get_num_sections_callback,
		.get_num_rows = menu_get_num_rows_callback,
		.get_header_height = menu_get_header_height_callback,
		.draw_header = menu_draw_header_callback,
		.draw_row = menu_draw_row_callback,
		.select_click = menu_select_callback,
	});
	
	menu_layer_set_click_config_onto_window(menu_layer, window);
	layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
		
	// Set Up Tuplet
	/*Tuplet initial_values[] = {
		TupletCString(TITLE_KEY, "Title"),
		TupletCString(BODY_KEY, "Body"),
		TupletInteger(DUE_KEY, (uint8_t) 0)
	};

	// Start Syncing
	app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values), app_updated, app_no_update, NULL);

	send_cmd();*/
}

static void window_unload(Window *window) {
	//app_sync_deinit(&sync);
	menu_layer_destroy(menu_layer);
}

static void init(void)
{
	window = window_create();
	window_set_background_color(window, GColorWhite);
	window_set_fullscreen(window, true);
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload
	});

	/*const int inbound_size = 1024;
	const int outbound_size = 128;
	app_message_open(inbound_size, outbound_size);*/

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