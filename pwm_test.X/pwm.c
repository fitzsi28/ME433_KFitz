#include<xc.h> // processor SFR definitions
#include<sys/attribs.h> // __ISR macro

#define LED1 LATBbits.LATB7
#define LED2 LATBbits.LATB15
#define RPWM OC1RS
#define LPWM OC3RS
#define RDIR LATAbits.LATA4
#define LDIR LATBbits.LATB14
#define RFOR 1
#define RREV 0
#define LFOR 0
#define LREV 1

// <editor-fold defaultstate="collapsed" desc="DEVCFG">
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
#pragma config OSCIOFNC = OFF // free up secondary osc pins?????
#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // slowest wdt- PS1
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN = OFF // wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 40MHz
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_20 // multiply clock after FPLLIDIV
#pragma config UPLLIDIV = DIV_2 // divide clock after FPLLMUL
#pragma config UPLLEN = ON // USB clock on
#pragma config FPLLODIV = DIV_2 // divide clock by 2 to output on pin

// DEVCFG3
#pragma config USERID = 0x0038 // some 16bit userid
#pragma config PMDL1WAY = ON // not multiple reconfiguration, check this
#pragma config IOL1WAY = ON // not multimple reconfiguration, check this
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // controlled by USB module
//           // </editor-fold>


int main() {
 // <editor-fold defaultstate="collapsed" desc="SETUP">
/* ***********************SETUP here ********************** */

    // startup
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
    //LATBbits.LATB7 = 1;
    // set up LED2 as digital output
    ANSELBbits.ANSB15 = 0;
    TRISBbits.TRISB15 = 0;
    //LATBbits.LATB15 = 0;

   //set up A4 as PH A
     TRISAbits.TRISA4 = 0;
     LATAbits.LATA4 = 1;

   //set up B4 as EN A/OC1
     RPB4Rbits.RPB4R = 0b0101;
     //********PWM set up*********
     T2CONbits.TCKPS = 0b00;     // Timer2 prescaler N=1
     PR2 = 1999;              // set period register-20kHz
     TMR2 = 0;                // initialize count to 0
     OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
     OC1CONbits.OCTSEL=0;     // Timer select bit for output compare 1
     OC1RS = 1000;            // set duty cycle = OC1RS/(PR2+1) = 50%
     OC1R = 1000;             // initialize before turning OC1 on; afterward it is read-only
     T2CONbits.ON = 1;        // turn on Timer3
     OC1CONbits.ON = 1;       // turn on OC1
     //********End PWM Set up*******

   //set B14 as PH B
     ANSELBbits.ANSB14 =0;
     TRISBbits.TRISB14 = 0;
     LATBbits.LATB14 = 1;

     //SET up B9 as EN B/OC3
       RPB9Rbits.RPB9R = 0b0101;
     //********PWM set up*********
     OC3CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
     OC3CONbits.OCTSEL=0;     // Timer select bit for output compare 1
     OC3RS = 1000;            // set duty cycle = OC1RS/(PR2+1) = 50%
     OC3R = 1000;             // initialize before turning OC1 on; afterward it is read-only
     OC3CONbits.ON = 1;       // turn on OC3
     //********End PWM Set up*******
 __builtin_enable_interrupts();
//           // </editor-fold>

    int val;

    
    _CP0_SET_COUNT(0); // set core timer to 0, remember it counts at half the CPU clock
    LATBINV = 0x80; // invert  pin B7

   // wait for 1 second, forward at half speed
    while (_CP0_GET_COUNT() < 80000000) {
        LED2 = 1;
        RDIR = RFOR;
        RPWM = 50 * (PR2+1)/100;
        LDIR = LFOR;
        LPWM = 45 * (PR2+1)/100;
     }
   _CP0_SET_COUNT(0);
   while (_CP0_GET_COUNT() < 80000000) { //forward at full speed
        LED2 = 1;
        RDIR = RFOR;
        RPWM = 100* (PR2+1)/100;
        LDIR = LFOR;
        OC3RS = 95 * (PR2+1)/100;
     }
   _CP0_SET_COUNT(0);
   while (_CP0_GET_COUNT() < 80000000) { //backward at half speed
        LED2 = 0;
        LED1 = 1;
        RDIR = RREV;
        RPWM = 50 * (PR2+1)/100;
        LDIR = LREV;
        LPWM = 38 * (PR2+1)/100;
     }
   _CP0_SET_COUNT(0);
   while (_CP0_GET_COUNT() < 80000000) { //turn right
        LED2 = 1;
        LED1 = 1;
        RDIR = RFOR;
        RPWM = 0 * (PR2+1)/100;
        LDIR = LFOR;
        LPWM = 50 * (PR2+1)/100; //left
     }
   _CP0_SET_COUNT(0);
   while (_CP0_GET_COUNT() < 80000000) { //turn left
        LED2 = 1;
        LED1 = 1;
        RDIR = RFOR;
        RPWM = 50 * (PR2+1)/100;//right
        LDIR = LFOR;
        LPWM = 0 * (PR2+1)/100;
     }
   _CP0_SET_COUNT(0);
   while (_CP0_GET_COUNT() < 80000000) { //turn clockwise
        LED2 = 1;
        LED1 = 1;
        RDIR = RFOR;
        RPWM = 50 * (PR2+1)/100;//right
        LDIR = LREV;
        LPWM = 50 * (PR2+1)/100;
     }
   _CP0_SET_COUNT(0);
   while (_CP0_GET_COUNT() < 80000000) { //turn counterclockwise
        LED2 = 1;
        LED1 = 1;
        RDIR = RREV;
        RPWM = 50 * (PR2+1)/100;//right
        LDIR = LFOR;
        LPWM = 50 * (PR2+1)/100;
     }
   RPWM = 0;
   LPWM = 0;

}

