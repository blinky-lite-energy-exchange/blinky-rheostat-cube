/*Example sketch to control a stepper motor with A4988/DRV8825 stepper motor driver and Arduino without a library. More info: https://www.makerguides.com */
// Define stepper motor connections and steps per revolution:
#define notEnabledPin 12
#define m0Pin         11
#define m1Pin         10
#define m2Pin          9
#define notResetPin    8
#define notSleepPin    7
#define stepPin        6
#define dirPin         5
#define notFaultPin    4

#define blueLEDPin    13
#define redLEDPin     21
#define greenLEDPin   22
#define yellowLEDPin  23

#define stepsPerRevolution 200
void setup() 
{
  // Declare pins as output:
  pinMode(notEnabledPin, OUTPUT);
  pinMode(m0Pin,         OUTPUT);
  pinMode(m1Pin,         OUTPUT);
  pinMode(m2Pin,         OUTPUT);
  pinMode(notResetPin,   OUTPUT);
  pinMode(notSleepPin,   OUTPUT);
  pinMode(stepPin,       OUTPUT);
  pinMode(dirPin,        OUTPUT);
  pinMode(notFaultPin,   INPUT);
  pinMode(blueLEDPin,    OUTPUT);
  pinMode(redLEDPin,     OUTPUT);
  pinMode(greenLEDPin,   OUTPUT);
  pinMode(yellowLEDPin,  OUTPUT);
 
  digitalWrite(notEnabledPin, LOW);    
  digitalWrite(m0Pin,         LOW);    
  digitalWrite(m1Pin,         LOW);    
  digitalWrite(m2Pin,         LOW);    
  digitalWrite(notResetPin,  HIGH);    
  digitalWrite(notSleepPin,   LOW);    
  digitalWrite(blueLEDPin,    LOW);    
  digitalWrite(redLEDPin,     LOW);    
  digitalWrite(greenLEDPin,   LOW);    
  digitalWrite(yellowLEDPin,  LOW);    
  
  wakeup(false);    
  blink();
  setResolution(0);
}
void loop() 
{
  wakeup(true);
  rotate(200, 1000);
  blink();
  delay(1000);
  rotate(-200, 1000);
  blink();
  wakeup(false);
  
  delay(10000);
  blink();
}
void wakeup(boolean wake)
{
  digitalWrite(redLEDPin,   wake);    
  digitalWrite(notSleepPin, wake);  
  delay(10);  
}
void rotate(int steps, int stepDelayuS)
{
  boolean direction = true;
  if (steps < 0)
  {
    direction = false;
    steps = -steps;
  }
  digitalWrite(greenLEDPin,   HIGH);    
  digitalWrite(yellowLEDPin,   direction);    
  digitalWrite(dirPin, direction);
  for (int i = 0; i < steps; i++) {
    // These four lines result in 1 step:
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelayuS);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelayuS);
  }
  digitalWrite(greenLEDPin,   LOW);    
  digitalWrite(yellowLEDPin,   LOW);    
}
void blink()
{
  digitalWrite(blueLEDPin, HIGH);  
  delay(200);             
  digitalWrite(blueLEDPin, LOW);    
  delay(200);                 
}
int setResolution(int ires)
{
  int iresSet = ires;
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
  return iresSet;
}
