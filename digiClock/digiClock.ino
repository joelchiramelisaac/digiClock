#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>  
#include <Fonts/FreeSans12pt7b.h>
#include "RTClib.h"

RTC_DS3231 rtc;

#define cs 10
#define dc 9
#define rst 8
#define GREY 0x8410

Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);

int menuState;
int newMenuState;
int menu2CursorPos;
int menu3CursorPos;
int menu4CursorPos; 

int currentH;
int currentM;
int currentS;

int currentD;
int currentMo;
int currentY;
int currentDofW; 

int newD;
int newMo;
int newY;

int newH;
int newM;
int newS;

void setup() {
  Serial.begin(9600);
  rtc.begin();
  Wire.begin();
  pinMode(3, INPUT);
  pinMode(6, INPUT);
  pinMode(12, INPUT); 
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);
  //tft.background(0, 0, 0);
  //tft.fillRect(0, 0, TFTscreen.width(), 16, 0x8410);
  menuState = 1;
  newMenuState = 1;
  menu2CursorPos = 1; 
  menu3CursorPos = 1;
  menu4CursorPos = 1;
  DateTime now = rtc.now();
  currentH = now.hour();
  currentM = now.minute();
  currentS = now.second();
  currentD = now.day();
  currentMo = now.month();
  currentY = now.year();
  currentDofW = now.dayOfTheWeek();
  newH = currentH;
  newM = currentM;
  newS = currentS;
  newD = currentD;
  newMo = currentMo;
  newY = currentY;
  modMenuState();
 
}

void loop() {
  DateTime now = rtc.now();
  currentH = now.hour();
  currentM = now.minute();
  currentS = now.second();
  currentD = now.day();
  currentMo = now.month();
  currentY = now.year();
  currentDofW = now.dayOfTheWeek();
  checkMenuState();
  int buttonState1 = digitalRead(3);
  if(buttonState1 == HIGH){
    button1();
  }
  int buttonState2 = digitalRead(6);
  if(buttonState2 == HIGH){
    button2();
  }
  int buttonState3 = digitalRead(12);
  if(buttonState3 == HIGH){
    button3();
  }
  
  if(menuState == 1){
      if(currentS == 59){
        delay(1000);
        DateTime now = rtc.now();
        currentH = now.hour();
        currentM = now.minute();
        currentS = now.second();
        currentD = now.day();
        currentDofW = now.dayOfTheWeek();
        modMenuState();
      }
  }
}

void button1(){
  if(menuState == 1){
    newMenuState++; 
  }
  else if(menuState == 2){
    if(menu2CursorPos == 1){
      newMenuState = 1;
    }
    if(menu2CursorPos == 2){
      newMenuState = 3;
    }
    if(menu2CursorPos == 3){
      newMenuState = 4;
    }
  }
  else if(menuState == 3){
    if(menu3CursorPos == 4){
      newMenuState = 2;
      menu3CursorPos = 1;
      setNewTime();
    }
    else{
      menu3CursorPos++;
      updateMenu3();
    }
  }
  else if(menuState == 4){
    if(menu4CursorPos == 4){
      newMenuState = 2;
      menu4CursorPos = 1;
      setNewDate();
    }
    else{
      menu4CursorPos++;
      updateMenu4();
    }
  }
  delay(100); 
}

void button2(){
  if(menuState == 2){
    menu2CursorPos++;
    if(menu2CursorPos > 3){
      menu2CursorPos = 1;
      updateMenu2();
    }
    else{
      updateMenu2();   
    }
    delay(100);
  }
  else if(menuState == 3){
    if(menu3CursorPos == 1){
      newH--;
      if(newH < 0){
        newH = 24;
      }
    }
    if(menu3CursorPos == 2){
      newM--;
      if(newM < 0){
        newM = 60;
      }
    }
    if(menu3CursorPos == 3){
      newS--;
      if(newS < 0){
        newS = 60;
      }
    }
    updateMenu3();
  }
  else if(menuState == 4){
    if(menu4CursorPos == 1){
      newD--;
      if(newD < 0){
        newD = 31;
      }
    }
    if(menu4CursorPos == 2){
      newMo--;
      if(newMo < 1){
        newMo = 12;
      }
    }
    if(menu4CursorPos == 3){
      newY--;
      if(newY < 2021){
        newY = 2099;
      }
    }
    updateMenu4();
  }
}

