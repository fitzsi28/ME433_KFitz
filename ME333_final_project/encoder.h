#ifndef ENCODER__H__
#define ENCODER__H__

void encoder_init();          // initialize the encoder module

int encoder_ticks();          // read the encoder, in ticks

void encoder_reset();         // reset the encoder position

int encoder_angle();          // read the encoder angle, in tenths of a degree

#endif
