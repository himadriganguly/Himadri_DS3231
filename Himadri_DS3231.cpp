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
  17. Get Temperature
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

#include "Himadri_DS3231.h"

/*-------------------------------
  Private Function Definition
-------------------------------*/

/**
  Returns Decimal to BCD.
**/
static uint8_t Himadri_DS3231::dec2bcd(uint8_t value) {
  // Left shifting the first number by 4 bits and adding the second number we get the BCD number
  uint8_t convertedVal = (value / 10 * 16) + (value % 10);
  return convertedVal;
};

/**
  Returns BCD to Decimal.
**/
static uint8_t Himadri_DS3231::bcd2dec(uint8_t value) {
  // Right shifting the first number and mulitplying by 10 and then extracting the lastnumber using mod
  // after that adding it to the first number
  // modul or % in binary is n % 2^i = n & (2^i - 1)
  uint8_t convertedVal = (value / 16 * 10) + (value % 16);
  return convertedVal;
};

/**
  Returns 2 digit decimal from String.
**/
static uint8_t Himadri_DS3231::conv2d2(const char* p) {
    uint8_t v = 0;
    if ('0' <= *p && *p <= '9')
        v = *p - '0';
    return 10 * v + *++p - '0';
};

/**
  Returns 4 digit decimal from String.
**/
static uint16_t Himadri_DS3231::conv2d4(const char* p) {
    uint16_t v = 0;
    if ('0' <= *p && *p <= '9')
        v = *p - '0';
    v = v * pow(10, 3);
    p = p + 1;
    v = v + ((*p - '0') * pow(10, 2));
    p = p + 1;
    v = v + ((*p - '0') * pow(10, 1));
    p = p + 1;
    v = v + (*p - '0');
    return v;
};

/**
  Validate Date, Month and Year
**/
boolean Himadri_DS3231::monthDateYearValidation(uint8_t date, uint8_t month, uint16_t year) {
  if(date <= 31 && date >= 0 && month <=12 && month >= 0 && year <= 2099 && year >= 1900) {
    if(month == 2 && date == 29) {
      if((year % 4 == 0) || ((year % 4 == 0) &&
        (year % 100 ==0) && (year % 400 == 0))) {
        return true;
      } else {
        return false;
      }
    } else {
      if(pgm_read_byte(daysInMonth + month - 1) >= date) {
        return true;
      } else {
        return false;
      }
    }
  } else {
    return false;
  }
}

/**
  Reads 8 bit register value from the address specified.
**/
uint8_t Himadri_DS3231::readRegister(uint8_t regaddress) {
    Wire.beginTransmission(DS3231_ADDRESS);
    #if (ARDUINO >= 100)
      Wire.write((byte)regaddress);
    #else
      Wire.send((byte)regaddress);
    #endif
    Wire.endTransmission();

    Wire.requestFrom(DS3231_ADDRESS, 1, true);
    #if (ARDUINO >= 100)
      return Wire.read();      // receive DATA
    #else
      return Wire.receive();   // receive DATA
    #endif
};

/**
  Writes 8 bit value to the register address specified.
**/
void Himadri_DS3231::writeRegister(uint8_t regaddress, uint8_t value) {
    Wire.beginTransmission(DS3231_ADDRESS);

    #if (ARDUINO >= 100)
      Wire.write((byte)regaddress);
      Wire.write((byte)value);
    #else
      Wire.send((byte)regaddress);
      Wire.send((byte)value);
    #endif
    Wire.endTransmission();
};

/*-------------------------------
  Private Function Definition
-------------------------------*/

/**
  Clear Alarm Flag in Status Register.
**/
void Himadri_DS3231::clearINTStatus(uint8_t alarm)
{
    // Clear interrupt flag
    uint8_t statusReg = readRegister(DS3231_STATUS_REG);
    if(alarm == 1) {
      statusReg &= 0xFE;        // 11111110
      writeRegister(DS3231_STATUS_REG, statusReg);
    } else if(alarm == 2) {
      statusReg &= 0xFD;        // 11111101
      writeRegister(DS3231_STATUS_REG, statusReg);
    } else if(alarm == 3) {
      statusReg &= 0xFC;        // 11111100
      writeRegister(DS3231_STATUS_REG, statusReg);
    }
};

/**
  Initialize the Timekeeping Register value with initial Date and Time of 1900/01/01 WED 00:00:00AM or 00:00:00.
**/
void Himadri_DS3231::clearTimeRegister(uint8_t timeFormat) {
  Wire.beginTransmission(DS3231_ADDRESS);

  #if (ARDUINO >= 100)
    Wire.write(DS3231_SECONDS_REG);
    Wire.write(dec2bcd(0));             // seconds
    Wire.write(dec2bcd(0));             // minutes
    if(timeFormat == 0x01) {            // 00000001
      Wire.write(dec2bcd(0) | 0x40);    // 01000000
    } else {
      Wire.write(dec2bcd(0));           // hour
    }
    Wire.write(dec2bcd(4));             // day
    Wire.write(dec2bcd(1));             // date
    Wire.write(dec2bcd(1));             // month
    Wire.write(dec2bcd(0));             // year
  #else
    Wire.send(DS3231_SECONDS_REG);
    Wire.send(dec2bcd(0));              // seconds
    Wire.send(dec2bcd(0));              // minutes
    if(timeFormat == 0x01) {
      Wire.send(dec2bcd(0) | 0x40);     // 01000000
    } else {
      Wire.send(dec2bcd(0));            // hour
    }
    Wire.send(dec2bcd(2));              // day
    Wire.send(dec2bcd(1));              // date
    Wire.send(dec2bcd(1));              // month
    Wire.send(dec2bcd(0));              // year
  #endif
  Wire.endTransmission();
};

/**
  Initialize the Control Register by disabling both the Alarm.
  Also clear the both Alarm Flag from Status Register.
**/
boolean Himadri_DS3231::begin() {
  Wire.begin();
  Wire.beginTransmission(DS3231_ADDRESS);
  if(Wire.endTransmission() != 0) {
    Serial.println("Cannot communicate with device.");
    return false;
  } else {
    Serial.println("Can communicate with device.");
  }
  // Initialize the Control Register
  uint8_t ctReg = readRegister(DS3231_CONTROL_REG);
  ctReg &= 0xFC;                                // 11111100
  writeRegister(DS3231_CONTROL_REG, ctReg);     //CONTROL Register Address
  clearINTStatus(BothAlarm);
  return true;
};

