
#include "system_config.h"
#include "peripheral/ports/plib_ports.h"


void SYS_PORTS_Initialize(void)
{
    /* AN and CN Pins Initialization */
    PLIB_PORTS_AnPinsModeSelect(PORTS_ID_0, SYS_PORT_AD1PCFG, PORTS_PIN_MODE_DIGITAL);
    PLIB_PORTS_CnPinsPullUpEnable(PORTS_ID_0, SYS_PORT_CNPUE);
    PLIB_PORTS_CnPinsEnable(PORTS_ID_0, SYS_PORT_CNEN);
    PLIB_PORTS_ChangeNoticeEnable(PORTS_ID_0);
    
    
    /* PORT B Initialization */
    PLIB_PORTS_DirectionOutputSet( PORTS_ID_0, PORT_CHANNEL_B,  0x90);
    LATBbits.LATB4 =0;
    ANSELBbits.ANSB13 = 0;
    PLIB_PORTS_DirectionInputSet( PORTS_ID_0, PORT_CHANNEL_B,  0x2000);
    _CP0_SET_COUNT(0);
     LATBbits.LATB4 = 1;
     while(_CP0_GET_COUNT()<2000000){/*wait 0.1sec*/}
    
}