void button3(){
  if(menuState == 2){
    menu2CursorPos--;
    if(menu2CursorPos < 0){
      menu2CursorPos = 3;
      updateMenu2();
    }
    else{
      updateMenu2();   
    }
    delay(100);
  }
  else if(menuState == 3){
    if(menu3CursorPos == 1){
      newH++;
      if(newH > 24){
        newH = 0;
      }
    }
    if(menu3CursorPos == 2){
      newM++;
      if(newM > 60){
        newM = 0;
      }
    }
    if(menu3CursorPos == 3){
      newS++;
      if(newS > 60){
        newS = 0;
      }
    }
    updateMenu3();
  }
  else if(menuState == 4){
    if(menu4CursorPos == 1){
      newD++;
      if(newD > 31){
        newD = 1;
      }
    }
    if(menu4CursorPos == 2){
      newMo++;
      if(newMo > 12){
        newMo = 1;
      }
    }
    if(menu4CursorPos == 3){
      newY++;
      if(newY > 2099){
        newY = 2021;
      }
    }
    updateMenu4();
  } 
}

void resetText(){
  tft.setFont();
  tft.setTextSize(1);
  tft.setTextColor(ST7735_WHITE);
}

void checkMenuState(){
  if(menuState != newMenuState){
    menuState = newMenuState;
    modMenuState();
  }
}

void modMenuState(){
  resetText();
  if(menuState == 1){
      tft.fillScreen(ST7735_BLACK);
      tft.setFont(&FreeSans12pt7b);
      tft.setCursor(20, 72);
      tft.setTextSize(2);
      if(currentH < 10){
        tft.print("0");
        tft.print(currentH);
      }
      else{
        tft.print(currentH);
      }  
      tft.print(":");
      if(currentM < 10){
        tft.print("0");
        tft.print(currentM);
      }
      else{
        tft.print(currentM);
      }
      resetText();
      tft.setCursor(49, 96);
      tft.setTextSize(1);
      if(currentDofW == 0){
        tft.print("Sunday ");
      }
      else if(currentDofW == 1){
        tft.print("Monday ");
      }
      else if(currentDofW == 2){
        tft.print("Tuesday ");
      }
      else if(currentDofW == 3){
        tft.print("Wednesday ");
      }
      else if(currentDofW == 4){
        tft.print("Thursday ");
      }
      else if(currentDofW == 5){
        tft.print("Friday ");
      }
      else if(currentDofW == 6){
        tft.print("Saturday ");
      }
      tft.print(currentD);
      resetText();
  }
  else if(menuState == 2){
      tft.fillScreen(ST7735_BLACK);
      tft.fillRect(0, 0, tft.width(), 16, 0x8410);
      tft.setCursor(2, 4);
      tft.print("Settings");
      updateMenu2();
  }
  else if(menuState == 3){
      tft.fillScreen(ST7735_BLACK);
      tft.fillRect(0, 0, tft.width(), 16, 0x8410);
      tft.setCursor(2, 4);
      tft.print("Set Time");
      tft.setFont(&FreeSans12pt7b);
      tft.setTextSize(1);
      tft.drawRect(16, 32, 40, 40, ST7735_WHITE);
      tft.setCursor(23, 60);
      if(newH < 10){
        tft.print("0");
        tft.print(newH);
      }
      else{
        tft.print(newH);
      }
      tft.drawRect(60, 32, 40, 40, ST7735_WHITE);
      tft.setCursor(67, 60);
      if(newM < 10){
        tft.print("0");
        tft.print(newM);
      }
      else{
        tft.print(newM);
      }
      tft.drawRect(104, 32, 40, 40, ST7735_WHITE);
      tft.setCursor(111, 60);
      if(newS < 10){
        tft.print("0");
        tft.print(newS);
      }
      else{
        tft.print(newS);
      }
      resetText();
      tft.setCursor(25, 78);
      tft.print("Hour");
      tft.setCursor(62, 78);
      tft.print("Minute");
      tft.setCursor(106, 78);
      tft.print("Second");
      tft.fillRect(48, 100, 64, 20, 0xAD55);
      tft.setCursor(70,106);
      tft.print("Set");
  }
  else if(menuState == 4){
      tft.fillScreen(ST7735_BLACK);
      tft.fillRect(0, 0, tft.width(), 16, 0x8410);
      tft.setCursor(2, 4);
      tft.print("Set Date");
      tft.setFont(&FreeSans12pt7b);
      tft.setTextSize(1);
      tft.drawRect(5, 32, 35, 40, ST7735_WHITE);
      tft.setCursor(9, 60);
      if(newD < 10){
        tft.print("0");
        tft.print(newD);
      }
      else{
        tft.print(newD);
      }
      tft.drawRect(45, 32, 35, 40, ST7735_WHITE);
      tft.setCursor(49, 60);
      if(newMo < 10){
        tft.print("0");
        tft.print(newMo);
      }
      else{
        tft.print(newMo);
      }
      tft.drawRect(85, 32, 70, 40, ST7735_WHITE);
      tft.setCursor(96, 60);
      tft.print(newY);
      resetText();
      tft.setCursor(14, 78);
      tft.print("Day");
      tft.setCursor(48, 78);
      tft.print("Month");
      tft.setCursor(108, 78);
      tft.print("Year");
      tft.fillRect(48, 100, 64, 20, 0xAD55);
      tft.setCursor(70,106);
      tft.print("Set");
  }
}

