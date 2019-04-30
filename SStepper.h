#include "Arduino.h"

class SStepper {
 public:
  //  constructor
  SStepper(uint8_t dirPin, 
           uint8_t stepPin, 
           uint8_t enablePin, 
           uint8_t enabledState, 
           uint8_t clockWiseDirection);

  // destructor
  ~SStepper();

  /* Used for control direction of motor rotetion
   * @param dir; true - rotation clockwise, otherwise counterclocwise
   */
  void setDirectionClockWise(bool dir);
  /* Used for enabling or disabling motor
   * @param status; true - motor enabled, otherwise disabled
   */
  void setStatusEnabled(bool status);

  /* Used for running one step
   */
  void step();

  /* Used for setting up settings for timer1
   * @param prescaler; sets timer 1 clock frequention
   * @param steps_rev; steps needed for one revolution
   * @param rpm;       revolutions per minute
   */
  void setupTimer1Interupt(int64_t prescaler, int64_t steps_rev, int64_t rpm);

  /* Used for setting up rpm
   * @param prescaler; sets timer 1 clock frequention
   * @param steps_rev; steps needed for one revolution
   * @param rpm;       revolutions per minute
   */
  void setRpm(int64_t prescaler, int64_t steps_rev, int64_t rpm);
 
 private:
  uint8_t _dirPin;
  uint8_t _stepPin;
  uint8_t _enablePin;
  uint8_t _enabledState;
  uint8_t _clockWiseDirection;
  bool    _status;
};
