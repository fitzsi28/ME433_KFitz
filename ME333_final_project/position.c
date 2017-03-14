#include "encoder.h"
#include "NU32.h"
#include "utilities.h"
#include "motor.h"
#include "position.h"
#include <xc.h>

//**Constants**
#define LOAD_SZ 200
#define REFPTS 2000

//**Global Variables**
static volatile float Kp_p=0, Ki_p=0,Kd_p=0; //Gains used by control law
volatile int Eint_p;       //integral error
int Trajectory[REFPTS];   //array of reference values from MATLAB
int Current;             //index of ISR
int RefLength;           //length of trajectory sent from MATLAB
extern int StoringData;  //flag declared in utilities.c

//internal helper function prototype
static int pos_control_law(control_data p); // 


//*************5kHz interrupt to update Current/PWM*************
void __ISR(_TIMER_4_VECTOR, IPL7SRS) Position_Controller(void) { //_TIMER_4_VECTOR = 16 
     
    control_data pos_control;
    if(util_state_get()==TRACK){
     pos_control.reference = Trajectory[Current];  //set the reference based on current index
     pos_control.sensor = encoder_angle();         //read encoder angle for comparison
     pos_control.control = pos_control_law(pos_control);  //calculate control law based on postition error
     if (StoringData) util_buffer_write(pos_control);     //If the buffer is not full, write to the buffer
     motor_amps_set(pos_control.control);                //set the current reference of the current control loop
     Current++;                                          //increment the index value
     if(Current==RefLength)Current=Current-1; //hold at final angle
               
     }   
     
   IFS0bits.T4IF = 0;              // clear interrupt flag
}
//**********end interrupt*********************



void position_init(void){       // initializes the position module  and the necessary positions
//*******Set up Interrupt***************************
__builtin_disable_interrupts(); //disable interrupts at CPU
                                 
  PR4 = 24999;                  // set period register-200Hz(400k core ticks)
  TMR4 = 0;                     // initialize count to 0
  T4CONbits.TCKPS =4;           // set prescaler to 16
  T4CONbits.TGATE = 0;          //  not gated input (the default)
  T4CONbits.TCS = 0;            // PCBLK input (the default)
  T4CONbits.ON = 1;             // turn on Timer4
  IPC4bits.T4IP = 7;            // priority higher than current control priority
  IPC4bits.T4IS = 0;            // subpriority
  IFS0bits.T4IF = 0;            // clear interrupt flag
  IEC0bits.T4IE = 1;            // enable interrupt
 __builtin_enable_interrupts(); // enable interrupts at CPU
//**************end Interrupt set up
//*****configure RD3 as digital output for debugging***
  TRISDbits.TRISD3=0;    // set D3 as output
  LATDbits.LATD3=1;   // initialize digital out pin RD1 high
//********end RD3 configure****************
}

int position_load(void){        // load a trajectory from the client and return its length
   char load[LOAD_SZ]; //message from MATLAB
    int i,x;           //declare index and temporary holder
     NU32_ReadUART1(load,LOAD_SZ);
     sscanf(load, "%d", &RefLength);   //read the length of the list coming from MATLAB
   for (i=0; i<RefLength; i++) {  //read reference data to Matlab
    NU32_ReadUART1(load,LOAD_SZ);
     sscanf(load, "%d", &x);      //read in one value at a tiem to temporary x
     Trajectory[i]=x;             //write x to the reference array           
    }  
 return RefLength;  
}

void position_reset(void){      // reset the current trajectory position
 Current=0;
}

PID_gains position_gains_read(void){ // read the position gains from the client
  PID_gains pos_temp;
   pos_temp.Kp=Kp_p;
   pos_temp.Ki=Ki_p;
   pos_temp.Kd=Kd_p;
return pos_temp;
}

void position_gains_send(PID_gains position){ // send the position gains to the client
__builtin_disable_interrupts(); //disable ISR
    Kp_p = position.Kp;            //copy local variables to globals used by ISR
    Ki_p = position.Ki;
    Kd_p = position.Kd; 
    position_control_reset();      //reset Eint_p
  __builtin_enable_interrupts();  
}

void position_control_reset(void){  //helper function to clear Eint_p
  __builtin_disable_interrupts(); //ISR disabled 
    Eint_p=0;
  __builtin_enable_interrupts();  
}

static int pos_control_law(control_data p){ //computes the control law
       static int e=0, edot=0,eprev=0, u=0;  
        e = p.reference - p.sensor;
        Eint_p = Eint_p + e;
        if(Eint_p > Ki_p*500) Eint_p = Ki_p*500;   //integrator anti-wind up
        if(Eint_p < Ki_p*-500) Eint_p = Ki_p*-500; //limited based on KI and and control effort
        edot = e - eprev;
        eprev = e;
        u = Kp_p*e + Ki_p*Eint_p + Kd_p*edot;
        if (u > 500) u = 500; //saturate at -500mA and 500mA
        if (u < -500) u = -500;
 return u;
}

