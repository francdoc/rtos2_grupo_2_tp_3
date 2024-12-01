#include "main.h"
#include "cmsis_os.h"
#include "logger.h"
#include "dwt.h"
#include "board.h"

#include "button.h"
#include "ui.h"
#include "leds.h"

#include "app.h"

#define LED_AO_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define UI_AO_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define BUTTON_TASK_PRIORITY (tskIDLE_PRIORITY + 1)

static ao_t ao_ui;
static pao_t pao_leds;

static system_t system = {
		.ui = &ao_ui,
		.leds = &pao_leds,
};

void app_init(void)
{
	ao_ui.ao_id = 0;
	ao_ui.ao_event_size = (uint8_t)sizeof(button_event_t);
	init_ao(&ao_ui, ui_process_event, UI_AO_TASK_PRIORITY, "task_ui");

	pao_leds.pao_id = 1;
	pao_leds.pao_event_size = sizeof(int);
	init_pao(&pao_leds, leds_process_event, LED_AO_TASK_PRIORITY, "task_leds");

	BaseType_t status;
	status = xTaskCreate(button_task, "Button_Task", configMINIMAL_STACK_SIZE, &system, BUTTON_TASK_PRIORITY, NULL);
	configASSERT(pdPASS == status);

	while (pdPASS != status)
	{
		// error
	}

	LOGGER_INFO("app init");

	cycle_counter_init();
}
