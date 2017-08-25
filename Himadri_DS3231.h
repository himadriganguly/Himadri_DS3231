/******************************
         DS3231 RTC

The DS3231 is a low-cost, extremely accurate I2C real-time clock
(RTC) with an integrated temperature-compensated crystal oscillator
(TCXO) and crystal.

DS3231 library for Arduino.

@author <a href="mailto:himadri@colourdrift.com">Himadri Ganguly</a>
@version 1.0.0 19/08/2017

Available from:  https://github.com/himadriganguly/Himadri_DS3231

This library implements the following features:
  1. Set Date and Time by passing structure variable.
  2. Set Date and Time by passing date as string and time as string.
  3. It can keep date from 1900 till 2099.
  4. Time can be set both in 12 and 24 Hour format
  5. Seconds, Minutes, Hour, Day, Date, Month and Year can be set seperately.
  6. Validation for Seconds, Minutes, Hour, Day, Date, Month and Year
  7. Read current Date and Time
  8. Set Alarm1 matching every seconds / minutes / hour
     OR Alarm2 matching every minute / hour
  9. Set Alarm1 matching for exact seconds / minutes / hour with time format
     and meridian OR Alarm2 matching for exact minutes / hour with time format
     and meridian
  10. Set Alarm1 matching seconds and minutes
  11. Set Alaram1 matching seconds, minutes and hour
      OR Alarm2 matching minutes and hour
  12. Set Alarm1 matching day / date with hour, minutes and seconds
      OR Alarm2 matching day / date with hour, minutes
  13. Enable Alarm1 / Alarm2
  14. Disable Alarm1 / Alarm2
  15. Get Alarm1 / Alarm2 status
  16. Get Alarm1 / Alarm2 configured time
  17. Get Temperature in Celsius / Fahrenheit / Kelvin scale
  18. Convert Temperature
  19. Get Control Register value
  20. Enable / Disable Oscillator
  21. Get Oscillator status
  22. Enable / Disable BBSQW
  23. Get BBSQW status
  24. Get INTCN status
  25. Get Status Register value
  26. Get OSF status
  27. Enable / Disable 32Khz
  28. Get 32Khz status
  29. Get BSY status
  30. Get Aging Register value
  31. Set Aging Register value

******************************/

/**
    Himadri_DS3231.h
    Purpose: Header file for Himadri_DS3231.cpp. Contains the all function declaration.
*/

