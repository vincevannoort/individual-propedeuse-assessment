#include "fingerprintsensor.hpp"

#include <cstring>
#include "hwlib.hpp"

/*
Constructor & Deconstructor
*/

// @brief Constructor for Fingerprintsensor
// @param hwlib::pin_out & tx, pin used for sending data/commands
// @param hwlib::pin_in & rx, pin used for recieving data/commands
Fingerprintsensor::Fingerprintsensor(hwlib::pin_out & tx, hwlib::pin_in & rx): tx(tx), rx(rx) { hwlib::wait_ms(250); }

// @brief Deconstructor for Fingerprintsensor, terminates when program ends
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
// @param int input_baud_rate, controls the speed of the protocol
void Fingerprintsensor::Command_packet::send(int input_baud_rate) {
    auto tx_pin = hwlib::target::pin_out( hwlib::target::pins::d18 );
    byte packet[12];
    packet[0] = start_code1;
    packet[1] = start_code2;
    packet[2] = device_id & 0xFF;
    packet[3] = (device_id >> 8) & 0xFF;
    packet[4] = parameter & 0xFF;
    packet[5] = (parameter >> 8) & 0xFF;
    packet[6] = (parameter >> 16) & 0xFF;
    packet[7] = (parameter >> 24) & 0xFF;
    packet[8] = command & 0xFF;
    packet[9] = (command >> 8) & 0xFF;
    packet[10] = checksum & 0xFF;
    packet[11] = (checksum >> 8) & 0xFF;

    hwlib::cout << "\n\n";
    for (const byte & packet_byte : packet) {
        // hwlib::uart_putc_bit_banged_pin(packet_byte, tx_pin);
        hwlib::uart_putc_bit_banged_pin_custom_baudrate(packet_byte, tx_pin, input_baud_rate);
        hwlib::cout << hwlib::hex << hwlib::setw(2) << hwlib::setfill('0') << (int) packet_byte << "-";
    }
    hwlib::cout << "\n";
}

// @brief Recieve command which polls for a response, then acquires the needed data
int Fingerprintsensor::Response_packet::recieve() {
    auto rx_pin = hwlib::target::pin_in( hwlib::target::pins::d19 );
    hwlib::cout << "\n" << "Polling response:";
    hwlib::cout << hwlib::uart_getc_bit_banged_pin(rx_pin);
    return 0;
}

/*
Communication Commands functions
*/

// @brief Initialise the fingerprint sensor
int Fingerprintsensor::initialise() {
    Fingerprintsensor::Command_packet command_packet;
    command_packet.setup_parameters_command_checksum(0x01, ((word) command_packet_data::Open));
    command_packet.send(baud_rate);

    // Fingerprintsensor::Response_packet response_packet;
    // response_packet.recieve();

    if (debug) {
        hwlib::cout << "Initialise: " << "checksum = " << command_packet.calculate_checksum();
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
    command_packet.send(baud_rate);

    if (debug) {
        hwlib::cout << "Control led: " << "checksum = " << command_packet.calculate_checksum();
    } 
    return 0;
}

// @brief Change the internal baud rate of the fingerprint sensor
// @param int baud_rate, baud rate to set (between 9600 - 115200)
int Fingerprintsensor::change_baud_rate(int input_baud_rate) {
    Fingerprintsensor::Command_packet command_packet;
    command_packet.setup_parameters_command_checksum(input_baud_rate, ((word) command_packet_data::ChangeBaudrate));
    command_packet.send(baud_rate);
    baud_rate = input_baud_rate;

    if (debug) {
        hwlib::cout << "Baud rate: " << baud_rate << " checksum = " << command_packet.calculate_checksum();
    }
    return 0;
}

// @brief Get enrolled fingerprint count
int Fingerprintsensor::get_enrolled_fingerprint_count() {
    Fingerprintsensor::Command_packet command_packet;
    command_packet.setup_parameters_command_checksum(0x00, ((word) command_packet_data::GetEnrollCount));
    command_packet.send(baud_rate);
    int count = 0; // TEMPORARY TILL RESPONSE IMPLEMENTED

    if (debug) {
        hwlib::cout << "Get count: " << "checksum = " << command_packet.calculate_checksum();
    } 
    return count;
}

// @brief Start a fingerprint enrollment
// @param int id, value between 0 - 19
int Fingerprintsensor::start_enrollment(int id) {
    Fingerprintsensor::Command_packet command_packet;
    double input_parameter = get_enrolled_fingerprint_count();
    command_packet.setup_parameters_command_checksum(input_parameter, ((word) command_packet_data::EnrollStart));
    command_packet.send(baud_rate);

    if (debug) {
        hwlib::cout << "Start enrollment: " << "checksum = " << command_packet.calculate_checksum();
    }
    return 0;
}

// @brief Function for each of the 3 enrollments based on the parameter
// @param int template, control over the enrollment number
int Fingerprintsensor::enrollment(int template_number) {
    Fingerprintsensor::Command_packet command_packet;
    word input_command;
    if (template_number == 1) {
        input_command = ((word) command_packet_data::Enroll1);
    } else if ( template_number == 2) {
        input_command = ((word) command_packet_data::Enroll2);
    } else if ( template_number == 3) {
        input_command = ((word) command_packet_data::Enroll3);
    }
    command_packet.setup_parameters_command_checksum(0x00, input_command);
    command_packet.send(baud_rate);

    if (debug) {
        hwlib::cout << "Enrollment: " << "checksum = " << command_packet.calculate_checksum();
    }
    return 0;
}

// @brief Capture a fingerprint on the fingerprintsensor
// @param int quality, controls the quality taken with the fingerprintsensor.
int Fingerprintsensor::capture_fingerprint(char quality[]) {
    Fingerprintsensor::Command_packet command_packet;
    double input_parameter;
    if (strcmp(quality, "fast")) {
        input_parameter = 0x00;
    } else if (strcmp(quality, "best")) {
        input_parameter = 0x01;
    }
    command_packet.setup_parameters_command_checksum(input_parameter, ((word) command_packet_data::CaptureFinger));
    command_packet.send(baud_rate);

    if (debug) {
        hwlib::cout << "Capture: " << "checksum = " << command_packet.calculate_checksum();
    }
    return 0;
}

// @brief Terminate/close the fingerprint sensor
int Fingerprintsensor::terminate() {
    Fingerprintsensor::Command_packet command_packet;
    command_packet.setup_parameters_command_checksum(0x00, ((word) command_packet_data::Close));
    command_packet.send(baud_rate);

    if (debug) {
        hwlib::cout << "Terminate: " << "checksum = " << command_packet.calculate_checksum();
    }
    return 0;
}