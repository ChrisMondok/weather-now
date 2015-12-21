#include <pebble.h>
#include "time_layer.h"
#include "constants.h"
#include "drawing.h"

static GFont time_font = NULL;

static void trim_leading_zero(char* buffer);

void update_time_layer(Layer* layer, GContext* ctx) {
	if(!time_font)
		time_font = fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS);
	
	GRect frame = layer_get_frame(layer);
	
	graphics_context_set_fill_color(ctx, TIME_BG);
	graphics_fill_rect(ctx, frame, 0, GCornerNone);
	
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
	static char s_buffer[8];
	
	strftime(s_buffer, sizeof(s_buffer), "%I:%M", tick_time);
	trim_leading_zero(s_buffer);
	
	graphics_draw_text_vertically_centered(ctx, s_buffer, time_font, frame, GTextAlignmentCenter);
}

static void trim_leading_zero(char* buffer) {
	uint8_t start = 0;
	uint8_t l = strlen(buffer);
	while(buffer[start] == '0')
		start++;
	for(uint8_t i = 0; i <= l - start; i++) {
		buffer[i] = buffer[i + start];
	}
}