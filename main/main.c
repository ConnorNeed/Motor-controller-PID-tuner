#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "encoder.h"
#include "pid.h"

#define PWM_PIN 21
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_HIGH_SPEED_MODE
#define LEDC_FREQUENCY 5000 // 5 kHz frequency
#define LEDC_RESOLUTION LEDC_TIMER_8_BIT // 16-bit resolution
#define HIGH_PIN 4
#define LOW_PIN 2

void run_motor(){
  char *taskName = pcTaskGetName(NULL);
  while(1){
    // duty = (duty + 15) % 256;
    // ESP_LOGI("Velocity: %.2f RPS", encoder_get_velocity());
    ESP_LOGI("MAIN", "Velocity: %.2f RPS, pwm: %d", encoder_get_velocity(), get_pwm());
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, get_pwm());
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
    vTaskDelay(pdMS_TO_TICKS(10));
    // ESP_LOGI(taskName, "Spinning again..., %d", duty);
  }
}

void app_main() {
  char *taskName = pcTaskGetName(NULL);
  TaskHandle_t motorTaskHandle = NULL;
  TaskHandle_t encoderTaskHandle = NULL;
  TaskHandle_t pidTaskHandle = NULL;
  
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

  xTaskCreate(run_motor, "run_motor", 2048, NULL, 1, &motorTaskHandle);
  xTaskCreate(encoder_run, "encoder_task", 2048, NULL, 5, &encoderTaskHandle);
  xTaskCreate(pid_calc, "pid_task", 2048, NULL, 4, &pidTaskHandle);

  // Create tasks and store their handles

  // Wait for all tasks to finish
  while (1) {
    if (eTaskGetState(motorTaskHandle) == eDeleted &&
      eTaskGetState(encoderTaskHandle) == eDeleted &&
      eTaskGetState(pidTaskHandle) == eDeleted) {
      ESP_LOGI(taskName, "All tasks have finished.");
      break;
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
