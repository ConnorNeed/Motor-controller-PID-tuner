#include "freertos/FreeRTOS.h"
#include "pid.h"
#include "encoder.h"
#include "esp_timer.h"

static volatile int target_rpm;

#define SAMPLE_TIME 10
#define RUN_TIME 60000

void pid_calc(pid_input_t* pid_input, int current_rpm){
  TickType_t startTick = xTaskGetTickCount();
  TickType_t endTick = startTick + pdMS_TO_TICKS(RUN_TIME); 
  float total_error = 0;
  float prev_error = 0;
  int64_t cur_time = esp_timer_get_time();
  int64_t prev_time = -1;

  int cur_error = 0;

  float proportional_term, integral_term, derivative_term;
  float term_sums = 0;

  while (xTaskGetTickCount() < endTick){
    prev_time = cur_time;
    cur_time = esp_timer_get_time();
    
    prev_error = cur_error;
    cur_error = pid_input->tgt_rmp - encoder_get_velocity();

    proportional_term = pid_input->Kp*cur_error;
    integral_term = total_error + (pid_input->Ki*cur_error);
    total_error += cur_error;
    derivative_term = (cur_error - prev_error) / SAMPLE_TIME;

    term_sums += (proportional_term + integral_term + derivative_term);
    
    // calcualte pwm
    // send pwm to pin
    vTaskDelay(pdMS_TO_TICKS(SAMPLE_TIME));
  }
}