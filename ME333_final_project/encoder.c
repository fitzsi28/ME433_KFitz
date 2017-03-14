#include "encoder.h"
#include <xc.h>

static int encoder_command(int read) { // send  a command to the encoder chip
  SPI4BUF = read;                 // send the command

  while (!SPI4STATbits.SPIRBF) { ; } // wait for the response
  
  SPI4BUF;                        // garbage was transfered over, ignore it
  SPI4BUF = 5;                    // write garbage, but the corresponding read will have the data
 
  while (!SPI4STATbits.SPIRBF) { ; }

  return SPI4BUF;
}

void encoder_init(void) {
  // SPI initialization for reading from the encoder chip
  SPI4CON = 0; 		          // stop and reset SPI4
  SPI4BUF;		          // read to clear the rx buffer
  SPI4BRG = 0x4; 	          // bit rate to 8MHz, SPI4BRG = 80000000/(2*desired)-1
  SPI4STATbits.SPIROV = 0;  // clear the overflow
  SPI4CONbits.MSTEN = 1;    // master mode
  SPI4CONbits.MSSEN = 1;    // Slave select enable
  SPI4CONbits.MODE16 = 1;   // 16 bit mode
  SPI4CONbits.MODE32 = 0; 
  SPI4CONbits.SMP = 1;      // sample at the end of the clock
  SPI4CONbits.ON = 1;       // turn spi on
}

int encoder_ticks(void) {
  encoder_command(0x01);	        // we need to read twice to get a valid reading
  return encoder_command(0x01);
}

void encoder_reset(void){        // reset the encoder position
  encoder_command(0x00);
}

int encoder_angle(void){          // read the encoder angle, in tenths of a degree
  int angle = 0;
   angle = (encoder_ticks()-32768)*2.69;  // subtract off zero angle
                                  // and account for the 3600/1336=2.69decidegrees resolution
  return angle;
}
