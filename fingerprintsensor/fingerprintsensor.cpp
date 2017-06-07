#include "fingerprintsensor.hpp"
#include "hwlib.hpp"

/*
Constructor & Deconstructor
*/
Fingerprintsensor::Fingerprintsensor(hwlib::pin_out & tx, hwlib::pin_in & rx): tx(tx), rx(rx) {}
Fingerprintsensor::~Fingerprintsensor(){ terminate(); }

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

// @brief Send the command which should be initialized from this packet via the UART protocol
void Fingerprintsensor::Command_packet::send() {
    auto tx_pin = hwlib::target::pin_out( hwlib::target::pins::d18 );
    byte packet[12];
    packet[0] = start_code1;
    packet[1] = start_code2;
    packet[2] = device_id;
    packet[4] = parameter;
    packet[8] = command;
    packet[10] = checksum;
    for (const byte & packet_byte : packet) {
        hwlib::uart_putc_bit_banged_pin(packet_byte, tx_pin);
    }
}

/*
Communication Commands functions
*/

// @brief Initialise the fingerprint sensor
int Fingerprintsensor::initialise() {
    Fingerprintsensor::Command_packet command_packet;
    command_packet.setup_parameters_command_checksum(0x01, ((word) command_packet_data::Open));
    command_packet.send();

    if (debug) {
        display << "\f" << "Initialise:" << "\n" << command_packet.calculate_checksum() << hwlib::flush;
    } 
    return 0;
}

// @brief Control the led
// @param bool on, true for turning the led on and false for turning it off
int Fingerprintsensor::control_led(bool on) {
    Fingerprintsensor::Command_packet command_packet;
    double input_parameter;
    if (on) { input_parameter = 0x01; } 
    else {  input_parameter = 0x00; }
    command_packet.setup_parameters_command_checksum(input_parameter, ((word) command_packet_data::CmosLed));
    command_packet.send();

    if (debug) {
        display << "\f" << "Control_led:" << "\n" << command_packet.calculate_checksum() << hwlib::flush;
    } 
    return 0;
}

// @brief Get enrolled fingerprint count
int Fingerprintsensor::get_enrolled_fingerprint_count() {
    Fingerprintsensor::Command_packet command_packet;
    command_packet.setup_parameters_command_checksum(0x00, ((word) command_packet_data::GetEnrollCount));
    command_packet.send();
    int count = 0; // TEMPORARY TILL RESPONSE IMPLEMENTED

    if (debug) {
        display << "\f" << "Get count:" << "\n" << command_packet.calculate_checksum() << hwlib::flush;
    } 
    return count;
}

// @brief Start a fingerprint enrollment
// @param int id, value between 0 - 19
int Fingerprintsensor::start_enrollment(int id) {
    Fingerprintsensor::Command_packet command_packet;
    double input_parameter = get_enrolled_fingerprint_count();
    command_packet.setup_parameters_command_checksum(input_parameter, ((word) command_packet_data::EnrollStart));
    command_packet.send();

    if (debug) {
        display << "\f" << "Start enrollment:" << "\n" << command_packet.calculate_checksum() << hwlib::flush;
    }
    return 0;
}

// @brief Terminate/close the fingerprint sensor
int Fingerprintsensor::terminate() {
    Fingerprintsensor::Command_packet command_packet;
    command_packet.setup_parameters_command_checksum(0x00, ((word) command_packet_data::Close));
    command_packet.send();

    if (debug) {
        display << "\f" << "Terminate:" << "\n" << command_packet.calculate_checksum() << hwlib::flush;
    }
    return 0;
}