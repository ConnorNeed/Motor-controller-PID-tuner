#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_log.h"

#define PWM_PIN 26
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_HIGH_SPEED_MODE
#define LEDC_FREQUENCY 5000 // 5 kHz frequency
#define LEDC_RESOLUTION LEDC_TIMER_8_BIT // 8-bit resolution
#define HIGH_PIN 0
#define LOW_PIN 2

void app_main() {
  char *taskName = pcTaskGetName(NULL);
  
  // Configure the PWM timer
  ledc_timer_config_t ledc_timer = {
    .speed_mode = LEDC_MODE,
    .duty_resolution = LEDC_RESOLUTION,
    .timer_num = LEDC_TIMER,
    .freq_hz = LEDC_FREQUENCY,
    .clk_cfg = LEDC_AUTO_CLK
  };
  ledc_timer_config(&ledc_timer);

  // Configure the PWM channel
  ledc_channel_config_t ledc_channel = {
    .gpio_num = PWM_PIN,
    .speed_mode = LEDC_MODE,
    .channel = LEDC_CHANNEL,
    .intr_type = LEDC_INTR_DISABLE,
    .timer_sel = LEDC_TIMER,
    .duty = 0, // Start with 0% duty cycle
    .hpoint = 0
  };
  ledc_channel_config(&ledc_channel);
  
  gpio_reset_pin(HIGH_PIN);
  gpio_reset_pin(LOW_PIN);

  gpio_set_direction(HIGH_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(LOW_PIN, GPIO_MODE_OUTPUT);
  gpio_set_level(HIGH_PIN, 1);
  gpio_set_level(LOW_PIN, 0);
  ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 10);
  ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
  
  while(1){
    vTaskDelay(pdMS_TO_TICKS(500));
    ESP_LOGI(taskName, "Spinning again...");
  }


  

  // while (1) {
  //   for (int duty = 0; duty <= 255; duty += 5) { // Increase brightness
  //     ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
  //     ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
  //     vTaskDelay(pdMS_TO_TICKS(50));
  //   }
  //   ESP_LOGI(taskName, "LED ON");
  //   for (int duty = 255; duty >= 0; duty -= 5) { // Decrease brightness
  //     ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
  //     ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
  //     vTaskDelay(pdMS_TO_TICKS(50));
  //   }
  //   ESP_LOGI(taskName, "LED OFF");
  // }
}
