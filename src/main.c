#include "pebble.h"

// UI Variables
static Window *window;
static SimpleMenuLayer *body_layer;
static SimpleMenuSection menu_section[1];
static SimpleMenuItem menu_items[2];

// Data Variables
static uint8_t num_items;
//static string item_array[];

// Guts and Stuff Variables
static AppSync sync;
static uint8_t sync_buffer[512];
enum Keys {
	TITLE_KEY = 0x0,	// TUPLE_CSTRING
	BODY_KEY = 0x1,		// TUPLE_CSTRING
	DUE_KEY = 0X2		// TUPLE_INTEGER
};

// FUNCTIONS
static char * strocc(const char * string)
{	
	/*int j = 0;
	char result[5];
	
	for(int i = 0 ; ; i++)
	{
		if(string[i] == "\u00BB")
			j++;
		else if(string[i] == EOF)
			break;
	}
	
	return sprintf(result, "%d", j);*/
	return "hello";
}

static void app_no_update(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
	
}

static void app_updated(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context)
{
	switch (key)
	{
		case TITLE_KEY:
			menu_section[0].title = new_tuple->value->cstring;
			break;

		case BODY_KEY:
			//\u00BB
			menu_items[0].title = "Number of Tasks";
			menu_items[0].subtitle = strocc(new_tuple->value->cstring);
			//create_menu_items(body_layer, new_tuple->value->cstring);
			break;

		case DUE_KEY:
			//text_layer_set_text(title_layer, new_tuple->value->cstring);
			break;
	}
	;
}

static void create_menu()
{
	;
}

static void create_menu_items(char *items)
{
	/*char item_array[100][ARRAY_LENGTH(strtok(items, "\u00BB"))] = strtok(items, "\u00BB");
	
	 for (int i = 0 ; i < (int) ARRAY_LENGTH(item_array)) ; i++)
	 {
		 menu_items[i] = (SimpleMenuItem)
		 {
			 .title = item_array[i]
		 };
	 }*/
}

static void send_cmd(void) {
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);

	if (iter == NULL) {return;}
	
	Tuplet value = TupletInteger(1, 1);

	dict_write_tuplet(iter, &value);
	dict_write_end(iter);

	app_message_outbox_send();
}

static void window_load(Window *window)
{		
	// Initial Setup of the Menu
	menu_items[0] = (SimpleMenuItem)
	{
		.title = "Title One",
		.subtitle = "This is the subtitle"
	};
	menu_items[1] = (SimpleMenuItem)
	{
		.title = "Title Two",
		.subtitle = "This is the second subtitle"
	};
	menu_section[0] = (SimpleMenuSection)
	{
		.title = "Today (?)",
		.items = menu_items,
		.num_items = ARRAY_LENGTH(menu_items)
	};
	
	body_layer = simple_menu_layer_create(layer_get_frame(window_get_root_layer(window)), window, menu_section, ARRAY_LENGTH(menu_section), NULL);
	
	layer_add_child(window_get_root_layer(window), simple_menu_layer_get_layer(body_layer));
		
	// Set Up Tuplet
	Tuplet initial_values[] = {
		TupletCString(TITLE_KEY, "Title"),
		TupletCString(BODY_KEY, "Body"),
		TupletInteger(DUE_KEY, (uint8_t) 0)
	};

	// Start Syncing
	app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values), app_updated, app_no_update, NULL);

	send_cmd();
}

static void window_unload(Window *window) {
	app_sync_deinit(&sync);
	simple_menu_layer_destroy(body_layer);
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

	const int inbound_size = 1024;
	const int outbound_size = 128;
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