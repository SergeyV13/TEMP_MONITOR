// Present a "Will be back soon web page", as stand-in webserver.
// 2011-01-30 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php
 
#include <EtherCard.h>
#include <stdio.h>

//##### temperature
#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire_in(7);
DallasTemperature INS_TMP_SENS(&oneWire_in);
OneWire oneWire_out(6);
DallasTemperature OUT_TMP_SENS(&oneWire_out);
//#####

//##### time
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
//#####

char T_INS[7];
char T_OUT[7];
char T_PIT[7];
char TIME[9];

//##### temperature


static byte myip[] = { 192,168,41,169 };
static byte gwip[] = { 192,168,41,254 };
static byte mymac[] = { 0x72,0x63,0x64,0x2D,0x30,0x32 };


byte Ethernet::buffer[1300];
BufferFiller bfill;
int datas;

unsigned long prev_ask;
#define ms_to_reask 1000

void setup(){
  Serial.begin(9600);
  
  if (ether.begin(sizeof Ethernet::buffer, mymac,53) == 0) 
    Serial.println( "Failed to access Ethernet controller");
  ether.staticSetup(myip, gwip);


  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  

  INS_TMP_SENS.begin();
  INS_TMP_SENS.setResolution(11); 
}



static word homePage() {


 
  

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
    "<h3>Температура внутри $S</h3>"
    "<h3>Температура на улице $S</h3>"
    "<!--a href='monitor.html'>Показать данные мониторинга</a>"
    "<br/><br/>"
    "<a href='data.html'>Установка даты и времени</a>"
    "<br/><br/>"
    "<a href='about.html'>О системе</a-->"
    "</center></body></html> "
  ),TIME, T_INS, T_OUT);

  Serial.println("HMP");
  
  return bfill.position();
  
}

void float_to_char(float p_f, char *p_ch_t)
{  
    for (int i=0; i < 5; i++) p_ch_t[i]='\0';
   
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
{  w
        String s_tmp=String(p_v);
        char* ch_arr_tmp=s_tmp.c_str();

        if (p_v>=10)
          {TIME[p_pos]=ch_arr_tmp[0]; TIME[p_pos+1]=ch_arr_tmp[1];}
        else
          {TIME[p_pos]='0'; TIME[p_pos+1]=ch_arr_tmp[0];}
        
};

void ask_sensors()
{
    tmElements_t tm;
    
    INS_TMP_SENS.requestTemperatures();
    OUT_TMP_SENS.requestTemperatures();

   
    float_to_char(INS_TMP_SENS.getTempCByIndex(0), T_INS);
    float_to_char(OUT_TMP_SENS.getTempCByIndex(0), T_OUT);


    if (RTC.read(tm)) {
      fill_time(tm.Hour,0);
      TIME[2]=':';
      fill_time(tm.Minute,3);
      TIME[5]=':';
      fill_time(tm.Second,6);
  } else {
      TIME[0]='N'; TIME[1]='O'; TIME[2]=' '; TIME[3]='C'; TIME[4]='O'; TIME[5]='N'; TIME[6]='N'; TIME[7]='\0'; TIME[8]='\0'; TIME[9]='\0';
    }
  
  
}

void loop () {
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);



  if (prev_ask==0 or prev_ask<(millis()-ms_to_reask)) 
      {
        ask_sensors();
        prev_ask = millis();
      }


 
  if (pos)  // check if valid tcp data is received
    {char *data = (char *) Ethernet::buffer + pos;
        // проверяем на наличие в заголовках нужных нам строк
        //if (strstr(data, "monitor.html"))
        ether.httpServerReply(homePage());
    }



    
}

