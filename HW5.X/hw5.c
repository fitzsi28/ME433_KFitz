#include<xc.h> // processor SFR definitions
#include<sys/attribs.h> // __ISR macro

//Include the 2 libraries for the LED
#include "i2c_display.h"
#include "i2c_master_int.h"
#include "accel.h"


//Wiring choices
//A0 for AN0, B7 for LED1, B13 for USER, and B15 for OC1/LED2

// <editor-fold defaultstate="collapsed" desc="DEVCFGs here">
/* ***********************DEVCFGs here ********************** */
// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // not boot write protect
#pragma config CP = OFF // no code protect
// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // free up secondary osc pins by turning sosc off
#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // slowest wdt
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN = OFF // wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%
// DEVCFG2 - get the CPU clock to 40MHz
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz:8MHz/2
#pragma config FPLLMUL = MUL_20 // multiply clock after FPLLIDIV:4*20 = 80MHz
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 40MHz:80/2 = 40MHz
#pragma config UPLLIDIV = DIV_2 // divide 8MHz input clock, then multiply by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on
// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = ON // allow only one reconfiguration
#pragma config IOL1WAY = ON // allow only one reconfiguration
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // controlled by USB module
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="ASCII table">
// lookup table for all of the ascii characters
static const char ASCII[96][5] = {
 {0x00, 0x00, 0x00, 0x00, 0x00} // 20  (space)
,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c �
,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j
,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e ?
,{0x00, 0x06, 0x09, 0x09, 0x06} // 7f ?
}; // end char ASCII[96][5]
         // </editor-fold>


int readADC(void);
void display_message_i(int row, int col, char *message);

int main() {
 // <editor-fold defaultstate="collapsed" desc="SETUP">
/* ***********************SETUP here ********************** */
        __builtin_disable_interrupts();
        // set the CP0 CONFIG register to indicate that
        // kseg0 is cacheable (0x3) or uncacheable (0x2)
        // see Chapter 2 "CPU for Devices with M4K Core"
        // of the PIC32 reference manual
        __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
        // no cache on this chip!
        // 0 data RAM access wait states
        BMXCONbits.BMXWSDRM = 0x0;
        // enable multi vector interrupts
        INTCONbits.MVEC = 0x1;
        // disable JTAG to be able to use TDI, TDO, TCK, TMS as digital
        DDPCONbits.JTAGEN = 0;

    // set up USER pin as input
    ANSELBbits.ANSB13 = 0;
    TRISBbits.TRISB13 = 1;
    // set up LED1 pin as a digital output
    TRISBbits.TRISB7 = 0;
    LATBbits.LATB7 = 0;
    // set up LED2 as OC1 using Timer2 at 1kHz
    ANSELBbits.ANSB15 = 0;
    RPB15Rbits.RPB15R = 0b0101;
    //********PWM set up*********
    T2CONbits.TCKPS = 0b100;     // Timer3 prescaler N=8 (1:1)
    PR2 = 2499;              // set period register-20kHz
    TMR2 = 0;                // initialize count to 0
    OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
    OC1CONbits.OCTSEL=0;     // Timer select bit or output compare 1
    OC1RS = 0;            // set duty cycle = OC1RS/(PR3+1) = 50%
    OC1R = 0;             // initialize before turning OC1 on; afterward it is read-only
    T2CONbits.ON = 1;        // turn on Timer3
    OC1CONbits.ON = 1;       // turn on OC1
//********End PWM Set up*******
    // set up A0 as AN0
    ANSELAbits.ANSA0 = 1;
    AD1CON3bits.ADCS = 3;
    AD1CHSbits.CH0SA = 0;
    AD1CON1bits.ADON = 1;
//set up RB10 pin as a digital output to power OLED
    TRISBbits.TRISB4 = 0;
    LATBbits.LATB4 = 0;
    acc_setup();
     __builtin_enable_interrupts();
           // </editor-fold>
        _CP0_SET_COUNT(0);
        LATBbits.LATB4 = 1;
        while(_CP0_GET_COUNT()<2000000){/*wait 0.1sec*/}       
        display_init();
        display_clear();
        char message[100];
        acc_setup();
        short accels[3]={1, 2, 3};
        int accelvec[2];
        short mags[3];
        short temp;
        // read the accelerometer from all three axes

       // the accelerometer and the pic32 are both little endian by default (the lowest address has the LSB)

      // the accelerations are 16-bit twos compliment numbers, the same as a short
while (1) {
      acc_read_register(OUT_X_L_A, (unsigned char *) accels, 6);

      // need to read all 6 bytes in one transaction to get an update.

       acc_read_register(OUT_X_L_M, (unsigned char *) mags, 6);

      // read the temperature data. Its a right justified 12 bit two's compliment number

      acc_read_register(TEMP_OUT_L, (unsigned char *) &temp, 2);

      //display_clear;
             //x acceleration
      display_clear();
            int l, d;
            //y acceleration
            if(accels[1]>0){
              for (l=32; l<(33+accels[1]*16/16000.0); l++){
                for(d=-2;d<3;d++){
                    display_pixel_set(l,64+d,1);
                }
              }
            }
            else{
              for (l=32; l>(33+accels[1]*16/16000.0); l--){
                for(d=-2;d<3;d++){
                    display_pixel_set(l,64+d,1);
                }
              }

            }
            //x acceleration
            if(accels[0]>0){
              for (l=64;l<65+accels[0]*32/16000.0;l++){
                for (d=-2;d<3;d++){
                    display_pixel_set(32+d,l,1);
                }
              }
            }
            else{
              for (l=64;l>65+accels[0]*32/16000.0;l--){
                for (d=-2;d<3;d++){
                    display_pixel_set(32+d,l,1);
                }
              }

            }
//      sprintf(message,"%3.2f, %3.2f, %3.2f",accels[0]/16000.0,accels[1]/16000.0,accels[2]/16000.0);
//      display_message_i(5,5, message);
      display_draw();

                
           if(!PORTBbits.RB13) {LATBbits.LATB7=1;display_clear();}
           else LATBbits.LATB7 = 0;

   }

        // <editor-fold defaultstate="collapsed" desc="LED code">
 //        __builtin_disable_interrupts();

}
// <editor-fold defaultstate="collapsed" desc="readADC function">
int readADC(void) {
    int elapsed = 0;
    int finishtime = 0;
    int sampletime = 20;
    int a = 0;

    AD1CON1bits.SAMP = 1;
    elapsed = _CP0_GET_COUNT();
    finishtime = elapsed + sampletime;
    while (_CP0_GET_COUNT() < finishtime) {
    }
    AD1CON1bits.SAMP = 0;
    while (!AD1CON1bits.DONE) {
    }
    a = ADC1BUF0;
    return a;
   }
// </editor-fold>

void display_message_i(int rowstart, int column, char *message){
    int i=0,cstart,rstart,m,pix,col,row;
    rstart = rowstart;
    cstart = column;
    while(message[i]){
            int charac = message[i]-0x20;
            cstart = cstart + 5;
            if(cstart+5>=128){
                rstart=rstart+8;
                cstart=column;
            }
        for(col=0; col<5; col++)
        {
            for(row=0; row<8; row++)
            {
             m=ASCII[charac][col];
             pix = (m&(1<<row))>>row;
            display_pixel_set(rstart+row,cstart+col,pix);
            }
        }
        i++;
        }
}