/**
  Initialize Date and Time structure variable by passing date and time as string
**/
boolean Himadri_DS3231::initializeDateTime(const char* date, const char* time, struct timeParameters* timeVals) {
  // sample input: date = "Jan 22 1991 MON", time = "10:20:13 AM" / time = "22:20:12"
  if(time[9] == 'A' || time[9] == 'P') {
    timeVals->tf = 1;
    switch(time[9]) {
      case 'A': timeVals->md = time[10] == 'M' ? 0 : 3; break;
      case 'P': timeVals->md = time[10] == 'M' ? 1 : 3; break;
      default: timeVals->md = 3; break;
    }
  } else {
    timeVals->tf = 0;
    timeVals->md = 3;
  }

  // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
  switch(date[0]) {
      case 'J': timeVals->m = date[1] == 'a' ? 1 : date[2] == 'n' ? 6 : 7; break;
      case 'F': timeVals->m = 2; break;
      case 'A': timeVals->m = date[2] == 'r' ? 4 : 8; break;
      case 'M': timeVals->m = date[2] == 'r' ? 3 : 5; break;
      case 'S': timeVals->m = 9; break;
      case 'O': timeVals->m = 10; break;
      case 'N': timeVals->m = 11; break;
      case 'D': timeVals->m = 12; break;
      default: timeVals->m = 13; break;
  }

  // sample input: date = "Jan 22 1991 MON", time = "10:20:13 AM" / time = "22:20:12"

  // SUN MON TUE WED THU FRI SAT
  switch(date[12]) {
    case 'S': timeVals->day = date[13] == 'U' ? 1 : 7; break;
    case 'M': timeVals->day = 2; break;
    case 'T': timeVals->day = date[13] == 'U' ? 3 : 5; break;
    case 'W': timeVals->day = 4; break;
    case 'F': timeVals->day = 6; break;
    default: timeVals->day = 1; break;
  }

  timeVals->d = conv2d2(date + 4);
  timeVals->y = conv2d4(date + 7);

  if(monthDateYearValidation(timeVals->d, timeVals->m, timeVals->y) == false) {
    Serial.println("I am in error.");
    timeVals = {}; // reset
    return false;
  }
  timeVals->hh = conv2d2(time);
  timeVals->mm = conv2d2(time + 3);
  timeVals->ss = conv2d2(time + 6);

  return true;
};

/**
  Set Date and Time in 12 / 24 Hr format by passing the timeParameters structure variable
**/
boolean Himadri_DS3231::setDateTime(struct timeParameters* timeVals) {
  // uint8_t tf;   // time format -> 0 for 24Hr format / 1 for 12Hr format
  // uint8_t ss;   // seconds
  // uint8_t mm;   // minutes
  // uint8_t hh;   // hour
  // uint8_t md;   // meridian AM/PM -> 0 for AM / 1 for PM / 3 for None
  // uint8_t day;  // day of the week
  // uint8_t d;    // date of the month
  // uint8_t m;    // month
  // uint16_t y;   // year

  if(monthDateYearValidation(timeVals->d, timeVals->m, timeVals->y) == false) {
    timeVals = {}; // reset
    return false;
  }

  Wire.beginTransmission(DS3231_ADDRESS);

  #if (ARDUINO >= 100)
    Wire.write((byte)DS3231_SECONDS_REG);

    // Setting seconds register
    if(timeVals->ss <= 59 && timeVals->ss >= 0) {
      Wire.write((byte)dec2bcd(timeVals->ss));
    } else {
      if(timeVals->tf == 1) {
        clearTimeRegister(TwelveHrFormat);
      } else {
        clearTimeRegister(TwentyFourHrFormat);
      }
      Wire.endTransmission();
      return false;
    }

    // Setting minutes register
    if(timeVals->mm <= 59 && timeVals->mm >= 0) {
      Wire.write((byte)dec2bcd(timeVals->mm));
    } else {
      if(timeVals->tf == 1) {
        clearTimeRegister(TwelveHrFormat);
      } else {
        clearTimeRegister(TwentyFourHrFormat);
      }
      Wire.endTransmission();
      return false;
    }

    // Checking the time format
    if(timeVals->tf == 1) {
      if(timeVals->md == 0 && timeVals->hh <= 12 && timeVals->hh >= 1) {
        // Changed the bit 6 of Hours register to 1 to set 12Hr format and bit 5 of Hours register to 0 to set meridian to AM
        timeVals->hh = dec2bcd(timeVals->hh) | 0x40;    // 01000000
        timeVals->hh &= 0xDF;                           // 11011111
        Wire.write((byte)timeVals->hh);
      } else if(timeVals->md == 1 && timeVals->hh <= 12 && timeVals->hh >= 1) {
        // Changed the bit 6 of Hours register to 1 to set 12Hr format and bit 5 of Hours register to 1 to set meridian to PM
        timeVals->hh = dec2bcd(timeVals->hh) | 0x60;    // 01100000
        Wire.write((byte)timeVals->hh);
      } else {
        if(timeVals->tf == 1) {
          clearTimeRegister(TwelveHrFormat);
        } else {
          clearTimeRegister(TwentyFourHrFormat);
        }
        Wire.endTransmission();
        return false;
      }
    } else {
      if(timeVals->hh <= 23 && timeVals->hh >= 0) {
        // Changed the bit 6 of Hours register to 0 to set 24Hr format
        timeVals->hh = dec2bcd(timeVals->hh) & 0xBF;  // 10111111
        Wire.write((byte)timeVals->hh);
      } else {
        if(timeVals->tf == 1) {
          clearTimeRegister(TwelveHrFormat);
        } else {
          clearTimeRegister(TwentyFourHrFormat);
        }
        Wire.endTransmission();
        return false;
      }
    }

    // Setting day register
    if(timeVals->day >= 1 && timeVals->day <= 7) {
      Wire.write((byte)dec2bcd(timeVals->day));
    } else {
      if(timeVals->tf == 1) {
        clearTimeRegister(TwelveHrFormat);
      } else {
        clearTimeRegister(TwentyFourHrFormat);
      }
      Wire.endTransmission();
      return false;
    }

    // Setting dates register
    if(timeVals->d > 0 && timeVals->d <= 31) {
      Wire.write((byte)dec2bcd(timeVals->d));
    } else {
      if(timeVals->tf == 1) {
        clearTimeRegister(TwelveHrFormat);
      } else {
        clearTimeRegister(TwentyFourHrFormat);
      }
      Wire.endTransmission();
      return false;
    }

    // Setting month and year register
    if(timeVals->y >= 2000 && timeVals->y < 2100) {
      timeVals->m = dec2bcd(timeVals->m) | 0x80;  // 10000000
      timeVals->y = timeVals->y - 2000;
      Wire.write((byte)timeVals->m);
      Wire.write((byte)dec2bcd(timeVals->y));
    } else if (timeVals->y >= 1900) {
      timeVals->m = dec2bcd(timeVals->m) & 0x7F;  // 01111111
      timeVals->y = timeVals->y - 1900;
      Wire.write((byte)timeVals->m);
      Wire.write((byte)dec2bcd(timeVals->y));
    } else {
      if(timeVals->tf == 1) {
        clearTimeRegister(TwelveHrFormat);
      } else {
        clearTimeRegister(TwentyFourHrFormat);
      }
      Wire.endTransmission();
      return false;
    }
  #else
    Wire.send((byte)DS3231_SECONDS_REG);

    // Setting seconds register
    if(timeVals->ss <= 59 && timeVals->ss >= 0) {
      Wire.send((byte)dec2bcd(timeVals->ss));
    } else {
      if(timeVals->tf == 1) {
        clearTimeRegister(TwelveHrFormat);
      } else {
        clearTimeRegister(TwentyFourHrFormat);
      }
      Wire.endTransmission();
      return false;
    }

    // Setting minutes register
    if(timeVals->mm <= 59 && timeVals->mm >= 0) {
      Wire.send((byte)dec2bcd(timeVals->mm));
    } else {
      if(timeVals->tf == 1) {
        clearTimeRegister(TwelveHrFormat);
      } else {
        clearTimeRegister(TwentyFourHrFormat);
      }
      Wire.endTransmission();
      return false;
    }

    // Checking the time format
    if(timeVals->tf == 1) {
      if(timeVals->md == 0 && timeVals->hh <= 12 && timeVals->hh >= 1) {
        // Changed the bit 6 of Hours register to 1 to set 12Hr format and bit 5 of Hours register to 0 to set meridian to AM
        timeVals->hh = dec2bcd(timeVals->hh) | 0x40;    // 01000000
        timeVals->hh &= 0xDF; // 11011111
        Wire.send((byte)timeVals->hh);
      } else if(timeVals->md == 1 && timeVals->hh <= 12 && timeVals->hh >= 1) {
        // Changed the bit 6 of Hours register to 1 to set 12Hr format and bit 5 of Hours register to 1 to set meridian to PM
        timeVals->hh = dec2bcd(timeVals->hh) | 0x60;    // 01100000
        Wire.send((byte)timeVals->hh);
      } else {
        if(timeVals->tf == 1) {
          clearTimeRegister(TwelveHrFormat);
        } else {
          clearTimeRegister(TwentyFourHrFormat);
        }
        Wire.endTransmission();
        return false;
      }
    } else {
      if(timeVals->hh <= 23 && timeVals->hh >= 0) {
        // Changed the bit 6 of Hours register to 0 to set 24Hr format
        timeVals->hh = dec2bcd(timeVals->hh) & 0xBF;  // 10111111
        Wire.send((byte)timeVals->hh);
      } else {
        if(timeVals->tf == 1) {
          clearTimeRegister(TwelveHrFormat);
        } else {
          clearTimeRegister(TwentyFourHrFormat);
        }
        Wire.endTransmission();
        return false;
      }
    }

    // Setting day register
    if(timeVals->day >= 1 && timeVals->day <= 7) {
      Wire.send((byte)dec2bcd(timeVals->day));
    } else {
      if(timeVals->tf == 1) {
        clearTimeRegister(TwelveHrFormat);
      } else {
        clearTimeRegister(TwentyFourHrFormat);
      }
      Wire.endTransmission();
      return false;
    }

    // Setting dates register
    if(timeVals->d > 0 && timeVals->d <= 31) {
      Wire.send((byte)dec2bcd(timeVals->d));
    } else {
      if(timeVals->tf == 1) {
        clearTimeRegister(TwelveHrFormat);
      } else {
        clearTimeRegister(TwentyFourHrFormat);
      }
      Wire.endTransmission();
      return false;
    }

    // Setting month and year register
    if(timeVals->y >= 2000 && timeVals->y < 2100) {
      timeVals->m = dec2bcd(timeVals->m) | 0x80;    // 10000000
      timeVals->y = timeVals->y - 2000;
      Wire.send((byte)timeVals->m);
      Wire.send((byte)dec2bcd(timeVals->y));
    } else if (timeVals->y >= 1900) {
      timeVals->m = dec2bcd(timeVals->m) & 0x7F;    // 01111111
      timeVals->y = timeVals->y - 1900;
      Wire.send((byte)timeVals->m);
      Wire.send((byte)dec2bcd(timeVals->y));
    } else {
      if(timeVals->tf == 1) {
        clearTimeRegister(TwelveHrFormat);
      } else {
        clearTimeRegister(TwentyFourHrFormat);
      }
      Wire.endTransmission();
      return false;
    }
  #endif

  Wire.endTransmission();
  return true;
};

