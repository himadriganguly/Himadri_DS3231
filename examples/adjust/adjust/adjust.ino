#include <Himadri_DS3231.h>

// declaring object of the DS3231 class
Himadri_DS3231 ds3231;

// timeParameters are struct datatype to store date and time
timeParameters timeVals;

// Days array 
String weekDay[7] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

void setup() {
  Serial.begin(9600);

  if (!ds3231.begin()) {    // It will initialize DS3231 register values
    Serial.println("Could not find a valid DS3231 sensor, check wiring!");
    while (1) {}
  } else {
    Serial.println("Initialization Done!");

//    uint8_t tf;   // time format -> 0 for 24Hr format / 1 for 12Hr format
//    uint8_t ss;   // seconds
//    uint8_t mm;   // minutes
//    uint8_t hh;   // hour
//    uint8_t md;   // meridian AM/PM -> 0 for AM / 1 for PM / 3 for None(24Hr format)
//    uint8_t day;  // day of the week -> 1 for SUN and 7 for SAT
//    uint8_t d;    // date of the month
//    uint8_t m;    // month
//    uint16_t y;   // year
    
    timeVals = {
      1,
      40,
      23,
      2,
      0,
      7,
      26,
      8,
      2017
    };

    /* Set Date and Time into DS3231 Timekeeping Register
     * Uncomment the following section to set Date and Time and upload
     * After that again comment this section and upload the code
     */
//    if (!ds3231.setDateTime(&timeVals)) {
//      Serial.println("Error setting time and date!");
//    } else {
//      Serial.println("Date and Time setup correctly!");
//    }    
  }
}

void loop() {
  if(ds3231.readDateTime(&timeVals)) {  // Read the Date and Time from Timekeeping Register of DS3231
    Serial.print(timeVals.y);
    Serial.print("/");
    Serial.print(timeVals.m);
    Serial.print("/");
    Serial.print(timeVals.d);
    Serial.print(" -- ");
    Serial.print(weekDay[timeVals.day - 1]);
    Serial.print(" -- ");
    Serial.print(timeVals.hh);
    Serial.print(":");
    Serial.print(timeVals.mm);
    Serial.print(":");
    Serial.print(timeVals.ss);
    Serial.print(" ");
    if(timeVals.tf == 1) {
      if(timeVals.md == 0) {
        Serial.println("AM");
      } else {
        Serial.println("PM");
      }
    }  
  } else {
    Serial.println("Error Reading Date and Time from DS3231.");
  }

  // Delay for 10 seconds
  delay(10000);
}
