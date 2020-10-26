/*Example sketch to control a stepper motor with A4988/DRV8825 stepper motor driver and Arduino without a library. More info: https://www.makerguides.com */
// Define stepper motor connections and steps per revolution:
#define notReset 9
#define notSleep 10
#define stepPin 11
#define dirPin 12
#define ledPin 13
#define stepsPerRevolution 200
void setup() 
{
  // Declare pins as output:
  pinMode(notReset, OUTPUT);
  pinMode(notSleep, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(ledPin, OUTPUT); 
  digitalWrite(notReset, HIGH);    
  wakeup(false);    
  blink();
}
void loop() 
{
  wakeup(true);
  rotate(200, 1000, true);
  blink();
  delay(1000);
  rotate(200, 1000, false);
  blink();
  wakeup(false);
  
  delay(10000);
  blink();
}
void wakeup(boolean wake)
{
  digitalWrite(notSleep, wake);  
  delay(10);  
}
void rotate(int steps, int stepDelayuS, boolean direction)
{
  digitalWrite(dirPin, direction);
  for (int i = 0; i < steps; i++) {
    // These four lines result in 1 step:
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelayuS);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelayuS);
  }
}
void blink()
{
  digitalWrite(ledPin, HIGH);  
  delay(200);             
  digitalWrite(ledPin, LOW);    
  delay(200);                 
}