void setNewTime(){
      rtc.adjust(DateTime(currentY, currentMo, currentD, newH, newM, newS));
}

void setNewDate(){
    rtc.adjust(DateTime(newY, newMo, newD, currentH, currentM, currentS)); 
}

//update the set time menu 

void updateMenu3(){
  if(menu3CursorPos == 1){
      tft.setFont(&FreeSans12pt7b);
      tft.setTextSize(1);
      tft.fillRect(16, 32, 40, 40, ST7735_WHITE);
      tft.fillRect(60, 32, 40, 38, ST7735_BLACK);
      tft.fillRect(104, 32, 40, 38, ST7735_BLACK);
      tft.drawRect(60, 32, 40, 40, ST7735_WHITE);
      tft.setCursor(67, 60);
      if(newM < 10){
        tft.print("0");
        tft.print(newM);
      }
      else{
        tft.print(newM);
      }
      tft.drawRect(104, 32, 40, 40, ST7735_WHITE);
      tft.setCursor(111, 60);
      if(newS < 10){
        tft.print("0");
        tft.print(newS);
      }
      else{
        tft.print(newS);
      }
      tft.setTextColor(ST7735_BLACK);
      tft.setCursor(23, 60);
      if(newH < 10){
        tft.print("0");
        tft.print(newH);
      }
      else{
        tft.print(newH);
      }
  }
  else if(menu3CursorPos == 2){
    tft.setFont(&FreeSans12pt7b);
    tft.setTextSize(1);
    tft.fillRect(16, 32, 40, 40, ST7735_BLACK);
    tft.fillRect(104, 32, 40, 40, ST7735_BLACK);
    tft.drawRect(16, 32, 40, 40, ST7735_WHITE);
    tft.setCursor(23, 60);
    if(newH < 10){
      tft.print("0");
      tft.print(newH);
    }
    else{
      tft.print(newH);
    }
    tft.drawRect(104, 32, 40, 40, ST7735_WHITE);
    tft.setCursor(111, 60);
    if(newS < 10){
      tft.print("0");
      tft.print(newS);
    }
    else{
      tft.print(newS);
    }
    tft.fillRect(60, 32, 40, 40, ST7735_WHITE);
    tft.setTextColor(ST7735_BLACK);
    tft.setCursor(67, 60);
    if(newM < 10){
      tft.print("0");
      tft.print(newM);
    }
    else{
      tft.print(newM);
    }
  }
  else if(menu3CursorPos == 3){
    tft.setFont(&FreeSans12pt7b);
    tft.setTextSize(1);
    tft.fillRect(16, 32, 40, 40, ST7735_BLACK);
    tft.drawRect(16, 32, 40, 40, ST7735_WHITE);
    tft.setCursor(23, 60);
    if(newH < 10){
      tft.print("0");
      tft.print(newH);
    }
    else{
      tft.print(newH);
    }
    tft.fillRect(60, 32, 40, 40, ST7735_BLACK);
    tft.drawRect(60, 32, 40, 40, ST7735_WHITE);
    tft.setCursor(67, 60);
    if(newM < 10){
      tft.print("0");
      tft.print(newM);
    }
    else{
      tft.print(newM);
    }
    tft.fillRect(104, 32, 40, 40, ST7735_WHITE);
    tft.setTextColor(ST7735_BLACK);
    tft.setCursor(111, 60);
    if(newS < 10){
      tft.print("0");
      tft.print(newS);
    }
    else{
      tft.print(newS);
    }
  }
  else if(menu3CursorPos == 4){
    tft.setTextColor(ST7735_WHITE);
    tft.setFont(&FreeSans12pt7b);
    tft.fillRect(104, 32, 40, 40, ST7735_BLACK);
    tft.drawRect(104, 32, 40, 40, ST7735_WHITE);
    tft.setTextColor(ST7735_WHITE);
    tft.setCursor(111, 60);
    if(newS < 10){
      tft.print("0");
      tft.print(newS);
    }
    else{
      tft.print(newS);
    }
    resetText();
    tft.setTextColor(GREY);
    tft.fillRect(48, 100, 64, 20, 0xFFDF);
    tft.setCursor(70,106);
    tft.print("Set");
  }
  resetText();
  delay(250);
}
//update the set date menu 

