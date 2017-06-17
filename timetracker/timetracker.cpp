#include <cstring>
#include "timetracker.hpp"
#include "hwlib.hpp"

// @brief Constructor for a workday used by timetracker
Workday::Workday() {}

// @brief Set start time of current workday
void Workday::start_day() {
	start_time = 0;
}

// @brief Set end time of current workday
void Workday::end_day() {
	end_time = 0;
}

// @brief Calculate hours worked
uint_fast64_t Workday::calculate_milliseconds() {
	return end_time - start_time;
}