/**
  Set Seconds
**/
boolean Himadri_DS3231::setSeconds(uint8_t ss) {
  Wire.beginTransmission(DS3231_ADDRESS);

  #if (ARDUINO >= 100)
    Wire.write((byte)DS3231_SECONDS_REG);

    // Setting seconds register
    if(ss <= 59 && ss >= 0) {
      Wire.write((byte)dec2bcd(ss));
    } else {
      Wire.endTransmission();
      return false;
    }
  #else
    Wire.send((byte)DS3231_SECONDS_REG);

    // Setting seconds register
    if(ss <= 59 && ss >= 0) {
      Wire.send((byte)dec2bcd(ss));
    } else {
      Wire.endTransmission();
      return false;
    }
  #endif

  Wire.endTransmission();
  return true;
};

/**
  Set Minutes
**/
boolean Himadri_DS3231::setMinutes(uint8_t mm) {
  Wire.beginTransmission(DS3231_ADDRESS);

  #if (ARDUINO >= 100)
    Wire.write((byte)DS3231_MINUTES_REG);

    // Setting seconds register
    if(mm <= 59 && mm >= 0) {
      Wire.write((byte)dec2bcd(mm));
    } else {
      Wire.endTransmission();
      return false;
    }
  #else
    Wire.send((byte)DS3231_MINUTES_REG);

    // Setting seconds register
    if(mm <= 59 && mm >= 0) {
      Wire.send((byte)dec2bcd(mm));
    } else {
      Wire.endTransmission();
      return false;
    }
  #endif

  Wire.endTransmission();
  return true;
};

/**
  Set Hour by passing timeformat and meridian
**/
boolean Himadri_DS3231::setHour(uint8_t hh, boolean tf, boolean md) {
  Wire.beginTransmission(DS3231_ADDRESS);

  #if (ARDUINO >= 100)
    Wire.write((byte)DS3231_HOUR_REG);

    // Checking the time format
    if(tf == true) {
      if(md == false && hh <= 12 && hh >= 1) {
        // Changed the bit 6 of Hours register to 1 to set 12Hr format and bit 5 of Hours register to 0 to set meridian to AM
        hh = dec2bcd(hh) | 0x40;    // 01000000
        hh &= 0xDF; // 11011111
        Wire.write((byte)hh);
      } else if(md == true && hh <= 12 && hh >= 1) {
        // Changed the bit 6 of Hours register to 1 to set 12Hr format and bit 5 of Hours register to 1 to set meridian to PM
        hh = dec2bcd(hh) | 0x60;    // 01100000
        Wire.write((byte)hh);
      } else {
        Wire.endTransmission();
        return false;
      }
    } else {
      if(hh <= 23 && hh >= 0) {
        // Changed the bit 6 of Hours register to 0 to set 24Hr format
        hh = dec2bcd(hh) & 0xBF;  // 10111111
        Wire.write((byte)hh);
      } else {
        Wire.endTransmission();
        return false;
      }
    }
  #else
    Wire.send((byte)DS3231_HOUR_REG);

    // Checking the time format
    if(tf == true) {
      if(md == false && hh <= 12 && hh >= 1) {
        // Changed the bit 6 of Hours register to 1 to set 12Hr format and bit 5 of Hours register to 0 to set meridian to AM
        hh = dec2bcd(hh) | 0x40;    // 01000000
        hh &= 0xDF; // 11011111
        Wire.send((byte)hh);
      } else if(md == true && hh <= 12 && hh >= 1) {
        // Changed the bit 6 of Hours register to 1 to set 12Hr format and bit 5 of Hours register to 1 to set meridian to PM
        hh = dec2bcd(hh) | 0x60;    // 01100000
        Wire.send((byte)hh);
      } else {
        Wire.endTransmission();
        return false;
      }
    } else {
      if(hh <= 23 && hh >= 0) {
        // Changed the bit 6 of Hours register to 0 to set 24Hr format
        hh = dec2bcd(hh) & 0xBF;  // 10111111
        Wire.send((byte)hh);
      } else {
        Wire.endTransmission();
        return false;
      }
    }
  #endif

  Wire.endTransmission();
  return true;
};

