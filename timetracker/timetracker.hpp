#ifndef TIMETRACKER_H
#define TIMETRACKER_H

#include "../fingerprintsensor/fingerprintsensor.hpp"
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
// class Employee {
// private:
// 	char first_name [20];
// 	char last_name [30];
// public:
// 	// Employee(char first_name [20], char last_name [30], byte hours_worked);
// 	Employee();
// 	char* get_name();
// 	void set_hours_worked();
// 	int get_hours_worked();
// };

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
	byte calculate_month();
};

/*
Workday
*/
class Workday {
private:
	Time start_time;
	Time end_time;
	// Employee employee_of_workday;
public:
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
	Fingerprintsensor & fps;
	i2cRTClib & rtc;
	hwlib::pin_in & checking_pin, & registering_pin, & storing_pin;

	int status;
	enum class status_data: int {
		Checking = 0,
		Registering = 1,
		Identifying = 2,
		StoringData = 3,
	};

	Workday time_entries;
public:
	Timetracker(Fingerprintsensor & fps, i2cRTClib & rtc, hwlib::pin_in & checking_pin, hwlib::pin_in & registering_pin, hwlib::pin_in & storing_pin, Time initial_time);
	void start_tracking();
	void check_buttons_and_store_status();
};

#endif