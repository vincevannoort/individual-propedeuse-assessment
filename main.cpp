#include "./fingerprintsensor/fingerprintsensor.hpp"
#include "./timetracker/timetracker.hpp"
#include "./realtimeclock/i2cRTC.hpp"
// #include "./unit_tests/unit_tes`ts.hpp"
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
	fingerprintsensor.control_led(true);
	hwlib::wait_ms(1000);
	fingerprintsensor.control_led(false);
	// fingerprintsensor.delete_all_fingerprints();
	// fingerprintsensor.register_fingerprint();
	// hwlib::wait_ms(1000);
	// int i = 0;
	// while(1) {
	// 	int ret_val = fingerprintsensor.identify_fingerprint_try_check_finger_pressing_status_once();
	// 	hwlib::cout << "try " << i++ << ": " << ret_val << "\n";
	// }

	/*
	Realtimeclock
	*/
	auto scl = due::pin_oc( hwlib::target::pins::d8 ); 	
	auto sda = due::pin_oc( hwlib::target::pins::d9 );
	auto i2c = hwlib::i2c_bus_bit_banged_scl_sda( scl, sda );
	i2cRTClib realtimeclock(i2c, 0, 0, 0, 0, 23, 5, 2017);
	// hwlib::wait_ms(1000);
	// hwlib::cout << "Start checking!\n";
	// while(1) {
	// 	hwlib::wait_ms(1000);
	// 	hwlib::cout << realtimeclock.get_seconden() << "\n"; 
	// }

	/*
	Timetracker
	*/
	auto checking_pin = hwlib::target::pin_in( hwlib::target::pins::d11 );
	auto registering_pin = hwlib::target::pin_in( hwlib::target::pins::d12 );
	auto storing_pin = hwlib::target::pin_in( hwlib::target::pins::d13 );
	Timetracker timetracker( fingerprintsensor, realtimeclock, checking_pin, registering_pin, storing_pin, Time( 0, 0, 0, 0, 23, 5, 2017 ));
	// timetracker.start_tracking();
}