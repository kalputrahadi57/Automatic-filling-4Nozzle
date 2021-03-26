#include "SevSeg.h"
SevSeg myDisplay;

#include <EEPROMex.h>

int digit = 3;
int digit1, digit2, digit3, digit4;
int relayPin = A4;
int buttonPin = A5;
int btnVal = 0;
int ledPin = 10;
int buzzer = 11;

int displayType = COMMON_ANODE;
int segA = 6;
int segB = 2;
int segC = A2;
//int displayType = COMMON_CATHODE;

//int segA = 6;/
//int segB = 2;/
//int segC = A2;/
int segD = 13;
int segE = 12;
int segF = 5;
int segG = 9;
int segDP = A1;

int numberOfDigits = digit; //Do you have a 1, 2 or 4 digit display?

unsigned char encoder_pin = 3; // pulse output from the module
volatile float pulses; // number of pulses
unsigned long timeold;
float  volume = 0;
unsigned long prevTime = 0;

bool flowSt = 0;
volatile float pulsesTot = 0.;
byte setSt = 0;
byte runSt = 0;
byte tekanSt = 0;
byte tunda = 5;

byte tresholdFlow = 20;
int volmL = 0;
int target = 500;
int koreksi = 0;
//float calFac = 1.2;
float calFac = 0.65;
float calibrationFac = 1.;
int intCalFac = 1000;
//float calFac = 1.6;
/*
   calfac Nozzle 1 = 0.42 pp1/10 s(pulse per 1/10 second) -- +-2
 * * calfac Nozzle 2 = 0.42 pp1/10 s
 * * calfac Nozzle 3 = 0.55 pp1/10 s -- ||+- 3
 * * calfac Nozzle 4 = 0.68 pp1/10 s -- ||+- 4
*/
int pulsesTarget = (target * calFac) + (koreksi * calFac);
unsigned long nowTimeIn, prevTimeIn;
int pulsesFlow;
void counter() {
  nowTimeIn = millis();
  if (nowTimeIn - prevTimeIn > 15) {
    pulsesFlow++;
    if (pulsesFlow > tresholdFlow) {
      flowSt = 1;
      //      pulsesFlow = 0;
    }
    prevTimeIn = nowTimeIn;

    if (!runSt) pulsesFlow = 0;
  }
}

void setup() {
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);
  Serial.begin(9600);
  Serial.println("7 Segment");
  pinMode(encoder_pin, INPUT);
  digitalWrite(encoder_pin, HIGH);
  pinMode(A3, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzer, OUTPUT);

  digitalWrite(ledPin, LOW);
  

  if (digit == 3) {
    digit3 = A0;//3; //Pin 3 digit 3
    digit2 = 4; //Pin 4 digit 2
    digit1 = 7; //Pin 7 digit 1
  }
  if (digit == 4) {
    digit4 = 8;
    digit3 = A0;//3
    digit2 = 4;
    digit1 = 7;
  }

  myDisplay.Begin(displayType, numberOfDigits, digit1, digit2, digit3, digit4, segA, segB, segC, segD, segE, segF, segG, segDP);
  myDisplay.SetBrightness(75); //Set the display to 100% brightness level
  pinMode(encoder_pin, INPUT_PULLUP);
  attachInterrupt(1, counter, RISING);//RISING);////
  sei();

  //pulses = 0;

  //  simpanData();/////////
  ambilData();
  if (isnan(calibrationFac)) {
    calibrationFac = 1;
    simpanData();
    ambilData();
    Serial.println("first Upload");
  }
  beep();
  beep();
  //setup1();
}
void loop1() {
  //runSt = 1;
  LP ();
}
void loop() {
  tombol();
  if (setSt != 0) {
    timerLed();
    if (setSt == 1) tampilSetting(target);
    if (setSt == 2) tampilSetting(koreksi * 10);
    if (setSt == 3) tampilSetting(intCalFac);
    //    Serial.println(koreksi);
  } else {
    bacaFlow();
    if (runSt == 1) {
      tampil();
      digitalWrite(relayPin, LOW);
    }
    else {
      tampilSettingU(target);
      digitalWrite(relayPin, HIGH);
    }
  }
}

void tampil() {
  char tempString[10];
  if (digit == 3) {
    sprintf(tempString, "%03d", volmL / 10); //Convert deciSecond into a string with leading zeros
  }
  if (digit == 4) {
    sprintf(tempString, "%04d", volmL); //Convert deciSecond into a string with leading zeros
  }
  myDisplay.DisplayString(tempString, 1); //0,2,4,8(numberToDisplay, decimal point location)
  //delay(5);
}

void tampilSettingU(int val) {
  char tempString[10];
  if (digit == 3) {
    sprintf(tempString, "%03d", val / 10); //Convert deciSecond into a string with leading zeros
  }
  if (digit == 4) {
    sprintf(tempString, "%4d", val); //Convert deciSecond into a string with leading zeros
  }
  if (setSt == 1) myDisplay.DisplayString(tempString, 0); //0,2,4,8(numberToDisplay, decimal point location)
  else {
    if (val <= 0 ) {
      myDisplay.DisplayString(tempString, 0);
    } else if (val >= 10000) {
      myDisplay.DisplayString(tempString, 2);
    }
    else {
      myDisplay.DisplayString(tempString, 1);
    }
  }
  if (setSt == 2) myDisplay.DisplayString(tempString, 0); //0,2,4,8(numberToDisplay, decimal point location)

  if (setSt == 3) myDisplay.DisplayString(tempString, 1);

  delay(5);
}

