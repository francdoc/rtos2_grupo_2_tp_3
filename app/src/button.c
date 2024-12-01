#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "ao.h"
#include "button.h"

/********************** macros and definitions *******************************/

#define TASK_PERIOD_MS_           (50)

#define BUTTON_PERIOD_MS_         (TASK_PERIOD_MS_)
#define BUTTON_PULSE_TIMEOUT_           (200)
#define BUTTON_SHORT_TIMEOUT_           (1000)
#define BUTTON_LONG_TIMEOUT_            (2000)

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

static struct
{
    uint32_t counter;
} button;

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

static button_type_t button_process_state(bool value)
{
    button_type_t ret = BUTTON_TYPE_NONE;

    if (value) {
        button.counter += BUTTON_PERIOD_MS_;
    } else {
        if (BUTTON_LONG_TIMEOUT_ <= button.counter) {
            ret = BUTTON_TYPE_LONG;
            // LOGGER_INFO("Button long detected, total press duration: %lu ms", button.counter);
        } else if (BUTTON_SHORT_TIMEOUT_ <= button.counter) {
            ret = BUTTON_TYPE_SHORT;
            // LOGGER_INFO("Button short detected, total press duration: %lu ms", button.counter);
        } else if (BUTTON_PULSE_TIMEOUT_ <= button.counter) {
            ret = BUTTON_TYPE_PULSE;
            // LOGGER_INFO("Button pulse detected, total press duration: %lu ms", button.counter);
        }
        button.counter = 0; // Reset counter after processing
    }

    return ret;
}

static void callback_(ao_event_t* ao_event)
{
  vPortFree((void*)ao_event);
}

static void send_(ao_t* ao, button_event_t* button_event)
{
	/*
	 * Memory Allocation:
	 * 		- pvPortMalloc is used to allocate memory for button_event, ensuring it can be safely cast to void* (the type expected by ao_event_t).
	 * Memory Copy:
	 * 		- memcpy copies the content of the button_event struct into the allocated memory. This ensures button_event data is preserved and correctly encapsulated as ao_event_t.
	 * Error Handling:
	 * 		- If memory allocation fails, an error is logged, and the function exits.
	 * 		- If ao_send fails, the allocated memory is freed to prevent memory leaks.
	 * Type Safety:
	 * 		- The use of dynamic memory ensures button_event_t is compatible with ao_event_t without breaking the generic design.
	*/

    ao_event_t ao_event = pvPortMalloc(sizeof(button_event_t));
    if (ao_event == NULL) {
        LOGGER_INFO("Memory allocation failed for button_event");
        return;
    }

    memset(ao_event, 0, sizeof(button_event_t));

    memcpy(ao_event, button_event, sizeof(button_event_t));
    // LOGGER_INFO("send_: Preparing to send button event with type: %d", button_event->type);

    if (BUTTON_TYPE_NONE != button_event->type) {
        if (ao_send(ao, callback_, ao_event)) {
            // LOGGER_INFO("send_: Message sent to ao_send func (ID: %d)", ao->ao_id);
            // LOGGER_INFO("send button msg: OK");
            return;
        } else {
            // LOGGER_INFO("send_: Failed to send button event, freeing memory");
            vPortFree((void*)ao_event);
        }
    }

    // LOGGER_INFO("send_: Button event type is NONE, skipping send");
    vPortFree((void*)ao_event);
}

void button_init(void)
{
	LOGGER_INFO("button init");
	button.counter = 0;
}

void button_task(void* argument)
{
    system_t *system = (system_t*)argument;
    ao_t *ui_ao = (ao_t*)system->ui;

    button_init();

    button_event_t button_event;
    button_event.ao_leds = system->leds;

    LOGGER_INFO("going to enter button loop");

    while (true) {
        GPIO_PinState button_state = HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN);
        button_event.type = button_process_state(button_state);

        if (button_event.type != BUTTON_TYPE_NONE) {
            LOGGER_INFO("Button event type detected: %d", button_event.type);
            send_(ui_ao, &button_event);
        }

        vTaskDelay((TickType_t)(TASK_PERIOD_MS_ / portTICK_PERIOD_MS));
    }
}

/********************** end of file ******************************************/
