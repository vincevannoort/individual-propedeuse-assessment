# Indiviual Propedeuse Assessment
For my last project of the first year of my study, i decided to make a timeregistration system. Combining the GT-511C1R (fingerprintscanner) and the DS1307 (realtimeclock) i mananaged to build a working system. The main goal of the project was writing a library for the GT-511C1R, which should be reusable for students next year. The whole project is based upon a close to hardware library called 'hwlib', build by Wouter van Ooijen.

Things included:
- GT-511C1R library (written by myself)
- DS1307 library (written by Jeremy Ruizenaar)
- Timetracking library (written by myself)

The timetracking library uses the fingerprint- and realtimeclock library for authentication & timetracking purposes. Normally a system like this (with some extra functionality) costs around €650. With this library it is possible to build a timetracking system for much less.

## Example for the fingerprintsensor
```c++
auto tx_pin = hwlib::target::pin_out( hwlib::target::pins::d18 );
auto rx_pin = hwlib::target::pin_in( hwlib::target::pins::d19 );
Fingerprintsensor fingerprintsensor(tx_pin, rx_pin);
fingerprintsensor.initialise();
```

## Example for the realtimeclock
```c++
auto scl = due::pin_oc( hwlib::target::pins::d8 );   
auto sda = due::pin_oc( hwlib::target::pins::d9 );
auto i2c = hwlib::i2c_bus_bit_banged_scl_sda( scl, sda );
i2cRTClib realtimeclock(i2c, 0, 0, 0, 0, 23, 5, 2017);
```

## Example for the timetracking library
```c++
auto checking_pin = hwlib::target::pin_in( hwlib::target::pins::d11 );
auto registering_pin = hwlib::target::pin_in( hwlib::target::pins::d12 );
auto storing_pin = hwlib::target::pin_in( hwlib::target::pins::d13 );
Timetracker timetracker( fingerprintsensor, realtimeclock, checking_pin, registering_pin, storing_pin, Time( 0, 0, 0, 0, 23, 5, 2017 ));
```