/**
  Set Day of the week
**/
boolean Himadri_DS3231::setDay(uint8_t day) {
  Wire.beginTransmission(DS3231_ADDRESS);

  #if (ARDUINO >= 100)
    Wire.write((byte)DS3231_DAY_REG);

    // Setting day register
    if(day >= 1 && day <= 7) {
      Wire.write((byte)dec2bcd(day));
    } else {
      Wire.endTransmission();
      return false;
    }
  #else
    Wire.send((byte)DS3231_DAY_REG);

    // Setting day register
    if(day >= 1 && day <= 7) {
      Wire.send((byte)dec2bcd(day));
    } else {
      Wire.endTransmission();
      return false;
    }
  #endif

  Wire.endTransmission();
  return true;
};

/**
  Set Date of the month
**/
boolean Himadri_DS3231::setDate(uint8_t date) {
  uint8_t month = readRegister(DS3231_MONTH_REG);
  uint16_t year = 0;
  if((month & 0x80) >> 7 == 1) {            // 10000000
    year = 2000 + readRegister(DS3231_YEAR_REG);
    month = bcd2dec(month & 0x7F);          // 01111111
  } else {
    year = 1900 + readRegister(DS3231_YEAR_REG);
    month = bcd2dec(month & 0x7F);          // 01111111
  }

  if(monthDateYearValidation(date, month, year) == false) {
    return false;
  }

  Wire.beginTransmission(DS3231_ADDRESS);

  #if (ARDUINO >= 100)
    Wire.write((byte)DS3231_DATE_REG);

    // Setting date register
    if(date >= 1 && date <= 31) {
      Wire.write((byte)dec2bcd(date));
    } else {
      Wire.endTransmission();
      return false;
    }
  #else
    Wire.send((byte)DS3231_DATE_REG);

    // Setting date register
    if(date >= 1 && date <= 31) {
      Wire.send((byte)dec2bcd(date));
    } else {
      Wire.endTransmission();
      return false;
    }
  #endif

  Wire.endTransmission();
  return true;
};

/**
  Set Month of the Year
**/
boolean Himadri_DS3231::setMonth(uint8_t month, boolean century = true) {
  Wire.beginTransmission(DS3231_ADDRESS);

  #if (ARDUINO >= 100)
    Wire.write((byte)DS3231_MONTH_REG);

    // Setting day register
    if(month >= 1 && month <= 12) {
      if(century == true) {
        Wire.write((byte)dec2bcd(month) | 0x80);        // 10000000
      } else {
        Wire.write((byte)dec2bcd(month) & 0x7F);        // 01111111
      }
    } else {
      Wire.endTransmission();
      return false;
    }
  #else
    Wire.send((byte)DS3231_MONTH_REG);

    // Setting day register
    if(month >= 1 && month <= 12) {
      if(century == true) {
        Wire.send((byte)dec2bcd(month) | 0x80);        // 10000000
      } else {
        Wire.send((byte)dec2bcd(month) & 0x7F);        // 01111111
      }
    } else {
      Wire.endTransmission();
      return false;
    }
  #endif

  Wire.endTransmission();
  return true;
};

/**
  Set Year
**/
boolean Himadri_DS3231::setYear(uint16_t year) {
  // Setting month and year register
  if(year >= 2000 && year < 2100) {
    uint8_t month = readRegister(DS3231_MONTH_REG);
    writeRegister(DS3231_MONTH_REG, (month | 0x80));          // 10000000
    year = year - 2000;
    writeRegister(DS3231_YEAR_REG, dec2bcd(year));
  } else if (year >= 1900) {
    uint8_t month = readRegister(DS3231_MONTH_REG);
    writeRegister(DS3231_MONTH_REG, (month & 0x7F));          // 10000000
    year = year - 1900;
    writeRegister(DS3231_YEAR_REG, dec2bcd(year));
  } else {
    return false;
  }

  return true;
};

/**
  Get current Date and Time
**/
boolean Himadri_DS3231::readDateTime(struct timeParameters* timeVals) {
  Wire.beginTransmission(DS3231_ADDRESS);

  #if (ARDUINO >= 100)
    Wire.write(DS3231_SECONDS_REG);
  #else
    Wire.send(DS3231_SECONDS_REG);
  #endif
  Wire.endTransmission();

  Wire.requestFrom(DS3231_ADDRESS, 7, true);

  #if (ARDUINO >= 100)
    timeVals->ss = bcd2dec(Wire.read());
    timeVals->mm = bcd2dec(Wire.read());
    timeVals->hh = Wire.read();
    if((timeVals->hh & 0x40) >> 6 == 1) {
      timeVals->tf = 1;
      if((timeVals->hh & 0x20) >> 5 == 1) {           // 00100000
        timeVals->md = 1;
      } else {
        timeVals->md = 0;
      }
      timeVals->hh = bcd2dec(timeVals->hh & 0x1F);    // 00011111
    } else {
      timeVals->hh = bcd2dec(Wire.read() & 0x3F);     // 00111111
    }
    timeVals->day = bcd2dec(Wire.read());
    timeVals->d = bcd2dec(Wire.read());
    timeVals->m = Wire.read();
    timeVals->y = bcd2dec(Wire.read());
    if((timeVals->m & 0x80) >> 7 == 1) {            // 10000000
      timeVals->y = 2000 + timeVals->y;
      timeVals->m = bcd2dec(timeVals->m & 0x7F);    // 01111111
    } else {
      timeVals->y = 1900 + timeVals->y;
      timeVals->m = bcd2dec(timeVals->m & 0x7F);    // 01111111
    }
  #else
    timeVals->ss = bcd2dec(Wire.receive());
    timeVals->mm = bcd2dec(Wire.receive());
    timeVals->hh = Wire.receive();
    if((alarmVals->hh & 0x40) >> 6 == 1) {
      timeVals->tf = 1;
      if((timeVals->hh & 0x20) >> 5 == 1) {           // 00100000
        timeVals->md = 1;
      } else {
        timeVals->md = 0;
      }
      timeVals->hh = bcd2dec(timeVals->hh & 0x1F);    // 00011111
    } else {
      timeVals->hh = bcd2dec(Wire.read() & 0x3F);     // 00111111
    }
    timeVals->day = bcd2dec(Wire.receive());
    timeVals->d = bcd2dec(Wire.receive());
    timeVals->m = Wire.receive();
    timeVals->y = bcd2dec(Wire.receive());
    if((timeVals->m & 0x80) >> 7 == 1) {            // 10000000
      timeVals->y = 2000 + timeVals->y;
      timeVals->m = bcd2dec(timeVals->m & 0x7F);    // 01111111
    } else {
      timeVals->y = 1900 + timeVals->y;
      timeVals->m = bcd2dec(timeVals->m & 0x7F);    // 01111111
    }
  #endif

  return true;
};

