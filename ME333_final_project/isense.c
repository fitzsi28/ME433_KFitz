#include "isense.h"
#include <xc.h>
#include <stdio.h>
#include <math.h>



void isense_init(void){
  AD1PCFGbits.PCFG14 = 0;                 // AN14 is an adc pin
  AD1CON3bits.ADCS = 2;                   // ADC clock period is Tad = 2*(ADCS+1)*Tpb =
                                          //                           2*3*12.5ns = 75ns
  AD1CON1bits.ADON = 1;                   // turn on A/D converter
}

//****internal helper function to read a single value from ADC pin
static unsigned int adc_sample(void) { // sample and convert the value on the given adc pin
    unsigned int elapsed = 0, finish_time = 0;
    AD1CHSbits.CH0SA = 14;                // connect pin AN14 to MUXA for sampling
    AD1CON1bits.SAMP = 1;                  // start sampling
    elapsed = _CP0_GET_COUNT();
    finish_time = elapsed + SAMPLE_TIME;
    while (_CP0_GET_COUNT() < finish_time) { 
      ;                                   // sample for more than 250 ns
    }
    AD1CON1bits.SAMP = 0;                 // stop sampling and start converting
    while (!AD1CON1bits.DONE) {
      ;                                   // wait for the conversion process to finish
    }
    return ADC1BUF0;                      // read the buffer with the result
}


short isense_ticks(void){    //read the current sensor, in ADC ticks
 int sampsum=0, sample=0, i;  
 for(i=1; i<100; i++){
     sampsum=sampsum + adc_sample();  //read the adc pin 100 times
   }
 sample=sampsum/100;                   //take the average and return as the read value
return sample;
}

int isense_amps(void){    //read current sensor and convert to mA
  int adc_val=0, amp_val=0;
  adc_val=isense_ticks();
  amp_val=(2.0*adc_val)-1002;  //conversion from ticks to miliamps based on linear regression

return amp_val;

}

 
