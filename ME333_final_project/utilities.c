#include "utilities.h"
#include <xc.h>
#include "NU32.h"     

#define DECIMATION 2     // plot every 2nd point
#define PLOTPTS 2000
#define SEND_SZ 200

static volatile state_t state=IDLE; //initialize the state 
                                    //static means the state can only be modified by
                                    //functions in this .c file. 
static int NumSamps;         //number of samples stored in buffer
static int AddSamps=0;       //number of additional samples beyond reference signal
volatile int StoringData=0;  //flag to store data when =1
int SenseArray[PLOTPTS];     //measured values to plot
int REFarray[PLOTPTS];       //reference value to plot
float CONTROLarray[PLOTPTS]; //array of control efforts

state_t util_state_get(void){               // get the current state
  int current_state=state;
  return current_state;
  }

void util_state_set(state_t s){         // automically set the current state. 
  state=s;
  }

void util_buffer_init(unsigned int n){  // initialize the buffer to record n samples
  NumSamps=n+AddSamps;
  StoringData=1;
  }

void util_samples_set(unsigned int n){  // set the addition samples to record
  AddSamps=n;
 }

void util_buffer_write(control_data d){ // write data to the current buffer position, if not full
    static int plotind=0, decctr=0;
    
    if (StoringData) {
    decctr++;
    if (decctr == DECIMATION) {             // after DECIMATION control loops,
      decctr = 0;                           // reset decimation counter
      SenseArray[plotind] = d.sensor;           // store data in global arrays
      REFarray[plotind] = d.reference;
      CONTROLarray[plotind]=d.control;
      plotind++;                            // increment plot data index
     }
    if (plotind == NumSamps) {              // if max number of plot points plot is reached,
      plotind = 0;                        // reset the plot index
      StoringData = 0;                    // tell util_buffer_output() data is ready to be sent to Matlab
     }
  }
  }

void util_buffer_output(void){              // wait for the buffer to be full and output it to the client
    while(StoringData) { //wait until flag says data storing is done
      ;     //do nothing 
    }
   char data[SEND_SZ]; //message to MATLAB
    sprintf(data, "%d\r\n", NumSamps);  //send total number of samples to be sent
    NU32_WriteUART1(data);
     int i;
   for (i=0; i<NumSamps; i++) {  //send plot data to Matlab
    sprintf(data, "%d %d %f\r\n", REFarray[i], SenseArray[i], CONTROLarray[i]);
    NU32_WriteUART1(data);
    }                
  }