/**
  Set Alaram on every period of Seconds / Minutes / Hour
**/
boolean Himadri_DS3231::setAlarm(uint8_t periodicity, uint8_t alarm) {
  Wire.beginTransmission(DS3231_ADDRESS);
  switch(periodicity)
  {
      case Second:
        #if (ARDUINO >= 100)
          if(alarm == 1) {
            Wire.write((byte)DS3231_AL1SEC_REG);
            Wire.write(128);                                   // 10000000
            Wire.write(128);                                   // 10000000
            Wire.write(128);                                   // 10000000
            Wire.write(128);                                   // 10000000
          } else {
            Wire.endTransmission();
            return false;
          }
        #else
          if(alarm == 1) {
            Wire.send((byte)DS3231_AL1SEC_REG);
            Wire.send(128);                                    // 10000000
            Wire.send(128);                                    // 10000000
            Wire.send(128);                                    // 10000000
            Wire.send(128);                                    // 10000000
          } else {
            Wire.endTransmission();
            return false;
          }
        #endif
      break;
      case Minute:
        #if (ARDUINO >= 100)
          if(alarm == 1) {
            Wire.write((byte)DS3231_AL1SEC_REG);
            Wire.write(0);                                     // 00000000
          } else if(alarm == 2) {
            Wire.write((byte)DS3231_AL2MIN_REG);
          } else {
            Wire.endTransmission();
            return false;
          }
          Wire.write(128);                                    // 10000000
          Wire.write(128);                                    // 10000000
          Wire.write(128);                                    // 10000000
        #else
          if(alarm == 1) {
            Wire.write((byte)DS3231_AL1SEC_REG);
            Wire.send(0);                                     // 00000000
          } else if(alarm == 2) {
            Wire.write((byte)DS3231_AL2MIN_REG);
          } else {
            Wire.endTransmission();
            return false;
          }
          Wire.send(128);                                     // 10000000
          Wire.send(128);                                     // 10000000
          Wire.send(128);                                     // 10000000
        #endif
      break;
      case Hour:
        #if (ARDUINO >= 100)
          if(alarm == 1) {
            Wire.write((byte)DS3231_AL1SEC_REG);
            Wire.write(0);                                   // 00000000
          } else if(alarm == 2) {
            Wire.write((byte)DS3231_AL2MIN_REG);
          } else {
            Wire.endTransmission();
            return false;
          }
          Wire.write(0);                                     // 00000000
          Wire.write(128);                                   // 10000000
          Wire.write(128);                                   // 10000000
        #else
          if(alarm == 1) {
            Wire.write((byte)DS3231_AL1SEC_REG);
            Wire.write(0);                                  // 00000000
          } else if(alarm == 2) {
            Wire.write((byte)DS3231_AL2MIN_REG);
          } else {
            Wire.endTransmission();
            return false;
          }
          Wire.send(0);                                      // 00000000
          Wire.send(128);                                    // 10000000
          Wire.send(128);                                    // 10000000
        #endif
      break;
      default:
        Wire.endTransmission();
        return false;
      break;
  };
  Wire.endTransmission();
  return true;
};

/**
  Set Alarm matching for exact Seconds / Minutes / Hour with time format and meridian
**/
boolean Himadri_DS3231::setAlarm(uint8_t periodicity, uint8_t val, boolean tf, boolean md, uint8_t alarm) {
  Wire.beginTransmission(DS3231_ADDRESS);

  switch(periodicity)
  {
      case Second:
        #if (ARDUINO >= 100)
          if(alarm == 1) {
            if(val <= 59 && val >=0) {
              Wire.write((byte)DS3231_AL1SEC_REG);
              Wire.write((byte)dec2bcd(val) & 0x7F);             // 10000000
              Wire.write(128);                                   // 10000000
              Wire.write(128);                                   // 10000000
              Wire.write(128);                                   // 10000000
            } else {
              Wire.endTransmission();
              return false;
            }
          } else {
            Wire.endTransmission();
            return false;
          }
        #else
        if(alarm == 1) {
          if(val <= 59 && val >=0) {
            Wire.send((byte)DS3231_AL1SEC_REG);
            Wire.send((byte)dec2bcd(val) & 0x7F);             // 10000000
            Wire.send(128);                                   // 10000000
            Wire.send(128);                                   // 10000000
            Wire.send(128);                                   // 10000000
          } else {
            Wire.endTransmission();
            return false;
          }
        } else {
          Wire.endTransmission();
          return false;
        }
        #endif
      break;
      case Minute:
        #if (ARDUINO >= 100)
          if(val <= 59 && val >=0) {
            if(alarm == 1) {
              Wire.write((byte)DS3231_AL1SEC_REG);
              Wire.write(0);                                      // 00000000
            } else if(alarm == 2) {
              Wire.write((byte)DS3231_AL2MIN_REG);
            } else {
              Wire.endTransmission();
              return false;
            }
            Wire.write((byte)dec2bcd(val) & 0x7F);              // 10000000
            Wire.write(128);                                    // 10000000
            Wire.write(128);                                    // 10000000
          } else {
            Wire.endTransmission();
            return false;
          }
        #else
          if(val <= 59 && val >=0) {
            if(alarm == 1) {
              Wire.send((byte)DS3231_AL1SEC_REG);
              Wire.send(0);                                      // 00000000
            } else if(alarm == 2) {
              Wire.send((byte)DS3231_AL2MIN_REG);
            } else {
              Wire.endTransmission();
              return false;
            }
            Wire.send((byte)dec2bcd(val) & 0x7F);              // 10000000
            Wire.send(128);                                    // 10000000
            Wire.send(128);                                    // 10000000
          } else {
            Wire.endTransmission();
            return false;
          }
        #endif
      break;
      case Hour:
        #if (ARDUINO >= 100)
          if(alarm == 1) {
            Wire.write((byte)DS3231_AL1SEC_REG);
            Wire.write(0);                                     // 00000000
          } else if(alarm == 2) {
            Wire.write((byte)DS3231_AL2MIN_REG);
          } else {
            Wire.endTransmission();
            return false;
          }
          Wire.write(0);                                        // 00000000
          if(tf == true && md == true) {
            if(val <= 12 && val >=1) {
              Wire.write((byte)dec2bcd(val) & 0x7F | 0x60);    // 01111111 | 01100000
            } else {
              Wire.endTransmission();
              return false;
            }
          } else if(tf == true && md == false) {
            if(val <= 12 && val >=1) {
              Wire.write((byte)dec2bcd(val) & 0x5F | 0x40);    // 01011111 | 01000000
            } else {
              Wire.endTransmission();
              return false;
            }
          } else {
            if(val <= 23 && val >=0) {
              Wire.write((byte)dec2bcd(val) & 0x3F);           // 00111111
            } else {
              Wire.endTransmission();
              return false;
            }
          }
          Wire.write(128);                                   // 10000000
        #else
          if(alarm == 1) {
            Wire.send((byte)DS3231_AL1SEC_REG);
            Wire.send(0);                                     // 00000000
          } else if(alarm == 2) {
            Wire.send((byte)DS3231_AL2MIN_REG);
          } else {
            Wire.endTransmission();
            return false;
          }
          Wire.send(0);                                        // 00000000
          if(tf == true && md == true) {
            if(val <= 12 && val >=1) {
              Wire.send((byte)dec2bcd(val) & 0x7F | 0x60);    // 01111111 | 01100000
            } else {
              Wire.endTransmission();
              return false;
            }
          } else if(tf == true && md == false) {
            if(val <= 12 && val >=1) {
              Wire.send((byte)dec2bcd(val) & 0x5F | 0x40);    // 01011111 | 01000000
            } else {
              Wire.endTransmission();
              return false;
            }
          } else {
            if(val <= 23 && val >=0) {
              Wire.send((byte)dec2bcd(val) & 0x3F);           // 00111111
            } else {
              Wire.endTransmission();
              return false;
            }
          }
          Wire.send(128);                                   // 10000000
        #endif
      break;
      default:
        Wire.endTransmission();
        return false;
      break;
  };
  Wire.endTransmission();
  return true;
};

