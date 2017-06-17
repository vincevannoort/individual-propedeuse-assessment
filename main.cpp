// #include "./fingerprintsensor/fingerprintsensor.hpp"
// #include "./timetracker/timetracker.hpp"
#include "./realtimeclock/realtimeclock.hpp"
// #include "./unit_tests/unit_tests.hpp"
#include "hwlib.hpp"
using namespace std;

int main() {
	WDT->WDT_MR = WDT_MR_WDDIS;

	/*
	Fingerprintsensor
	*/
	// hwlib::wait_ms(4); // max wait_ms, doesnt work above 4 millisecondstx_pin
	// auto tx_pin = hwlib::target::pin_out( hwlib::target::pins::d18 );
	// auto rx_pin = hwlib::target::pin_in( hwlib::target::pins::d19 );
 //    Fingerprintsensor fingerprintsensor(tx_pin, rx_pin);
	// fingerprintsensor.initialise();
	// fingerprintsensor.register_fingerprint();
	
	// ce_pin_(ce_pin), io_pin_(io_pin), sclk_pin_(sclk_pin)
	auto ce_pin = hwlib::target::pin_out( hwlib::target::pins::d11 );
	auto io_pin = hwlib::target::pin_in_out( hwlib::target::pins::d12 );
	auto sclk_pin = hwlib::target::pin_out( hwlib::target::pins::d13 );
	auto green_led = hwlib::target::pin_out( hwlib::target::pins::d2 );
	DS1302 realtimeclock(ce_pin, io_pin, sclk_pin);
	hwlib::wait_ms(500);

	realtimeclock.writeProtect(false);
	realtimeclock.halt(false);
	// Make a new time object to set the date and time.
	// Sunday, September 22, 2013 at 01:38:50.
	Time t(2015, 9, 22, 1, 38, 50, Time::kSunday);
	// Set the time and date on the chip.
	realtimeclock.time(t);
	hwlib::wait_ms(500);
	Time yolo = realtimeclock.time();
	hwlib::cout << "Time:" 
	<< "\n" << "[" << t.yr << "]" 
	<< "\n" << "[" << t.mon << "]" 
	<< "\n" << "[" << t.date << "]" 
	<< "\n" << "[" << t.hr << "]" 
	<< "\n" << "[" << t.min << "]" 
	<< "\n" << "[" << t.sec << "]";
}