#include "Himadri_DS3231.h"

// This function converts Decimal to BCD(Binary Coded Decimal)
static uint8_t Himadri_DS3231::dec2bcd(uint8_t value) {
  uint8_t convertedVal = (value / 10 * 16) + (value % 10);
  return convertedVal;
};

// This function converts BCD(Binary Coded Decimal) to Decimal
static uint8_t Himadri_DS3231::bcd2dec(uint8_t value) {
  uint8_t convertedVal = (value / 16 * 10) + (value % 16);
  return convertedVal;
}

uint8_t Himadri_DS3231::readRegister(uint8_t regaddress, uint8_t numByte) {
    Wire.beginTransmission(DS3231_ADDRESS);
    #if (ARDUINO >= 100)
      Wire.write((byte)regaddress);
    #else
      Wire.send((byte)regaddress);
    #endif
    Wire.endTransmission();

    Wire.requestFrom(DS3231_ADDRESS, numByte, true);
    #if (ARDUINO >= 100)
      return Wire.read();      // receive DATA
    #else
      return Wire.receive();   // receive DATA
    #endif
}

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
}

void Himadri_DS3231::clearTimeRegister(uint8_t timeFormat) {
  Wire.beginTransmission(DS3231_ADDRESS);

  #if (ARDUINO >= 100)
    Wire.write(DS3231_SECONDS_REG);
    Wire.write(dec2bcd(0)); // seconds
    Wire.write(dec2bcd(0)); // minutes
    if(timeFormat == 0x01) {
      Wire.write(dec2bcd(0) | 0x40);  // 01000000
    } else {
      Wire.write(dec2bcd(0)); // hour
    }
    Wire.write(dec2bcd(4)); // day
    Wire.write(dec2bcd(1)); // date
    Wire.write(dec2bcd(1)); // month
    Wire.write(dec2bcd(0)); // year
  #else
    Wire.send(DS3231_SECONDS_REG);
    Wire.send(dec2bcd(0)); // seconds
    Wire.send(dec2bcd(0)); // minutes
    if(timeFormat == 0x01) {
      Wire.send(dec2bcd(0) | 0x40); // 01000000
    } else {
      Wire.send(dec2bcd(0)); // hour
    }
    Wire.send(dec2bcd(2)); // day
    Wire.send(dec2bcd(1)); // date
    Wire.send(dec2bcd(1)); // month
    Wire.send(dec2bcd(0)); // year
  #endif
  Wire.endTransmission();
};

boolean Himadri_DS3231::begin() {
  // Initialize the Control Register
  uint8_t ctReg = 0x1C;                           // 00011100;
  Wire.begin();
  writeRegister(DS3231_CONTROL_REG, ctReg);     //CONTROL Register Address
  return true;
};

