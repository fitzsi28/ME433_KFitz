#ifndef POSITION__H__
#define POSITION__H__

void position_init();       // initializes the position module  and the necessary positions

int position_load();        // load a trajectory from the client and return its length

void position_reset();      // reset the current trajectory position

PID_gains position_gains_read(); // read the position gains from the client

void position_gains_send(PID_gains p); // send the position gains to the client

void position_control_reset();   //clear Eint_p to prevent windup

#endif