#ifndef HIMADRI_DS3231_H

  #define HIMADRI_DS3231_H

  #if (ARDUINO >= 100)
    #include <Arduino.h>
  #else
    #include <WProgram.h>
  #endif

  #include <Wire.h>

  /******************************
           DS3231 RTC
  ******************************/
  /*-------------------------------
    I2C Slave Address Of DS3231
  -------------------------------*/
  #define DS3231_ADDRESS              0x68
  /*-------------------------------
    I2C Slave Address Of DS3231
  -------------------------------*/

  /*-------------------------------
    Control Register Address
  -------------------------------*/
  #define DS3231_CONTROL_REG          0x0E
  /*-------------------------------
    Control Register Address
  -------------------------------*/

  /*-------------------------------
    Status Register Address
  -------------------------------*/
  #define DS3231_STATUS_REG           0x0F
  /*-------------------------------
    Status Register Address
  -------------------------------*/

  /*-------------------------------
      Aging Register Address
  -------------------------------*/
  #define DS3231_AGING_REG             0x10
  /*-------------------------------
      Aging Register Address
  -------------------------------*/

  /*-------------------------------
        Timekeeping Register
  -------------------------------*/
  // Date And Time Registers
  #define DS3231_SECONDS_REG          0x00    // Seconds Register
  #define DS3231_MINUTES_REG          0x01    // Minutes Register
  #define DS3231_HOUR_REG             0x02    // Hour Register
  #define DS3231_DAY_REG              0x03    // Day Register
  #define DS3231_DATE_REG             0x04    // Date Register
  #define DS3231_MONTH_REG            0x05    // Month Register
  #define DS3231_YEAR_REG             0x06    // Year Register

  // Alarm1 Register
  #define DS3231_AL1SEC_REG           0x07    // Alaram1 Seconds Register
  #define DS3231_AL1MIN_REG           0x08    // Alarm1 Minutes Register
  #define DS3231_AL1HOUR_REG          0x09    // Alarm1 Hour Register
  #define DS3231_AL1WDAY_REG          0x0A    // Alarm1 Day Register

  // Alarm2 Register
  #define DS3231_AL2MIN_REG           0x0B    // Alarm2 Minutes Register
  #define DS3231_AL2HOUR_REG          0x0C    // Alarm2 Hour Register
  #define DS3231_AL2WDAY_REG          0x0D    // Alarm2 Day Register

  // Temperature Registers
  #define DS3231_TEMPERATURE_MSG_REG  0x11    // Temperature MSB Register
  #define DS3231_TEMPERATURE_LSB_REG  0x12    // Temperature LSB Register
  /*-------------------------------
        Timekeeping Register
  -------------------------------*/

  /*-------------------------------
        Timeformat Choice
  -------------------------------*/
  #define TwelveHrFormat             0x01
  #define TwentyFourHrFormat         0x02
  /*-------------------------------
        Timeformat Choice
  -------------------------------*/

  /*-------------------------------
        Alarm Choice
  -------------------------------*/
  #define Alarm1                     0x01
  #define Alarm2                     0x02
  #define BothAlarm                  0x03
  /*-------------------------------
        Alarm Choice
  -------------------------------*/

  /*-------------------------------
        Interrupt Choice
  -------------------------------*/
  #define Second                     0x01
  #define Minute                     0x02
  #define Hour                       0x03
  /*-------------------------------
        Interrupt Choice
  -------------------------------*/

  /*-------------------------------
      Temperature Scale Choice
  -------------------------------*/
  #define Celsius                    0x01
  #define Fahrenheit                 0x02
  #define Kelvin                     0x03
  /*-------------------------------
      Temperature Scale Choice
  -------------------------------*/

  /*-------------------------------
      Enable / Disable Choice
  -------------------------------*/
  #define Enable                      0x01
  #define Disable                     0x02
  /*-------------------------------
      Enable / Disable Choice
  -------------------------------*/

  /*-------------------------------
          Days Of Month
        Storing It In SRAM
  -------------------------------*/
  static const uint8_t daysInMonth [] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };
  /*-------------------------------
        Days Of Month
      Storing It In SRAM
  -------------------------------*/

  /*-------------------------------
        Date And Time
    Structure Declaration
  -------------------------------*/
  typedef struct timeParameters {
    uint8_t tf;             // time format -> 0 for 24Hr format / 1 for 12Hr format
    uint8_t ss;             // seconds
    uint8_t mm;             // minutes
    uint8_t hh;             // hour
    uint8_t md;             // meridian AM/PM -> 0 for AM / 1 for PM / 3 for None(24Hr format)
    uint8_t day;            // day of the week -> 1 for SUN and 7 for SAT
    uint8_t d;              // date of the month
    uint8_t m;              // month
    uint16_t y;             // year
  };

  /*-------------------------------
        Alarm Date And Time
      Structure Declaration
  -------------------------------*/
  typedef struct alarmParameters {
    uint8_t tf;             // time format -> 0 for 24Hr format / 1 for 12Hr format
    uint8_t ss;             // seconds
    uint8_t mm;             // minutes
    uint8_t hh;             // hour
    uint8_t md;             // meridian AM/PM -> 0 for AM / 1 for PM / 3 for None(24Hr format)
    uint8_t dayDate;        // day of the week -> 1 for SUN and 7 for SAT / date of the month
    uint8_t dyDt;           // 0 -> Date of the month / 1 -> Day of the week
  };

  class Himadri_DS3231 {
    /*-------------------------------
      Private Function Declaration
    -------------------------------*/
    private:
      /**
        Returns Decimal to BCD

        @param value The value in Decimal
        @return The converted value in BCD
      */
      static uint8_t dec2bcd(uint8_t value);

      /**
        Returns BCD to Decimal

        @param value The value in BCD
        @return The converted value in Decimal
      */
      static uint8_t bcd2dec(uint8_t value);

      /**
        Returns 2 digit decimal from String

        @param p The string pointer
        @return Return 2 digit from the string starting from position 0
      */
      static uint8_t conv2d2(const char* p);

      /**
        Returns 4 digit decimal from String

        @param p The string pointer
        @return Return 4 digit from the string starting from position 0
      */
      static uint16_t conv2d4(const char* p);

      /**
        Validate Date, Month and Year.

        @param date Unsigned integer value for Date
        @param month Unsigned integer value for Month
        @param year Unsigned integer value for Year
        @return Return boolean
      */
      boolean monthDateYearValidation(uint8_t date, uint8_t month, uint16_t year);

      /**
        Reads 8 bit register value from the address specified

        @param regaddress The address of the register to read
        @return Return 8 bit unsigned integer from the register address
      */
      uint8_t readRegister(uint8_t regaddress);

      /**
        Writes 8 bit value to the register address specified

        @param regaddress The address of the register to write
        @param value 8 bit unsigned integer value to write to register
        @return Return 8 bit unsigned integer from the register address
      */
      void writeRegister(uint8_t regaddress, uint8_t value);

    /*-------------------------------
      Public Function Declaration
    -------------------------------*/
    public:
      /**
        Clear Alarm Flag in Status Register

        @param alarm The Alarm1 / Alarm2 / Both option parameter to clear the specified flag or both flag
        @return Return void
      */
      void clearINTStatus(uint8_t alarm);

      /**
        Initialize the Timekeeping Register value with initial Date and Time of
        1900/01/01 WED 00:00:00AM or 00:00:00

        @param timeFormat The TwelveHrFormat / TwentyFourHrFormat option parameter to initial the Date and Time accordingly
        @return Return void
      */
      void clearTimeRegister(uint8_t timeFormat);

      /**
        Initialize the Control Register by disabling both the Alarm
        Also clear the both Alarm Flag from Status Register

        @return Return boolean
      */
      boolean begin(void);

      /**
        Initialize Date and Time structure variable by passing date and time as string

        @param date Character pointer of the passing date string
        @param time Character pointer of the passing time string
        @param timeVals Data and Time structure variable pointer
        @return Return boolean
      */
      boolean initializeDateTime(const char* date, const char* time, struct timeParameters* timeVals);

      /**
        Set Date and Time in 12 / 24 Hr format by passing the timeParameters structure variable

        @param timeVals Date and Time timeParameters structure variable
        @return Return boolean
      */
      boolean setDateTime(struct timeParameters* timeVals);

      /**
        Set Seconds

        @param ss Seconds unsigned integer value
        @return Return boolean
      */
      boolean setSeconds(uint8_t ss);

      /**
        Set Minutes

        @param mm Minutes unsigned integer value
        @return Return boolean
      */
      boolean setMinutes(uint8_t mm);

      /**
        Set Hour by passing timeformat and meridian

        @param hh Hour unsigned integer value
        @param tf Boolean for Timeformat 12 / 24. True for 12 and false for 24
        @param md Boolean for Meridian AM / PM. True for PM and false for AM
        @return Return boolean
      */
      boolean setHour(uint8_t hh, boolean tf, boolean md);

      /**
        Set Day of the week

        @param day Day unsigned value. Week start with Sunday as 1 and Sturday as 7
        @return Return boolean
      */
      boolean setDay(uint8_t day);

      /**
        Set Date of the month

        @param date Date of the month unsigned integer value
        @return Return boolean
      */
      boolean setDate(uint8_t date);

      /**
        Set Month of the Year

        @param month Month of the year unsigned integer value
        @param century Centry value of the Year. If the year is great or equal to 2000 then it will be true or else false. By default it is true
        @return Return boolean
      */
      boolean setMonth(uint8_t month, boolean century = true);

      /**
        Set Year

        @param year Year unsigned integer value
        @return Return boolean
      */
      boolean setYear(uint16_t year);

      /**
        Get current Date and Time

        @param timeVals Pointer reference of the timeParameters structure variable
        @return Return boolean
      */
      boolean readDateTime(struct timeParameters* timeVals);

      /**
        Set Alaram on every period of Seconds / Minutes / Hour

        @param periodicity Choice value of unsigned integer of setting the Alarm
        @param alarm Alarm choice unsigned integer value. It can be Alarm1 / Alarm2
        @return Return boolean
      */
      boolean setAlarm(uint8_t periodicity, uint8_t alarm);

      /**
        Set Alarm matching for exact Seconds / Minutes / Hour with time format and meridian

        @param periodicity Choice value of unsigned integer of setting the Alarm
        @param val Unsigned integer value for the periodicity
        @param tf Timeformat choice in boolean. True for 12 hour format and false for 24 hour format
        @param md Boolean for Meridian AM / PM. True for PM and false for AM
        @param alarm Alarm choice unsigned integer value. It can be Alarm1 / Alarm2
        @return Return boolean
      */
      boolean setAlarm(uint8_t periodicity, uint8_t val, boolean tf, boolean md, uint8_t alarm);

      /**
        Set Alarm matching for exact Seconds and Minutes

        @param ss Unsigned integer value for Seconds
        @param mm Unsigned integer value for Minutes
        @param alarm Alarm choice unsigned integer value. It will only be Alarm1
        @return Return boolean
      */
      boolean setAlarm(uint8_t ss, uint8_t mm, uint8_t alarm);

      /**
        Set Alaram matching seconds, minutes and hour

        @param ss Unsigned integer value for Seconds
        @param mm Unsigned integer value for Minutes
        @param hh Unsigned integer value for Hour
        @param tf Boolean for Timeformat 12 / 24. True for 12 and false for 24
        @param md Boolean for Meridian AM / PM. True for PM and false for AM
        @param alarm Alarm choice unsigned integer value. It can be Alarm1 / Alarm2
        @return Return boolean
      */
      boolean setAlarm(uint8_t ss, uint8_t mm, uint8_t hh, boolean tf, boolean md, uint8_t alarm);

      /**
        Set Alaram matching seconds, minutes and hour

        @param ss Unsigned integer value for Seconds
        @param mm Unsigned integer value for Minutes
        @param hh Unsigned integer value for Hour
        @param dyDt Unsigned integer value for Day / Date
        @param dy Boolean for Day or Date selection. True for Day and false for Date
        @param tf Boolean for Timeformat 12 / 24. True for 12 and false for 24
        @param md Boolean for Meridian AM / PM. True for PM and false for AM
        @param alarm Alarm choice unsigned integer value. It can be Alarm1 / Alarm2
        @return Return boolean
      */
      boolean setAlarm(uint8_t ss, uint8_t mm, uint8_t hh, uint8_t dyDt,
        boolean dy, boolean tf, boolean md, uint8_t alarm);

      /**
        Enable Alarm

        @param alarm Alarm choice unsigned integer value. It can be Alarm1 / Alarm2
        @return Return void
      */
      void enableAlarm(uint8_t alarm);

      /**
        Disable Alarm

        @param alarm Alarm choice unsigned integer value. It can be Alarm1 / Alarm2
        @return Return void
      */
      void disableAlarm(uint8_t);

      /**
        Get Alarm status

        @param alarm Alarm choice unsigned integer value. It can be Alarm1 / Alarm2
        @return Return boolean
      */
      boolean alarmStatus(uint8_t alarm);

      /**
        Get Alarm time

        @param alarmVals Alarm time structure variable reference
        @param alarm Alarm choice unsigned integer value. It can be Alarm1 / Alarm2
        @return Return boolean
      */
      boolean readAlarmTime(struct alarmParameters* alarmVals, uint8_t alarm);

      /**
        Get Temperature

        @param scale Temperature scale selection unsigned integer variable
        @return Return float
      */
      float readTemp(uint8_t scale = Celsius);

      /**
        Force activate Temperature convertion by the chip

        @return Return boolean
      */
      boolean convertTemp(void);

      /*-------------------------------
              Control Register
      -------------------------------*/
      /**
        Get Control Register value

        @return Return the value of Control Register
      */
      uint8_t controlRegValue(void);

      /**
        Set EOSC bit on Control Register with 0 for start
        and 1 for stop when the device switches to VBAT

        @param choice Enable / Disable choice using unsigned interger value
        @return Return boolean
      */
      boolean enableDisableOscillator(uint8_t choice);

      /**
        Get EOSC bit on Control Register with 0 for start
        and 1 for stop when the device switches to VBAT

        @return Return the status of the oscillator in boolean
      */
      boolean oscillatorStatus(void);

      /**
        Set BBSQW bit on Control Register with 1 for start
        and 0 for stop with INTCN = 0 and VCC < VPF, enabling this generates
        1Hz square wave

        @param choice Enable / Disable choice using unsigned interger value
        @return Return boolean
      */
      boolean enableDisableBBSQW(uint8_t choice);

      /**
        Get BBSQW bit on Control Register with 1 for start
        and 0 for stop with INTCN = 0 and VCC < VPF, enabling this generates
        1Hz square wave

        @return Return the status of the BBSQW in boolean
      */
      boolean bbsqwStatus(void);

      /**
        Get INTCN bit on Control Register with 1 for enable and 0 for disable

        @return Return the status of the INTCN in boolean
      */
      boolean intcnStatus(void);

      /*-------------------------------
              Status Register
      -------------------------------*/
      /**
        Get Status Register value

        @return Return the value of Status Register
      */
      uint8_t statusRegValue(void);

      /**
        Get OSF (Oscillator stop flag) bit on Status Register with 1 for stop
        and 0 for start

        @return Return boolean
      */
      boolean osfStatus(void);

      /**
        Set EN32KHZ bit on Status Register with 0 for disable and 1 for enable.
        It Enable / Disable 32.768Khz Square Wave output

        @param choice Enable / Disable choice using unsigned interger value
        @return Return boolean
      */
      boolean enableDisable32Khz(uint8_t choice);

      /**
        Get EN32KHZ bit on Status Register with 1 for start and 0 for stop

        @return Return boolean
      */
      boolean en32KhzStatus(void);

      /**
        Get Temperature Busy Status

        @return Return void
      */
      boolean bsyStatus(void);

      /*-------------------------------
              Aging Register
      -------------------------------*/
      /**
        Get Aging Register value

        @return Return the value of Aging Register
      */
      uint8_t agingRegValue(void);

      /**
        Set Aging Register value, it takes user-provided value to add to or
        subtract from the factory-trimmed value that adjusts the accuracy of the
        time base

        @param val Signed integer value
        @return Return the value of Aging Register
      */
      boolean setAgingRegValue(int8_t val);
  };

#endif;

/******************************
         DS3231 RTC
******************************/
