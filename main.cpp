#include "./fingerprintsensor/fingerprintsensor.hpp"
#include "hwlib.hpp"
using namespace std;

int main() {
	auto tx_pin = hwlib::target::pin_out( hwlib::target::pins::d18 );
	auto rx_pin = hwlib::target::pin_in( hwlib::target::pins::d19 );
    Fingerprintsensor fingerprintsensor(tx_pin, rx_pin);
    fingerprintsensor.initialise();
    fingerprintsensor.control_led(true);
    return 0;
}