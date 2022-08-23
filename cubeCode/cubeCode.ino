#include "BlinkyBus.h"
#define BAUD_RATE  19200
#define commLEDPin    13
#define BLINKYBUSBUFSIZE  10

#define notEnabledPin 12
#define m0Pin         11
#define m1Pin         10
#define m2Pin          9
#define notResetPin    8
#define notSleepPin    7
#define stepPin        6
#define dirPin         5
#define notFaultPin    4

#define redLEDPin     21
#define greenLEDPin   22
#define yellowLEDPin  23

union BlinkyBusUnion
{
  struct
  {
    int16_t state;
    int16_t moveMotor;
    int16_t resetStepReading;
    int16_t noStepsToTurn;
    int16_t stepForward;
    int16_t stepDelayuS;
    int16_t stayAwakeAfterRotation;
    int16_t resolution;
    int16_t delayAfterRotationmS;
    int16_t stepLocation;
  };
  int16_t buffer[BLINKYBUSBUFSIZE];
} bb;
BlinkyBus blinkyBus(bb.buffer, BLINKYBUSBUFSIZE, Serial1, commLEDPin);

int stepLocation = 0;

void setup()
{
  pinMode(notEnabledPin, OUTPUT);
  pinMode(m0Pin,         OUTPUT);
  pinMode(m1Pin,         OUTPUT);
  pinMode(m2Pin,         OUTPUT);
  pinMode(notResetPin,   OUTPUT);
  pinMode(notSleepPin,   OUTPUT);
  pinMode(stepPin,       OUTPUT);
  pinMode(dirPin,        OUTPUT);
  pinMode(notFaultPin,   INPUT);
  pinMode(commLEDPin,    OUTPUT);
  pinMode(redLEDPin,     OUTPUT);
  pinMode(greenLEDPin,   OUTPUT);
  pinMode(yellowLEDPin,  OUTPUT);
 
  digitalWrite(notEnabledPin, LOW);    
  digitalWrite(m0Pin,         LOW);    
  digitalWrite(m1Pin,         LOW);    
  digitalWrite(m2Pin,         LOW);    
  digitalWrite(notResetPin,  HIGH);    
  digitalWrite(notSleepPin,   LOW);    
  digitalWrite(commLEDPin,    LOW);    
  digitalWrite(redLEDPin,     LOW);    
  digitalWrite(greenLEDPin,   LOW);    
  digitalWrite(yellowLEDPin,  LOW);    

  bb.state               = 1;
  bb.moveMotor              = 0;
  bb.resetStepReading       = 0;
  bb.noStepsToTurn          = 0;
  bb.stepForward            = 1;
  bb.stepDelayuS            = 10000;
  bb.stayAwakeAfterRotation = 0;
  bb.resolution             = 0;
  bb.delayAfterRotationmS   = 100;
  bb.stepLocation           = 0;
  
  wakeup(0);
  setResolution(bb.resolution);

  Serial1.begin(BAUD_RATE);
  blinkyBus.start();
  delay(1000);
}

void loop()
{
  if (blinkyBus.poll() == 2)
  {
    if (blinkyBus.getLastWriteAddress() == 9)
    {
      stepLocation = (int) blinkyBus.getLastWriteValue();
    }
  }

  setResolution(bb.resolution);
  if (bb.resetStepReading > 0)
  {
    stepLocation = 0;
    bb.stepLocation = 0;
    bb.resetStepReading = 0;
  }
  wakeup(bb.stayAwakeAfterRotation);

  if (bb.moveMotor > 0)
  {
    wakeup(1);
    rotate(bb.noStepsToTurn, bb.stepDelayuS, bb.stepForward);
    delay(bb.delayAfterRotationmS);
    wakeup(bb.stayAwakeAfterRotation);
    bb.moveMotor = 0;
  }

  delay(50);
}
void incrementStepCounter()
{
    if (bb.stepForward > 0 )
    {
      stepLocation = stepLocation + 1;
    }
    else
    {
      stepLocation = stepLocation - 1;
    }
    bb.stepLocation = (int16_t) stepLocation;
}
void wakeup(int16_t wake)
{
  digitalWrite(redLEDPin,   wake);    
  digitalWrite(notSleepPin, wake);  
  delay(10);  
}
void rotate(int16_t steps, int16_t stepDelayuS, int16_t stepForward)
{
  digitalWrite(greenLEDPin,   HIGH);    
  digitalWrite(yellowLEDPin,   stepForward);    
  digitalWrite(dirPin, stepForward);
  for (int16_t i = 0; i < steps; i++) 
  {
    if (bb.moveMotor > 0)
    {
      // These four lines result in 1 step:
      digitalWrite(stepPin, HIGH);
      delayMicroseconds((int)stepDelayuS);
      digitalWrite(stepPin, LOW);
      delayMicroseconds((int) stepDelayuS);
      incrementStepCounter();
      blinkyBus.poll();
    }
    else
    {
      i = steps;
      delay(bb.delayAfterRotationmS);
      wakeup(bb.stayAwakeAfterRotation);
    }
  }
  digitalWrite(greenLEDPin,   LOW);    
  digitalWrite(yellowLEDPin,   LOW);    
}
int16_t setResolution(int16_t ires)
{
  int16_t iresSet = ires;
  int16_t iresSetDec = 1;
  if (ires < 0) iresSet = 0;
  if (ires > 5) iresSet = 5;
  switch (iresSet) 
  {
    case 0:
        digitalWrite(m0Pin,         LOW);    
        digitalWrite(m1Pin,         LOW);    
        digitalWrite(m2Pin,         LOW);    
      break;
    case 1:
        digitalWrite(m0Pin,        HIGH);    
        digitalWrite(m1Pin,         LOW);    
        digitalWrite(m2Pin,         LOW);    
      break;
    case 2:
        digitalWrite(m0Pin,         LOW);    
        digitalWrite(m1Pin,        HIGH);    
        digitalWrite(m2Pin,         LOW);    
      break;
    case 3:
        digitalWrite(m0Pin,        HIGH);    
        digitalWrite(m1Pin,        HIGH);    
        digitalWrite(m2Pin,         LOW);    
      break;
    case 4:
        digitalWrite(m0Pin,         LOW);    
        digitalWrite(m1Pin,         LOW);    
        digitalWrite(m2Pin,        HIGH);    
      break;
    case 5:
        digitalWrite(m0Pin,        HIGH);    
        digitalWrite(m1Pin,         LOW);    
        digitalWrite(m2Pin,        HIGH);    
      break;
    default:
        digitalWrite(m0Pin,         LOW);    
        digitalWrite(m1Pin,         LOW);    
        digitalWrite(m2Pin,         LOW);    
        iresSet = 0;
      break;
  }
  for (int i = 0; i < iresSet; i++) 
  {
    iresSetDec = iresSetDec * 2;
  }  
  return iresSetDec;
}
