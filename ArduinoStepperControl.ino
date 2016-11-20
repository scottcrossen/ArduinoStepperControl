#include <avr/interrupt.h>
#define TIMER0_MIDDLE 0xAF

enum motorControl_st_t {
  init_st,
  wait_st,
  step_up_1_st,
  step_up_2_st,
  step_up_3_st,
  step_up_4_st,
  step_down_1_st,
  step_down_2_st,
  step_down_3_st,
  step_down_4_st,
} motorControlState = init_st;

uint16_t stepCounter = 0;

void motorControl_tick() {
  // state actions switch
  switch(motorControlState) {
    case init_st:
      break;
    case wait_st:
      break;
    case step_up_1_st:
      break;
    case step_up_2_st:
      break;
    case step_up_3_st:
      break;
    case step_up_4_st:
      break;
    case step_down_1_st:
      break;
    case step_down_2_st:
      break;
    case step_down_3_st:
      break;
    case step_down_4_st:
      break;
    default:
      //TODO: handle error from unreachable state
      break;
  }
  
  // state transitions switch
  switch(motorControlState) {
    case init_st:
      break;
    case wait_st:
      break;
    case step_up_1_st:
      motorControlState=step_up_2_st;
      break;
    case step_up_2_st:
      motorControlState=step_up_3_st;
      break;
    case step_up_3_st:
      motorControlState=step_up_4_st;
      break;
    case step_up_4_st:
      motorControlState=wait_st;
      stepCounter++;
      break;
    case step_down_1_st:
      motorControlState=step_down_2_st;
      break;
    case step_down_2_st:
      motorControlState=step_down_3_st;
      break;
    case step_down_3_st:
      motorControlState=step_down_4_st;
      stepCounter--;
      break;
    case step_down_4_st:
      motorControlState=wait_st;
      break;
    default:
      //TODO: handle error from unreachable state
      break;
  }
}

void setup() {
  // configure the hardware to interrupt every millisecond.
  // this will use timer 0 (which has a 1khz) frequency
  OCR0A = TIMER0_MIDDLE; // interrupt in the middle of the timer to avoid conflict with system ticks at end
  TIMSK0 |= _BV(OCIE0A);

}

void loop() {
  // we're too cool to have an infinite busy-waiting loop
  //TODO: put arduino to sleep until timer interrupt
}

//interrupt called by timer 0
SIGNAL(TIMER0_COMPA_vect) {
  motorControl_tick();
}

