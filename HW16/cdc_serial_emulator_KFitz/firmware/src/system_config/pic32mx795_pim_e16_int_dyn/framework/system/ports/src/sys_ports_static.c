
#include "system_config.h"
#include "peripheral/ports/plib_ports.h"


void SYS_PORTS_Initialize(void)
{
    /* AN and CN Pins Initialization */
    PLIB_PORTS_AnPinsModeSelect(PORTS_ID_0, SYS_PORT_AD1PCFG, PORTS_PIN_MODE_DIGITAL);
    PLIB_PORTS_CnPinsPullUpEnable(PORTS_ID_0, SYS_PORT_CNPUE);
    PLIB_PORTS_CnPinsEnable(PORTS_ID_0, SYS_PORT_CNEN);
    PLIB_PORTS_ChangeNoticeEnable(PORTS_ID_0);
    
    /* PORT A Initialization */
    PLIB_PORTS_DirectionOutputSet( PORTS_ID_0, PORT_CHANNEL_A,  SYS_PORT_A_TRIS ^ 0xFFFF);
    PLIB_PORTS_OpenDrainEnable(PORTS_ID_0, PORT_CHANNEL_A, SYS_PORT_A_ODC);
  // <editor-fold defaultstate="collapsed" desc="SETUP">
    // set up USER pin as input
    ANSELBbits.ANSB13 = 0;
    TRISBbits.TRISB13 = 1;
    // set up LED1 pin as a digital output
    TRISBbits.TRISB7 = 0;
    LATBbits.LATB7 = 0;
    // set up LED2 as digital output
    ANSELBbits.ANSB15 = 0;
    TRISBbits.TRISB15 = 0;
    LATBbits.LATB15 = 0;

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
     OC1RS = 0;            // set duty cycle = OC1RS/(PR2+1) = 50%
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
     OC3RS = 0;            // set duty cycle = OC1RS/(PR2+1) = 50%
     OC3R = 1000;             // initialize before turning OC1 on; afterward it is read-only
     OC3CONbits.ON = 1;       // turn on OC3
     //********End PWM Set up*******
     //           // </editor-fold>
    
}


/*******************************************************************************
 End of File
*/
