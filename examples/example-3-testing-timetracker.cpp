#include "./timetracker/timetracker.hpp"
#include "./timetracker/unit_tests.hpp"
#include "hwlib.hpp"
using namespace std;

int main() {
	WDT->WDT_MR = WDT_MR_WDDIS;

	/*
	Time ADT from timetracker
	*/
	TEST_time_minus_time();
}