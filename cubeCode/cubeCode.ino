#define BAUD_RATE 9600

struct TransmitData
{
  int stepLocation = 0;
};
struct ReceiveData
{
  int noStepsToTurn = 0;
  int motorRirection = 0;
  int stepDelayuS = 4000;
  int stayAwakeAfterRotation = 0;
  int loopDelay = 1000;
};

const int notReset = 9;
const int notSleep = 10;
const int stepPin = 11;
const int dirPin = 12;
const int ledPin = 13;
const int stepsPerRevolution = 200;

void setupPins()
{
  pinMode(ledPin, OUTPUT);    
  pinMode(notReset, OUTPUT);
  pinMode(notSleep, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(ledPin, OUTPUT); 
  digitalWrite(notReset, HIGH); 
  wakeup(0);   
}
void wakeup(int wake)
{
  digitalWrite(notSleep, wake);  
  delay(10);  
}
void rotate(int steps, int stepDelayuS, int direction)
{
  digitalWrite(dirPin, direction);
  for (int i = 0; i < steps; i++) 
  {
    // These four lines result in 1 step:
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelayuS);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelayuS);
  }
}
void processNewSetting(TransmitData* tData, ReceiveData* rData, ReceiveData* newData)
{
  rData->noStepsToTurn = newData->noStepsToTurn;
  rData->motorRirection = newData->motorRirection;
  rData->stepDelayuS = newData->stepDelayuS;
  rData->stayAwakeAfterRotation = newData->stayAwakeAfterRotation;
  rData->loopDelay = newData->loopDelay;

  wakeup(1);
  rotate(rData->noStepsToTurn, rData->stepDelayuS,rData->motorRirection);
  wakeup(rData->stayAwakeAfterRotation);
  if (rData->motorRirection == 1) tData->stepLocation = tData->stepLocation + rData->noStepsToTurn;
  if (rData->motorRirection == 0) tData->stepLocation = tData->stepLocation - rData->noStepsToTurn;
}
boolean processData(TransmitData* tData, ReceiveData* rData)
{
  delay(rData->loopDelay);
  return true;
}

const int microLEDPin = 13;
const int commLEDPin = 13;
boolean commLED = true;

struct TXinfo
{
  int cubeInit = 1;
  int newSettingDone = 0;
};
struct RXinfo
{
  int newSetting = 0;
};

struct TX
{
  TXinfo txInfo;
  TransmitData txData;
};
struct RX
{
  RXinfo rxInfo;
  ReceiveData rxData;
};
TX tx;
RX rx;
ReceiveData settingsStorage;

int sizeOfTx = 0;
int sizeOfRx = 0;

void setup()
{
  setupPins();
  pinMode(microLEDPin, OUTPUT);    
  pinMode(commLEDPin, OUTPUT);  
  digitalWrite(commLEDPin, commLED);
//  digitalWrite(microLEDPin, commLED);

  sizeOfTx = sizeof(tx);
  sizeOfRx = sizeof(rx);
  Serial1.begin(BAUD_RATE);
  delay(1000);
}
void loop()
{
  boolean goodData = false;
  goodData = processData(&(tx.txData), &settingsStorage);
  if (goodData)
  {
    tx.txInfo.newSettingDone = 0;
    if(Serial1.available() > 0)
    { 
      commLED = !commLED;
      digitalWrite(commLEDPin, commLED);
      Serial1.readBytes((uint8_t*)&rx, sizeOfRx);
      
      if (rx.rxInfo.newSetting > 0)
      {
        processNewSetting(&(tx.txData), &settingsStorage, &(rx.rxData));
        tx.txInfo.newSettingDone = 1;
        tx.txInfo.cubeInit = 0;
      }
    }
    Serial1.write((uint8_t*)&tx, sizeOfTx);
  }
  
}
