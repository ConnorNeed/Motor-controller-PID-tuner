typedef struct pid_input{
  float Kp;              // Proportional gain constant
  float Ki;              // Integral gain constant
  float Kd;              // Derivative gain constant
  int tgt_rmp;           // Target RPM
} pid_input_t;

void pid_calc(pid_input_t* pid_input, int current_rpm);
