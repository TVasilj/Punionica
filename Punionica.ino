#include <Arduino.h>
#include "SStepper.h"

#define PRESCALER        8
#define STEPS_REV        1600
#define CLOCKWISE        HIGH           // Motor with external driver 
#define ENABLED          LOW            // Motor with external driver

#define DIR_PIN          3              // controls direction of motor  
#define STEP_PIN         4              // (PUL_PIN) send peaks to driver, runs motor 
#define ENABLE_PIN       5              // enables or disables motor

#define INPUT_ENABLED    6              // enabled/disabled "button"  
#define INPUT_DIRECTION  7              // direction "button"
#define INPUT_RPM        A0             // potentiometer

uint8_t  _enabled = HIGH;
uint8_t  _dir     = HIGH;    
uint32_t _rpm     = 0; 

SStepper stepper(DIR_PIN, STEP_PIN, ENABLE_PIN, ENABLED, CLOCKWISE);

void setup() {
  Serial.begin(115200);
  stepper.setStatusEnabled(false);                        // disable motor
  _enabled = HIGH;                                        // if mottor disabled, button not pressed
  stepper.setDirectionClockWise(true);                    // direction clockwise
  _dir     = LOW;                                         // if direction clockwise, button pressed
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
  uint32_t rpm = analogRead(INPUT_RPM);
  rpm = map(rpm, 0, 1023, 2, 240);                // rpm range (2 - 240)
  if (_rpm != rpm) {                              // if rpm has changed
    Serial.println(rpm);
    stepper.setRpm(PRESCALER, STEPS_REV, rpm);    // set new rpm
    _rpm = rpm;                                   // save new value
  }

	uint8_t enabled = digitalRead(INPUT_ENABLED);
	if (enabled != _enabled) {                      // if state has changed
		if (enabled == LOW) {                         // button pressed
      stepper.setStatusEnabled(true);             // enable motor
			_enabled = enabled;                         // save new state
		Serial.println("Motor enabled");
    }
		else {			                                  // button not pressed
      stepper.setStatusEnabled(false);            // disable motor
			_enabled = enabled;                         // save new state
      Serial.println("Motor disabled");
		}
	}
	
	uint8_t dir = digitalRead(INPUT_DIRECTION);
	if (dir != _dir) {                              // if direction has changed
    stepper.setStatusEnabled(false);              // disable motor

		if (dir == LOW) {                             // button pressed
      stepper.setDirectionClockWise(true);        // direction clockwise
			_dir = dir;                                 // save new direction
      Serial.println("Direction clock wise");
    } else {                                      // button not pressed
      stepper.setDirectionClockWise(false);       // direction counter clockwise
			_dir = dir;                                 // save new direction
      Serial.println("Direction counter clock wise");
		}

    stepper.setStatusEnabled(true);               // enable motor
	}
	
}
