#include <pebble.h>
#include "effect_layer.h"

int tap = 0;
int vertical_divide = 75;

bool invert = false;

#define KEY_INVERT 0

static Window *s_main_window;
static TextLayer *s_text_layer;
static EffectLayer *s_effect_layer;

//declare the font we will use
static GFont att_font;
static GFont now_font;

//decalre the png image we will use
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void tap_handler(AccelAxisType axis, int32_t direction) {
    switch (axis) {
  case ACCEL_AXIS_X:
    break;
  case ACCEL_AXIS_Y:
		if (tap == 0){
      text_layer_set_text(s_text_layer, "NOW");
      text_layer_set_font(s_text_layer, now_font);
      tap = 1;
	  } else{          
      text_layer_set_text(s_text_layer, "Appreciate\nThe\nTrees");
      text_layer_set_font(s_text_layer, att_font);
      tap = 0;
    }
    break;
  case ACCEL_AXIS_Z:
    break;
  }
}

//handle the configuration window
static void in_recv_handler(DictionaryIterator *iterator, void *context)
{
  //Get Tuple
  Tuple *t = dict_read_first(iterator);
  if(t)
  {
    switch(t->key)
    {
   	case KEY_INVERT:
      //It's the KEY_INVERT key
      if(strcmp(t->value->cstring, "on") == 0)
      {
        //save as inverted
        persist_write_bool(KEY_INVERT, true);
      }
      else if(strcmp(t->value->cstring, "off") == 0)
      {
        //save as not inverted
        persist_write_bool(KEY_INVERT, false);
      }
      break;
    }
  }
}

static void main_window_load(Window *window) {
  	s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_Red_Woods);
    s_background_layer = bitmap_layer_create(GRect(vertical_divide,1,94,167));
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
    
    Layer *window_layer = window_get_root_layer(window);

    // Create fonts
    now_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_Roboto_Slab_Light_40));
    att_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_Roboto_Slab_Light_18));
    
    // Create output TextLayer
    s_text_layer = text_layer_create(GRect(5,65,vertical_divide+30,190));
    text_layer_set_text(s_text_layer, "NOW");
    text_layer_set_font(s_text_layer, now_font);
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
	
		text_layer_set_background_color(s_text_layer, GColorClear);
    text_layer_set_text_color(s_text_layer, GColorBlack);
    text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
	
		//Check for saved option
	  invert = persist_read_bool(KEY_INVERT);
 
  	//Option-specific setup
  	if (invert){
			s_effect_layer = effect_layer_create(GRect(0,0,144,168));
			effect_layer_add_effect(s_effect_layer, effect_invert, NULL);
			layer_add_child(window_layer,effect_layer_get_layer(s_effect_layer));
		} else{}
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_text_layer);

    
  //Unload GFont
  fonts_unload_custom_font(now_font);
  fonts_unload_custom_font(att_font);
    
  // Destory Gbitmap
  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_background_layer);
	if(invert){
	//Destroy inverter layer
	effect_layer_destroy(s_effect_layer);}
}

static void init() {
  // Create main Window
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);

  // Subscribe to the accelerometer tap service
  accel_tap_service_subscribe(tap_handler);
	
	//start AppMessage to communicate with phone
  app_message_register_inbox_received((AppMessageInboxReceived) in_recv_handler);
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
  // Destroy main Window
  window_destroy(s_main_window);
  accel_tap_service_unsubscribe();
  
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}