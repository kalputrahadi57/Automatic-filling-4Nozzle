int lcd_key     = 0;
int adc_key_in  = 0;

#define btnSELECT 0
#define btnUP     1
#define btnDOWN   2
#define btnNONE   3

int read_LCD_buttons() {
  adc_key_in = analogRead(A3);
  if (adc_key_in > 1000) return btnNONE;
  if (adc_key_in < 100)  return btnDOWN;
  if (adc_key_in < 600)  return btnUP;
  if (adc_key_in < 700)  return btnSELECT;
  return btnNONE;
}
unsigned long timeNowButton, timePrevButton;
bool getFirstDataTimeButSt = 0;

void tombol() {
  //  Serial.println(digitalRead(buttonPin));/
  if (digitalRead(buttonPin) == LOW) {
    timeNowButton = millis();
    if (!getFirstDataTimeButSt) {
      timePrevButton = timeNowButton;
      getFirstDataTimeButSt = 1;
    }
    if (timeNowButton - timePrevButton > 250) {
      if (runSt == 0 && setSt == 0) {
        volume = 0;
        runSt = 1;
        beep();
        delay(300);
      }
    }
    //delay(100);
  } else getFirstDataTimeButSt = 0;
  lcd_key = read_LCD_buttons();
  switch (lcd_key) {
    case btnSELECT: {
        if (!runSt) {
          if (tekanSt == 0) {
            beep();
            if (setSt < 3) setSt ++; else setSt = 0;
            //          Serial.print("SetSt ");
            //          Serial.println(setSt);
            tekanSt = 1;
            tunda = 5;
          }
          //delay(100);
          break;
        }
      }

    case btnUP: {
        //        Serial.println("Up");/
        if (setSt != 0) {
          if (tekanSt == 0) {
            beep();

            if (setSt == 1) {
              if (target < 10000) {
                target += 100;
                if (target >= 10000) target = 9999;
              }
            }

            if (setSt == 2) {
              if (koreksi < 990) koreksi += 1;
            }
            if (setSt == 3) {
              if (calibrationFac < 100.) calibrationFac += 0.01;
              intCalFac = calibrationFac * 1000;
//              Serial.print(calibrationFac);
//              Serial.print(" Up ");
//              Serial.println(intCalFac);
            }
            simpanData();
            tekanSt = 1;
          }
          //delay(100);

          tunda = 5;
        }
        break;
      }
    case btnDOWN: {
        //        Serial.println("Down");/
        if (setSt != 0) {
          if (tekanSt == 0) {
            beep();
            if (setSt == 1) {
              if (target > 100) {
                if (target == 9999) target = 9000;
                target -= 100;
              }
              else target = 0;
            }
            if (setSt == 2) {
              if (koreksi > -990) koreksi -= 1;
            }
            if (setSt == 3) {
              if (calibrationFac > 0.05) calibrationFac -= 0.01;
              intCalFac = calibrationFac * 1000;
              Serial.print(calibrationFac);
              Serial.print(" ");
              Serial.println(intCalFac);
            }
            tekanSt = 1;
            simpanData();
          }
          //delay(100);

          tunda = 5;
        }
        break;
      }

    case btnNONE:
      {
        //Serial.println("None");
        tekanSt = 0;
        break;
      }
  }
}
