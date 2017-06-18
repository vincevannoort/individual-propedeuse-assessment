#include "fingerprintsensor.hpp"
#include "unit_tests.hpp"

#include <cstring>
#include "hwlib.hpp"

/*
Constructor & Deconstructor
*/

// @brief Constructor without parameters for testing purposes
Fingerprintsensor::Fingerprintsensor(): tx(hwlib::pin_out_dummy), rx(hwlib::pin_in_dummy) { hwlib::wait_ms(200); }

// @brief Constructor for Fingerprintsensor
// @param hwlib::pin_out & tx, pin used for sending data/commands
// @param hwlib::pin_in & rx, pin used for recieving data/commands
Fingerprintsensor::Fingerprintsensor(hwlib::pin_out & tx, hwlib::pin_in & rx): tx(tx), rx(rx) { hwlib::wait_ms(200); }

// @brief Deconstructor for Fingerprintsensor, terminates when program ends
Fingerprintsensor::~Fingerprintsensor(){ /* terminate(); */ }

/*
Constructors
*/

// @brief Constructor for Command packet
Fingerprintsensor::Command_packet::Command_packet(double_word input_parameter, word input_command, int input_baud_rate):
parameter(input_parameter), command(input_command), baud_rate(input_baud_rate)
{
    set_checksum(calculate_checksum());
    send(baud_rate);
}

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

// @brief Send the command which should be initialized from this packet via the UART protocol
// @param int input_baud_rate, controls the speed of the protocol
void Fingerprintsensor::Command_packet::send(int input_baud_rate) {
    auto tx_pin = hwlib::target::pin_out( hwlib::target::pins::d18 );
    auto green_led = hwlib::target::pin_out( hwlib::target::pins::d2 );

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

    green_led.set(1);
    for(const byte & packet_byte : packet) {
        hwlib::uart_putc_bit_banged_pin_custom_baudrate(packet_byte, tx_pin, input_baud_rate);
    }
    green_led.set(0);
}

// @brief Constructor for Response packet
Fingerprintsensor::Response_packet::Response_packet(int input_baud_rate): 
baud_rate(input_baud_rate)
{
    recieve(baud_rate);
}

// @brief Recieve command which polls for a response, then acquires the needed data
int Fingerprintsensor::Response_packet::recieve(int input_baud_rate) {
    auto tx_pin = hwlib::target::pin_out( hwlib::target::pins::d14 );
    auto rx_pin = hwlib::target::pin_in( hwlib::target::pins::d19 );
    auto red_led = hwlib::target::pin_out( hwlib::target::pins::d3 );

    red_led.set(1);
    for(byte & packet_byte : packet) {
        packet_byte = hwlib::uart_getc_bit_banged_pin_custom_baudrate(rx_pin, input_baud_rate);
    }
    red_led.set(0);

    /*
    Debugging OLED | TEMPORARY
    */
    hwlib::target::pin_oc scl                 = hwlib::target::pin_oc( hwlib::target::pins::scl );
    hwlib::target::pin_oc sda                 = hwlib::target::pin_oc( hwlib::target::pins::sda );
    hwlib::i2c_bus_bit_banged_scl_sda i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda( scl, sda );
    hwlib::glcd_oled oled                     = hwlib::glcd_oled( i2c_bus, 0x3c ); 

    hwlib::font_default_8x8 font              = hwlib::font_default_8x8();
    hwlib::window_ostream display             = hwlib::window_ostream( oled, font );

    display << "\f" << "Response: " 
    << "\n"
    << "0:[" << (int) packet[0] << "]" << " " 
    << "1:[" << (int) packet[1] << "]" << " " 
    << "\n"
    << "2:[" << (int) packet[2] << "]" << " " 
    << "3:[" << (int) packet[3] << "]" << " " 
    << "\n"
    << "4:[" << (int) packet[4] << "]" << " " 
    << "5:[" << (int) packet[5] << "]" << " " 
    << "\n"
    << "6:[" << (int) packet[6] << "]" << " " 
    << "7:[" << (int) packet[7] << "]" << " " 
    << "\n"
    << "8:[" << (int) packet[8] << "]" << " " 
    << "9:[" << (int) packet[9] << "]" << " " 
    << "\n"
    << "10:[" << (int) packet[10] << "]" << " " 
    << "11:[" << (int) packet[11] << "]" << hwlib::flush;
    return 0;
}

