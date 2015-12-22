#pragma once
#include <pebble.h>

typedef struct WeatherLayer {
	Layer *root_layer;
	GDrawCommandImage* icon;
	const char* summary;
	const char temperature[5];
	uint16_t middle_column_width;
	uint16_t icon_height;
} WeatherLayer;

WeatherLayer* weather_layer_create(GRect frame);

void weather_layer_destroy(WeatherLayer* weather_layer);

Layer* weather_layer_get_layer(WeatherLayer* weather_layer);

void update_weather_layer(Layer* layer, GContext* ctx);

void weather_layer_set_icon(WeatherLayer* weather_layer, uint8_t icon);

void weather_layer_set_temperature(WeatherLayer* weather_layer, int8_t temperature);

void weather_layer_set_summary(WeatherLayer* weather_layer, const char* summary);

GSize weather_layer_get_content_size(WeatherLayer* weather_layer);