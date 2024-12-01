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
#include "leds.h"

#include "queue_p.h"

#define MESSAGE_TIMEOUT_MS_      (100)
#define QUEUE_LENGTH_            (10)

// #define DEBUGGING

static char *QUEUE_ID_1 = "Queue_id_1";
static char *QUEUE_ID_2 = "Queue_id_2";
static char *QUEUE_ID_3 = "Queue_id_3";
static char *QUEUE_ID_4 = "Queue_id_4";
static char *QUEUE_ID_DEF = "Queue_id_default";

char *get_queue_name(uint8_t id) {
	char *buffer[4] = {0};
	if (id > 4) {
		return QUEUE_ID_DEF;
	}

	buffer[0] = QUEUE_ID_1;
	buffer[1] = QUEUE_ID_2;
	buffer[2] = QUEUE_ID_3;
	buffer[3] = QUEUE_ID_4;

	return buffer[id-1];
}

void ao_task(void* parameters){
	ao_t *ao = (ao_t*) parameters;

	ao_msg_t ao_msg;

	while (pdPASS == xQueueReceive(ao->event_queue_h, &ao_msg, (TickType_t)(portMAX_DELAY))) {
		// LOGGER_INFO("received message");

		// Log the contents of ao_msg
		// LOGGER_INFO("ao_task: ao_msg.ao_event pointer: %p", ao_msg.ao_event);
		// LOGGER_INFO("ao_task: ao_msg.ao_msg_callback pointer: %p", ao_msg.ao_msg_callback);

#ifdef DEBUGGING
		// Attempt to cast ao_event to a specific type for logging
		button_event_t* button_event = (button_event_t*)(ao_msg.ao_event);
		if (button_event) {
			LOGGER_INFO("ao_task: button_event.type: %d", button_event->type);
		}
#endif

		// LOGGER_INFO("ao_task: executing process_event");
		ao->ao_process_event(&ao_msg.ao_event); // ao_process_event expects: ao_event_t* (a void**).

		// LOGGER_INFO("ao_task: executing callback");
		if (ao_msg.ao_msg_callback){
			ao_msg.ao_msg_callback(ao_msg.ao_event);
		}
	}
}

void pao_task(void* parameters){
	pao_t *pao = (pao_t*) parameters;

    pao_event_t pao_event = (pao_event_t)pvPortMalloc(pao->pao_event_size);

	for (;;) {
		if (queue_pop(pao->pevent_queue_h, pao_event) && pao->pao_process_event != NULL) {
			pao->pao_process_event(pao_event);
		}
	}
}

bool_t ao_send(ao_t* ao, ao_msg_callback_t ao_msg_callback, ao_event_t ao_event)
{
    ao_msg_t ao_msg;
    ao_msg.ao_msg_callback = ao_msg_callback;
    ao_msg.ao_event = ao_event;

    // Log the contents of ao_msg before sending
	// LOGGER_INFO("ao_send: ao_msg.ao_event pointer: %p", ao_msg.ao_event);
	// LOGGER_INFO("ao_send: ao_msg.ao_msg_callback pointer: %p", ao_msg.ao_msg_callback);

    // Log the type of event being sent
    // LOGGER_INFO("ao_send: Sending event to queue with ID: %d", ao->ao_id);

    if (pdPASS == xQueueSendToBack(ao->event_queue_h, (void*)&ao_msg, 0)) {
        // LOGGER_INFO("ao_send: Message successfully sent to queue (ID: %d)", ao->ao_id);
        return true;
    } else {
        // LOGGER_INFO("ao_send: Failed to send message to queue (ID: %d)", ao->ao_id);
        return false;
    }
}

bool_t pao_send(pao_t* pao, int data, int priority) {
	if (!pao) {
		// LOGGER_INFO("pao_send: NULL pao pointer");
		return false;
	}
	return (queue_push(pao->pevent_queue_h, data, priority));
}

void init_ao(ao_t* ao,
		ao_process_event_t ao_process_event,
		uint8_t ao_task_priority,
		const char* task_name)
{
	ao->event_queue_h = xQueueCreate(QUEUE_LENGTH_, ao->ao_event_size);
	vQueueAddToRegistry(ao->event_queue_h, get_queue_name(ao->ao_id));
	configASSERT(NULL != ao->event_queue_h);

	ao->ao_process_event = ao_process_event;

	BaseType_t status;
	status = xTaskCreate(ao_task, task_name, configMINIMAL_STACK_SIZE, ao, ao_task_priority, NULL);
	configASSERT(pdPASS == status);
}

void init_pao(pao_t* pao,
		pao_process_event_t pao_process_event,
		uint8_t pao_task_priority,
		const char* task_name)
{
	queue_create((queue_p_t**)&pao->pevent_queue_h);
    configASSERT(NULL != pao->pevent_queue_h);

    pao->pao_process_event = pao_process_event;

    BaseType_t status;
    status = xTaskCreate(pao_task, task_name, configMINIMAL_STACK_SIZE, pao, pao_task_priority, NULL);
    configASSERT(pdPASS == status);
}