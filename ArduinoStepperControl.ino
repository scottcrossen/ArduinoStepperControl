#include <avr/interrupt.h>
#define TIMER0_MIDDLE 0xAF
#define IN_START 2
#define IN_DIRECTION 13
#define OUT_WIRE_1 7
#define OUT_WIRE_2 6
#define OUT_WIRE_3 5
#define OUT_WIRE_4 4
#define WIRE_1_STEP_1 HIGH
#define WIRE_1_STEP_2 LOW
#define WIRE_1_STEP_3 LOW
#define WIRE_1_STEP_4 HIGH
#define WIRE_2_STEP_1 LOW
#define WIRE_2_STEP_2 HIGH
#define WIRE_2_STEP_3 HIGH
#define WIRE_2_STEP_4 LOW
#define WIRE_3_STEP_1 HIGH
#define WIRE_3_STEP_2 HIGH
#define WIRE_3_STEP_3 LOW
#define WIRE_3_STEP_4 LOW
#define WIRE_4_STEP_1 LOW
#define WIRE_4_STEP_2 LOW
#define WIRE_4_STEP_3 HIGH
#define WIRE_4_STEP_4 HIGH

enum motorControl_st_t {
  init_st,
  wait_st,
  step_1_st,
  step_2_st,
  step_3_st,
  step_4_st,
} motorControlState = init_st;

uint16_t stepCounter = 0;
bool directionUp = true;

void motorControl_tick() {
  // state actions switch
  switch(motorControlState) {
    case init_st:
      break;
    case wait_st:
      break;
    case step_1_st:
      digitalWrite(OUT_WIRE_1, WIRE_1_STEP_1);
      digitalWrite(OUT_WIRE_2, WIRE_2_STEP_1);
      digitalWrite(OUT_WIRE_3, WIRE_3_STEP_1);
      digitalWrite(OUT_WIRE_4, WIRE_4_STEP_1);
      break;
    case step_2_st:
      digitalWrite(OUT_WIRE_1, WIRE_1_STEP_2);
      digitalWrite(OUT_WIRE_2, WIRE_2_STEP_2);
      digitalWrite(OUT_WIRE_3, WIRE_3_STEP_2);
      digitalWrite(OUT_WIRE_4, WIRE_4_STEP_2);
      break;
    case step_3_st:
      digitalWrite(OUT_WIRE_1, WIRE_1_STEP_3);
      digitalWrite(OUT_WIRE_2, WIRE_2_STEP_3);
      digitalWrite(OUT_WIRE_3, WIRE_3_STEP_3);
      digitalWrite(OUT_WIRE_4, WIRE_4_STEP_3);
      break;
    case step_4_st:
      digitalWrite(OUT_WIRE_1, WIRE_1_STEP_4);
      digitalWrite(OUT_WIRE_2, WIRE_2_STEP_4);
      digitalWrite(OUT_WIRE_3, WIRE_3_STEP_4);
      digitalWrite(OUT_WIRE_4, WIRE_4_STEP_4);
      break;
    default:
      //TODO: handle error from unreachable state
      break;
  }
  
  // state transitions switch
  switch(motorControlState) {
    case init_st:
      motorControlState=wait_st;
      break;
    case wait_st:
      if (digitalRead(IN_START) == HIGH) {
        if (digitalRead(IN_DIRECTION) == LOW) {
          directionUp=true;
          motorControlState=step_1_st;
        } else if (stepCounter > 0) {
          directionUp=false;
          motorControlState=step_4_st;
        }
      }
      break;
    case step_1_st:
      if (directionUp) {
        motorControlState=step_2_st;
      } else {
        motorControlState=wait_st;
        stepCounter--;
      }
      break;
    case step_2_st:
      if (directionUp) {
        motorControlState=step_3_st;
      } else {
        motorControlState=step_1_st;
      }
      break;
    case step_3_st:
      if (directionUp) {
        motorControlState=step_4_st;
      } else {
        motorControlState=step_2_st;
      }
      break;
    case step_4_st:
      if (directionUp) {
        motorControlState=wait_st;
        stepCounter++;
      } else {
        motorControlState=step_3_st;
      }
      break;
    default:
      //TODO: handle error from unreachable state
      break;
  }
}

void setup() {
  pinMode(OUT_WIRE_1, OUTPUT);
  pinMode(OUT_WIRE_2, OUTPUT);
  pinMode(OUT_WIRE_3, OUTPUT);
  pinMode(OUT_WIRE_4, OUTPUT);
  pinMode(IN_START, INPUT);
  pinMode(IN_DIRECTION, INPUT);
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

