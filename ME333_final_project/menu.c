#include "NU32.h"          // config bits, constants, funcs for startup and UART
#include "encoder.h"// include other module headers here
#include "utilities.h"
#include "isense.h"
#include "motor.h"
#include "position.h"
#include <math.h>

#define BUF_SIZE 200

//Global varaibles
 volatile int DutyCycle=0; //DutyCycle set in PWM mode
int Samps; //number of  reference samps sent by MATLAB
 
int main() 
{
  char buffer[BUF_SIZE];
  NU32_Startup(); 	// cache on, min flash wait, interrupts on, LED/button init, UART init
  NU32_LED1 = 1;        // turn off the LEDs
  NU32_LED2 = 1;        
  __builtin_disable_interrupts(); //set up peripherals and ISR's
      encoder_init();
      isense_init();
      motor_init();
      position_init();
  __builtin_enable_interrupts();

  while(1)
  {
    NU32_ReadUART1(buffer,BUF_SIZE);	  // we expect the next character to be a menu command
    NU32_LED2 = 1;                        // clear the error led
    switch (buffer[0]) {
     case 'a':                           // read current sensor ticks
      {
	sprintf(buffer,"%d\r\n", isense_ticks());
        NU32_WriteUART1(buffer);
        break;
      }
      case 'b':                           // read current sensor in mA
      {       
	sprintf(buffer,"%d\r\n", isense_amps());
        NU32_WriteUART1(buffer);
        break;
      }
      
      case 'c':                           // encoder ticks
      {
        sprintf(buffer,"%d\r\n", encoder_ticks());
        NU32_WriteUART1(buffer);
        break;
      }
      case 'd':                           // encoder angle
      {
        sprintf(buffer,"%d\r\n", encoder_angle());
        NU32_WriteUART1(buffer);
        break;
      }
      case 'e':                           // encoder reset
      {
        encoder_reset();
        break;
      }
      case 'f':                           // Set PWM mode
      { 
        util_state_set(PWM);     //set state variable to PWM mode
        NU32_ReadUART1(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &DutyCycle); //update DutyCycle for interrupt to use
        break;
       }
       case 'g':                           // Set current gains
       { PID_gains motor;                     //local variable of type PID_gains
         motor.Kp=0; motor.Ki=0; motor.Kd=0;  //set default value of gains
        NU32_ReadUART1(buffer,BUF_SIZE);
        sscanf(buffer, "%f %f", &motor.Kp, &motor.Ki); //Read Kp and Ki
        motor_gains_write(motor);   //pass PID_gains structure to gain write function
        break;
       }
       case 'h':                           // Get current gains
       { PID_gains motor;
         motor=motor_gains_read();       
         sprintf(buffer,"%f %f\r\n", motor.Kp, motor.Ki);
          NU32_WriteUART1(buffer);
        
        break;
        }
        case 'i':                           // Set position gains
        { PID_gains position;               //use same PID_gains stucture as current gains
         position.Kp=0; position.Ki=0; position.Kd=0;
        NU32_ReadUART1(buffer,BUF_SIZE);
        sscanf(buffer, "%f %f %f", &position.Kp, &position.Ki, &position.Kd);
        position_gains_send(position);
        
        break;
        }
        case 'j':                           // Get position gains
        { PID_gains position;
         position=position_gains_read();
         sprintf(buffer,"%f %f %f\r\n", position.Kp, position.Ki, position.Kd);
          NU32_WriteUART1(buffer);
        
         break;
        }
        case 'k':                           // Tune current loop
         util_buffer_init(50);
         motor_control_reset();      //reset Eint
         util_state_set(TUNE);       //set state to TUNE for ISR
         util_buffer_output();       //send buffer to MATLAB
         util_state_set(IDLE);       //set state back to IDLE
        { 
        
        break;
        }
        case 'l':                           // Set recording samples
        { int addsamps;          
          NU32_ReadUART1(buffer,BUF_SIZE);
          sscanf(buffer, "%d", &addsamps);
          util_samples_set(addsamps);       //set additional sample for the buffer
        break;
        }
        case 'm':                           // Go to angle
        { Samps = position_load();        //read the values from Matlab
          util_buffer_init(Samps);        //set buffer to take samples=reference samples+additional samples
          position_control_reset();       //clear position integral error
          position_reset();               //set trajectory index to beginning
          util_state_set(TRACK);          //set state to TRACK
          util_buffer_output();           //send position control data to matlab
        break;
        }
        case 'n':                           // Load step trajectory
        { util_state_set(IDLE);       //set state back to IDLE
          Samps = position_load();         //read values from Matlab and store number of reference samples
          position_control_reset();        //clear Eint
          position_reset();                //set trajectory index to beginning
        
        break;
        }
        case 'o':                           // Load cubic trajectory
        { util_state_set(IDLE);       //set state back to IDLE
          Samps = position_load(); 
          position_control_reset();
          position_reset();
        
        break;
        }
        case 'p':                           // Execute trajectory
        { util_buffer_init(Samps);        //set buffer to take samples=reference samples+additional samples
         position_reset();                //set trajectory index to beginning
         util_state_set(TRACK);           //set state to TRACK
         util_buffer_output();            //send position control data to matlab
        break;
        }
      case 'q':
      {
        state_t local_state;
	local_state=IDLE;
        util_state_set(local_state);// handle q for quit and return to idle state here 
        break;
      }
      case 'r':
      { 
        sprintf(buffer,"%d\r\n", util_state_get());
        NU32_WriteUART1(buffer);    // query the state 
        break;
      }
      default:
      {
        NU32_LED2 = 0;    // turn on LED2 to indicate an error
        break;
      }
    }
  }
  return 0;
}


