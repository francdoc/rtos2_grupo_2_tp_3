/*
 * Copyright (c) 2023 Sebastian Bedin <sebabedin@gmail.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 */

/********************** inclusions *******************************************/

#include "main.h"
#include "cmsis_os.h"
#include "logger.h"
#include "dwt.h"
#include "board.h"

#include "task_button.h"
#include "task_led.h"
#include "task_ui.h"

#include "ao.h"

/********************** macros and definitions *******************************/
#define LED_AO_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define UI_AO_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define BUTTON_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
/********************** internal data declaration ****************************/
static active_object_t led_obj, ui_interface;

static all_obj_t all_obj = {
  .led = &led_obj,
  .ui = &ui_interface
};

enum {
  RED_LED_AO_ID = 1,
  GREEN_LED_AO_ID,
  BLUE_LED_AO_ID,
  UI_INTERFACE_AO_ID
};
/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data declaration *****************************/

/********************** external functions definition ************************/
void app_init(void)
{

  init_ui_active_object(&ui_interface, ui_process_event, UI_AO_TASK_PRIORITY);
  init_led_active_object(&led_obj, task_led, LED_AO_TASK_PRIORITY);

  BaseType_t status;

  status = xTaskCreate(task_button, "Button_Task", configMINIMAL_STACK_SIZE, &all_obj, BUTTON_TASK_PRIORITY, NULL);
  configASSERT(pdPASS == status);
  while (pdPASS != status)
  {
    // error
  }

  LOGGER_INFO("app init");

  cycle_counter_init();
}

/********************** end of file ******************************************/