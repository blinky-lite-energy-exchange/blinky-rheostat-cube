#define BAUD_RATE 115200
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

#define stepsPerRevolution 200.0

struct TransmitData
{
  float nrevs = 0.0;
};
struct ReceiveData
{
  int moveMotor = 0;
  int resetNrevReading = 0;
  int noStepsToTurn = 0;
  int stepDelayuS = 4000;
  int stayAwakeAfterRotation = 0;
  int resolution = 0;
  int delayAfterRotationmS = 0;
  int loopDelay = 1000;
};


void setupPins()
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

  wakeup(0);   
}
void wakeup(int wake)
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
int setResolution(int ires)
{
  int iresSet = ires;
  int iresSetDec = 1;
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

void processNewSetting(TransmitData* tData, ReceiveData* rData, ReceiveData* newData)
{
  rData->noStepsToTurn = newData->noStepsToTurn;
  rData->stepDelayuS = newData->stepDelayuS;
  rData->stayAwakeAfterRotation = newData->stayAwakeAfterRotation;
  rData->resolution = newData->resolution;
  rData->delayAfterRotationmS = newData->delayAfterRotationmS;
  rData->loopDelay = newData->loopDelay;
  rData->moveMotor = newData->moveMotor;
  rData->resetNrevReading = newData->resetNrevReading;

  int iresSetDec = setResolution(rData->resolution);
  if (rData->moveMotor > 0)
  {
    wakeup(1);
    rotate(rData->noStepsToTurn, rData->stepDelayuS);
    delay(rData->delayAfterRotationmS);
    wakeup(rData->stayAwakeAfterRotation);
    float deltaRevs = ((float) rData->noStepsToTurn) / ((float)iresSetDec) / stepsPerRevolution;
    tData->nrevs = tData->nrevs + deltaRevs;
  }
  if (rData->resetNrevReading > 0)
  {
    tData->nrevs = 0.0;
  }
  wakeup(rData->stayAwakeAfterRotation);
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
