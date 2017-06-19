#include <cstring>
#include "timetracker.hpp"
#include "hwlib.hpp"

/*
Employee
*/


/*
Workday
*/
Workday::Workday(){}

// @brief Set start time of current workday
void Workday::start_day(Time t) {
	start_time = Time( 0, 0, 0, 0, 23, 5, 2017 );
}

// @brief Set end time of current workday
void Workday::end_day(Time t) {
	end_time = Time( 0, 0, 0, 0, 23, 5, 2017 );
}

// @brief Calculate hours worked
uint_fast64_t Workday::calculate_work_time() {
	return 0; // end_time - start_time;
}

/*
Time
*/

// @brief Empty constructor for time
Time::Time():
seconds(0), minutes(0), hours(0), day(0), date(0), month(0), year(2000) {}

// @brief Constructor for time
Time::Time(byte seconds, byte minutes, byte hours, byte day, byte date, byte month, word year):
seconds(seconds), minutes(minutes), hours(hours), day(day), date(date), month(month), year(year) {}

/*
Timetracker
*/

// @brief Constructor for the timetracker
Timetracker::Timetracker(Fingerprintsensor & fps, i2cRTClib & rtc, Time initial_time):
fps(fps), rtc(rtc), status(0) {

	rtc.set_time(initial_time.seconds, initial_time.minutes, initial_time.hours, initial_time.day, initial_time.date, initial_time.month, initial_time.year);
	fps.register_fingerprint();
	hwlib::wait_ms(500);
}

// @brief Initialise, set time of current day
void Timetracker::start_tracking() {
	while(true) {
		if (status == (int)Timetracker::status_data::Checking) {
			fps.identify_fingerprint();
		} else if (status == (int)Timetracker::status_data::Registering) {
			fps.register_fingerprint();
		} else if (status == (int)Timetracker::status_data::Identifying) {

		} else if (status == (int)Timetracker::status_data::StoringData) {

		}
	}
}