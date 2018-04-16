#include <Math.h>
#include <LiquidCrystal.h>
 
LiquidCrystal lcd(13, 12, 11, 10, 9, 8 );
//Инициализация дисплея в общем виде выглядит так
//LiquidCrystal lcd(rs, e, d5, d4, d3, d2);

byte outside[8] = {B00100,B01110,B10101,B00100,B10101,B10001,B10001,B11111};
byte inside[8] =  {B00100,B10101,B01110,B00100,B10001,B10001,B10001,B11111};
byte direct[8] =  {B00000,B01111,B00000,B10000,B01000,B10000,B00000,B01111};
byte back[8] =    {B00000,B11110,B00000,B00010,B00001,B00010,B00000,B11110};
byte difft[8] =   {B11001,B10101,B11001,B00000,B01001,B10010,B11011,B10010};

//##### time
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
//#####


//##### temperature
#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire_in(36);   DallasTemperature INS_TMP_SENS(&oneWire_in);
OneWire oneWire_out(A10); DallasTemperature OUT_TMP_SENS(&oneWire_out);

OneWire afr_in(A8);       DallasTemperature afr_in_sens(&afr_in);
OneWire afr_out(A7);      DallasTemperature afr_out_sens(&afr_out);

float T_INS;
float T_OUT;
float t_afr_in;
float t_afr_out;    
//#####

#define KT1 23
#define KT2 25
#define SG7 32




void ask_sensors()
{
   
    INS_TMP_SENS.requestTemperatures();
    OUT_TMP_SENS.requestTemperatures();

    T_INS=INS_TMP_SENS.getTempCByIndex(0);
    T_OUT=OUT_TMP_SENS.getTempCByIndex(0);
    
    afr_in_sens.requestTemperatures();
    afr_out_sens.requestTemperatures();

    t_afr_in=afr_in_sens.getTempCByIndex(0);
    t_afr_out=afr_out_sens.getTempCByIndex(0);
}

void lcd_print_tmps(){
   lcd_print_tmp(T_INS,0,1);
   lcd_print_tmp(T_OUT,0,7);
   lcd_print_diff(T_INS,T_OUT,0,13);
   
   lcd_print_tmp(t_afr_in,1,1);
   lcd_print_tmp(t_afr_out,1,7);
   lcd_print_diff(t_afr_in,t_afr_out,1,13);
   
};

void lcd_print_diff(float p_tmp1, float p_tmp2, byte p_rw, byte p_cl){
  String tmps;
  lcd.setCursor(p_cl,p_rw);
  
  if (p_tmp1<-126 || p_tmp2<-126)        {lcd.print("err");}
  else 
    {
      float p_tmp=p_tmp1-p_tmp2;
      if (p_tmp>=10)        {lcd.print(p_tmp,0); lcd.print(" ");}
      else if (p_tmp>=0)    {lcd.print(p_tmp,1);}
      else if (p_tmp<=-10)  {lcd.print(p_tmp,0);}
      else if (p_tmp<0)     {lcd.print(p_tmp,0);lcd.print(" ");}
    }
};


void lcd_print_tmp(float p_tmp, byte p_rw, byte p_cl){
  String tmps;
  lcd.setCursor(p_cl,p_rw);
  
  if (p_tmp>=10)        {lcd.print(p_tmp,1);}
  else if (p_tmp>=0)    {lcd.print(p_tmp,1); lcd.print(" ");}
  else if (p_tmp<-126)  {lcd.print("errr");}
  else if (p_tmp<=-10)  {lcd.print(p_tmp,0); lcd.print(" ");}
  else if (p_tmp<0)     {lcd.print(p_tmp,1);}
};


void setup() {
  Serial.begin(9600);
//Определение количество символов в строке и количество строк:

lcd.createChar(0, outside);
lcd.createChar(1, inside);
lcd.createChar(2, direct);
lcd.createChar(3, back);
lcd.createChar(4, difft);

lcd.begin(16, 2);  

lcd.setCursor(0,0); lcd.write(byte(1));
lcd.setCursor(6,0); lcd.write(byte(0));
lcd.setCursor(12,0); lcd.write(byte(4));

lcd.setCursor(0,1); lcd.write(byte(2));
lcd.setCursor(6,1); lcd.write(byte(3));
lcd.setCursor(12,1); lcd.write(byte(4));

//lcd.print("String2");


pinMode(KT1, OUTPUT);
pinMode(SG7, OUTPUT);
}
 
void loop() {
  
  tmElements_t tm;
  String tme;

  lcd.setCursor(1,1);
  
 /* if (RTC.read(tm)) {
    tme=String(tm.Hour) +':'+String(tm.Minute)+':'+String(tm.Second);
    
    lcd.print(tme);
  } else {
    lcd.print("TME ERR");
  }*/

  ask_sensors();  

  lcd_print_tmps();


//delay(1000);

}

