#include <leds.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "app.h"
#include "button.h"
#include "ui.h"

void ui_process_event(ao_event_t* ao_event) {
	if ((ao_event == NULL) | (*ao_event == NULL)) {
		LOGGER_INFO("ui_process_event: Received NULL event pointer");
		return;
	}

    // LOGGER_INFO("ui_process_event: Received ao_event pointer: %p", *ao_event);

    /* Adjusting ui_process_event to accept a pointer matches the type expected by ao_process_event_t. */
    button_event_t* button_event = (button_event_t*)(*ao_event);

    // Check if the casted pointer is valid
    if (button_event == NULL) {
        LOGGER_INFO("ui_process_event: button_event is NULL after casting");
        return;
    }

    if (button_event->type < BUTTON_TYPE_PULSE || button_event->type > BUTTON_TYPE_NONE) {
        LOGGER_INFO("ui_process_event: Invalid button event type: %d", button_event->type);
        return;
    }

    // LOGGER_INFO("ui_process_event: Button event type received: %d", button_event->type);

    int priority = 0;
    led_color_t led_color = LED_COLOR_NONE;

    // LOGGER_INFO("ui_process_event: Entering switch case");

    switch (button_event->type) {
        case BUTTON_TYPE_PULSE:
            // LOGGER_INFO("PULSE");
            priority = 3;
            led_color = LED_COLOR_RED;
            break;
        case BUTTON_TYPE_SHORT:
            // LOGGER_INFO("SHORT");
            priority = 2;
            led_color = LED_COLOR_YELLOW;
            break;
        case BUTTON_TYPE_LONG:
            // LOGGER_INFO("LONG");
            priority = 1;
            led_color = LED_COLOR_BLUE;
            break;
        default:
            LOGGER_INFO("Unknown button event type: %d", button_event->type);
            return; // Exit if the event type is unknown
    }

    if (button_event->type != BUTTON_TYPE_NONE) {
          // LOGGER_INFO("Priority: %d, LED Color: %d", priority, led_color);

          if (!pao_send(button_event->ao_leds, led_color, (int) priority)) {
              LOGGER_INFO("ui_process_event: Failed to send LED event to PAO");
          }
          else {
        	  // LOGGER_INFO("ui_process_event: Successfully sent LED event to PAO");
          }
    }
}

