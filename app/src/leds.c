#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "leds.h"
#include "ao.h"
#include "button.h"

#define TASK_PERIOD_MS_           (5000)

void led_set_colors(bool r, bool g, bool b)
{
  HAL_GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, r ? GPIO_PIN_SET: GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LED_YELLOW_PORT, LED_YELLOW_PIN, g ? GPIO_PIN_SET: GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, b ? GPIO_PIN_SET: GPIO_PIN_RESET);
}

/********************** external functions definition ************************/

void leds_process_event(pao_event_t* pao_event)
{
  led_color_t color = *(led_color_t*)pao_event;
  switch (color)
  {
    case LED_COLOR_NONE:
      led_set_colors(false, false, false);
      break;
    case LED_COLOR_RED:
      LOGGER_INFO("led red");
      led_set_colors(true, false, false);
      break;
    case LED_COLOR_YELLOW:
      LOGGER_INFO("led yellow");
      led_set_colors(false, true, false);
      break;
    case LED_COLOR_BLUE:
      LOGGER_INFO("led blue");
      led_set_colors(false, false, true);
      break;
    case LED_COLOR_WHITE:
      LOGGER_INFO("led white");
      led_set_colors(true, true, true);
      break;
    default:
      break;
  }
  vTaskDelay((TickType_t)(TASK_PERIOD_MS_ / portTICK_PERIOD_MS));
  led_set_colors(false, false, false);
}

/********************** end of file ******************************************/
