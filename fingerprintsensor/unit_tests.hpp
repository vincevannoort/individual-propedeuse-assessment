#include "./fingerprintsensor/fingerprintsensor.hpp"

// Test 1: Check if initialise command packet contains the right bits to send
bool TEST_check_initialise_command_packet(const byte (& command_packet) [12]) {
	if (
		command_packet[0] == 0x55 &&
		command_packet[1] == 0xAA &&
		command_packet[2] == 0x01 &&
		command_packet[3] == 0x00 &&
		command_packet[4] == 0x00 &&
		command_packet[5] == 0x00 &&
		command_packet[6] == 0x00 &&
		command_packet[7] == 0x00 &&
		command_packet[8] == 0x01 &&
		command_packet[9] == 0x00 &&
		command_packet[10] == 0x01 &&
		command_packet[11] == 0x01
	)
}

// Test 2: Check if initialise response packet contains the right bits
bool TEST_check_initialise_response_packet(const byte (& response_packet) [12]) {
	if (
		response_packet[0] == 0x55 &&
		response_packet[1] == 0xAA &&
		response_packet[2] == 0x01 &&
		response_packet[3] == 0x00 &&
		response_packet[4] == 0x00 &&
		response_packet[5] == 0x00 &&
		response_packet[6] == 0x00 &&
		response_packet[7] == 0x00 &&
		response_packet[8] == 0x30 &&
		response_packet[9] == 0x00 &&
		response_packet[10] == 0x30 &&
		response_packet[11] == 0x01
	)
}

// Test 3: Check if led on command packet contains the right bits to send
bool TEST_check_led_on_command_packet(const byte (& command_packet) [12]) {
	if (
		command_packet[0] == 0x55 &&
		command_packet[1] == 0xAA &&
		command_packet[2] == 0x01 &&
		command_packet[3] == 0x00 &&
		command_packet[4] == 0x01 &&
		command_packet[5] == 0x00 &&
		command_packet[6] == 0x00 &&
		command_packet[7] == 0x00 &&
		command_packet[8] == 0x12 &&
		command_packet[9] == 0x00 &&
		command_packet[10] == 0x13 &&
		command_packet[11] == 0x01
	)
}

// Test 4: Check if led on response packet contains the right bits
bool TEST_check_led_on_response_packet(const byte (& response_packet) [12]) {
	if (
		response_packet[0] == 0x55 &&
		response_packet[1] == 0xAA &&
		response_packet[2] == 0x01 &&
		response_packet[3] == 0x00 &&
		response_packet[4] == 0x00 &&
		response_packet[5] == 0x00 &&
		response_packet[6] == 0x00 &&
		response_packet[7] == 0x00 &&
		response_packet[8] == 0x30 &&
		response_packet[9] == 0x00 &&
		response_packet[10] == 0x30 &&
		response_packet[11] == 0x01
	)
}

// Test 5: Check if led off command packet contains the right bits to send
bool TEST_check_led_off_command_packet(const byte (& command_packet) [12]) {
	if (
		command_packet[0] == 0x55 &&
		command_packet[1] == 0xAA &&
		command_packet[2] == 0x01 &&
		command_packet[3] == 0x00 &&
		command_packet[4] == 0x01 &&
		command_packet[5] == 0x00 &&
		command_packet[6] == 0x00 &&
		command_packet[7] == 0x00 &&
		command_packet[8] == 0x12 &&
		command_packet[9] == 0x00 &&
		command_packet[10] == 0x12 &&
		command_packet[11] == 0x01
	)
}

// Test 6: Check if led off response packet contains the right bits
bool TEST_check_led_off_response_packet(const byte (& response_packet) [12]) {
	if (
		response_packet[0] == 0x55 &&
		response_packet[1] == 0xAA &&
		response_packet[2] == 0x01 &&
		response_packet[3] == 0x00 &&
		response_packet[4] == 0x00 &&
		response_packet[5] == 0x00 &&
		response_packet[6] == 0x00 &&
		response_packet[7] == 0x00 &&
		response_packet[8] == 0x30 &&
		response_packet[9] == 0x00 &&
		response_packet[10] == 0x30 &&
		response_packet[11] == 0x01
	)
}