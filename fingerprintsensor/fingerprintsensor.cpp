#include "fingerprintsensor.hpp"

/*
Constructors
*/
Fingerprintsensor::Fingerprintsensor(hwlib::pin_out & tx, hwlib::pin_out & rx):  
tx(tx), rx(rx) {}

/*
Constructors
*/

// @brief Constructor for Command packet
Fingerprintsensor::Command_packet::Command_packet() {}

// @brief Set parameter for command packet
// @param double_word input_parameter, the parameter to set for the command packet
void Fingerprintsensor::Command_packet::set_parameter(double_word input_parameter) {
    parameter = input_parameter;
}

// @brief Set command for command packet
// @param word input_command, the command to set for the command packet
void Fingerprintsensor::Command_packet::set_command(word input_command) {
    command = input_command;
}

// @brief Set checksum for command packet
// @param word input_checksum, the checksum to set for the command packet
void Fingerprintsensor::Command_packet::set_checksum(word input_checksum) {
    checksum = input_checksum;
}

// @brief Calculate checksum for command packet
word Fingerprintsensor::Command_packet::calculate_checksum() {
    word temp_checksum = 0;
    temp_checksum += start_code1;
    temp_checksum += start_code2;
    temp_checksum += device_id;
    temp_checksum += parameter;
    temp_checksum += command;
    return temp_checksum;
}

// @brief Setup the needed parameters, commands and checksum
// @param double_word input_parameter, the parameter to set for the command packet
// @param word input_command, the command to set for the command packet
void Fingerprintsensor::Command_packet::setup_parameters_command_checksum(double_word input_parameter, word input_command) {
    set_parameter(input_parameter);
    set_command(input_command);
    set_checksum(calculate_checksum());
}

/*
Communication Commands functions
*/

// @brief Initialise the fingerprint sensor
int Fingerprintsensor::initialise() {
    Fingerprintsensor::Command_packet command_packet;
    command_packet.setup_parameters_command_checksum(0x00000000, Fingerprintsensor::command_packet::Open);
    return 0;
}

// @brief Control the led
// @param bool on, true for turning the led on and false for turning it off
int Fingerprintsensor::control_led(bool on) {
    Fingerprintsensor::Command_packet command_packet;
    double input_parameter;
    if (on) { input_parameter = 0x00000001; } 
    else {  input_parameter = 0x00000000; }
    command_packet.setup_parameters_command_checksum(input_parameter, Fingerprintsensor::command_packet::CmosLed);
    return 0;
}