#ifndef INC_AO_H_
#define INC_AO_H_

#include "app.h"
#include "queue_p.h"

#include <stdbool.h>

#define MAX_SIZE 10

typedef void* ao_event_t;
typedef void* pao_event_t;

typedef void (*ao_process_event_t)(ao_event_t* ao_event);
typedef void (*pao_process_event_t)(pao_event_t* pao_event);

typedef void (*ao_msg_callback_t)(ao_event_t* ao_event);

typedef struct {
	ao_msg_callback_t ao_msg_callback;
	ao_event_t ao_event;
} ao_msg_t;

typedef struct {
	uint8_t ao_event_size;
	ao_process_event_t ao_process_event;
	QueueHandle_t event_queue_h;
	uint8_t ao_id;
 } ao_t;

typedef struct {
	queue_p_t* pevent_queue_h;
	pao_process_event_t pao_process_event;
	uint8_t pao_event_size;
	uint8_t pao_id;
} pao_t;

typedef struct {
	ao_t *ui;
	pao_t *leds;
} system_t;

void ao_task(void* parameters);

void pao_task(void* parameters);

void init_ao(ao_t* ao,
		ao_process_event_t ao_process_event,
		uint8_t ao_task_priority,
		const char* task_name);

void init_pao(pao_t* pao,
		pao_process_event_t pao_process_event,
		uint8_t pao_task_priority,
		const char* task_name);

bool_t ao_send(ao_t* ao,
		ao_msg_callback_t ao_msg_callback,
		ao_event_t ao_event);

bool_t pao_send(pao_t* pao,
		int data,
		int priority);

#endif /* INC_AO_H_ */
