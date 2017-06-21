#include <cstring>
#include "timetracker.hpp"
#include "hwlib.hpp"

/* ==========================================================================================================================
Employee
*/ // =======================================================================================================================


/* ==========================================================================================================================
Workday
*/ // =======================================================================================================================
Workday::Workday(){}

// @brief Set start time of current workday
void Workday::start_day(Time t) {
	start_time = t;
}

// @brief Set end time of current workday
void Workday::end_day(Time t) {
	end_time = t;
}

// @brief Calculate hours worked
uint_fast64_t Workday::calculate_work_time() {
	return 0; // end_time - start_time;
}

/* ==========================================================================================================================
Time
*/ // =======================================================================================================================

// @brief Empty constructor for time
Time::Time():
seconds(0), minutes(0), hours(0), day(0), date(0), month(0), year(2000) {}

// @brief Constructor for time
Time::Time(byte seconds, byte minutes, byte hours, byte day, byte date, byte month, word year):
seconds(seconds), minutes(minutes), hours(hours), day(day), date(date), month(month), year(year) {}

/* ==========================================================================================================================
Timetracker
*/ // =======================================================================================================================

// @brief Constructor for the timetracker
Timetracker::Timetracker(Fingerprintsensor & fps, i2cRTClib & rtc, hwlib::pin_in & checking_pin, hwlib::pin_in & registering_pin, hwlib::pin_in & storing_pin, Time initial_time):
fps(fps), rtc(rtc), checking_pin(checking_pin), registering_pin(registering_pin), storing_pin(storing_pin), status(0) {
	rtc.set_time(initial_time.seconds, initial_time.minutes, initial_time.hours, initial_time.day, initial_time.date, initial_time.month, initial_time.year);
}

// @brief Initialise, set time of current day
void Timetracker::start_tracking() {
	while(true) {
		check_buttons_and_store_status();
		
		if (status == (int)Timetracker::status_data::Checking) {
			int checked_id = fps.identify_fingerprint_try_check_finger_pressing_status_once();
			if (checked_id >= 0) {
				hwlib::cout << "Goedemorgen werknemer: " << checked_id << ", tijd: " << hwlib::setfill('0') << hwlib::setw(2) << rtc.get_uren() << ":" << hwlib::setfill('0') << hwlib::setw(2) << rtc.get_minuten() << ":" << hwlib::setfill('0') << hwlib::setw(2) << rtc.get_seconden() << "\n";
				if (time_entries[checked_id].start_time.year == 2000) { // year 2000 means it is not set
					time_entries[checked_id].start_day(get_current_time_as_time_class());
					hwlib::cout << "set time!" << "\n";
				} else if (time_entries[checked_id].end_time.year == 2000) { // year 2000 means it is not set
					time_entries[checked_id].end_day(get_current_time_as_time_class());
					hwlib::cout << "ended time!" << "\n";
				}
				hwlib::wait_ms(1000);
			}
		} else if (status == (int)Timetracker::status_data::Registering) {
			// fps.register_fingerprint();
		} else if (status == (int)Timetracker::status_data::Identifying) {

		} else if (status == (int)Timetracker::status_data::StoringData) {

		}
	}
}

// @brief Check each button and set the state of the timetracker
void Timetracker::check_buttons_and_store_status() {
	int old_status = status;

	while(!checking_pin.get()) { status = (int)Timetracker::status_data::Checking; }
	while(!registering_pin.get()) { status = (int)Timetracker::status_data::Registering; }
	while(!storing_pin.get()) { status = (int)Timetracker::status_data::StoringData; }

	if (old_status != status) {
		hwlib::cout << "Status changed: " << status << " | " << "Current time: " << rtc.get_uren() << ":" << rtc.get_minuten() << ":" << rtc.get_seconden() << "\n";
	}
}

// @brief Get current time from the rtc and give the time back as a Time class
Time Timetracker::get_current_time_as_time_class() {
	return Time(rtc.get_seconden(), rtc.get_minuten(), rtc.get_uren(), rtc.get_dag_week(), rtc.get_dag_maand(), rtc.get_maand(), rtc.get_jaar());
}