#include "./fingerprintsensor/fingerprintsensor.hpp"
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
	hwlib::wait_ms(4); // max wait_ms, doesnt work above 4 millisecondstx_pin
	auto tx_pin = hwlib::target::pin_out( hwlib::target::pins::d18 );
	auto rx_pin = hwlib::target::pin_in( hwlib::target::pins::d19 );
    Fingerprintsensor fingerprintsensor(tx_pin, rx_pin);
	fingerprintsensor.initialise();
	fingerprintsensor.register_fingerprint();
	
	// CE (RST), 
	// I / O (line Data), 
	// and SCLK (serial clock).
	// auto ce_pin = hwlib::target::pin_out( hwlib::target::pins::d11 );
	// auto io_pin = hwlib::target::pin_in_out( hwlib::target::pins::d12 );
	// auto sclk_pin = hwlib::target::pin_out( hwlib::target::pins::d13 );
	// auto green_led = hwlib::target::pin_out( hwlib::target::pins::d2 );
	// DS1302 realtimeclock(ce_pin, io_pin, sclk_pin);
	// hwlib::wait_ms(500);

	// SETTING TIME
	// hwlib::cout << "SETTING TIME:\n"; 
	// realtimeclock.writeProtect(false);
	// realtimeclock.halt(false);
	// Time t(2018, 9, 23, 1, 38, 50, Time::kSunday);
	// realtimeclock.time(t);

	// RETRIEVING TIME
	// hwlib::cout << "RETRIEVING TIME:\n"; 
	// hwlib::wait_ms(500);
	// Time real_time = realtimeclock.time();
	// hwlib::cout << "\nTime:" 
	// << "\n" << "[" << (int) real_time.yr << "]" 
	// << "\n" << "[" << (int) real_time.mon << "]" 
	// << "\n" << "[" << (int) real_time.date << "]" 
	// << "\n" << "[" << (int) real_time.hr << "]" 
	// << "\n" << "[" << (int) real_time.min << "]" 
	// << "\n" << "[" << (int) real_time.sec << "]";
}