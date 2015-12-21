#include <pebble.h>
#include "drawing.h"

void graphics_draw_text_vertically_centered(GContext* ctx, const char* text, const GFont font, const GRect rect, const GTextAlignment halign) {
	GSize text_size = graphics_text_layout_get_content_size(text, font, rect, GTextOverflowModeWordWrap, halign);
	GRect text_rect = GRect(rect.origin.x, rect.origin.y + (rect.size.h / 2) - (text_size.h / 2), rect.size.w, text_size.h);
	graphics_draw_text(ctx, text, font, text_rect, GTextOverflowModeWordWrap, halign, NULL);
}