void updateMenu4(){
  if(menu4CursorPos == 1){
      tft.setFont(&FreeSans12pt7b);
      tft.setTextSize(1);
      tft.fillRect(5, 32, 35, 40, ST7735_WHITE);
      tft.fillRect(45, 32, 35, 40, ST7735_BLACK);
      tft.fillRect(85, 32, 70, 40, ST7735_BLACK);
      tft.drawRect(45, 32, 35, 40, ST7735_WHITE);
      tft.setCursor(49, 60);
      if(newMo < 10){
        tft.print("0");
        tft.print(newMo);
      }
      else{
        tft.print(newMo);
      }
      tft.drawRect(85, 32, 70, 40, ST7735_WHITE);
      tft.setCursor(96, 60);
      tft.print(newY);
      tft.setTextColor(ST7735_BLACK);
      tft.setCursor(9, 60);
      if(newD < 10){
        tft.print("0");
        tft.print(newD);
      }
      else{
        tft.print(newD);
      }
  }
  else if(menu4CursorPos == 2){
    tft.setFont(&FreeSans12pt7b);
    tft.setTextSize(1);
    tft.fillRect(5, 32, 35, 40, ST7735_BLACK);
    tft.fillRect(85, 32, 70, 40, ST7735_BLACK);
    tft.drawRect(5, 32, 35, 40, ST7735_WHITE);
    tft.setCursor(9, 60);
    if(newD < 10){
      tft.print("0");
      tft.print(newD);
    }
    else{
      tft.print(newD);
    }
    tft.drawRect(85, 32, 70, 40, ST7735_WHITE);
    tft.setCursor(96, 60);
    tft.print(newY);
    tft.fillRect(45, 32, 35, 40, ST7735_WHITE);
    tft.setTextColor(ST7735_BLACK);
    tft.setCursor(49, 60);
    if(newMo < 10){
      tft.print("0");
      tft.print(newMo);
    }
    else{
      tft.print(newMo);
    }
  }
  else if(menu4CursorPos == 3){
    tft.setFont(&FreeSans12pt7b);
    tft.setTextSize(1);
    tft.fillRect(5, 32, 35, 40, ST7735_BLACK);
    tft.drawRect(5, 32, 35, 40, ST7735_WHITE);
    tft.setCursor(9, 60);
    if(newD < 10){
      tft.print("0");
      tft.print(newD);
    }
    else{
      tft.print(newD);
    }
    tft.fillRect(45, 32, 35, 40, ST7735_BLACK);
    tft.drawRect(45, 32, 35, 40, ST7735_WHITE);
    tft.setCursor(49, 60);
    if(newMo < 10){
      tft.print("0");
      tft.print(newMo);
    }
    else{
      tft.print(newMo);
    }
    tft.fillRect(85, 32, 70, 40, ST7735_WHITE);
    tft.setTextColor(ST7735_BLACK);
    tft.setCursor(96, 60);
    tft.print(newY);
  }
  else if(menu4CursorPos == 4){
    tft.setTextColor(ST7735_WHITE);
    tft.setFont(&FreeSans12pt7b);
    tft.fillRect(85, 32, 70, 40, ST7735_BLACK);
    tft.drawRect(85, 32, 70, 40, ST7735_WHITE);
    tft.setTextColor(ST7735_WHITE);
    tft.setCursor(96, 60);
    tft.print(newY);
    resetText();
    tft.setTextColor(GREY);
    tft.fillRect(48, 100, 64, 20, 0xFFDF);
    tft.setCursor(70,106);
    tft.print("Set");
  }
  resetText();
  delay(250);
}