boolean Himadri_DS3231::setTime12Hr(struct timeParameters12Hr *timeVals) {
  // uint8_t ss;   // seconds
  // uint8_t mm;   // minutes
  // uint8_t hh;   // hour
  // uint8_t md;   // meridian AM/PM -> 0 for AM / 1 for PM
  // uint8_t day;  // day of the week
  // uint8_t d;    // date of the month
  // uint8_t m;    // month
  // uint16_t y;   // year
  if(timeVals->ss <= 59 && timeVals->ss >= 0) {
    writeRegister(DS3231_SECONDS_REG, dec2bcd(timeVals->ss));
  } else {
    clearTimeRegister(TwelveHrFormat);
    return false;
  }

  if(timeVals->mm <= 59 && timeVals->mm >= 0) {
    writeRegister(DS3231_MINUTES_REG, dec2bcd(timeVals->mm));
  } else {
    clearTimeRegister(TwelveHrFormat);
    return false;
  }

  if(timeVals->md == 0 && timeVals->hh <= 12 && timeVals->hh >= 1) {
    // Changed the bit 6 of Hours register to 1 to set 12Hr format and bit 5 of Hours register to 0 to set meridian to AM
    timeVals->hh = dec2bcd(timeVals->hh) | 0x40;  // 01000000
    timeVals->hh &= 0xDF; // 11011111
    writeRegister(DS3231_HOUR_REG, timeVals->hh);
  } else if(timeVals->md == 1 && timeVals->hh <= 12 && timeVals->hh >= 1) {
    // Changed the bit 6 of Hours register to 1 to set 12Hr format and bit 5 of Hours register to 1 to set meridian to PM
    timeVals->hh = dec2bcd(timeVals->hh) | 0x60; // 01100000
    writeRegister(DS3231_HOUR_REG, timeVals->hh);
  } else {
    clearTimeRegister(TwelveHrFormat);
    return false;
  }

  if(timeVals->day >= 1 && timeVals->day <= 7) {
    writeRegister(DS3231_DAY_REG, dec2bcd(timeVals->day));
  } else {
    clearTimeRegister(TwelveHrFormat);
    return false;
  }

  if(timeVals->d > 0 && timeVals->d <= 31) {
    writeRegister(DS3231_DATE_REG, dec2bcd(timeVals->d));
  } else {
    clearTimeRegister(TwelveHrFormat);
    return false;
  }

  if(timeVals->y >= 2000 && timeVals->y < 2100) {
    timeVals->m = dec2bcd(timeVals->m) | 0x80;  // 10000000
    timeVals->y = timeVals->y - 2000;
    writeRegister(DS3231_MONTH_REG, timeVals->m);
    writeRegister(DS3231_YEAR_REG, dec2bcd(timeVals->y));
  } else if (timeVals->y >= 1990) {
    timeVals->m = dec2bcd(timeVals->m) & 0x7F;
    timeVals->y = timeVals->y - 1900;
    writeRegister(DS3231_MONTH_REG, timeVals->m);
    writeRegister(DS3231_YEAR_REG, dec2bcd(timeVals->y));
  } else {
    clearTimeRegister(TwelveHrFormat);
    return false;
  }

  return true;
};

boolean Himadri_DS3231::setTime24Hr(struct timeParameters24Hr *timeVals) {
  // uint8_t ss;   // seconds
  // uint8_t mm;   // minutes
  // uint8_t hh;   // hour
  // uint8_t day;  // day of the week
  // uint8_t d;    // date of the month
  // uint8_t m;    // month
  // uint16_t y;    // year
  if(timeVals->ss <= 59 && timeVals->ss >= 0) {
    writeRegister(DS3231_SECONDS_REG, dec2bcd(timeVals->ss));
  } else {
    clearTimeRegister(TwentyFourHrFormat);
    return false;
  }

  if(timeVals->mm <= 59 && timeVals->mm >= 0) {
    writeRegister(DS3231_MINUTES_REG, dec2bcd(timeVals->mm));
  } else {
    clearTimeRegister(TwentyFourHrFormat);
    return false;
  }

  if(timeVals->hh <= 23 && timeVals->hh >= 0) {
    // Changed the bit 6 of Hours register to 0 to set 24Hr format
    timeVals->hh = dec2bcd(timeVals->hh) & 0xBF;  // 10111111
    writeRegister(DS3231_HOUR_REG, timeVals->hh);
  } else {
    clearTimeRegister(TwentyFourHrFormat);
    return false;
  }

  if(timeVals->day >= 1 && timeVals->day <= 7) {
    writeRegister(DS3231_DAY_REG, dec2bcd(timeVals->day));
  } else {
    clearTimeRegister(TwelveHrFormat);
    return false;
  }

  if(timeVals->d > 0 && timeVals->d <= 31) {
    writeRegister(DS3231_DATE_REG, dec2bcd(timeVals->d));
  } else {
    clearTimeRegister(TwentyFourHrFormat);
    return false;
  }

  if(timeVals->y >= 2000 && timeVals->y < 2100) {
    timeVals->m = dec2bcd(timeVals->m) | 0x80;  // 10000000
    timeVals->y = timeVals->y - 2000;
    writeRegister(DS3231_MONTH_REG, timeVals->m);
    writeRegister(DS3231_YEAR_REG, dec2bcd(timeVals->y));
  } else if (timeVals->y >= 1990) {
    timeVals->m = dec2bcd(timeVals->m) & 0x7F;
    timeVals->y = timeVals->y - 1900;
    writeRegister(DS3231_MONTH_REG, timeVals->m);
    writeRegister(DS3231_YEAR_REG, dec2bcd(timeVals->y));
  } else {
    clearTimeRegister(TwentyFourHrFormat);
    return false;
  }

  return true;
};

