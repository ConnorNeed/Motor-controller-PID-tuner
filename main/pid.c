#include <math.h>

#include "freertos/FreeRTOS.h"
#include "pid.h"
#include "encoder.h"
#include "esp_timer.h"
#include "esp_log.h"

// static volatile int target_rpm;

#define SAMPLE_TIME 100

// Target rpm is modified from main thread during genetic algo
static double target_rpm = 6;
static double Kp = 1;
static double Ki = 0.001;
static double Kd = 0.0001;

// Fitness value, also used for integral term
float total_error = 0;

// Used for derivative term
float prev_error = 0;
int64_t cur_time = 0;
int64_t prev_time = 0;

// duty (pwm) is modified from the main thread
static double pwm = 0;
float term_sums = 0;

float proportional_term, integral_term, derivative_term;

void change_pwm(float delta){
  pwm += delta;
  if (pwm >= 255) { pwm = 255; }
  if (pwm <= 0) { pwm = 0; }
}

int get_pwm(){
  return round(pwm);
}

float get_total_error(){
  return total_error;
}

void reset_total_error(){
  total_error = 0;
}

float cur_error = 0;
void pid_calc(){

  while(1){
    prev_time = cur_time;
    cur_time = esp_timer_get_time();
    
    prev_error = cur_error;
    cur_error = target_rpm - encoder_get_velocity();
  
    proportional_term = Kp*cur_error;
    integral_term = Ki*(total_error + cur_error);
    total_error += cur_error;
    derivative_term = Kd*((cur_error - prev_error) / SAMPLE_TIME);
  
    term_sums = (proportional_term + integral_term + derivative_term);
    change_pwm(term_sums);
  
    // calcualte pwm
    // send pwm to pin
    vTaskDelay(pdMS_TO_TICKS(SAMPLE_TIME));
  }
}

//Interface with genetic alg
void set_k_values(pid_input_t new_values){
  Kd = new_values.Kd;
  Ki = new_values.Ki;
  Kp = new_values.Kp;
}
void set_target_speed(double rpm){
  target_rpm = rpm;
}
float get_total_error(){
  return total_error;
}
void reset_pid(){
  //Nothing yet
}