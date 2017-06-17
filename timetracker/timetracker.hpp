#ifndef TIMETRACKER_H
#define TIMETRACKER_H

#include <cstring>
#include "hwlib.hpp"

using namespace std;
typedef uint8_t byte;
typedef int16_t word;
typedef int32_t double_word;

class Time {
private:
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t date;
	uint8_t month;
	uint16_t year;
}

/*
Workday
*/
class Workday {
private:
	Time start_time;
	Time end_time;
public:
	Workday();
	void start_day();
	void end_day();
	uint_fast64_t calculate_milliseconds();
};

/*
Timetracker
*/
class Timetracker {
};

#endif