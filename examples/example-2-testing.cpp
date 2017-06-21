#include "./GT511C1R/GT511C1R.hpp"
#include "./timetracker/timetracker.hpp"
#include "./realtimeclock/i2cRTC.hpp"
#include "hwlib.hpp"
using namespace std;

int main() {
	WDT->WDT_MR = WDT_MR_WDDIS;

	/*
	Fingerprintsensor
	*/
	hwlib::wait_ms(4); // max wait_ms, doesnt work above 4 millisecondstx_pin
	auto tx_pin = hwlib::target::pin_out( hwlib::target::pins::d18 );
	auto rx_pin = hwlib::target::pin_in( hwlib::target::pins::d19 );
 	GT511C1R fingerprintsensor(tx_pin, rx_pin);
	fingerprintsensor.initialise();
	fingerprintsensor.control_led(true);
	hwlib::wait_ms(1000);
	fingerprintsensor.control_led(false);
}