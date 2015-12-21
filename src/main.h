#include <pebble.h>

static void arrange_layers(void);
static void arrange_time_layer(GRect bounds, uint16_t split);
static void arrange_weather_layer(GRect bounds, uint16_t split);