boolean Himadri_DS3231::readTime12Hr(struct timeParameters12Hr *timeVals) {
  if((readRegister(DS3231_HOUR_REG) & 0x40) >> 6 == 1) {
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
      if((timeVals->hh & 0x20) >> 5 == 1) {
        timeVals->hh = bcd2dec(timeVals->hh & 0x9F);  // 10011111
        timeVals->md = 1;
      } else {
        timeVals->hh = bcd2dec(timeVals->hh & 0x9F);  // 10011111
        timeVals->md = 0;
      }
      timeVals->day = bcd2dec(Wire.read());
      timeVals->d = bcd2dec(Wire.read());
      timeVals->m = Wire.read();
      timeVals->y = bcd2dec(Wire.read());
      if((timeVals->m & 0x80) >> 7 == 1) {
        timeVals->y = 2000 + timeVals->y;
        timeVals->m = bcd2dec(timeVals->m & 0x7F);
      } else {
        timeVals->y = 1900 + bcd2dec(timeVals->y);
        timeVals->m = bcd2dec(timeVals->m);
      }
    #else
      timeVals->ss = bcd2dec(Wire.receive());
      timeVals->mm = bcd2dec(Wire.receive());
      timeVals->hh = Wire.receive();
      if((timeVals->hh & 0x20) >> 5 == 1) {
        timeVals->hh = bcd2dec(timeVals->hh & 0x9F);  // 10011111
        timeVals->md = 1;
      } else {
        timeVals->hh = bcd2dec(timeVals->hh & 0x9F);  // 10011111
        timeVals->md = 0;
      }
      timeVals->day = bcd2dec(Wire.receive());
      timeVals->d = bcd2dec(Wire.receive());
      timeVals->m = Wire.receive();
      timeVals->y = bcd2dec(Wire.receive());
      if((timeVals->m & 0x80) >> 7 == 1) {
        timeVals->y = 2000 + timeVals->y;
        timeVals->m = bcd2dec(timeVals->m & 0x7F);
      } else {
        timeVals->y = 1900 + bcd2dec(timeVals->y);
        timeVals->m = bcd2dec(timeVals->m);
      }
    #endif

    return true;
  } else {
    return false;
  }
};

boolean Himadri_DS3231::readTime24Hr(struct timeParameters24Hr *timeVals) {
  if((readRegister(DS3231_HOUR_REG) & 0x40) >> 6 != 1) {
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
      timeVals->hh = bcd2dec(Wire.read() & 0xBF); // 10111111
      timeVals->day = bcd2dec(Wire.read());
      timeVals->d = bcd2dec(Wire.read());
      timeVals->m = Wire.read();
      timeVals->y = bcd2dec(Wire.read());
      if((timeVals->m & 0x80) >> 7 == 1) {
        timeVals->y = 2000 + timeVals->y;
        timeVals->m = bcd2dec(timeVals->m & 0x7F);
      } else {
        timeVals->y = 1900 + bcd2dec(timeVals->y);
        timeVals->m = bcd2dec(timeVals->m);
      }
    #else
      timeVals->ss = bcd2dec(Wire.receive());
      timeVals->mm = bcd2dec(Wire.receive());
      timeVals->hh = bcd2dec(Wire.receive() & 0xBF);
      timeVals->day = bcd2dec(Wire.receive());
      timeVals->d = bcd2dec(Wire.receive());
      timeVals->m = Wire.receive();
      timeVals->y = bcd2dec(Wire.receive());
      if((timeVals->m & 0x80) >> 7 == 1) {
        timeVals->y = 2000 + timeVals->y;
        timeVals->m = bcd2dec(timeVals->m & 0x7F);
      } else {
        timeVals->y = 1900 + bcd2dec(timeVals->y);
        timeVals->m = bcd2dec(timeVals->m);
      }
    #endif

    return true;
  } else {
    return false;
  }
};
