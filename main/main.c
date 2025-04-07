#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "encoder.h"
#include "pid.h"
#include "GeneticAlg.h"

#define PWM_PIN 21
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_HIGH_SPEED_MODE
#define LEDC_FREQUENCY 5000 // 5 kHz frequency
#define LEDC_RESOLUTION LEDC_TIMER_8_BIT // 16-bit resolution
#define HIGH_PIN 4
#define LOW_PIN 2
#define GEN_ALG_ON 1

void run_motor(){
  char *taskName = pcTaskGetName(NULL);
  int count = 0;
  while(1){
    // duty = (duty + 15) % 256;
    // ESP_LOGI("Velocity: %.2f RPS", encoder_get_velocity());
    if(GEN_ALG_ON){
      if (count++ % 10 == 0){
        ESP_LOGI("Velocity", "%.2f", encoder_get_velocity());
        ESP_LOGI("PWM", "%d", get_pwm());
      }
    }else{
      ESP_LOGI("MAIN", "Velocity: %.2f RPM, pwm: %d", encoder_get_velocity(), get_pwm());
    }
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
  TaskHandle_t genAlgTaskHandle = NULL;
  
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

  // Create tasks and store their handles
  xTaskCreate(run_motor, "run_motor", 2048, NULL, 1, &motorTaskHandle);
  xTaskCreate(encoder_run, "encoder_task", 2048, NULL, 5, &encoderTaskHandle);
  xTaskCreate(pid_calc, "pid_task", 2048, NULL, 4, &pidTaskHandle);
  
  // Wait for all tasks to finish
  if (GEN_ALG_ON){
    xTaskCreate(run_gen_alg, "gen_alg_task", 8192, NULL, 2, &genAlgTaskHandle);
    while (1) {
      if (eTaskGetState(genAlgTaskHandle) == eDeleted) {
        ESP_LOGI(taskName, "Genetic alg has finished running, terminating processes.");
        vTaskDelete(motorTaskHandle);
        vTaskDelete(encoderTaskHandle);
        vTaskDelete(motorTaskHandle);
        break;
      }
      vTaskDelay(pdMS_TO_TICKS(100));
    }
  }else{
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

}
