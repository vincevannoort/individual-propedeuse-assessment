#include "./fingerprintsensor/fingerprintsensor.hpp"
#include "hwlib.hpp"
using namespace std;

int main() {
	WDT->WDT_MR = WDT_MR_WDDIS;
	auto tx_pin = hwlib::target::pin_out( hwlib::target::pins::d18 );
	auto rx_pin = hwlib::target::pin_in( hwlib::target::pins::d19 );
    Fingerprintsensor fingerprintsensor(tx_pin, rx_pin);
    fingerprintsensor.initialise();
    fingerprintsensor.change_baud_rate(9600);
    fingerprintsensor.control_led(true);
    fingerprintsensor.control_led(false);
}