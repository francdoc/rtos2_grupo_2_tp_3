#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "ao.h"

typedef enum
{
  BUTTON_TYPE_PULSE,
  BUTTON_TYPE_SHORT,
  BUTTON_TYPE_LONG,
  BUTTON_TYPE__N,
  BUTTON_TYPE_NONE,
} button_type_t;

typedef struct {
  button_type_t type;
  pao_t *ao_leds;
} button_event_t;

void button_task(void* argument);

#endif /* INC_BUTTON_H_ */
