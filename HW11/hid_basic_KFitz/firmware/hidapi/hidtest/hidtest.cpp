
#include <stdio.h>
#include <stdlib.h>
#include "hidapi.h"

#define MAX_STR 255
#define DATAPNTS 1000

int main(int argc, char* argv[])
{
	int res=1;
	unsigned char buf[65];
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	int i;
        
        char message[25];

	// Initialize the hidapi library
	res = hid_init();

	// Open the device using the VID, PID,
	// and optionally the Serial number.
	handle = hid_open(0x4d8, 0x3f, NULL);

	// Read the Manufacturer String
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	wprintf(L"Manufacturer String: %s\n", wstr);

	// Read the Product String
	res = hid_get_product_string(handle, wstr, MAX_STR);
	wprintf(L"Product String: %s\n", wstr);

	// Read the Serial Number String
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	wprintf(L"Serial Number String: (%d) %s\n", wstr[0], wstr);

	// Read Indexed String 1
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	wprintf(L"Indexed String 1: %s\n", wstr);

	// Toggle LED (cmd 0x80). The first byte is the report number (0x0).
	buf[0] = 0x0;
	buf[1] = 0x80;
        //Get user input
        wprintf(L"Enter Row(<64): ");
        scanf("%d", &buf[2]);
        wprintf(L"Enter String <25chars: ");
        scanf("%s", message);
        for (i=0; i<25; i++){
          buf[i+3] =message[i];
        }
        // Print out the returned buffer.
//	for (i = 0; i < 10; i++)
//		wprintf(L"buf[%d]: %c\n", i, buf[i]);

	res = hid_write(handle, buf, 65);

	// Request state (cmd 0x81). The first byte is the report number (0x0).
	buf[0] = 0x0;
	buf[1] = 0x81;
	res = hid_write(handle, buf, 65);

        // Read requested state
	res = hid_read(handle, buf, 65);
        short accels[3];
        accels[0] = (buf[2]<<8)|(buf[1]&0xff);
        accels[1] = (buf[4]<<8)|(buf[3]&0xff);
        accels[2] = (buf[6]<<8)|(buf[5]&0xff);
        for(i = 0; i<3;i++)
               wprintf(L"accel[%d]: %d\n",i,accels[i]);

        FILE *ofp;

       ofp = fopen("accels.txt", "w");
       
       short z[DATAPNTS];
       for (i=0; i<DATAPNTS; i++) {
        buf[0] = 0x0;
	buf[1] = 0x81;
	res = hid_write(handle, buf, 65);
        res = hid_read(handle, buf, 65);
        z[i] = (buf[2]<<8)|(buf[1]&0xff);
        

       fprintf(ofp,"%f\n",(z[i]/16000.0));

       }

fclose(ofp);
	// Print out the returned buffer.
//	for (i = 0; i < 4; i++)
//		wprintf(L"buf[%d]: %d\n", i, buf[i]);
        // Finalize the hidapi library

        
	res = hid_exit();

	return 0;
}

