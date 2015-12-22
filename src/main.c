#include <pebble.h>
#include "constants.h"
#include "main.h"
#include "weather_layer.h"
#include "time_layer.h"

Window *main_window;
Layer *time_layer;
WeatherLayer *weather_layer;

bool wants_weather = false;

static void arrange_layers(void) {
	GRect window_frame = layer_get_frame(window_get_root_layer(main_window));
	
	uint16_t split = window_frame.size.h - (wants_weather ? weather_layer_get_content_size(weather_layer).h : 0);
	
	arrange_time_layer(window_frame, split);
	arrange_weather_layer(window_frame, split);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	wants_weather = true;
	arrange_layers();
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
	Tuple *temp_tuple = dict_find(iterator, KEY_TEMPERATURE);
	Tuple *summary_tuple = dict_find(iterator, KEY_SUMMARY);
	
	if(temp_tuple)
		weather_layer_set_temperature(weather_layer, temp_tuple->value->int8);
	if(summary_tuple)
		weather_layer_set_summary(weather_layer, summary_tuple->value->cstring);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void handle_init(void) {
  main_window = window_create();
	
	Layer* root = window_get_root_layer(main_window);
	GRect frame = layer_get_frame(root);

  time_layer = layer_create(frame);
	layer_set_update_proc(time_layer, update_time_layer);
	
	weather_layer = weather_layer_create(GRect(frame.origin.x, frame.origin.y + frame.size.h, frame.size.w, 0));
	
	layer_add_child(root, time_layer);
	layer_add_child(root, weather_layer_get_layer(weather_layer));
  window_stack_push(main_window, true);
	
	arrange_layers();
	
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	
	app_message_register_inbox_received(inbox_received_callback);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_sent(outbox_sent_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	
	app_message_open(APP_MESSAGE_INBOX_SIZE_MINIMUM, APP_MESSAGE_OUTBOX_SIZE_MINIMUM);
}

static void arrange_time_layer(GRect bounds, uint16_t split) {
	static PropertyAnimation *s_time_animation = NULL;
	
	GRect from = layer_get_frame(time_layer);
	GRect to = GRect(bounds.origin.x, bounds.origin.y, bounds.size.w, split);
	
	s_time_animation = property_animation_create_layer_frame(time_layer, &from, &to);
	animation_set_duration(property_animation_get_animation(s_time_animation), ANIMATION_DURATION);
	animation_schedule(property_animation_get_animation(s_time_animation));
}

static void arrange_weather_layer(GRect bounds, uint16_t split) {
	static PropertyAnimation *s_weather_animation = NULL;
	
	GRect from = layer_get_frame(weather_layer_get_layer(weather_layer));
	GRect to = GRect(bounds.origin.x, split, bounds.size.w, bounds.size.h - split);
	
	s_weather_animation = property_animation_create_layer_frame(weather_layer_get_layer(weather_layer), &from, &to);
	animation_set_duration(property_animation_get_animation(s_weather_animation), ANIMATION_DURATION);
	animation_schedule(property_animation_get_animation(s_weather_animation));
}

static void handle_deinit(void) {
  layer_destroy(time_layer);
	weather_layer_destroy(weather_layer);
  window_destroy(main_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