/**
  Set Alarm matching for exact Seconds and Minutes
**/
boolean Himadri_DS3231::setAlarm(uint8_t ss, uint8_t mm, uint8_t alarm) {
  Wire.beginTransmission(DS3231_ADDRESS);

  #if (ARDUINO >= 100)
    if(alarm == 1) {
      Wire.write((byte)DS3231_AL1SEC_REG);

      if(ss <= 59 && ss >= 0 && mm <= 59 && mm >= 0) {
        Wire.write((byte)dec2bcd(ss) & 0x7F);             // 01111111
        Wire.write((byte)dec2bcd(mm) & 0x7F);             // 01111111
        Wire.write(128);                                  // 10000000
        Wire.write(128);                                  // 10000000
      } else {
        Wire.endTransmission();
        return false;
      }
    } else {
      Wire.endTransmission();
      return false;
    }
  #else
    if(alarm == 1) {
      Wire.send((byte)DS3231_AL1SEC_REG);

      if(ss <= 59 && ss >= 0 && mm <= 59 && mm >= 0) {
        Wire.send((byte)dec2bcd(ss) & 0x7F);             // 01111111
        Wire.send((byte)dec2bcd(mm) & 0x7F);             // 01111111
        Wire.send(128);                                  // 10000000
        Wire.send(128);                                  // 10000000
      } else {
        Wire.endTransmission();
        return false;
      }
    } else {
      Wire.endTransmission();
      return false;
    }
  #endif
  Wire.endTransmission();
  return true;
};

/**
  Set Alaram matching seconds, minutes and hour
*/
boolean Himadri_DS3231::setAlarm(uint8_t ss, uint8_t mm, uint8_t hh, boolean tf, boolean md, uint8_t alarm) {
  Wire.beginTransmission(DS3231_ADDRESS);

  #if (ARDUINO >= 100)
    if(alarm == 1) {
      Wire.write((byte)DS3231_AL1SEC_REG);
      if(ss <= 59 && ss >= 0) {
        Wire.write((byte)dec2bcd(ss) & 0x7F);                // 01111111
      } else {
        Wire.endTransmission();
        return false;
      }
    } else if(alarm == 2) {
      Wire.write((byte)DS3231_AL2MIN_REG);
    } else {
      Wire.endTransmission();
      return false;
    }

    if(mm <= 59 && mm >= 0) {
      Wire.write((byte)dec2bcd(mm) & 0x7F);                // 01111111
    } else {
      Wire.endTransmission();
      return false;
    }
    if(tf == true && md == true) {
      if(hh <= 12 && hh >=1) {
        Wire.write((byte)dec2bcd(hh) & 0x7F | 0x60);     // 01111111 | 01100000
      } else {
        Wire.endTransmission();
        return false;
      }
    } else if(tf == true && md == false) {
      if(hh <= 12 && hh >=1) {
        Wire.write((byte)dec2bcd(hh) & 0x5F | 0x40);     // 01011111 | 01000000
      } else {
        Wire.endTransmission();
        return false;
      }
    } else {
      if(hh <= 23 && hh >=0) {
        Wire.write((byte)dec2bcd(hh) & 0x3F);            // 00111111
      } else {
        Wire.endTransmission();
        return false;
      }
    }
    Wire.write(128);                                       // 10000000
  #else
    if(alarm == 1) {
      Wire.send((byte)DS3231_AL1SEC_REG);
      if(ss <= 59 && ss >= 0) {
        Wire.send((byte)dec2bcd(ss) & 0x7F);                // 01111111
      } else {
        Wire.endTransmission();
        return false;
      }
    } else if(alarm == 2) {
      Wire.send((byte)DS3231_AL2MIN_REG);
    } else {
      Wire.endTransmission();
      return false;
    }

    if(mm <= 59 && mm >= 0) {
      Wire.send((byte)dec2bcd(mm) & 0x7F);                // 01111111
    } else {
      Wire.endTransmission();
      return false;
    }
    if(tf == true && md == true) {
      if(hh <= 12 && hh >=1) {
        Wire.send((byte)dec2bcd(hh) & 0x7F | 0x60);     // 01111111 | 01100000
      } else {
        Wire.endTransmission();
        return false;
      }
    } else if(tf == true && md == false) {
      if(hh <= 12 && hh >=1) {
        Wire.send((byte)dec2bcd(hh) & 0x5F | 0x40);     // 01011111 | 01000000
      } else {
        Wire.endTransmission();
        return false;
      }
    } else {
      if(hh <= 23 && hh >=0) {
        Wire.send((byte)dec2bcd(hh) & 0x3F);            // 00111111
      } else {
        Wire.endTransmission();
        return false;
      }
    }
    Wire.write(128);                                       // 10000000
  #endif

  Wire.endTransmission();
  return true;
};

/**
  Set Alaram matching seconds, minutes and hour
**/
boolean Himadri_DS3231::setAlarm(uint8_t ss, uint8_t mm, uint8_t hh,
  uint8_t dyDt, boolean dy, boolean tf, boolean md, uint8_t alarm) {
  Wire.beginTransmission(DS3231_ADDRESS);

  #if (ARDUINO >= 100)
    if(alarm == 1) {
      Wire.write((byte)DS3231_AL1SEC_REG);
      if(ss <= 59 && ss >= 0) {
        Wire.write((byte)dec2bcd(ss) & 0x7F);             // 01111111
      } else {
        Wire.endTransmission();
        return false;
      }
    } else if(alarm == 2) {
      Wire.write((byte)DS3231_AL2MIN_REG);
    } else {
      Wire.endTransmission();
      return false;
    }

    if(mm <= 59 && mm >= 0) {
      Wire.write((byte)dec2bcd(mm) & 0x7F);             // 01111111
    } else {
      Wire.endTransmission();
      return false;
    }
    if(tf == true && md == true) {
      if(hh <= 12 && hh >=1) {
        Wire.write((byte)dec2bcd(hh) & 0x7F | 0x60);    // 01111111 | 01100000
      } else {
        Wire.endTransmission();
        return false;
      }
    } else if(tf == true && md == false) {
      if(hh <= 12 && hh >=1) {
        Wire.write((byte)dec2bcd(hh) & 0x5F | 0x40);    // 01011111 | 01000000
      } else {
        Wire.endTransmission();
        return false;
      }
    } else {
      if(hh <= 23 && hh >=0) {
        Wire.write((byte)dec2bcd(hh) & 0x3F);           // 00111111
      } else {
        Wire.endTransmission();
        return false;
      }
    }
    if(dy == true) {
      Wire.write((byte)dec2bcd(dyDt) & 0x7F | 0x40);  // 01111111 | 01000000
    } else {
      Wire.write((byte)dec2bcd(dyDt) & 0x3F);         // 00111111
    }
  #else
    Wire.send((byte)DS3231_AL1SEC_REG);
    if(ss <= 59 && ss >= 0 && mm <= 59 && mm >= 0) {
      Wire.send((byte)dec2bcd(ss) & 0x7F);            // 01111111
      Wire.send((byte)dec2bcd(mm) & 0x7F);            // 01111111
    } else {
      Wire.endTransmission();
      return false;
    }
    if(tf == true && md == true) {
      if(hh <= 12 && hh >=1) {
        Wire.send((byte)dec2bcd(hh) & 0x7F | 0x60);    // 01111111 | 01100000
      } else {
        Wire.endTransmission();
        return false;
      }
    } else if(tf == true && md == false) {
      if(hh <= 12 && hh >=1) {
        Wire.send((byte)dec2bcd(hh) & 0x5F | 0x40);    // 01011111 | 01000000
      } else {
        Wire.endTransmission();
        return false;
      }
    } else {
      if(hh <= 23 && hh >=0) {
        Wire.send((byte)dec2bcd(hh) & 0x3F);           // 00111111
      } else {
        Wire.endTransmission();
        return false;
      }
    }
    if(dy == true) {
      Wire.send((byte)dec2bcd(dyDt) & 0x7F | 0x40);  // 01111111 | 01000000
    } else {
      Wire.send((byte)dec2bcd(dyDt) & 0x3F);         // 00111111
    }
  #endif
  Wire.endTransmission();
  return true;
};

