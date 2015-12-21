#include <pebble.h>
#include "constants.h"
#include "main.h"
#include "weather_layer.h"
#include "time_layer.h"

Window *main_window;
Layer *time_layer;
WeatherLayer *weather_layer;

bool has_weather = true;

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	
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
}

static void arrange_layers(void) {
	GRect window_frame = layer_get_frame(window_get_root_layer(main_window));
	
	uint16_t split = window_frame.size.h - weather_layer_get_content_size(weather_layer).h;
	
	arrange_time_layer(window_frame, split);
	arrange_weather_layer(window_frame, split);
}

static void arrange_time_layer(GRect bounds, uint16_t split) {
	static PropertyAnimation *s_time_animation = NULL;
	
	GRect from = layer_get_frame(time_layer);
	GRect to = GRect(bounds.origin.x, bounds.origin.y, bounds.size.w, split);
	
	if(s_time_animation != NULL) {
		free(s_time_animation);
		s_time_animation = NULL;
	}
	
	s_time_animation = property_animation_create_layer_frame(time_layer, &from, &to);
	animation_set_duration(property_animation_get_animation(s_time_animation), ANIMATION_DURATION);
	animation_schedule(property_animation_get_animation(s_time_animation));
}

static void arrange_weather_layer(GRect bounds, uint16_t split) {
	static PropertyAnimation *s_weather_animation = NULL;
	
	GRect from = layer_get_frame(weather_layer_get_layer(weather_layer));
	GRect to = GRect(bounds.origin.x, split, bounds.size.w, bounds.size.h - split);
	
	if(s_weather_animation != NULL) {
		free(s_weather_animation);
		s_weather_animation = NULL;
	}
	
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