/*
Communication Commands functions
*/

// @brief Initialise the fingerprint sensor
int Fingerprintsensor::initialise() {
    Fingerprintsensor::Command_packet command_packet(0x00, ((word) command_packet_data::Open), baud_rate);
    Fingerprintsensor::Response_packet response_packet(baud_rate);

    if (debug) {
        hwlib::cout << "Initialise" << "\n";
        hwlib::cout << TEST_check_initialise_command_packet(command_packet.packet) << "\n";
    } 
    return 0;
}

// @brief Control the led
// @param bool on, true for turning the led on and false for turning it off
int Fingerprintsensor::control_led(bool on) {
    double input_parameter;
    if (on) { input_parameter = 0x01; } 
    else {  input_parameter = 0x00; }
    Fingerprintsensor::Command_packet command_packet(input_parameter, ((word) command_packet_data::CmosLed), baud_rate);
    Fingerprintsensor::Response_packet response_packet(baud_rate);

    if (debug) {
        hwlib::cout << "Control led" << "\n";
    } 
    return 0;
}

// @brief Change the internal baud rate of the fingerprint sensor
// @param int baud_rate, baud rate to set (between 9600 - 115200)
int Fingerprintsensor::change_baud_rate(int input_baud_rate) {
    Fingerprintsensor::Command_packet command_packet(input_baud_rate, ((word) command_packet_data::ChangeBaudrate), baud_rate);
    Fingerprintsensor::Response_packet response_packet(baud_rate);
    baud_rate = input_baud_rate;

    if (debug) {
        hwlib::cout << "Baud rate: " << baud_rate << " checksum = " << command_packet.calculate_checksum();
    }
    return 0;
}

// @brief Get enrolled fingerprint count
int Fingerprintsensor::get_enrolled_fingerprint_count() {
    Fingerprintsensor::Command_packet command_packet(0x00, ((word) command_packet_data::GetEnrollCount), baud_rate);
    Fingerprintsensor::Response_packet response_packet(baud_rate);

    if (debug) {
        hwlib::cout << "Get count" << "\n";
    } 
    return response_packet.packet[4];
}

// @brief Check status of fingerprint id
// @param int id, value between 0 - 19
int Fingerprintsensor::check_enrollment_status(int id) {
    Fingerprintsensor::Command_packet command_packet(id, ((word) command_packet_data::CheckEnrolled), baud_rate);
    Fingerprintsensor::Response_packet response_packet(baud_rate);

    if (debug) {
        hwlib::cout << "Check status" << "\n";
    }
    return 0;
}

// @brief Start a fingerprint enrollment
// @param int id, value between 0 - 19
int Fingerprintsensor::start_enrollment() {
    double input_parameter = get_enrolled_fingerprint_count();
    Fingerprintsensor::Command_packet command_packet(input_parameter, ((word) command_packet_data::EnrollStart), baud_rate);
    Fingerprintsensor::Response_packet response_packet(baud_rate);

    if (debug) {
        hwlib::cout << "Start enrollment" << "\n";
    }
    return 0;
}

// @brief Function for each of the 3 enrollments based on the parameter
// @param int template, control over the enrollment number
int Fingerprintsensor::enrollment(int template_number) {
    word input_command;
    if (template_number == 1) {
        input_command = ((word) command_packet_data::Enroll1);
    } else if ( template_number == 2) {
        input_command = ((word) command_packet_data::Enroll2);
    } else if ( template_number == 3) {
        input_command = ((word) command_packet_data::Enroll3);
    }
    Fingerprintsensor::Command_packet command_packet(0x00, input_command, baud_rate);
    Fingerprintsensor::Response_packet response_packet(baud_rate);

    if (debug) {
        hwlib::cout << "Enrollment" << "\n";
    }
    return 0;
}

// @brief Check if a finger sits on the fingerprintsensor
int Fingerprintsensor::check_finger_pressing_status() {
    Fingerprintsensor::Command_packet command_packet(0x00, ((word) command_packet_data::IsPressFinger), baud_rate);
    Fingerprintsensor::Response_packet response_packet(baud_rate);

    if (debug) {
        hwlib::cout << "Check fingerpress" << "\n";
    }
    return response_packet.packet[4] | response_packet.packet[5] | response_packet.packet[6] | response_packet.packet[7];
}

