#ifndef MOTOR__H__
#define MOTOR__H__

typedef struct {

 float Kp;
 float Ki;
 float Kd;
} PID_gains;  //a convenient type definition to pass multiple gain values

void motor_init(void);                    // Initializes the module and the peripherals it uses

void motor_control_reset(void);                 // helper function to clear Eint(prevent wind-up)

void motor_pwm_set(int duty_percent);     // Specifies the user's PWM value 

void motor_amps_set(int amps);            // specifies the motor current reference, in milliamps

PID_gains motor_gains_read();                  // read the gains from the client

void motor_gains_write(PID_gains motor);                 // sends the motor gains to the client



#endif
