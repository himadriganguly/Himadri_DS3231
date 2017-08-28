# DS3231 RTC Library For Arduino

 * The DS3231 is a low-cost, extremely accurate I2C real-time clock (RTC) with an integrated temperature-compensated crystal oscillator (TCXO) and crystal.

 ## Datasheet
* The datasheet used for this library can be found at [DS3231M Datasheet](https://datasheets.maximintegrated.com/en/ds/DS3231M.pdf).

## Features

* Set Date and Time by passing structure variable.
* Set Date and Time by passing date as string and time as string.
* It can keep date from 1900 till 2099.
* Time can be set both in 12 and 24 Hour format
* Seconds, Minutes, Hour, Day, Date, Month and Year can be set seperately.
* Validation for Seconds, Minutes, Hour, Day, Date, Month and Year
* Read current Date and Time
* Set Alarm1 matching every seconds / minutes / hour OR Alarm2 matching every minute / hour
* Set Alarm1 matching for exact seconds / minutes / hour with time format and meridian OR Alarm2 matching for exact minutes / hour with time format and meridian
* Set Alarm1 matching seconds and minutes
* Set Alaram1 matching seconds, minutes and hour OR Alarm2 matching minutes and hour
* Set Alarm1 matching day / date with hour, minutes and seconds OR Alarm2 matching day / date with hour, minutes
* Enable Alarm1 / Alarm2
* Disable Alarm1 / Alarm2
* Get Alarm1 / Alarm2 status
* Get Alarm1 / Alarm2 configured time
* Get Temperature in Celsius / Fahrenheit / Kelvin scale
* Convert Temperature
* Get Control Register value
* Enable / Disable Oscillator
* Get Oscillator status
* Enable / Disable BBSQW
* Get BBSQW status
* Get INTCN status
* Get Status Register value
* Get OSF status
* Enable / Disable 32Khz
* Get 32Khz status
* Get BSY status
* Get Aging Register value
* Set Aging Register value

## Installation

1. Download zip archive from the github repository at [https://github.com/himadriganguly/Himadri_DS3231](https://github.com/himadriganguly/Himadri_DS3231).
2. Copy folders `Himadri_DS3231` from the zip archive into Arduino `libraries` folder on your computer. Arduino folder is e.g. `My Documents\Arduino\libraries` on Windows or `Documents/Arduino/libraries` on Linux and Mac.

## Other Projects Using This library
1. DS3231 Alarm to switch on Relay - [https://github.com/himadriganguly/ds3231_alarm_relay_switch](https://github.com/himadriganguly/ds3231_alarm_relay_switch).

## Author

1. Himadri Ganguly [https://github.com/himadriganguly/](https://github.com/himadriganguly/)

## Contribute

If you want to contribute to this project by enhancing its features please create a pull request and I will do my best to merge it appropriately.