/**
  Enable Alarm
**/
void Himadri_DS3231::enableAlarm(uint8_t alarm) {
  uint8_t ctReg=0;
  // Read control register
  ctReg = readRegister(DS3231_CONTROL_REG);

  switch(alarm)
  {
      case Alarm1:
        ctReg |= 0x05;                              // 00000101
        writeRegister(DS3231_CONTROL_REG, ctReg);   //CONTROL Register Address
      break;
      case Alarm2:
        ctReg |= 0x06;                              // 00000110
        writeRegister(DS3231_CONTROL_REG, ctReg);   //CONTROL Register Address
      break;
      case BothAlarm:
        ctReg |= 0x07;                              // 00000111;
        writeRegister(DS3231_CONTROL_REG, ctReg);   //CONTROL Register Address
      break;
  }
};

/**
  Disable Alarm
*/
void Himadri_DS3231::disableAlarm(uint8_t alarm) {
  uint8_t ctReg=0;
  // Read control register
  ctReg = readRegister(DS3231_CONTROL_REG);

  switch(alarm)
  {
      case Alarm1:
        ctReg &= 0xFE;                              // 11111110
        writeRegister(DS3231_CONTROL_REG, ctReg);   //CONTROL Register Address
      break;
      case Alarm2:
        ctReg &= 0xFD;                              // 11111101
        writeRegister(DS3231_CONTROL_REG, ctReg);   //CONTROL Register Address
      break;
      case BothAlarm:
        ctReg &= 0xFC;                              // 11111100;
        writeRegister(DS3231_CONTROL_REG, ctReg);   //CONTROL Register Address
      break;
  }
};

