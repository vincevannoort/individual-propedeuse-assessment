#include <cstring>
#include <string>
#include "timetracker.hpp"
#include "hwlib.hpp"
using namespace std;

/* ==========================================================================================================================
Employee
*/ // =======================================================================================================================

// @brief Contructor for Employee
Employee::Employee(){ 
	char first_name_array[] = "Vince";
	char last_name_array[] = "van Noort";
	strncpy(first_name, first_name_array, sizeof(first_name_array));
	strncpy(last_name, last_name_array, sizeof(last_name_array));
}

// @brief Contructor for Employee with names
Employee::Employee(const char* first_name_array, int size_first_name, const char* last_name_array, int size_last_name) {
	strncpy(first_name, first_name_array, size_first_name);
	strncpy(last_name, last_name_array, size_last_name);
}

/* ==========================================================================================================================
Workday
*/ // =======================================================================================================================

// @brief Contructor for Workday
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
	return end_time - start_time; // end_time - start_time;
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

Time Time::operator-( const Time & rhs ) const {
	int difference_seconds = 0;
	int difference_minutes = 0;
	int difference_hours = 0;

	if(seconds < rhs.seconds) {
        --difference_minutes;
        difference_seconds += 60;
    }
    if(minutes > rhs.minutes) {
        --difference_hours;
        difference_minutes += 60;
    }

    difference_seconds += (seconds - rhs.seconds);
    difference_minutes += (minutes - rhs.minutes);
    difference_hours += (hours - rhs.hours);
    return Time(difference_seconds, difference_minutes, difference_hours, day, date, month, year);
}

/* ==========================================================================================================================
Timetracker
*/ // =======================================================================================================================

// @brief Constructor for the timetracker
Timetracker::Timetracker(fingeprintsensor_interface * fps, i2cRTClib & rtc, hwlib::pin_in & checking_pin, hwlib::pin_in & registering_pin, hwlib::pin_in & storing_pin, Time initial_time):
fps(fps), rtc(rtc), checking_pin(checking_pin), registering_pin(registering_pin), storing_pin(storing_pin), status(0) {
	rtc.set_time(initial_time.seconds, initial_time.minutes, initial_time.hours, initial_time.day, initial_time.date, initial_time.month, initial_time.year);
}

// @brief Initialise, set time of current day
void Timetracker::start_tracking() {
	while(true) {
		check_buttons_and_store_status();
		
		if (status == (int)Timetracker::status_data::Checking) {
			int checked_id = fps->identify_fingerprint_try_check_finger_pressing_status_once();
			if (checked_id >= 0) {
				if (time_entries[checked_id].start_time.year == 2000) { // year 2000 means it is not set
					hwlib::cout << "Goodmorning " << time_entries[checked_id].employee_of_workday.first_name << " [" << checked_id << "]" << ", current time: " << hwlib::setfill('0') << hwlib::setw(2) << rtc.get_uren() << ":" << hwlib::setfill('0') << hwlib::setw(2) << rtc.get_minuten() << ":" << hwlib::setfill('0') << hwlib::setw(2) << rtc.get_seconden() << "\n";
					time_entries[checked_id].start_day(get_current_time_as_time_class());
				} else if (time_entries[checked_id].end_time.year == 2000) { // year 2000 means it is not set
					hwlib::cout << "Have a nice day " << time_entries[checked_id].employee_of_workday.first_name << " [" << checked_id << "]" << ", current time: " << hwlib::setfill('0') << hwlib::setw(2) << rtc.get_uren() << ":" << hwlib::setfill('0') << hwlib::setw(2) << rtc.get_minuten() << ":" << hwlib::setfill('0') << hwlib::setw(2) << rtc.get_seconden() << "\n";
					time_entries[checked_id].end_day(get_current_time_as_time_class());
				} else if (time_entries[checked_id].start_time.year != 2000 && time_entries[checked_id].end_time.year != 2000) {
					hwlib::cout << "You've already tracked your time for today " << time_entries[checked_id].employee_of_workday.first_name << " [" << checked_id << "]" << ", try again another day!" << "\n";
				}
				hwlib::wait_ms(1000);
			}
		} else if (status == (int)Timetracker::status_data::Registering) {
			fps->register_fingerprint();
			status = (int)Timetracker::status_data::Checking;
			display_change_status(status);
		} else if (status == (int)Timetracker::status_data::StoringData) {
			for(Workday entry : time_entries) {
				if (entry.start_time.year != 2000 && entry.end_time.year != 2000) {
					hwlib::cout << "Time entry from " << entry.employee_of_workday.first_name << ", worked for: " << hwlib::setfill('0') << hwlib::setw(2) << rtc.get_uren() << ":" << hwlib::setfill('0') << hwlib::setw(2) << rtc.get_minuten() << ":" << hwlib::setfill('0') << hwlib::setw(2) << rtc.get_seconden() << "\n";
				}
			}
			status = (int)Timetracker::status_data::Checking;
			display_change_status(status);
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
		display_change_status(status);
	}
}

// @brief Display the change of status with the current time included
void Timetracker::display_change_status(int status) {
	hwlib::cout << "Status changed: " << status << " | " << "Current time: " << rtc.get_uren() << ":" << rtc.get_minuten() << ":" << rtc.get_seconden() << "\n";
}

// @brief Get current time from the rtc and give the time back as a Time class
Time Timetracker::get_current_time_as_time_class() {
	return Time(rtc.get_seconden(), rtc.get_minuten(), rtc.get_uren(), rtc.get_dag_week(), rtc.get_dag_maand(), rtc.get_maand(), rtc.get_jaar());
}

void Timetracker::set_workday_employee(int employee_id, Employee employee_to_copy) {
	time_entries[employee_id].employee_of_workday = employee_to_copy;
}