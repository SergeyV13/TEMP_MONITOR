#include <Math.h>


//##### LiquidCrystal
#include <LiquidCrystal.h>
 
LiquidCrystal lcd(13, 12, 11, 10, 9, 8 );
//Инициализация дисплея в общем виде выглядит так
//LiquidCrystal lcd(rs, e, d5, d4, d3, d2);

byte outside[8] = {B00100,B01110,B10101,B00100,B10101,B10001,B10001,B11111};
byte inside[8] =  {B00100,B10101,B01110,B00100,B10001,B10001,B10001,B11111};
byte direct[8] =  {B00000,B01111,B00000,B10000,B01000,B10000,B00000,B01111};
byte back[8] =    {B00000,B11110,B00000,B00010,B00001,B00010,B00000,B11110};
byte difft[8] =   {B11001,B10101,B11001,B00000,B01001,B10010,B11011,B10010};
//#####


//##### time
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

char TIME_A[9];

#define KT1 23
#define KT2 25
#define SG7 32
//#####


//##### temperature
#include <OneWire.h>
#include <DallasTemperature.h>

//OneWire oneWire_in(36);   DallasTemperature INS_TMP_SENS(&oneWire_in);
OneWire oneWire_in(A10);   DallasTemperature INS_TMP_SENS(&oneWire_in);
OneWire oneWire_out(A8); DallasTemperature OUT_TMP_SENS(&oneWire_out);

OneWire afr_in(A7);       DallasTemperature afr_in_sens(&afr_in);
OneWire afr_out(A5);      DallasTemperature afr_out_sens(&afr_out);

OneWire pit(A3);      DallasTemperature pit_sens(&pit);

float T_INS;
float T_OUT;
float t_afr_in;
float t_afr_out; 
float T_PIT;
float MST_PIT;   
//#####


//##### EtherCard
#include <EtherCard.h>
#include <stdio.h>


static byte myip[] = { 192,168,41,169 };
static byte gwip[] = { 192,168,41,254 };
static byte mymac[] = { 0x72,0x63,0x64,0x2D,0x30,0x32 };


byte Ethernet::buffer[1300];
BufferFiller bfill;
int datas;

char T_INS_A[7];
char T_OUT_A[7];
char T_DFF_A[7];
char t_afr_in_a[7];
char t_afr_out_a[7];
char t_afr_dff_a[7];
char T_PIT_A[7];
char MST_PIT_A[7];
char SUPPL[7]; 
char PUMP[7]; 
//#####

#define ms_to_reask 1000
unsigned long prev_ask=0;
byte tmp_printed=0;

void setup() {
  Serial.begin(9600);


  //##### LiquidCrystal
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
  //##### LiquidCrystal
  
  
  
  pinMode(KT1, OUTPUT);
  pinMode(SG7, OUTPUT);



  if (ether.begin(sizeof Ethernet::buffer, mymac,53) == 0) 
    Serial.println( "Failed to access Ethernet controller");
  ether.staticSetup(myip, gwip);


  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  

  INS_TMP_SENS.begin();
  INS_TMP_SENS.setResolution(9); 
}



static word homePage() {

  float_to_char(T_INS, T_INS_A);
  float_to_char(T_OUT, T_OUT_A);

  if (T_INS<-126 || T_OUT<-126)
    {T_DFF_A[0]='E';T_DFF_A[1]='R';T_DFF_A[2]='R';T_DFF_A[3]='\0';}
  else  
    float_to_char(T_INS - T_OUT, T_DFF_A);

  float_to_char(t_afr_in, t_afr_in_a);
  float_to_char(t_afr_out, t_afr_out_a);

  if (t_afr_in<-126 || t_afr_out<-126)
    {t_afr_dff_a[0]='E';t_afr_dff_a[1]='R';t_afr_dff_a[2]='R';t_afr_dff_a[3]='\0';}
  else  
    float_to_char(t_afr_in - t_afr_out, t_afr_dff_a);
    
  MST_PIT=-127; ///not realized
   
  float_to_char(T_PIT, T_PIT_A);
  float_to_char(MST_PIT, MST_PIT_A);


  if (1)
    {SUPPL[0]='O';SUPPL[1]='N';SUPPL[2]='\0';}
  else
    {SUPPL[0]='O';SUPPL[1]='F';SUPPL[2]='F';SUPPL[3]='\0';};

  
  if (1)
    {PUMP[0]='O';PUMP[1]='N';PUMP[2]='\0';}
  else
    {PUMP[0]='O';PUMP[1]='F';PUMP[2]='F';PUMP[3]='\0';};

    
  
  
  
  
  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR(
    "<html><head>"
    "<meta charset=""UTF-8"">"
    "<title>Система...</title>"
    "<style type='text/css'>"
    " A {color: black; "
    "text-decoration:underline;"
    "font-size: 34px;}"
    "A:visited {color: black;"
    "font-size: 34px;}"
    "A:hover {color: red; "
    "font-size: 34px;}"
    "</style></head><body>"
    "<!--img src='' width='100%'/-->"
    "<br/><br/><br/><br/>"
    "<center>"
    "<h2>$S</h3>"
    "<h3>"
    "<table cellspacing=""2"" border=""1"" cellpadding=""5"" width=""800"">"
    "<tr>"
    "<td><strong>Температура внутри $S°</strong></td>"
    "<td><strong>Температура на улице $S°</strong></td>"
    "<td><strong>Разница температур $S°</strong></td>"
    "</tr>"
    "<tr>"
    "<td><strong>Антифриз вход $S°</strong></td>"
    "<td><strong>Антифриз выход $S°</strong></td>"
    "<td><strong>Разница температур $S°</strong></td>"
    "</tr>"
    "<tr>"
    "<td><strong>Температура в яме $S°</strong></td>"
    "<td><strong>Влажность в яме $S°</strong></td>"
    "<td></td>"
    "</tr>"
    "</table>"
    "<br>Питание: $S"
    "<br>Насос: $S"
    "</h3>"
    "</center></body></html> "
  ),TIME_A, T_INS_A, T_OUT_A, T_DFF_A,
            t_afr_in_a, t_afr_out_a, t_afr_dff_a,
            T_PIT_A, MST_PIT_A,
            SUPPL, PUMP);  
  return bfill.position();
}

