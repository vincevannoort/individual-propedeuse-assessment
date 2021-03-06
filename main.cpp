#include "./GT511C1R/GT511C1R.hpp"
#include "./timetracker/timetracker.hpp"
#include "./DS1307/i2cRTC.hpp"
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

	/*
	Realtimeclock
	*/
	auto scl = due::pin_oc( hwlib::target::pins::d8 ); 	
	auto sda = due::pin_oc( hwlib::target::pins::d9 );
	auto i2c = hwlib::i2c_bus_bit_banged_scl_sda( scl, sda );
	i2cRTClib realtimeclock(i2c, 0, 0, 0, 0, 23, 5, 2017);

	/*
	Timetracker
	*/
	auto checking_pin = hwlib::target::pin_in( hwlib::target::pins::d11 );
	auto registering_pin = hwlib::target::pin_in( hwlib::target::pins::d12 );
	auto storing_pin = hwlib::target::pin_in( hwlib::target::pins::d13 );
	Timetracker timetracker(&fingerprintsensor, realtimeclock, checking_pin, registering_pin, storing_pin, Time( 0, 0, 0, 0, 23, 5, 2017 ));
	Employee vince("Vince", sizeof("Vince"), "van Noort", sizeof("van Noort"));
	Employee wouter("Wouter", sizeof("Wouter"), "van Ooijen", sizeof("van Ooijen"));
	Employee joost("Joost", sizeof("Joost"), "Schalken", sizeof("Schalken"));
	timetracker.set_workday_employee(0, vince);
	timetracker.set_workday_employee(1, wouter);
	timetracker.set_workday_employee(2, joost);
	timetracker.start_tracking();
}