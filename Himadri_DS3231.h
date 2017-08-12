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
    Control Register Address
  -------------------------------*/

  /*-------------------------------
        Timekeeping Register
  -------------------------------*/
  // Date And Time Registers
  #define DS3231_SECONDS_REG          0x00  // Seconds Register
  #define DS3231_MINUTES_REG          0x01  // Minutes Register
  #define DS3231_HOUR_REG             0x02  // Hour Register
  #define DS3231_DAY_REG               0x03  // Day Register
  #define DS3231_DATE_REG             0x04  // Date Register
  #define DS3231_MONTH_REG            0x05  // Month Register
  #define DS3231_YEAR_REG             0x06  // Year Register

  // Alarm1 Register
  #define DS3231_AL1SEC_REG           0x07  // Alaram1 Seconds Register
  #define DS3231_AL1MIN_REG           0x08  // Alarm1 Minutes Register
  #define DS3231_AL1HOUR_REG          0x09  // Alarm1 Hour Register
  #define DS3231_AL1WDAY_REG          0x0A  // Alarm1 Day Register

  // Temperature Registers
  #define DS3231_TEMPERATURE_MSG_REG  0x11  // Temperature MSB Register
  #define DS3231_TEMPERATURE_LSB_REG  0x12  // Temperature LSB Register
  /*-------------------------------
        Timekeeping Register
  -------------------------------*/

  /*-------------------------------
        Timeformat Choice
  -------------------------------*/
  #define TwelveHrFormat             0x01
  #define TwentyFourHrFormat         0x02

  /*-------------------------------
        Interrupt Choice
  -------------------------------*/
  #define EverySecond                 0x01
  #define EveryMinute                 0x02
  #define EveryHour                   0x03

  /*-------------------------------
        Structure Declaration
  -------------------------------*/
  typedef struct timeParameters12Hr {
    uint8_t ss;   // seconds
    uint8_t mm;   // minutes
    uint8_t hh;   // hour
    uint8_t md;   // meridian AM/PM -> 0 for AM / 1 for PM
    uint8_t day;  // day of the week
    uint8_t d;    // date of the month
    uint8_t m;    // month
    uint16_t y;   // year
  };

  typedef struct timeParameters24Hr {
    uint8_t ss;   // seconds
    uint8_t mm;   // minutes
    uint8_t hh;   // hour
    uint8_t day;  // day of the week
    uint8_t d;    // date of the month
    uint8_t m;    // month
    uint16_t y;    // year
  };

  class Himadri_DS3231 {
    /*-------------------------------
          Function Declaration
    -------------------------------*/
    private:
      static uint8_t dec2bcd(uint8_t);                                 // Binary to BCD number conversion
      static uint8_t bcd2dec(uint8_t);                                 // BCD to Binary number conversion
      uint8_t readRegister(uint8_t, uint8_t numByte=1);                    // Read from register address
      void writeRegister(uint8_t, uint8_t);                            // Write to register address

    public:
      boolean begin(void);                                             // Initialize all the register
      void clearTimeRegister(uint8_t);                                 // Clear Time register
      boolean setTime12Hr(struct timeParameters12Hr *timeVals);        // Set time using 12Hr format
      boolean setTime24Hr(struct timeParameters24Hr *timeVals);         // Set time using 24Hr format
      boolean readTime12Hr(struct timeParameters12Hr *timeVals);          // Get current time in 12Hr format
      boolean readTime24Hr(struct timeParameters24Hr *timeVals);          // Get current time in 24Hr format
  };

#endif;
