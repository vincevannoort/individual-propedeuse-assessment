#include "./fingerprintsensor/fingerprintsensor.hpp"
// #include "./unit_tests/unit_tests.hpp"
#include "hwlib.hpp"
using namespace std;

int main() {
	WDT->WDT_MR = WDT_MR_WDDIS;
	// hwlib::wait_ms(4); // max wait_ms, doesnt work above 4 milliseconds
	auto tx_pin = hwlib::target::pin_out( hwlib::target::pins::d18 );
	auto rx_pin = hwlib::target::pin_in( hwlib::target::pins::d19 );
    Fingerprintsensor fingerprintsensor(tx_pin, rx_pin);
    while(1) {
		fingerprintsensor.initialise();
	}
    // fingerprintsensor.control_led(true);
    // fingerprintsensor.control_led(false);
}