#include "motor.h"
#include <xc.h>
#include <math.h>
#include "NU32.h"         
#include "isense.h"
#include "utilities.h"

//GLobal variables
extern int DutyCycle;  //GLobal variable set in menu
static volatile float Kp_m=0, Ki_m=0; //gains set by user
extern int StoringData;   //if this flag = 1, currently storing plot data
volatile int Eint;           //allows us to reset Eint to 0 when gains or reference signal is changed
volatile int REFcurrent;    //this is the current value that the control is trying to track in TUNE or TRACK mode

static int control_law(control_data d); //function prototype of local helper function that calculates the control

//*************5kHz interrupt to update Current/PWM*************
void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Current_Controller(void) { // _TIMER_2_VECTOR = 8 
        
      switch(util_state_get()) {
      case IDLE:
        motor_pwm_set(0); //set PWM to 0
       break;
      case PWM:
        motor_pwm_set(DutyCycle);  // set PWM to user specified value
      break;
      case TUNE: ;
          control_data tune;
        if(StoringData){ 
         static int counter = -1;               // initialize counter once
           counter++;                           // add one to counter every time ISR is entered
         if (counter==50) counter = 0;  // roll over counter every 50 ISR cycles (100Hz)
         if (counter<25) motor_amps_set(200); //create 100Hz-200mA square wave reference signal
           else motor_amps_set(-200);
         tune.reference=REFcurrent;          //set reference
         tune.sensor = isense_amps();        //get sensed data
         tune.control = control_law(tune);   //calculate control
         util_buffer_write(tune);            //write control_data to buffer
         motor_pwm_set(tune.control);        //set PWM based on control law
        }
              
      break;
      case TRACK: ;
          control_data track;      
          track.reference = REFcurrent;       //reference current set in position.c
          track.sensor = isense_amps();       //get sensor data
          track.control = control_law(track); //calculate control effort
          motor_pwm_set(track.control);       //set PWM based on control law
      break;
      default:
       //error, unknown state
       NU32_LED2 = 0;
     }
   IFS0bits.T2IF = 0;              // clear interrupt flag
}
//**********end interrupt*********************

void motor_init(void){           // Initializes the module and the peripherals it uses

//*******Set up Interrupt***************************
__builtin_disable_interrupts(); //disable interrupts at CPU
                                 
  PR2 = 15999;                  // set period register-5kHz
  TMR2 = 0;                     // initialize count to 0
  T2CONbits.TCKPS =0;           // set prescaler to 1
  T2CONbits.TGATE = 0;          //  not gated input (the default)
  T2CONbits.TCS = 0;            // PCBLK input (the default)
  T2CONbits.ON = 1;             // turn on Timer2
  IPC2bits.T2IP = 5;            // priority
  IPC2bits.T2IS = 0;            // subpriority
  IFS0bits.T2IF = 0;            // clear interrupt flag
  IEC0bits.T2IE = 1;            // enable interrupt
 __builtin_enable_interrupts(); // enable interrupts at CPU
//**************end Interrupt set up

//********PWM set up*********  
  T3CONbits.TCKPS = 0;     // Timer3 prescaler N=1 (1:1)
  PR3 = 3999;              // set period register-20kHz
  TMR3 = 0;                // initialize count to 0
  OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
  OC1CONbits.OCTSEL=1;     // Timer select bit or output compare 1 on port D
  OC1RS = 2000;            // set duty cycle = OC1RS/(PR3+1) = 50%
  OC1R = 2000;             // initialize before turning OC1 on; afterward it is read-only
  T3CONbits.ON = 1;        // turn on Timer3
  OC1CONbits.ON = 1;       // turn on OC1
//********End PWM Set up*******

//*****configure RD1 as digital output******
  TRISDCLR=0b0010;    // set D1 as output
  LATDbits.LATD1=1;   // initialize digital out pin RD1 high
//********end RD1 configure****************
 

}

 void motor_pwm_set(int duty_percent){   // Specifies the user's PWM value 
   int dc=0;
   if (duty_percent>=0) {
     dc=((duty_percent)/100.0)*(PR3+1.0); // calculate what OC1RS should be
     LATDbits.LATD1=1;                    // based on input argument of function
   }
   else {                                 //if duty cylce is negative make digital outpout high
     dc=((-duty_percent)/100.0)*(PR3+1.0);
     LATDbits.LATD1=0;
   }
  OC1RS=(unsigned int)dc; //set OC1RS by typecasting calculated value
}

void motor_amps_set(int amps){            // specifies the motor current reference, in milliamps
   REFcurrent= amps;
 }

PID_gains motor_gains_read(void){                  // send gains to the client
 PID_gains motortemp;
 motortemp.Kp=Kp_m;
 motortemp.Ki=Ki_m;
 motortemp.Kd=0;
return motortemp;
}

void motor_gains_write(PID_gains motor){                 // read the motor gains from the client
  __builtin_disable_interrupts(); //keep ISR disabled as brifly as possible
    Kp_m = motor.Kp;                    //copy local variables to globals used by ISR
    Ki_m = motor.Ki; 
    motor_control_reset();        //reset Eint
  __builtin_enable_interrupts();  //3 simple C commands while ISRs diabled
}

void motor_control_reset(void){  //helper function to clear Eint when tracking a new reference signal
  __builtin_disable_interrupts(); //keep ISR disabled as brifly as possible
    Eint=0;
  __builtin_enable_interrupts();  //3 simple C commands while ISRs diabled
}

static int control_law(control_data d){ //computes the control law
       static int e=0,u=50;  
        e=d.reference-d.sensor;
        Eint=Eint+e;
        u=Kp_m*e+Ki_m*Eint;
        if (u > 100) u = 100; //saturate at -100% and 100%
        if (u < -100) u = -100;
 return u;
}
