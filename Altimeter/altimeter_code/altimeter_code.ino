// Program: Arduino Altimeter
// Author: Michael Rouse
// Date: 4/2015
// Contact: michael@michaelrouse.net

#include <Wire.h>
#include "MPL3115A2.h"
#include "SevSeg.h"
#include <EEPROM.h>
#include "MEMORY.h"

//#define SERIAL_DEBUG

// Sev-Seg variables
SevSeg output;
char tempString[10] = ""; // For printing

// Altimeter variables
MPL3115A2 altimeter;
int altitude; // Current Altitude
int refAltitude; // Reference altitude (altitude - baseAltitude);
int apogee;
int baseAltitude = 0;
int maxAltitude = -30000; // Maximum acheived altitude
int minAltitude = 30000; // Minimum acheived altitude
bool launched = false;
bool landed = false;
bool apogeeDetected = false;

// Data logging
bool logging = false; // True when logging data
int flightLog[500]; // Collection of flight data
unsigned int timer;
int logIndex = 0;
int logDisplay = 0;

// Constants
const int ERROR_MARGIN = 1; // 1m needs to change before it's detected
const int LAUNCH_MARGIN = 2; // 2m needs to change in the positive direction for a launch to be detected
const int APOGEE_MARGIN = 2; // 3m needs to change from the max alt for apogee detection
const int TIMER_BEAT = 200; // Every 200ms
const int FLIGHT_TIME = 29;


// Setup Function
void setup()
{
  Wire.begin(); 
  
  altimeter.begin();
  
  // Setup SevSeg
  output.Begin(COMMON_CATHODE, 4, 9, 12, 6, 4, 10, 11, 7, A0, 8, 13, A1, 5);
  output.SetBrightness(100);
  
  // Setup Altimeter
  altimeter.setModeAltimeter();
  altimeter.setOversampleRate(7);
  altimeter.enableEventFlags();
  
  delay(000);
  
  Serial.begin(9600);
  
  // Get last known apogee
  EEPROM_read(225, apogee);
  sprintf(tempString, "%04d", (int)apogee);
  printString(tempString, 500);
  
  /*Serial.print("Last Flight Altitude: ");
  Serial.print(apogee);
  Serial.println(" ft.");*/
  
  // Get base altitude level
  baseAltitude = getBaseAltitude();
  
  printString("HOLD", 1000);
  
  // Show that the altimeter is ready
  printString("REDY", 200);
  
  timer = millis();
 
}

// Program loop
void loop()
{
  static int timesSame = 0;
  static int oldNumber = 0;
  
  if (millis() > timer)
  {  
    timer = millis() + TIMER_BEAT;
    
    // Get the altitude
    int lastAltitude = altitude;
    altitude = getAltitude();
    
    // Check if altitude is higher than max altitude
    if (altitude >= (maxAltitude + ERROR_MARGIN))
    {
      maxAltitude = altitude;
      //LogData();
    }
     
    if (!launched && altitude <= (baseAltitude - ERROR_MARGIN))
    {
       //baseAltitude = altitude;
    }
    
    // Check for apogee
    if (altitude <= (maxAltitude - APOGEE_MARGIN) && launched && !apogeeDetected)
    {
      apogeeDetected = true;
      
      apogee = (maxAltitude - baseAltitude);
      
      EEPROM_write(225, apogee); // Save the apogee to the EEPROM
    }
    
    // Check for a launch
    if (altitude >= (minAltitude + LAUNCH_MARGIN) && !launched && !landed)
    {
      launched = true;
      
      logging = true; // Start logging data
    }
    
    logData();
    
    if (landed)
    {
      if(logDisplay == 0)
      {
        Serial.print("a,");
        logDisplay++;
      }else{
        Serial.print(flightLog[logDisplay-1]);
        Serial.print(",");
        
        if (flightLog[logDisplay] == oldNumber)
          timesSame++;
          
         oldNumber = flightLog[logDisplay];
      }
      
      logDisplay++;
      
      if (logDisplay > (FLIGHT_TIME * 5) || timesSame >= 15)
      {
        logDisplay = 0;
        timesSame = 0;
      }
    }
    
  }
  displayAltitude();
}

// Returns the current measured altitude in ft.
int getAltitude()
{
  float alt = altimeter.readAltitude();
  refAltitude = (int)alt - baseAltitude; // Altitude in reference to the base altitude taken in setup();
  return (int)alt;
}

// Get the starting altitude
// Returns the lowest altitude out of 100
int getBaseAltitude()
{
  int lowAlt = 30000;
  int alt = 0;

  for (int i = 0; i < 100; i++)
  {
    alt = getAltitude();
    
    if (alt < lowAlt)
      lowAlt = alt;
  }
  
  minAltitude = lowAlt;
  
  return lowAlt;
}

// Display the altitude
void displayAltitude()
{
  if (apogeeDetected)
  {
    sprintf(tempString, "%04d", apogee);
  }else{
    sprintf(tempString, "%04d", refAltitude);
  }
  
  printString(tempString, 10);
}

// Print string to display
void printString(char* stringy, int delayTime)
{
  for (int i = 0; i < delayTime; i++)
  {
     output.DisplayString(stringy, 0);
  }
}

void logData()
{
 static int lastLog = -30000; 
 static int timesTheSame = 0;
 
 if (logging && launched)
 {
   if (!apogeeDetected)
   {
     if (altitude > lastLog)
     {
       flightLog[logIndex] = refAltitude;
       logIndex++;
       //Serial.println(refAltitude);
     }
   }else{
     // Prevent logging of false values
     if (altitude < lastLog)
     {
       flightLog[logIndex] = refAltitude;
       logIndex++;
       //Serial.println(refAltitude);
     }
   }
   if (altitude <= (lastLog - ERROR_MARGIN) && altitude >= (lastLog + ERROR_MARGIN))
     timesTheSame++;
   else
      timesTheSame = 0;

   lastLog = altitude;
   
   if (refAltitude <= 0 || timesTheSame >= 30 || logIndex > (FLIGHT_TIME * 5))
   {
     logging = false;
     launched = false;
     landed = true;
   }
 }
}
