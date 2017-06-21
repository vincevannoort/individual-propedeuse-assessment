#include "hwlib.hpp"
#include "timetracker.hpp"

// Test 1: Check two times minus eachother which is used by the tracking application
bool TEST_time_minus_time() {
	hwlib::cout << "TEST_time_minus_time: ";
	Time a( 45, 53, 00, 0, 23, 5, 2017 );
	Time b( 31, 51, 01, 0, 23, 5, 2017 );
	Time c = b - a;
	if (
		(int)c.seconds == 46 &&
		(int)c.minutes == 57 &&
		(int)c.hours == 0
	) { hwlib::cout << "Passed" << "\n"; return true; } hwlib::cout << "Failed" << "\n"; return false;
}