#include <Himadri_DS3231.h>

// Please checkout the wiring diagram for this sketch. The diagram can be found within the folder of this example.

// declaring object of the DS3231 class
Himadri_DS3231 ds3231;

// timeParameters are struct datatype to store date and time
timeParameters timeVals;

// Days array 
String weekDay[7] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};


// alarmParameters are struct datatype to store alarm date and time
alarmParameters alarmVals;

// The PIN 2 is the interrupt 0 PIN in Arduino UNO
// By attaching the INT/SQW PIN for DS3231 we can take the interrupt from the RTC and notify that to MicroController
const int intPin = 2;

// This variable will change state when the interrupt will occur from within the ISR function
volatile boolean state = false;

// This is the Interrupt Service Routine function that will be called when the interrupt occur.
void isr() {
  Serial.println("External Interrupt Detected");
  state = true;
}

void setup() {
  Serial.begin(9600);

  if (!ds3231.begin()) {
    Serial.println("Could not find a valid DS3231 sensor, check wiring!");
    while (1) {}
  } else {
    Serial.println("Initialization Done!");

    /*Initialize INT0 for accepting interrupts */
    // For more information check out https://www.arduino.cc/en/Reference/PortManipulation
    // Set PIN 2 as HIGH. If you are using external PULL-UP register you don't need this.
    // PORTD |= 0x04;    // 00000100
    // Set digital PIN 2 as INPUT
    DDRD &=~ 0x04;    // 00000100

    // pinMode(intPin, INPUT);

    // It will set the Alarm1 for every seconds
    // This alarm can only be applied to Alarm1 not Alarm2 as Alarm2 is minutes precision not seconds precision
    // ds3231.setAlarm(Second, Alarm1);
    
    // It will set the Alarm1 for every minutes
    // ds3231.setAlarm(Minute, Alarm1);    
    // OR for Alarm2
    // ds3231.setAlarm(Minute, Alarm2);

    // It will set the Alarm1 for every hour
    // ds3231.setAlarm(Hour, Alarm1);
    // OR for Alarm2
    // ds3231.setAlarm(Hour, Alarm2);

    // It will set Alarm1 for whenever Seconds will be 10
    /* Format is
     *  periodicity Choice value of unsigned integer of setting the Alarm i.e Second / Minute / Hour
     *  value for the periodicity
     *  timeformat choice in boolean. True for 12 hour format and false for 24 hour format
     *  meridian boolean. True for PM and false for AM
     *  alarm choice. It can be Alarm1 / Alarm2
     *  
     */    
    // ds3231.setAlarm(Second, 20, false, false, Alarm1);
    // OR Alarm2 is set when Minute will be 46
    ds3231.setAlarm(Minute, 46, false, false, Alarm2);

    // It will set Alarm1 for exact seconds and minutes
    /* Format is
     * seconds value
     * minutes value
     * alarm choice. It will be always Alarm1 as Alarm2 don't have seconds option.
     */
    ds3231.setAlarm(10, 10, Alarm1);

    // It will enable Alarm1
    ds3231.enableAlarm(Alarm1);
    // OR Alarm2
    ds3231.enableAlarm(Alarm2);      

    attachInterrupt(0, isr, FALLING);
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

  if(state == true) {
    // It will clear the Alarm Flag after the alarm interrupt.
    // Without clearing the flag it will do the interrupt for the second time.
    // ds3231.clearINTStatus(Alarm1);
    // OR Alarm2
    // ds3231.clearINTStatus(Alarm2);
    // OR BothAlarm
    ds3231.clearINTStatus(BothAlarm);
  }

  // Delay for 10 seconds
  delay(10000);
}
