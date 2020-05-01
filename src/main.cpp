#include <Arduino.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Bounce2.h>
#include "ui/menu.h"

#include "arch/e93xx.h"
#include <string.h>


E93xx e(512, 9, 8);
LiquidCrystal_I2C lcd(0x27, 20, 4);

uint8_t currentCursor = 0;
uint8_t offset = 0;

void debug_eeprom(int address) {
  uint8_t data = e.read(address);
  char buf[128];
  sprintf(buf, " 0x%02X ", data);
  Serial.print(buf);
}

void render_menu(uint8_t cursor) {
  uint8_t block = (int)(cursor/4)-1;

  int n_offset = cursor > 4 ? cursor - 4 : 0;
  if(n_offset != offset) {
    lcd.clear();
  }
  offset = n_offset % 7;

  lcd.setCursor(0, cursor-offset);
  lcd.print(">");

  for(int i=0; i<4; i++) {
    lcd.setCursor(1, i);
    lcd.print(current_menu[i+offset].title);
  }
}


void setup() {
  Serial.begin(9600);

  

  

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