void tampilSetting(int val) {
  char tempString[10];
  if (digit == 3) {
    sprintf(tempString, "%3d", val / 10); //Convert deciSecond into a string with leading zeros
  }
  if (digit == 4) {
    sprintf(tempString, "%4d", val); //Convert deciSecond into a string with leading zeros
  }
  if (setSt == 2) myDisplay.DisplayString(tempString, 0); //0,2,4,8(numberToDisplay, decimal point location)
  else {
    if (val <= 0 ) {
      myDisplay.DisplayString(tempString, 0);
    } else if (val >= 10000) {
      myDisplay.DisplayString(tempString, 2);
    } else {
      myDisplay.DisplayString(tempString, 1);
    }
  }
  delay(5);
}
unsigned long timeNow, timePrev;

bool firstData;
int pulsesOld;

bool  getFirstTimePrevSt = 0;
unsigned long timeNowStFlow, timePrevStFlow;
int countLoopData;

void bacaFlow() {
  timeNow = millis();
  if (timeNow - timePrev >= 100) {

//    Serial.println(pulsesFlow);/
    if (pulsesFlow > 1) {
      timeNowStFlow = millis();
      if (!getFirstTimePrevSt) {
        timePrevStFlow = timeNowStFlow;
        getFirstTimePrevSt = 1;
      }
      if (timeNowStFlow - timePrevStFlow > 2000) {
        if (pulsesFlow < tresholdFlow) {
          pulsesFlow = 0;
//          Serial.println("masuk-----------------------");/
        }
        getFirstTimePrevSt = 0;
      }
    }

    if (flowSt == 1) {
      pulses = 4.;
      //      Serial.print("Pulses ");///
      //      Serial.println(pulses);///

      if (!firstData) {
//        Serial.println("firstData -----------------");///
        pulsesOld = pulsesFlow;
        firstData = 1;
        countLoopData = 0;
      }
      countLoopData++;
      if (pulsesFlow == pulsesOld) {

        if (countLoopData > 1) {
          pulsesFlow = 0;
          flowSt = 0;
        } else {
          pulsesTot = pulsesTot + pulses * calibrationFac;
//          Serial.println("masuk2 -----------------");///
        }

      } else {
//        Serial.println("outt -----------------");/
        pulsesTot = pulsesTot + pulses * calibrationFac;
        firstData = 0;
      }
      Serial.print(pulsesTarget);  
      Serial.print("  ");  
      Serial.println(pulsesTot);//
      if (pulsesTot >= pulsesTarget) {
        reseT();
      }
      //      Serial.print("PulsesTot ");/////
      //      Serial.println(pulsesTot);//////
      
      Serial.println();//
      
      if (!runSt) {
        firstData = 0;
        pulsesTot = 0;
      }
    }
    pulses = 0;//
    timePrev = timeNow;
  }

  //  ----------------------------tampilan
  if (millis() - timeold >= 1000) {
    //    detachInterrupt(1);/
    timeold = millis();
    volume = pulsesTot / calFac; // / calFac;
    if (digit == 3) {
      volmL = (int)volume;
    }
    if (digit == 4) {
      volmL = (int)volume;
    }

//    Serial.print("TARGET ");//
//    Serial.println(target);//
//    Serial.print("Pulses Target ");//
//    Serial.println(pulsesTarget);//
    //        Serial.print("Pulses ");
    //        Serial.println(pulses);
    //        Serial.print("PulsesTot ");//
    //        Serial.println(pulsesTot);//
    //        Serial.println();
    //Serial.print("volmL ");
    //Serial.println(volmL);
    //    attachInterrupt(1, counter, RISING);//FALLING);////
  }

}
void reseT() {
  Serial.print("pulsesTot -------------------  ");
  Serial.println(pulsesTot);
  runSt = 0;
  flowSt = 0;
  pulsesFlow = 0;
  volume = 0;
  pulsesTot = 0;
  digitalWrite(relayPin, HIGH);
  volmL = 0;


  beep();
  beep();
  //  delay(2000);/
}

void simpanData() {

  EEPROM.updateInt(128, target);
  EEPROM.updateInt(132, koreksi);
  EEPROM.updateFloat(140, calibrationFac);
  delay(10);
  ambilData();
}

void ambilData() {
  target = EEPROM.readInt(128);
  koreksi = EEPROM.readInt(132);
  calibrationFac = EEPROM.readFloat(140);
  intCalFac = calibrationFac * 1000;
  pulsesTarget = (target * calFac) + (koreksi * calFac);

  //  pulsesTarget *= calibrationFac;
}

void beep() {
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);
  delay(100);
}
