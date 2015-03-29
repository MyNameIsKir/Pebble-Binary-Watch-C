#include <pebble.h>
#include <stdio.h>

Window *s_main_window;
TextLayer *text_layer;
char binary_time[27], binary_hours[9], binary_minutes[9], binary_seconds[9];

void getBin(int num, char *str)
{
  *(str+8) = '\0';
  int mask = 0x80 << 1;
  while(mask >>= 1){
    *str++ = !!(mask & num) + '0';
  }
}

static void update_time() {
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  int hours = tick_time->tm_hour, minutes = tick_time->tm_min, seconds = tick_time->tm_sec;

  if(clock_is_24h_style() == false && hours > 12) {
      hours = hours - 12;
  }
  
  strcpy(binary_hours, "00000000");
  strcpy(binary_minutes, "00000000");
  strcpy(binary_seconds, "00000000");
  getBin(hours, binary_hours);
  getBin(minutes, binary_minutes);
  getBin(seconds, binary_seconds);
  
  strcpy(binary_time, binary_hours);
  strcat(binary_time, "\n");
  strcat(binary_time, binary_minutes);
  strcat(binary_time, "\n");
  strcat(binary_time, binary_seconds);

  text_layer_set_text(text_layer, binary_time);
}

static void main_window_load(Window *window) {
  text_layer = text_layer_create(GRect(0, 15, 144, 90));
  text_layer_set_background_color(text_layer, GColorBlack);
  text_layer_set_text_color(text_layer, GColorClear);
  text_layer_set_text(text_layer, "00000000\n00000000\n00000000");

  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void handle_init(void) {
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);
  
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void handle_deinit(void) {
  window_destroy(s_main_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
