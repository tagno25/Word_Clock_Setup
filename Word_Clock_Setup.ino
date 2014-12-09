#include <Wire.h> //required for DS1307RTC.h
#include <Time.h>  
#include <DS1307RTC.h> //required for RTC
#include <TimeAlarms.h> //Time Scheduling
#include <Timezone.h>    //Required to set the timzone

#include <EEPROM.h> //Used to read and write the time to EEPROM

#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by Unix time_t as ten ASCII digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

// T1262347200  //noon Jan 1 2010

void setup()  {
  Serial.begin(9600);
//  SaveTZ();
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  setSyncInterval(5);
}

void loop(){    
  if(Serial.available() ) 
  {
    processSyncMessage();
  }
  if(timeStatus() == timeNotSet) 
    Serial.println("waiting for sync message");
  else       
      digitalClockDisplay();  
  delay(1000);
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void processSyncMessage() {
  // if time sync available from serial port, update time and return true
  while(Serial.available() >=  TIME_MSG_LEN ){  // time message consists of header & 10 ASCII digits
    char c = Serial.read() ; 
    Serial.print(c);  
    if( c == TIME_HEADER ) {       
      time_t pctime = 0;
      for(int i=0; i < TIME_MSG_LEN -1; i++){   
        c = Serial.read();          
        if( c >= '0' && c <= '9'){   
          pctime = (10 * pctime) + (c - '0') ; // convert digits to a number    
        }
      }   
      setTime(pctime);   // Sync Arduino clock to the time received on the serial port
      RTC.set(now());
    }  
  }
}

//set Timezone
void SaveTZ(){

  TimeChangeRule usEdt = {"EDT", Second, Sun, Mar, 2, -240};
  TimeChangeRule usEst = {"EST", First, Sun, Nov, 2, -300};
  TimeChangeRule usCdt = {"CDT", Second, Sun, Mar, 2, -300};
  TimeChangeRule usCst = {"CST", First, Sun, Nov, 2, -360};
  TimeChangeRule usMdt = {"MDT", Second, Sun, Mar, 2, -360};
  TimeChangeRule usMst = {"MST", First, Sun, Nov, 2, -420};
  TimeChangeRule usPdt = {"PDT", Second, Sun, Mar, 2, -420};
  TimeChangeRule usPst = {"PST", First, Sun, Nov, 2, -480};

  Timezone timeZoneE(usEdt, usEst);
  timeZoneE.writeRules(100);
  Timezone timeZoneC(usCdt, usCst);
  timeZoneC.writeRules(124);
  Timezone timeZoneM(usMdt, usMst);
  timeZoneM.writeRules(148);
  Timezone timeZoneP(usPdt, usPst);
  timeZoneP.writeRules(172);
}
