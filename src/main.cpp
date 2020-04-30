#include <Arduino.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Bounce2.h>

#include "arch/e93xx.h"

#define UP 7
#define DOWN 8

E93xx e(512, 9, 8);
LiquidCrystal_I2C lcd(0x27, 20, 4);
Bounce bouncerUp = Bounce();
Bounce bouncerDown = Bounce();

uint8_t currentCursor = 0;

void debug_eeprom(int address) {
  uint8_t data = e.read(address);
  char buf[128];
  sprintf(buf, " 0x%02X ", data);
  Serial.print(buf);
}

void render_menu(uint8_t cursor) {
  lcd.setCursor(0, cursor);
  lcd.print(">");

  lcd.setCursor(1, 0);
  lcd.print("D");
  lcd.setCursor(1, 1);
  lcd.print("Configuracoes");
  lcd.setCursor(1, 2);
  lcd.print("Informacoes");
  lcd.setCursor(1, 3);  
}


void setup() {
  Serial.begin(9600);

  pinMode(UP, INPUT);
  pinMode(DOWN, INPUT);

  bouncerUp.attach(UP);
  bouncerUp.interval(20);

  bouncerDown.attach(DOWN);
  bouncerDown.interval(20);

  lcd.init();
  lcd.backlight();

  e.setup();

}

char state = 'W';
char option;
char address[8];
char data[8];
int current = 0;
uint16_t code;
void loop() {


  render_menu(currentCursor);

   if(bouncerUp.update() && bouncerUp.rose()) {
     lcd.setCursor(0, currentCursor);
    lcd.print(" ");
     currentCursor = (currentCursor + 1) % 4;
   }

   if(bouncerDown.update() && bouncerDown.rose()) {
     lcd.setCursor(0, currentCursor);
    lcd.print(" ");
     currentCursor = (currentCursor - 1) % 4;
   }

   if (Serial.available()) {
    char value = Serial.read();

    if(state == 'W') {

      for(int i=0; i<8; i++) address[i] = 0;
      for(int i=0; i<8; i++) data[i] = 0;
      current = 0;

      option = value;
      state = 'A';
    } else {

      if(value == '#') {
        switch(option) {
          case 'R':
            // e.read_all();
            debug_eeprom((int)strtol(address, NULL, 16));
            break;
          case 'P':
            e.print();
            break;
          case 'W':
            e.write((int)strtol(address, NULL, 16), (int)strtol(data, NULL, 16));
            break;
          default:
            Serial.println("Opção inválida");
        }

        state = 'W';
      } else if (value == '$')  {
        current = 0;
        state = 'D';
      }
      else {
          switch(state) {
            case 'A':
              if(current < 7) {
                address[current] = value;
                current++;
              }
              break;
            case 'D':
              if(current < 7) {
                data[current] = value;
                current++;
              }
              break;
          }
      }
    }
  }
}
