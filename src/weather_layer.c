#include <pebble.h>
#include "weather_layer.h"
#include "constants.h"
#include "drawing.h"
#include "utils.h"

static GFont summary_font = NULL;
static GFont temperature_font = NULL;
static const uint8_t padding = 4;

WeatherLayer* weather_layer_create(GRect frame) {
	if(!summary_font) {
		summary_font = fonts_get_system_font(FONT_KEY_GOTHIC_18);
		temperature_font = fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21);
	}
	
	WeatherLayer* weather_layer = (WeatherLayer*) malloc(sizeof(WeatherLayer));
	
	weather_layer->icon = gdraw_command_image_create_with_resource(RESOURCE_ID_GENERIC_QUESTION);
	
	Layer* root = weather_layer->root_layer = layer_create_with_data(frame, sizeof(WeatherLayer**));
	WeatherLayer** data = (WeatherLayer**)layer_get_data(root);
	*data = weather_layer;
	
	APP_LOG(APP_LOG_LEVEL_INFO, "Setting weather layer update proc");
	layer_set_update_proc(weather_layer_get_layer(weather_layer), update_weather_layer);
	
	weather_layer_set_summary(weather_layer, "Hang tight...");
	weather_layer_set_temperature(weather_layer, 0);
	weather_layer_set_icon(weather_layer, 1);
	
	return weather_layer;
}

static uint32_t look_up_icon(uint8_t icon) {
	switch (icon) {
		case 0:
			return RESOURCE_ID_CLOUDY_DAY;
		case 1:
			return RESOURCE_ID_GENERIC_QUESTION;
		case 2:
			return RESOURCE_ID_GENERIC_WEATHER;
		case 3:
			return RESOURCE_ID_HEAVY_RAIN;
		case 4:
			return RESOURCE_ID_HEAVY_SNOW;
		case 5:
			return RESOURCE_ID_LIGHT_RAIN;
		case 6:
			return RESOURCE_ID_PARTLY_CLOUDY;
		case 7:
			return RESOURCE_ID_RAINING_AND_SNOWING;
		case 8:
			return RESOURCE_ID_SUNNY_DAY;
		case 9:
			return RESOURCE_ID_SUNSET;
		default:
		return RESOURCE_ID_GENERIC_WEATHER;
	}
}

void weather_layer_set_icon(WeatherLayer* wl, uint8_t icon) {
	gdraw_command_image_destroy(wl->icon);
	wl->icon = gdraw_command_image_create_with_resource(look_up_icon(icon));
	GSize icon_size = gdraw_command_image_get_bounds_size(wl->icon);
	wl->middle_column_width = icon_size.w + 2 * padding;
	wl->icon_height = icon_size.h;
}

void weather_layer_set_temperature(WeatherLayer* wl, int8_t temp) {
	snprintf((char*) wl->temperature, sizeof(wl->temperature), "%d", (int)temp);
	layer_mark_dirty(wl->root_layer);
}

void weather_layer_set_summary(WeatherLayer* wl, const char* summary) {
	wl->summary = summary;
	layer_mark_dirty(wl->root_layer);
}

void weather_layer_destroy(WeatherLayer* weather_layer) {
	layer_destroy(weather_layer_get_layer(weather_layer));
	gdraw_command_image_destroy(weather_layer->icon);
	free(weather_layer);
}

Layer* weather_layer_get_layer(WeatherLayer* weather_layer) {
	return weather_layer->root_layer;
}

void draw_left_column(GContext *ctx, WeatherLayer* wl) {
	GRect bounds = layer_get_bounds(weather_layer_get_layer(wl));
	GRect left_column_rect = GRect(padding, 0, bounds.size.w - wl->middle_column_width - 1 - padding, bounds.size.h);
	graphics_context_set_text_color(ctx, GColorBlack);
	graphics_draw_text_vertically_centered(ctx, wl->summary, summary_font, left_column_rect, GTextAlignmentLeft);
}

void draw_middle_column(GContext* ctx, WeatherLayer* wl) {
	
	GRect bounds = layer_get_bounds(weather_layer_get_layer(wl));
	GRect middle_column_rect = GRect(bounds.size.w - wl->middle_column_width, 0, wl->middle_column_width, bounds.size.h);
	GSize temperature_text_size = graphics_text_layout_get_content_size(wl->temperature, temperature_font, middle_column_rect, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter);
	uint16_t temperature_y = middle_column_rect.origin.y + (middle_column_rect.size.h * 2 / 3) - temperature_text_size.h / 2;
	GRect temperature_rect = GRect(middle_column_rect.origin.x, temperature_y, middle_column_rect.size.w, temperature_text_size.h);
	graphics_draw_text(ctx, wl->temperature, temperature_font, temperature_rect, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
	
	uint16_t icon_y = middle_column_rect.origin.y + middle_column_rect.size.h / 3 - wl->icon_height / 2;
	gdraw_command_image_draw(ctx, wl->icon, GPoint(middle_column_rect.origin.x + padding, icon_y));
}

void update_weather_layer(Layer* layer, GContext *ctx) {
	WeatherLayer** data = layer_get_data(layer);
	WeatherLayer* wl = *data;
	
	draw_left_column(ctx, wl);
	draw_middle_column(ctx, wl);
}

GSize weather_layer_get_content_size(WeatherLayer* wl) {
	GRect bounds = layer_get_bounds(window_get_root_layer(layer_get_window(weather_layer_get_layer(wl))));
	GRect left_column_box = GRect(padding, padding, bounds.size.w - wl->middle_column_width - padding, bounds.size.h - padding);
	GRect middle_column_box = GRect(0, 0, wl->middle_column_width, bounds.size.h);
	
	uint16_t left_height = graphics_text_layout_get_content_size(wl->summary, summary_font, left_column_box, GTextOverflowModeWordWrap, GTextAlignmentLeft).h
		+ 2 * padding;
	uint16_t middle_height = graphics_text_layout_get_content_size(wl->temperature, summary_font, middle_column_box, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft).h
		+ wl->icon_height + padding * 3;
	
	return GSize(bounds.size.w, max(left_height, middle_height));
}