void float_to_char(float p_f, char *p_ch_t)
{  
    for (byte i=0; i < 7; i++) p_ch_t[i]='\0';
   
    if (p_f < -126)
      {
         p_ch_t[0]='N';p_ch_t[1]='/';p_ch_t[2]='C';
      } 
      else {
        String s_tmp=String(p_f);
        char* ch_arr_tmp=s_tmp.c_str();
        for (int i=0; i < 5; i++) p_ch_t[i]=ch_arr_tmp[i];   
      }
};

void fill_time(byte p_v, byte p_pos)
{
    String s_tmp=String(p_v);
    char* ch_arr_tmp=s_tmp.c_str();
    
    if (p_v>=10)
      {TIME_A[p_pos]=ch_arr_tmp[0]; TIME_A[p_pos+1]=ch_arr_tmp[1];}
    else
      {TIME_A[p_pos]='0'; TIME_A[p_pos+1]=ch_arr_tmp[0];}
        
};

void ask_sensors(byte stp)
{

   switch (stp) {
    case 0:
      INS_TMP_SENS.requestTemperatures();
      break;
    case 1:
      T_INS=INS_TMP_SENS.getTempCByIndex(0);
      break;
    case 2:
      OUT_TMP_SENS.requestTemperatures();
      break;
    case 3:
      T_OUT=OUT_TMP_SENS.getTempCByIndex(0);
      break;
  }

    if (stp)
    

  //  
 //   
    
 /*   afr_in_sens.requestTemperatures();
    afr_out_sens.requestTemperatures();
    
    t_afr_in=afr_in_sens.getTempCByIndex(0);
    t_afr_out=afr_out_sens.getTempCByIndex(0);

    pit_sens.requestTemperatures();
    T_PIT=pit_sens.getTempCByIndex(0);

    MST_PIT=0;

    tmElements_t tm;
    if (RTC.read(tm))
        {
          fill_time(tm.Hour,0);
          TIME_A[2]=':';
          fill_time(tm.Minute,3);
          TIME_A[5]=':';
          fill_time(tm.Second,6);
        }
      else
        {
          TIME_A[0]='N'; TIME_A[1]='O'; TIME_A[2]=' '; TIME_A[3]='C'; TIME_A[4]='O'; TIME_A[5]='N'; TIME_A[6]='N'; TIME_A[7]='\0'; TIME_A[8]='\0'; TIME_A[9]='\0';
        };*/
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
  
  if (p_tmp1<-126 || p_tmp2<-126) {lcd.print("err");}
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



 
void loop() {
  long strt;
  
  for (byte cl_pos=0; cl_pos < 7; cl_pos++)
    {    
      
      Serial.print("STRT "); Serial.println(cl_pos);
      strt=millis();
      
      if (prev_ask<(millis()-ms_to_reask) || prev_ask>millis()) 
          {
            ask_sensors();
            prev_ask = millis();
            Serial.println("ASK");
          }
    
      if (prev_ask<(millis()-ms_to_reask/2 || prev_ask>millis())) 
          {
            lcd_print_tmps();
            Serial.println("PRINT");
          }
      
      
    
    
    
      word len = ether.packetReceive();
      word pos = ether.packetLoop(len);
        if (pos)  // check if valid tcp data is received
          {
            
      
      
            char *data = (char *) Ethernet::buffer + pos;
            ether.httpServerReply(homePage());
    
    
          }
    
      Serial.print("CEND ");
      strt=millis()-strt;
      Serial.println(strt);
      delay(100);
      
  }
}

