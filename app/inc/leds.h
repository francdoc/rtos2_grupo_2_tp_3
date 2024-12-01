#ifndef INC_LEDS_H_
#define INC_LEDS_H_

#include "ao.h"

typedef enum {
	LED_COLOR_NONE,
	LED_COLOR_RED,
	LED_COLOR_YELLOW,
	LED_COLOR_BLUE,
	LED_COLOR_WHITE,
	LED_COLOR_N,
} led_color_t;

void leds_process_event(pao_event_t* pao_event);

#endif /* INC_BUTTON_H_ */
