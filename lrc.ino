/*
Copyright (c) 2023, Grzegorz Danecki
*/

#include <SimpleTimer.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h> 

LiquidCrystal_I2C lcd(0x27, 16, 2);

SimpleTimer CH1Timer;
SimpleTimer CH2Timer;

#define CLK 8
#define DT 9
#define SW 7
#define TOGGLE 6
#define MAX_TIME 300
#define MIN_TIME -1
#define PW_SWITCH_CH1 12
#define PW_SWITCH_CH2 11

bool ACTIVE = true;
bool CH1 = false;
bool CH2 = false;

long CH1_counter = 0;
long CH2_counter = 0;

int currentStateCLK;
int lastStateCLK;

unsigned long lastButtonPress = 0;

void setup() {
	pinMode(CLK,INPUT);
	pinMode(DT,INPUT);
	pinMode(SW, INPUT_PULLUP);
  pinMode(TOGGLE, INPUT_PULLUP);
	Serial.begin(9600);

  // Configure relays, and switch them OFF 
  pinMode(PW_SWITCH_CH1, OUTPUT);
  pinMode(PW_SWITCH_CH2, OUTPUT);

  digitalWrite(PW_SWITCH_CH1, HIGH);
  digitalWrite(PW_SWITCH_CH2, HIGH);

	lastStateCLK = digitalRead(CLK);

  lcd.init();
  lcd.backlight();
  updateLCD();
}

void updateLCD() {
  lcd.setCursor(0, 0);
  lcd.print("CH 1: ");
  if (CH1_counter == -1) { lcd.print("ON  "); };
  if (CH1_counter == 0) { lcd.print("OFF "); };
  if (CH1_counter > 0) { lcd.print((CH1_counter / 60) % 60); lcd.print(":"); if ((CH1_counter % 60) < 10) { lcd.print('0'); } lcd.print(CH1_counter % 60); };
  if (ACTIVE == 1) { lcd.println(" <    "); }
  if (ACTIVE == 0) { lcd.println("      "); }
    
  lcd.setCursor(0, 1);
  lcd.print("CH 2: ");
  if (CH2_counter == -1) { lcd.print("ON  "); };
  if (CH2_counter == 0) { lcd.print("OFF "); };
  if (CH2_counter > 0) { lcd.print((CH2_counter / 60) % 60); lcd.print(":"); if ((CH2_counter % 60) < 10) { lcd.print('0'); }  lcd.print(CH2_counter % 60); };
  if (ACTIVE == 1) { lcd.println("      "); }
  if (ACTIVE == 0) { lcd.println(" <    "); }
}

void toggle() {
  int toggleState = digitalRead(TOGGLE);
  if (toggleState == LOW) { 
    ACTIVE = !ACTIVE;
    delay(250);
    updateLCD();
  }
}

void start() {
  if (CH1_counter == -1) {
    digitalWrite(PW_SWITCH_CH1, LOW);
  }
  if (CH2_counter == -1) {
    digitalWrite(PW_SWITCH_CH2, LOW);
  }
  if (CH1_counter == 0) {
    digitalWrite(PW_SWITCH_CH1, HIGH);
  }
  if (CH2_counter == 0) {
    digitalWrite(PW_SWITCH_CH2, HIGH);
  }

  if (CH1_counter > 0) {
    digitalWrite(PW_SWITCH_CH1, LOW);
    CH1Timer.reset();
    CH1Timer.setInterval(CH1_counter * 1000);
  }
  if (CH2_counter > 0) {
    digitalWrite(PW_SWITCH_CH2, LOW);
    CH2Timer.reset();
    CH2Timer.setInterval(CH2_counter * 1000);
  }
}

void loop() {
	currentStateCLK = digitalRead(CLK);
	if (currentStateCLK != lastStateCLK  && currentStateCLK == 1) {
		if (digitalRead(DT) != currentStateCLK) {
			if ((CH1_counter > MIN_TIME) and (ACTIVE == 1)) {
        CH1_counter --;
        updateLCD();
      }
			if ((CH2_counter > MIN_TIME) and (ACTIVE == 0)) {
        CH2_counter --;
        updateLCD();
      } 
    } else {
      if ((CH1_counter < MAX_TIME) and (ACTIVE == 1)) {
			  CH1_counter ++;
			  updateLCD();
      }
        if ((CH2_counter < MAX_TIME) and (ACTIVE == 0)) {
			  CH2_counter ++;
			  updateLCD();
      }
		}
}

toggle();

	lastStateCLK = currentStateCLK;
  
	int btnState = digitalRead(SW);
  if (btnState == LOW) {
		if (millis() - lastButtonPress > 100) {
      start();
    }
		lastButtonPress = millis();
	}

	delay(1);

      if (CH1Timer.isReady() && !CH1) {
        if ((CH1_counter != -1) && (CH1_counter != 0)) {
          digitalWrite(PW_SWITCH_CH1, HIGH);
          CH1 = false;
        }
      }

      if (CH2Timer.isReady() && !CH2) {
        if ((CH2_counter != -1) && (CH2_counter != 0)) {
          digitalWrite(PW_SWITCH_CH2, HIGH);
          CH2 = false;
        }
      } 
}