#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ENCODER_A_GPIO 23
#define ENCODER_B_GPIO 22

#define ENCODER_RESOLUTION 512.0
#define VELOCITY_RESOLUTION 20 // Frequency to calculate velocity (hz)

static volatile int encoderPos = 0;
static double velocity = 0.0;

static void IRAM_ATTR encoder_isr_handler_a(void *arg) {
  static volatile bool encoderA = false;
  bool currentAState = gpio_get_level(ENCODER_A_GPIO);
  bool currentBState = gpio_get_level(ENCODER_B_GPIO);
  if (currentAState != encoderA) {
    if (currentBState != currentAState) {
      __sync_fetch_and_add(&encoderPos, 1);
    } else {
      __sync_fetch_and_sub(&encoderPos, 1);
    }
  }
  encoderA = currentAState;
}

static void IRAM_ATTR encoder_isr_handler_b(void *arg) {
  static volatile bool encoderB = false;
  bool currentAState = gpio_get_level(ENCODER_A_GPIO);
  bool currentBState = gpio_get_level(ENCODER_B_GPIO);
  if (currentBState != encoderB) {
    if (currentBState != currentAState) {
      __sync_fetch_and_sub(&encoderPos, 1);
    } else {
      __sync_fetch_and_add(&encoderPos, 1);
    }
  }
  encoderB = currentBState;
}

double encoder_get_velocity(void) { return velocity; }

void calculate_velocity() {
  static uint32_t lastTime = 0;
  static int lastEncoderPos = 0;
  uint32_t currentTime = esp_timer_get_time(); // microseconds
  int deltaPosition = encoderPos - lastEncoderPos;

  double deltaTime = (currentTime - lastTime) / 1e6; // seconds
  if (deltaTime > 0.001) {
    velocity = (deltaPosition / ENCODER_RESOLUTION) / deltaTime * 60;
  }

  lastTime = currentTime;
  lastEncoderPos = encoderPos;
}

void encoder_run(void *) {
  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_INTR_ANYEDGE;
  io_conf.pin_bit_mask = (1ULL << ENCODER_A_GPIO) | (1ULL << ENCODER_B_GPIO);
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
  gpio_config(&io_conf);

  gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
  gpio_isr_handler_add(ENCODER_A_GPIO, encoder_isr_handler_a, NULL);
  gpio_isr_handler_add(ENCODER_B_GPIO, encoder_isr_handler_b, NULL);

  const esp_timer_create_args_t timer_args = {
      .callback = &calculate_velocity,
      .arg = NULL,
      .name = "velocity_timer",
  };
  esp_timer_handle_t timer_handle;
  esp_timer_create(&timer_args, &timer_handle);
  uint32_t period = 1000000 / VELOCITY_RESOLUTION; // microseconds
  esp_timer_start_periodic(timer_handle, period);

  // Main loop to print velocity
  while (1) {
    vTaskDelay(500 / portTICK_PERIOD_MS); // Update every 0.1 second
  }
  esp_timer_stop(timer_handle);
  esp_timer_delete(timer_handle);
}
