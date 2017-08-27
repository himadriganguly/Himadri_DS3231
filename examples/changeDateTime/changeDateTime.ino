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

    // It sets the seconds of the clock
    ds3231.setSeconds(30);
    // It sets the minutes of the clock
    ds3231.setMinutes(50);
    // It sets the hour of the clock. It can be 24 hour / 12 hour format
    /* Format is 
     *  hour value
     *  time format boolean. True for 12 Hour and false for 24 Hour format.
     *  meridian boolean. True for PM and false for AM.
     */
    ds3231.setHour(1, true, false);
    // Set day of the week. Week start with Sunday as 1 and ending with Sturday as 7.
    ds3231.setDay(1);
    // Set date of the month
    ds3231.setDate(13);
    // Set month of the year 
    /*
     * Format is
     * month value
     * century boolean. If the year is great or equal to 2000 then it will be true or else false. By default it is true.
     */
    ds3231.setMonth(9, false);
    // Set year. It cannot be greater than 2099 and less than 1900.
    ds3231.setYear(1920);
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
