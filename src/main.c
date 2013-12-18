#include "pebble.h"
	
// Constants
#define NUM_MENU_SECTIONS 1

// UI Variables
static Window *window;
static MenuLayer *menu_layer;

// Data Variables
static const char *section_title = "Loading...";
static const char *item_titles = "";
static uint16_t rows = 0;
static const char *section = "lists";
static char *title_divider = "\u00BB";
static GBitmap *menu_icons[5];

//static layer = MAIN;

// Guts and Stuff Variables
static AppSync sync;
static uint8_t sync_buffer[512];
enum Keys {
	TITLE_KEY = 0x0,	// TUPLE_CSTRING
	BODY_KEY = 0x1,		// TUPLE_CSTRING
	ROWS_KEY = 0X2,		// TUPLE_INTEGER
	SECTION_KEY = 0x3,	// TUPLE_CSTRING
};
/*enum Layers{
	MAIN = 0x0,
	LIST = 0x1,
	TASK = 0x2
};*/

// FUNCTIONS
char *strtok( char *s, const char *delim )
{
	register char *spanp;
	register int c, sc;
	char *tok;
	static char *last;

	if (s == NULL && (s = last) == NULL)
		return (NULL);

	/*
	 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	if (c == 0) {		/* no non-delimiter characters */
		last = NULL;
		return (NULL);
	}
	tok = s - 1;

	/*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */
	for (;;) {
		c = *s++;
		spanp = (char *)delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				last = s;
				return (tok);
			}
		} while (sc != 0);
	}
	/* NOTREACHED */
}

static void app_no_update(DictionaryResult dict_error, AppMessageResult app_message_error, void *context)
{
	APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);	
}

static void app_updated(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context)
{
	switch (key)
	{
		case TITLE_KEY:
			section_title = new_tuple->value->cstring;
			break;

		case BODY_KEY:
			item_titles = new_tuple->value->cstring;
			break;

		case ROWS_KEY:
			rows = new_tuple->value->uint8;
			break;
		
		case SECTION_KEY:
			section = new_tuple->value->cstring;
			break;
	}
	
	menu_layer_reload_data(menu_layer);
}

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data)
{
	return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data)
{
	return rows;
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
	uint16_t counter = 0;
	char titles[strlen(item_titles) + 1];
	strcpy(titles, item_titles);
	
	char *token = strtok(titles, title_divider);
		
	while(token != NULL)
	{		
		if(cell_index->row == counter)
		{
			if(counter < 3)
			{
				menu_cell_basic_draw(ctx, cell_layer, token, NULL, menu_icons[counter]);
			}
			else
			{
				menu_cell_basic_draw(ctx, cell_layer, token, NULL, menu_icons[3]);
			}
			
			break;
		}
		
		token = strtok(NULL, title_divider);
		counter++;
	}
}

void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data)
{
	layer_mark_dirty(menu_layer_get_layer(menu_layer));
}

static void send_cmd(void)
{
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
	// Setting Up the Icons
	menu_icons[0] = gbitmap_create_with_resource(INBOX_ICON);
	menu_icons[1] = gbitmap_create_with_resource(TODAY_ICON);
	menu_icons[2] = gbitmap_create_with_resource(WEEK_ICON);
	menu_icons[3] = gbitmap_create_with_resource(LIST_ICON);
	menu_icons[4] = gbitmap_create_with_resource(GROUP_ICON);
	
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
	Tuplet initial_values[] = {
		TupletCString(TITLE_KEY, "Loading..."),
		TupletCString(BODY_KEY, ""),
		TupletInteger(ROWS_KEY, (uint8_t) 0),
		TupletCString(SECTION_KEY, "lists"),
	};

	// Start Syncing
	app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values), app_updated, app_no_update, NULL);

	send_cmd();
}

static void window_unload(Window *window) {
	app_sync_deinit(&sync);
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