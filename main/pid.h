typedef struct pid_input{
  float Kp;              // Proportional gain constant
  float Ki;              // Integral gain constant
  float Kd;              // Derivative gain constant
} pid_input_t;

void pid_calc();
void change_pwm(float delta);
int get_pwm();
void set_k_values(pid_input_t new_values);
void set_target_speed(double rpm);
float get_total_error();
void reset_pid();