#include <avr/interrupt.h>
// a value somewhere in the middle of the timer0 register that doesn't conflict with the system interrupts at the end
#define TIMER0_MIDDLE 0xAF
// the pin to use to trigger a cycle
#define IN_START 2
// the pin to use to pass in direction, low = up, high = down
#define IN_DIRECTION 13
// the pins to use as outputs
#define OUT_WIRE_1 7
#define OUT_WIRE_2 6
#define OUT_WIRE_3 5
#define OUT_WIRE_4 4
// the outputs of the wire in each step
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

// the 'startEdgeDetect' state machine detects when the 'start' pin goes from low to high
enum startEdgeDetect_st_t {
  low_st,
  edge_st,
  high_st,
} startEdgeDetectState = high_st; // start high to avoid initialization errors

// the output of the startEdgeDetect state machine
boolean start_is_edge = false;

// the tick function of the startEdgeDetect state machine
// this is called once every interrupt
void startEdgeDetect_tick() {
  // state actions switch
  switch(startEdgeDetectState) {
    case low_st:
      start_is_edge = false;
      break;
    case edge_st:
      start_is_edge = true;
      break;
    case high_st:
      start_is_edge = false;
      break;
    default:
      start_is_edge = false;
      //TODO: handle error for unreachable state
      break;
  }

  // state transitions switch
  switch(startEdgeDetectState) {
    case low_st:
      if (digitalRead(IN_START) == HIGH) {
        startEdgeDetectState=edge_st;
      }
      break;
    case edge_st:
      startEdgeDetectState=high_st;
      break;
    case high_st:
      if (digitalRead(IN_START) == LOW) {
        startEdgeDetectState=low_st;
      }
      break;
    default:
      startEdgeDetectState=high_st;
      break;
  }
}


// the 'motorControl' state machine controls what step the motor is in
enum motorControl_st_t {
  init_st,
  wait_st,
  step_1_st,
  step_2_st,
  step_3_st,
  step_4_st,
} motorControlState = wait_st;

// the motorControl state machine output
bool directionUp = true;
// the motorControl state machine internal counter (don't use)
uint16_t stepCounter = 0;

// the motorControl state machine tick function.
// this is called once every timer interrupt
void motorControl_tick() {
  // state actions switch
  switch(motorControlState) {
    case wait_st:
      //TODO: should the output wires be set to low?
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
    case wait_st:
      if (start_is_edge) {
        if (digitalRead(IN_DIRECTION) == LOW) {
          directionUp=true;
          motorControlState=step_1_st;
        } else if (stepCounter > 0) {
          directionUp=false;
          motorControlState=step_4_st;
        } else {
          //TODO: handle case where arduino is triggered to go down, but the counter is already at zero.
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
      motorControlState=wait_st;
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
  startEdgeDetect_tick();
}

