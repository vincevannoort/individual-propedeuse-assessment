#ifndef TIMETRACKER_H
#define TIMETRACKER_H

#include "../GT511C1R/GT511C1R.hpp"
#include "../realtimeclock/i2cRTC.hpp"
#include <cstring>
#include "hwlib.hpp"

using namespace std;
typedef uint8_t byte;
typedef int16_t word;
typedef int32_t double_word;

/*
Employee
*/
class Employee {
public:
	char first_name[20];
	char last_name[30];
	Employee();
	Employee(const char* first_name_array, int size_first_name, const char* last_name_array, int size_last_name);
};

/*
Time
*/
class Time {
public:
	byte seconds;
	byte minutes;
	byte hours;
	byte day;
	byte date;
	byte month;
	word year;
	Time();
	Time(byte seconds, byte minutes, byte hours, byte day, byte date, byte month, word year);
};

/*
Workday
*/
class Workday {
public:
	Time start_time;
	Time end_time;
	Employee employee_of_workday;
	Workday();
	void start_day(Time t);
	void end_day(Time t);
	uint_fast64_t calculate_work_time();
};

/*
Timetracker
*/
class Timetracker {
private:
	GT511C1R & fps;
	i2cRTClib & rtc;
	hwlib::pin_in & checking_pin, & registering_pin, & storing_pin;

	int status;
	enum class status_data: int {
		Checking = 0,
		Registering = 1,
		StoringData = 2,
	};

	Workday time_entries[20];
public:
	Timetracker(GT511C1R & fps, i2cRTClib & rtc, hwlib::pin_in & checking_pin, hwlib::pin_in & registering_pin, hwlib::pin_in & storing_pin, Time initial_time);
	void start_tracking();
	void check_buttons_and_store_status();
	void display_change_status(int status);
	Time get_current_time_as_time_class();
	void set_workday_employee(int employee_id, Employee employee_to_copy);
};

#endif