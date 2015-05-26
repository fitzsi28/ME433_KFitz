#include<xc.h> // processor SFR definitions
#include<sys/attribs.h> // __ISR macro

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

int readADC(void);

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
    // set up LED2 as OC1 using Timer2 at 1kHz
    ANSELBbits.ANSB15 = 0;
    RPB15Rbits.RPB15R = 0b0101;
    //********PWM set up*********
    T2CONbits.TCKPS = 0b100;     // Timer3 prescaler N=8 (1:1)
    PR2 = 2499;              // set period register-20kHz
    TMR2 = 0;                // initialize count to 0
    OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
    OC1CONbits.OCTSEL=0;     // Timer select bit for output compare 1
    OC1RS = 1000;            // set duty cycle = OC1RS/(PR2+1) = 50%
    OC1R = 1000;             // initialize before turning OC1 on; afterward it is read-only
    T2CONbits.ON = 1;        // turn on Timer3
    OC1CONbits.ON = 1;       // turn on OC1
//********End PWM Set up*******
    // set up A0 as AN0
    ANSELAbits.ANSA0 = 1;
    AD1CON3bits.ADCS = 3;
    AD1CHSbits.CH0SA = 0;
    AD1CON1bits.ADON = 1;
    //set up RA4 pin as a digital output to power OLED
    RPA4Rbits.RPA4R = 0b000;
    TRISAbits.TRISA4 = 0;
    LATAbits.LATA4 = 0;
 __builtin_enable_interrupts();
//   _CP0_SET_COUNT(0);
//        LATAbits.LATA4 = 1;
//        while(_CP0_GET_COUNT()<2000000){/*wait 0.1sec*/}
//           // </editor-fold>

    int val;

    while (1) {
    _CP0_SET_COUNT(0); // set core timer to 0, remember it counts at half the CPU clock
    LATBINV = 0x80; // invert  pin B7

   // wait for half a second, setting LED brightness to pot angle while waiting
    while (_CP0_GET_COUNT() < 10000000) {
        val = readADC();
        OC1RS = val * (PR2+1)/1023;
        if (PORTBbits.RB13 == 1) {
        // LATBbits.LATB7 = 0;
     } else {
         LATBINV = 0x80;
     }
    }
    
    
}
}

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