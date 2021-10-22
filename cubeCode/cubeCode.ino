#include "ModbusRtu.h"
#define BAUD_RATE  19200
#define MODBUSBUFSIZE  11
#define notEnabledPin 12
#define m0Pin         11
#define m1Pin         10
#define m2Pin          9
#define notResetPin    8
#define notSleepPin    7
#define stepPin        6
#define dirPin         5
#define notFaultPin    4

#define commLEDPin    13
#define redLEDPin     21
#define greenLEDPin   22
#define yellowLEDPin  23

/**
 *  Modbus object declaration
 *  u8id : node id = 0 for master, = 1..247 for slave
 *  port : serial port
 *  u8txenpin : 0 for RS-232 and USB-FTDI 
 *               or any pin number > 1 for RS-485
 */
Modbus slave(1,Serial1,0); // this is slave @1 and RS-232 or USB-FTDI

union ModbusUnion
{
  struct
  {
    uint16_t initCube;
    uint16_t moveMotor;
    uint16_t resetStepReading;
    uint16_t noStepsToTurn;
    uint16_t stepForward;
    uint16_t stepDelayuS;
    uint16_t stayAwakeAfterRotation;
    uint16_t resolution;
    uint16_t delayAfterRotationmS;
    uint16_t stepLocation;
    uint16_t stepLocationPositive;
  };
  uint16_t modbusBuffer[MODBUSBUFSIZE];
} mb;
int stepLocation = 0;
boolean commLED = false;
uint16_t msgCnt = 0;

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

  mb.initCube               = 1;
  mb.moveMotor              = 0;
  mb.resetStepReading       = 0;
  mb.noStepsToTurn          = 0;
  mb.stepForward            = 1;
  mb.stepDelayuS            = 10000;
  mb.stayAwakeAfterRotation = 0;
  mb.resolution             = 0;
  mb.delayAfterRotationmS   = 100;
  mb.stepLocation           = 0;
  mb.stepLocationPositive   = 1;
  
  wakeup(0);
  setResolution(mb.resolution);

//  Serial.begin(9600);
  Serial1.begin(BAUD_RATE);
  slave.start();
  delay(1000);
//  for (int ii = 0; ii < 10; ++ii) Serial.println(mb.modbusBuffer[ii]);     
}

void loop()
{
  slave.poll( mb.modbusBuffer, MODBUSBUFSIZE );
  checkComm();

  setResolution(mb.resolution);
  if (mb.resetStepReading > 0)
  {
    stepLocation = 0;
    mb.stepLocation = 0;
    mb.stepLocationPositive = 1;
    mb.resetStepReading = 0;
  }
  wakeup(mb.stayAwakeAfterRotation);

  if (mb.moveMotor > 0)
  {
    wakeup(1);
    rotate(mb.noStepsToTurn, mb.stepDelayuS, mb.stepForward);
    delay(mb.delayAfterRotationmS);
    wakeup(mb.stayAwakeAfterRotation);
    mb.moveMotor = 0;
  }

  delay(50);
}
void checkComm()
{
  uint16_t numMessages;
  numMessages = slave.getInCnt();
  if (numMessages != msgCnt)
  {
    msgCnt = numMessages;
    commLED = !commLED;
    digitalWrite(commLEDPin, commLED);    
  }
  
}
void incrementStepCounter()
{
    if (mb.stepForward > 0 )
    {
      stepLocation = stepLocation + 1;
    }
    else
    {
      stepLocation = stepLocation - 1;
    }
    if (stepLocation >= 0)
    {
      mb.stepLocation = (uint16_t) stepLocation;
      mb.stepLocationPositive = 1;
    }
    else
    {
      mb.stepLocation = (uint16_t) -stepLocation;
      mb.stepLocationPositive = 0;
    }
}
void wakeup(uint16_t wake)
{
  digitalWrite(redLEDPin,   wake);    
  digitalWrite(notSleepPin, wake);  
  delay(10);  
}
void rotate(uint16_t steps, uint16_t stepDelayuS, uint16_t stepForward)
{
  digitalWrite(greenLEDPin,   HIGH);    
  digitalWrite(yellowLEDPin,   stepForward);    
  digitalWrite(dirPin, stepForward);
  for (uint16_t i = 0; i < steps; i++) 
  {
    if (mb.moveMotor > 0)
    {
      // These four lines result in 1 step:
      digitalWrite(stepPin, HIGH);
      delayMicroseconds((int)stepDelayuS);
      digitalWrite(stepPin, LOW);
      delayMicroseconds((int) stepDelayuS);
      incrementStepCounter();
      slave.poll( mb.modbusBuffer, MODBUSBUFSIZE );
      checkComm();
    }
    else
    {
      i = steps;
      delay(mb.delayAfterRotationmS);
      wakeup(mb.stayAwakeAfterRotation);
    }
  }
  digitalWrite(greenLEDPin,   LOW);    
  digitalWrite(yellowLEDPin,   LOW);    
}
uint16_t setResolution(uint16_t ires)
{
  uint16_t iresSet = ires;
  uint16_t iresSetDec = 1;
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
