#include <Arduino.h>
#include "SStepper.h"

#define PRESCALER        8
#define STEPS_REV        1600
#define CLOCKWISE        LOW             // Motor with external/integrated driver 
#define ENABLED          HIGH            // Motor with external/integrated driver

#define STEP_PIN         10              // (PUL_PIN) send peaks to driver, runs motor 
#define DIR_PIN          11              // controls direction of motor  
#define ENABLE_PIN       12              // enables or disables motor

#define INPUT_ENABLED    4              // enabled/disabled "button"  
#define INPUT_DIRECTION  5              // direction "button"
#define INPUT_RPM        A1             // potentiometer

uint8_t  _enabled = HIGH;
uint8_t  _dir     = HIGH;    
int      _rpm     = 0; 

SStepper stepper(DIR_PIN, STEP_PIN, ENABLE_PIN, ENABLED, CLOCKWISE);

void setup() {
  Serial.begin(115200);
  stepper.setStatusEnabled(false);                        // disable motor
  _enabled = HIGH;                                        // if mottor disabled, button not pressed
  stepper.setDirectionClockWise(true);                    // direction clockwise
  _dir     = HIGH;                                        // if direction clockwise, button not pressed
  stepper.setupTimer1Interupt(PRESCALER, STEPS_REV, 60);  // sets the timer1 settings, and defalut RPM is 60

	pinMode(INPUT_ENABLED, INPUT_PULLUP);                   
	pinMode(INPUT_DIRECTION, INPUT_PULLUP);

}
// timer interupt, when timer reaches desired(compare register) value, this method is triggerd, 
// counter registar is reset, and starts again to count
ISR(TIMER1_COMPA_vect) {   
	stepper.step();
}

void loop() {
  int rpm = analogRead(INPUT_RPM);
  rpm = map(rpm, 0, 1023, 2, 400);                // rpm range (2 - 400)
  if (abs(_rpm - rpm) >= 3) {                      // if rpm has changed
    stepper.setRpm(PRESCALER, STEPS_REV, rpm);    // set new rpm
    _rpm = rpm;                                   // save new value
  }

	uint8_t enabled = digitalRead(INPUT_ENABLED);
	if (enabled != _enabled) {                      // if state has changed
		if (enabled == LOW) {                         // button pressed
      stepper.setStatusEnabled(true);             // enable motor
			_enabled = enabled;                         // save new state
    }
		else {			                                  // button not pressed
      stepper.setStatusEnabled(false);            // disable motor
			_enabled = enabled;                         // save new state
		}
	}
	
	uint8_t dir = digitalRead(INPUT_DIRECTION);
	if (dir != _dir) {                              // if direction has changed
		if (dir == LOW) {                             // button pressed
      stepper.setDirectionClockWise(false);        // direction clockwise
			_dir = dir;                                 // save new direction
    } else {                                      // button not pressed
      stepper.setDirectionClockWise(true);       // direction counter clockwise
			_dir = dir;                                 // save new direction
		}   
	}
}
