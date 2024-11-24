/*
* ao.h
* author: José Luis Krüger
*/

#ifndef ACTIVE_OBJECT_H
#define ACTIVE_OBJECT_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

typedef void* event_data_t;
typedef void (*event_callback_t)(event_data_t event);

typedef struct {
    QueueHandle_t event_queue;
    event_callback_t process_event;
    uint8_t event_size;
    uint8_t obj_id;
} active_object_t;

void active_object_init(active_object_t *obj,
                        event_callback_t process_event,
                        size_t queue_size,
                        int task_priority,
                        const char* task_name);

void active_object_send_event(active_object_t *obj, event_data_t event);

void active_object_task(void *pv_parameters);

#endif // ACTIVE_OBJECT_H