/**
  Get Alarm status
**/
boolean Himadri_DS3231::alarmStatus(uint8_t alarm) {
  uint8_t ctReg=0;
  // Read control register
  ctReg = readRegister(DS3231_CONTROL_REG);

  if(alarm == Alarm1) {
    if((ctReg & 0x01) == 1) {                 // 00000001
      return true;
    } else {
      return false;
    }
  } else if(alarm == Alarm2) {
    if((ctReg & 0x02) == 2) {                // 00000010
      return true;
    } else {
      return false;
    }
  } else if(alarm == BothAlarm) {
    if((ctReg & 0x03) == 3) {               // 00000011
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
};

/**
  Get Alarm time
**/
boolean Himadri_DS3231::readAlarmTime(struct alarmParameters* alarmVals, uint8_t alarm) {
  Wire.beginTransmission(DS3231_ADDRESS);

  switch(alarm) {
    case Alarm1:
      #if (ARDUINO >= 100)
        Wire.write(DS3231_AL1SEC_REG);
      #else
        Wire.send(DS3231_AL1SEC_REG);
      #endif
      Wire.endTransmission();

      Wire.requestFrom(DS3231_ADDRESS, 4, true);

      #if (ARDUINO >= 100)
        alarmVals->ss = bcd2dec(Wire.read() & 0x7F);                // 01111111
        alarmVals->mm = bcd2dec(Wire.read() & 0x7F);                // 01111111
        alarmVals->hh = Wire.read();
        alarmVals->dayDate = Wire.read();
      #else
        alarmVals->ss = bcd2dec(Wire.receive() & 0x7F);                // 01111111
        alarmVals->mm = bcd2dec(Wire.receive() & 0x7F);                // 01111111
        alarmVals->hh = Wire.receive();
        alarmVals->dayDate = Wire.receive();
      #endif

      if((alarmVals->hh & 0x40) >> 6 == 1) {                      // 01000000
        alarmVals->tf = 1;
        if((alarmVals->hh & 0x20) >> 5 == 1) {
          alarmVals->md = 1;
        } else {
          alarmVals->md = 0;
        }
        alarmVals->hh = bcd2dec(alarmVals->hh & 0x1F);              // 00011111
      } else {
        alarmVals->tf = 0;
        alarmVals->md =3;
        alarmVals->hh = bcd2dec(alarmVals->hh & 0x3F);             // 00111111
      }

      if((alarmVals->dayDate & 0x40) >> 6 == 1) {                 // 01000000
        alarmVals->dyDt = 1;
      } else {
        alarmVals->dyDt = 0;
      }
      alarmVals->dayDate = bcd2dec(alarmVals->dayDate & 0x3F);    // 00111111
    break;
    case Alarm2:
      #if (ARDUINO >= 100)
        Wire.write(DS3231_AL2MIN_REG);
      #else
        Wire.send(DS3231_AL2MIN_REG);
      #endif
      Wire.endTransmission();

      Wire.requestFrom(DS3231_ADDRESS, 3, true);

      alarmVals->ss = 0;

      #if (ARDUINO >= 100)
        alarmVals->mm = bcd2dec(Wire.read() & 0x7F);                  // 01111111
        alarmVals->hh = Wire.read();
        alarmVals->dayDate = Wire.read();
      #else
        alarmVals->mm = bcd2dec(Wire.receive() & 0x7F);                  // 01111111
        alarmVals->hh = Wire.receive();
        alarmVals->dayDate = Wire.receive();
      #endif

      if((alarmVals->hh & 0x40) >> 6 == 1) {                        // 01000000
        alarmVals->tf = 1;
        if((alarmVals->hh & 0x20) >> 5 == 1) {
          alarmVals->md = 1;
        } else {
          alarmVals->md = 0;
        }
        alarmVals->hh = bcd2dec(alarmVals->hh & 0x1F);              // 00011111
      } else {
        alarmVals->tf = 0;
        alarmVals->md =3;
        alarmVals->hh = bcd2dec(alarmVals->hh & 0x3F);              // 00111111
      }

      if((alarmVals->dayDate & 0x40) >> 6 == 1) {                   // 01000000
        alarmVals->dyDt = 1;
      } else {
        alarmVals->dyDt = 0;
      }
      alarmVals->dayDate = bcd2dec(alarmVals->dayDate & 0x3F);    // 00111111
    break;
    default:
      return false;
    break;
  }
  return true;
};

/**
  Get Temperature
*/
float Himadri_DS3231::readTemp(uint8_t scale = Celsius) {
  float rv;
  uint8_t tempMsb, tempLsb;
  int8_t nint;

  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write(DS3231_TEMPERATURE_MSG_REG);
  Wire.endTransmission();

  Wire.requestFrom(DS3231_ADDRESS, 2);

  #if (ARDUINO >= 100)
    tempMsb = Wire.read();
    tempLsb = Wire.read() >> 6;
  #else
    tempMsb = Wire.receive();
    tempLsb = Wire.receive() >> 6;
  #endif

  if ((tempMsb & 0x80) != 0) {               // 10000000
    tempMsb ^= 0xFF;                         // 11111111
    tempMsb  += 0x1;                         // 00000001
    rv = tempMsb + (tempLsb * 0.25);
    rv = rv * -1;
  } else {
      rv = tempMsb + (tempLsb * 0.25);
  }

  switch(scale) {
    case Celsius:
    break;
    case Fahrenheit:
      rv = rv * 1.8 + 32;
    break;
    case Kelvin:
      rv = rv + 273.15;
    break;
  }

  return rv;
};

/**
  Force activate Temperature convertion by the chip
**/
boolean Himadri_DS3231::convertTemp() {
  uint8_t ctReg=0;
  // Read control register
  ctReg = readRegister(DS3231_CONTROL_REG);

  ctReg |= 0x20;                              // 00100000
  writeRegister(DS3231_CONTROL_REG, ctReg);   //CONTROL Register Address

  //wait until CONV is cleared. Indicates new temperature value is available in register.
  do
  {
     //do nothing
  } while ((readRegister(DS3231_CONTROL_REG) & 0x20) >> 5 == 1);    // 00100000

  return true;
};

/**
  Get Control Register value
**/
uint8_t Himadri_DS3231::controlRegValue() {
  uint8_t ctReg = readRegister(DS3231_CONTROL_REG);

  return ctReg;
};

/**
  Enable EOSC register which enables the oscillator
**/
boolean Himadri_DS3231::enableDisableOscillator(uint8_t choice) {
  uint8_t ctReg = readRegister(DS3231_CONTROL_REG);

  if(choice == 1) {
    ctReg &= 0x7F;          // 01111111
    writeRegister(DS3231_CONTROL_REG, ctReg);
  } else if(choice == 2) {
    ctReg |= 0x80;          // 10000000
    writeRegister(DS3231_CONTROL_REG, ctReg);
  } else {
    return false;
  }

  return true;
};

/**
  Get EOSC bit on Control Register with 0 for start
  and 1 for stop when the device switches to VBAT
*/
boolean Himadri_DS3231::oscillatorStatus() {
  uint8_t ctReg = readRegister(DS3231_CONTROL_REG);

  if((ctReg & 0x80) >> 7 == 1) {    // 10000000
    return true;
  } else {
    return false;
  }
};

/**
  Set BBSQW bit on Control Register with 1 for start
  and 0 for stop with INTCN = 0 and VCC < VPF, enabling this generates
  1Hz square wave
**/
boolean Himadri_DS3231::enableDisableBBSQW(uint8_t choice) {
  uint8_t ctReg = readRegister(DS3231_CONTROL_REG);

  if(choice == 1) {
    ctReg &= 0x40 | 0xFB;          // 01000000  11111011
    writeRegister(DS3231_CONTROL_REG, ctReg);
  } else if(choice == 2) {
    ctReg &= 0xBF | 0x04;          // 01111111  00000100
    writeRegister(DS3231_CONTROL_REG, ctReg);
  } else {
    return false;
  }
};

/**
  Get BBSQW bit on Control Register with 1 for start
  and 0 for stop with INTCN = 0 and VCC < VPF, enabling this generates
  1Hz square wave
**/
boolean Himadri_DS3231::bbsqwStatus() {
  uint8_t ctReg = readRegister(DS3231_CONTROL_REG);

  if((ctReg & 0x44) == 64) {    // 01000100
    return true;
  } else {
    return false;
  }
};

/**
  Get INTCN bit on Control Register with 1 for enable and 0 for disable
**/
boolean Himadri_DS3231::intcnStatus() {
  uint8_t ctReg = readRegister(DS3231_CONTROL_REG);

  if((ctReg & 0x04) >> 2 == 1) {    // 00000100
    return true;
  } else {
    return false;
  }
};

/**
  Get Status Register value
**/
uint8_t Himadri_DS3231::statusRegValue() {
  uint8_t statusReg = readRegister(DS3231_STATUS_REG);

  return statusReg;
};

/**
  Get OSF (Oscillator stop flag) bit on Status Register with 1 for stop
  and 0 for start
*/
boolean Himadri_DS3231::osfStatus() {
  uint8_t statusReg = readRegister(DS3231_STATUS_REG);

  if((statusReg & 0x80) >> 7 == 1) {
    return true;
  } else {
    return false;
  }
};

/**
  Set EN32KHZ bit on Status Register with 0 for disable and 1 for enable.
  It Enable / Disable 32.768Khz Square Wave output
**/
boolean Himadri_DS3231::enableDisable32Khz(uint8_t choice) {
  uint8_t statusReg = readRegister(DS3231_STATUS_REG);
  if(choice == 1) {
    statusReg |= 0x08;                  // 00001000
    writeRegister(DS3231_STATUS_REG, statusReg);
  } else if(choice == 2) {
    statusReg &= 0xF7;                  // 11110111
    writeRegister(DS3231_STATUS_REG, statusReg);
  } else {
    return false;
  }
};

/**
  Get EN32KHZ bit on Status Register with 1 for start and 0 for stop
**/
boolean Himadri_DS3231::en32KhzStatus() {
  uint8_t statusReg = readRegister(DS3231_STATUS_REG);

  if((statusReg & 0x08) >> 3 == 1) {        // 00001000
    return true;
  } else {
    return false;
  }
};

/**
  Get Temperature Busy Status
*/
boolean Himadri_DS3231::bsyStatus() {
  uint8_t statusReg = readRegister(DS3231_STATUS_REG);

  if((statusReg & 0x04) >> 2 == 1) {      // 00000100
    return true;
  } else {
    return false;
  }
};

/**
  Get Aging Register value
**/
uint8_t Himadri_DS3231::agingRegValue() {
  uint8_t agingReg = readRegister(DS3231_AGING_REG);

  if ((agingReg & 0x80) != 0) {                 // 10000000
    agingReg ^= 0xFF;                           // 11111111
    agingReg  += 0x1;                           // 00000001
    agingReg = agingReg * -1;
  }

  return agingReg;
};

/**
  Set Aging Register value, it takes user-provided value to add to or
  subtract from the factory-trimmed value that adjusts the accuracy of the
  time base
**/
boolean Himadri_DS3231::setAgingRegValue(int8_t val) {
  if(val <= 127 && val >= 0) {
    writeRegister(DS3231_AGING_REG, val);
  } else if(val < 0 && val >= -127) {
    val = ~(-val) + 1;      // 2's complement
    writeRegister(DS3231_AGING_REG, val);
  } else {
    return false;
  }
  return true;
};
