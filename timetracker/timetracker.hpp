#ifndef TIMETRACKER_H
#define TIMETRACKER_H

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
private:
	char[20] first_name;
	char[30] last_name;
public:
	Employee(char[20] first_name, char[30] last_name, byte hours_worked);
	char[50] get_name();
	void set_hours_worked();
	int get_hours_worked();
}

/*
Time
*/
class Time {
private:
	byte seconds;
	byte minutes;
	byte hours;
	byte date;
	byte month;
	word year;
public:
	Time(byte seconds, byte minutes, byte hours, byte date, byte month, word year)
}

/*
Workday
*/
class Workday {
private:
	Time start_time;
	Time end_time;
	Employee employee_of_workday;
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
	Employee employees[20];
	Workday workdays_from_each_employee;
public:
	Timetracker(Employee employees[20]);
};

#endif