// @brief Delete one fingerprint based on a id
// @param int id, id to delete from the fingerprintsensor
int Fingerprintsensor::delete_one_fingerprint(int id) {
    Fingerprintsensor::Command_packet command_packet(id, ((word) command_packet_data::DeleteID), baud_rate);
    Fingerprintsensor::Response_packet response_packet(baud_rate);

    if (debug) {
        hwlib::cout << "Delete one fingerprint" << "\n";
    }
    return 0;
}

// @brief Delete one fingerprint based on a id
// @param int id, id to delete from the fingerprintsensor
int Fingerprintsensor::delete_all_fingerprints() {
    Fingerprintsensor::Command_packet command_packet(0x00, ((word) command_packet_data::DeleteAll), baud_rate);
    Fingerprintsensor::Response_packet response_packet(baud_rate);

    if (debug) {
        hwlib::cout << "Delete all fingerprints" << "\n";
    }
    return 0;
}

// @brief Verify a fingerprint based on id
// @param int id, id to verify a fingerprint with
int Fingerprintsensor::verification_1_1(int id) {
    Fingerprintsensor::Command_packet command_packet(id, ((word) command_packet_data::Verify1_1), baud_rate);
    Fingerprintsensor::Response_packet response_packet(baud_rate);

    if (debug) {
        hwlib::cout << "Verification 1:1" << "\n";
    }
    return 0;
}

// @brief Verify a fingerprint on all existing fingerprints
int Fingerprintsensor::identification_1_N() {
    Fingerprintsensor::Command_packet command_packet(0x00, ((word) command_packet_data::Identify1_N), baud_rate);
    Fingerprintsensor::Response_packet response_packet(baud_rate);

    if (debug) {
        hwlib::cout << "Verification 1:N" << "\n";
    }
    return 0;
}

// @brief Capture a fingerprint on the fingerprintsensor
// @param int quality, controls the quality taken with the fingerprintsensor
int Fingerprintsensor::capture_fingerprint(const char* quality) {
    double input_parameter;
    if (strcmp(quality, "fast")) {
        input_parameter = 0x00;
    } else if (strcmp(quality, "best")) {
        input_parameter = 0x01;
    }
    Fingerprintsensor::Command_packet command_packet(input_parameter, ((word) command_packet_data::CaptureFinger), baud_rate);
    Fingerprintsensor::Response_packet response_packet(baud_rate);

    if (debug) {
        hwlib::cout << "Capture" << "\n";
    }
    return 0;
}

// @brief Terminate/close the fingerprint sensor
int Fingerprintsensor::terminate() {
    Fingerprintsensor::Command_packet command_packet(0x00, ((word) command_packet_data::Close), baud_rate);
    Fingerprintsensor::Response_packet response_packet(baud_rate);

    if (debug) {
        hwlib::cout << "Terminate" << "\n";
    }
    return 0;
}

// @brief Register a fingerprint according to the steps to take in the datasheet
int Fingerprintsensor::register_fingerprint() {
    auto green_led = hwlib::target::pin_out( hwlib::target::pins::d4 );
    auto red_led = hwlib::target::pin_out( hwlib::target::pins::d5 );
    start_enrollment();

    red_led.set(1);
    control_led(true);
    while (check_finger_pressing_status()) {}
    capture_fingerprint("best");
    enrollment(1);
    green_led.set(1);
    while (!check_finger_pressing_status()) {}
    green_led.set(0); red_led.set(0);

    red_led.set(1);
    while (check_finger_pressing_status()) {}
    capture_fingerprint("best");
    enrollment(2);
    green_led.set(1);
    while (!check_finger_pressing_status()) {}
    green_led.set(0); red_led.set(0);
    
    red_led.set(1);
    while (check_finger_pressing_status()) {}
    capture_fingerprint("best");
    enrollment(3);
    green_led.set(1);
    control_led(false);
    green_led.set(0); red_led.set(0);

    if (debug) {
        hwlib::cout << "Registered fingerprint" << "\n";
    }
    return 0;
}