//for the settings menu
void updateMenu2(){
  if(menu2CursorPos == 1){
      tft.fillRect(0, 16, tft.width(), 32, 0xFFDF);
      tft.setTextColor(GREY);
      tft.setCursor(4,24);
      tft.setTextSize(2);
      tft.print("Return");
      tft.setTextColor(ST7735_WHITE);
      tft.fillRect(0, 48, tft.width(), 32, 0xAD55);
      tft.setCursor(4,56);
      tft.print("Set Time");
      tft.fillRect(0, 80, tft.width(), 32, 0xAD55);
      tft.setCursor(4,88);
      tft.print("Set Date");
  }
  else if(menu2CursorPos == 2){
      tft.setTextColor(ST7735_WHITE);
      tft.fillRect(0, 16, tft.width(), 32, 0xAD55);
      tft.setCursor(4,24);
      tft.setTextSize(2);
      tft.print("Return");
       tft.setTextColor(GREY);
      tft.fillRect(0, 48, tft.width(), 32, 0xFFDF);
      tft.setCursor(4,56);
      tft.print("Set Time");
       tft.setTextColor(ST7735_WHITE);
      tft.fillRect(0, 80, tft.width(), 32, 0xAD55);
      tft.setCursor(4,88);
      tft.print("Set Date");
  }
  else if(menu2CursorPos == 3){
      tft.setTextColor(ST7735_WHITE);
      tft.fillRect(0, 16, tft.width(), 32, 0xAD55);
      tft.setCursor(4,24);
      tft.setTextSize(2);
      tft.print("Return");
       tft.setTextColor(ST7735_WHITE);
      tft.fillRect(0, 48, tft.width(), 32, 0xAD55);
      tft.setCursor(4,56);
      tft.print("Set Time");
       tft.setTextColor(GREY);
      tft.fillRect(0, 80, tft.width(), 32, 0xFFDF);
      tft.setCursor(4,88);
      tft.print("Set Date");
  }
}
