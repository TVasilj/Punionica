#include <Arduino.h>
#include "SStepper.h"

// constructor
SStepper::SStepper(uint8_t dirPin,
                   uint8_t stepPin,
                   uint8_t enablePin,
                   uint8_t enabledState,
                   uint8_t clockWiseDirection) {
  _dirPin             = dirPin;
  _stepPin            = stepPin;
  _enablePin          = enablePin;
  _enabledState       = enabledState;
  _clockWiseDirection = clockWiseDirection; 
  _status             = false;

  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
}

// destructor
SStepper::~SStepper(){
}

void SStepper::setDirectionClockWise(bool dir) {
  cli();                                  //stop interrupts
  if (dir) {                              // true - direction clockwise
    if (_clockWiseDirection == LOW) {     // if clockwise direction is LOW vale on output
      digitalWrite(_dirPin, LOW);         // set direction pin to LOW
    } else {                              // if clockwise direction is HIGH vale on output
      digitalWrite(_dirPin, HIGH);        // set direction pin to HIGH
    }
  } else {                                // false - direction counterclockwise
    if (_clockWiseDirection == LOW) {     // if clockwise direction is LOW vale on output
      digitalWrite(_dirPin, HIGH);        // set direction pin to HIGH
    } else {                              // if clockwise direction is LOW vale on output
      digitalWrite(_dirPin, LOW);         // set direction pin to LOW
    }
  }
  if (_status) {                          // if motor enabled, trigger step
    sei();                                // allow interrupts
  }
}

void SStepper::setStatusEnabled(bool status) {
  cli();                                  //stop interrupts
  TCNT1 = 0;                              // initialize counter value to 0
                         
  if (status) {                           // true - motor enabled
    if (_enabledState == LOW) {           // if motor enabled is LOW vale on output
      digitalWrite(_enablePin, LOW);      // set enable pin to LOW
    } else {                              // if motor enabled is HIGH vale on output
      digitalWrite(_enablePin, HIGH);     // set enable pin to HIGH
    }

    sei();			                          // allow interrupts					                
  } else {                                // false - motor disabled                 
    if (_enabledState == LOW) {           // if motor enabled is LOW vale on output
      digitalWrite(_enablePin, HIGH);     // set enable pin to HIGH
    } else {                              // if motor enabled is HIGH vale on output
      digitalWrite(_enablePin, LOW);      // set enable pin to LOW
    }
  }
  _status = status;                       // save new status
}

void SStepper::step() {
  if (_status) {                          // if motor enabled, trigger step
    digitalWrite(_stepPin, HIGH);
    digitalWrite(_stepPin, LOW);
  }
}

void SStepper::setupTimer1Interupt(int64_t prescaler, int64_t steps_rev, int64_t rpm) {
  //stop interrupts  
  cli();
  // set entire TCCR1A register to 0
  TCCR1A = 0;
  // same for TCCR1B
  TCCR1B = 0;
  //initialize counter value to 0
  TCNT1 = 0;
  // sets comapre registar to desired value for received rpm
  /* (16000000.0 / prescaler)   -> sets timer1 frequency (16MHz / prescaler)         // timer1 frequnecy
   * (steps_rev * (rpm / 60.0)) -> steps_per_revolution * revolutions_per_second     // steps frequnecy
   * - 1 -> is because timer counter is zerobased
   */
  OCR1A = (uint32_t)((16000000.0 / prescaler) / (steps_rev * (rpm / 60.0))) - 1;
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);

  switch (prescaler)
  {
  case 0:                                      // no prescaler
    TCCR1B |= (1 << CS10);
    break;
  case 8:                                      // prescaler 8
    TCCR1B |= (1 << CS11);
    break;
  case 64:                                     // prescaler 64
    TCCR1B |= (1 << CS11) | (1 << CS10);
    break;
  case 256:                                    // prescaler 256
    TCCR1B |= (1 << CS12);
    break;
  case 1024:                                   // prescaler 1024
    TCCR1B |= (1 << CS12) | (1 << CS10);
    break;
  default:
    break;
  }

  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  //allow interrupts
  sei();
}

void SStepper::setRpm(int64_t prescaler, int64_t steps_rev, int64_t rpm) {
  //initialize counter value to 0                                               
  TCNT1 = 0;
  // sets comapre registar to desired value for received rpm                                         
  OCR1A = (uint32_t)((16000000.0 / prescaler) / (steps_rev * (rpm / 60.0))) - 1;
}
