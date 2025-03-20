typedef struct pid_input{
  float Kp;              // Proportional gain constant
  float Ki;              // Integral gain constant
  float Kd;              // Derivative gain constant
} pid_input_t;

void pid_calc();
void change_pwm(float delta);
void set_target_spd(float target_speed);
int get_pwm();
