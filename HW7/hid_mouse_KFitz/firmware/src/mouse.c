#include "mouse.h"

void MOUSE_ReportCreate
(
    MOUSE_COORDINATE x,
    MOUSE_COORDINATE y,
    MOUSE_BUTTON_STATE * buttonArray,
    MOUSE_REPORT * mouseReport
)
{
    int index;

    /* Initialize the mouse buttons byte */
    mouseReport->data[0] = 0;

    for (index = 0; index < MOUSE_BUTTON_NUMBERS; index ++)
    {
        /* Create the mouse button bit map */
        mouseReport->data[0] |= buttonArray[index];
    }

    /* Update the x and y co-ordinate */
	mouseReport->data[1] = x;
	mouseReport->data[2] = y;

	return;	
}


