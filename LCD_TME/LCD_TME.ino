
#include <LiquidCrystal.h>
 
LiquidCrystal lcd(13, 12, 11, 10, 9, 8 );
//Инициализация дисплея в общем виде выглядит так
//LiquidCrystal lcd(rs, e, d5, d4, d3, d2);

byte outside[8] = {
  B00100,
  B01110,
  B10101,
  B00100,
  B10101,
  B10001,
  B10001,
  B11111
};

byte inside[8] = {
  B00100,
  B10101,
  B01110,
  B00100,
  B10001,
  B10001,
  B10001,
  B11111
};


byte direct[8] = {
B00000,
B01111,
B00000,
B10000,
B01000,
B10000,
B00000,
B01111
};

byte back[8] = {
  B00000,
  B11110,
  B00000,
  B00010,
  B00001,
  B00010,
  B00000,
  B11110
};

//##### time
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
//#####

#define KT1 23
#define SG7 32



byte i=190;


void setup() {
  Serial.begin(9600);
//Определение количество символов в строке и количество строк:

lcd.createChar(0, outside);
lcd.createChar(1, inside);
lcd.createChar(2, direct);
lcd.createChar(3, back);

lcd.begin(16, 2);  
lcd.write(byte(1));

lcd.setCursor(0,1);
lcd.write(byte(0));

lcd.setCursor(6,0);
lcd.write(byte(2));

lcd.setCursor(6,1);
lcd.write(byte(3));

//lcd.print("String2");


pinMode(KT1, OUTPUT);
pinMode(SG7, OUTPUT);
}
 
void loop() {
  i+=1;
  tmElements_t tm;
  String tme;

  lcd.setCursor(1,1);
  
 /* if (RTC.read(tm)) {
    tme=String(tm.Hour) +':'+String(tm.Minute)+':'+String(tm.Second);
    
    lcd.print(tme);
  } else {
    lcd.print("TME ERR");
  }*/

    


delay(1000);

}

