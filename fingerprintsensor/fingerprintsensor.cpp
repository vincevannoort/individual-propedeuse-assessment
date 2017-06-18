#include "fingerprintsensor.hpp"
#include "unit_tests.hpp"

#include <cstring>
#include "hwlib.hpp"

#define debug 0

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
    send();
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
void Fingerprintsensor::Command_packet::send() {
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
        hwlib::uart_putc_bit_banged_pin_custom_baudrate(packet_byte, tx_pin, baud_rate);
    }
    green_led.set(0);
}

// @brief Constructor for Response packet
Fingerprintsensor::Response_packet::Response_packet(int input_baud_rate): 
baud_rate(input_baud_rate)
{
    recieve();
}

int Fingerprintsensor::Response_packet::get_parameter() {
    return parameter;
}

// @brief Recieve command which polls for a response, then acquires the needed data
void Fingerprintsensor::Response_packet::recieve() {
    auto rx_pin = hwlib::target::pin_in( hwlib::target::pins::d19 );
    auto red_led = hwlib::target::pin_out( hwlib::target::pins::d3 );

    red_led.set(1);
    for(byte & packet_byte : packet) {
        packet_byte = hwlib::uart_getc_bit_banged_pin_custom_baudrate(rx_pin, baud_rate);
    }
    red_led.set(0);

    parameter = packet[4];
    parameter = (packet[5] << 8) | parameter;
    parameter = (packet[6] << 16) | parameter;
    parameter = (packet[7] << 24) | parameter;
    response = packet[8];
    response = (packet[9] << 8) | response;
    checksum = packet[10];
    checksum = (packet[11] << 8) | checksum;

    if (debug) {
        auto tx_pin = hwlib::target::pin_out( hwlib::target::pins::d14 );
        hwlib::uart_putc_bit_banged_pin_custom_baudrate('\n', tx_pin, baud_rate);
        hwlib::uart_putc_bit_banged_pin_custom_baudrate(packet[4], tx_pin, baud_rate);

        /*
        Debugging OLED
        */
        hwlib::target::pin_oc scl                 = hwlib::target::pin_oc( hwlib::target::pins::scl );
        hwlib::target::pin_oc sda                 = hwlib::target::pin_oc( hwlib::target::pins::sda );
        hwlib::i2c_bus_bit_banged_scl_sda i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda( scl, sda );
        hwlib::glcd_oled oled                     = hwlib::glcd_oled( i2c_bus, 0x3c ); 

        hwlib::font_default_8x8 font              = hwlib::font_default_8x8();
        hwlib::window_ostream display             = hwlib::window_ostream( oled, font );

        switch(parameter) {
            case 0x0000:
                display << "\f" << "NO_ERROR" << "\n" << parameter << "\n" << response << hwlib::flush; break;
            case 0x1001:
                display << "\f" << "TIMEOUT" << "\n" << parameter << "\n" << response << hwlib::flush; break;
            case 0x1002:
                display << "\f" << "INVALID_BAUDRATE" << "\n" << parameter << "\n" << response << hwlib::flush; break;
            case 0x1003:
                display << "\f" << "INVALID_POS" << "\n" << parameter << "\n" << response << hwlib::flush; break;
            case 0x1004:
                display << "\f" << "IS_NOT_USED" << "\n" << parameter << "\n" << response << hwlib::flush; break;
            case 0x1005:
                display << "\f" << "IS_ALREADY_USED" << "\n" << parameter << "\n" << response << hwlib::flush; break;
            case 0x1006:
                display << "\f" << "COMM_ERR" << "\n" << parameter << "\n" << response << hwlib::flush; break;
            case 0x1007:
                display << "\f" << "VERIFY_FAILED" << "\n" << parameter << "\n" << response << hwlib::flush; break;
            case 0x1008:
                display << "\f" << "IDENTIFY_FAILED" << "\n" << parameter << "\n" << response << hwlib::flush; break;
            case 0x1009:
                display << "\f" << "DB_IS_FULL" << "\n" << parameter << "\n" << response << hwlib::flush; break;
            case 0x100A:
                display << "\f" << "DB_IS_EMPTY" << "\n" << parameter << "\n" << response << hwlib::flush; break;
            case 0x100B:
                display << "\f" << "TURN_ERR" << "\n" << parameter << "\n" << response << hwlib::flush; break;
            case 0x100C:
                display << "\f" << "BAD_FINGER" << "\n" << parameter << "\n" << response << hwlib::flush; break;
            case 0x100D:
                display << "\f" << "ENROLL_FAILED" << "\n" << parameter << "\n" << response << hwlib::flush; break;
            case 0x100E:
                display << "\f" << "IS_NOT_SUPPORTED" << "\n" << parameter << "\n" << response << hwlib::flush; break;
            case 0x100F:
                display << "\f" << "DEV_ERR" << "\n" << parameter << "\n" << response << hwlib::flush; break;
            case 0x1010:
                display << "\f" << "CAPTURE_CANCELED" << "\n" << parameter << "\n" << response << hwlib::flush; break;
            case 0x1011:
                display << "\f" << "INVALID_PARAM" << "\n" << parameter << "\n" << response << hwlib::flush; break;
            case 0x1012:
                display << "\f" << "FINGER_IS_NOT_PRESSED" << "\n" << parameter << "\n" << response << hwlib::flush; break;
            case 0XFFFF:
                display << "\f" << "INVALID" << "\n" << parameter << "\n" << response << hwlib::flush; break;
            default:
                display << "\f" << "Nothing" << "\n" << parameter << "\n" << response << hwlib::flush;
        }
    }
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

    auto tx_pin_debug = hwlib::target::pin_out( hwlib::target::pins::d14 );
    hwlib::uart_putc_bit_banged_pin_custom_baudrate('\n', tx_pin_debug, baud_rate);
    hwlib::uart_putc_bit_banged_pin_custom_baudrate(command_packet.packet[4], tx_pin_debug, baud_rate);
    hwlib::uart_putc_bit_banged_pin_custom_baudrate(command_packet.packet[5], tx_pin_debug, baud_rate);
    hwlib::uart_putc_bit_banged_pin_custom_baudrate(command_packet.packet[6], tx_pin_debug, baud_rate);
    hwlib::uart_putc_bit_banged_pin_custom_baudrate(command_packet.packet[7], tx_pin_debug, baud_rate);
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

    return response_packet.get_parameter();
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
    return response_packet.get_parameter();
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

    auto tx_pin = hwlib::target::pin_out( hwlib::target::pins::d14 );
    hwlib::uart_putc_bit_banged_pin_custom_baudrate('\n', tx_pin, baud_rate);
    hwlib::uart_putc_bit_banged_pin_custom_baudrate('\n', tx_pin, baud_rate);
    hwlib::uart_putc_bit_banged_pin_custom_baudrate('\n', tx_pin, baud_rate);
    hwlib::uart_putc_bit_banged_pin_custom_baudrate(response_packet.packet[4], tx_pin, baud_rate);
    hwlib::uart_putc_bit_banged_pin_custom_baudrate(response_packet.packet[5], tx_pin, baud_rate);
    hwlib::uart_putc_bit_banged_pin_custom_baudrate(response_packet.packet[6], tx_pin, baud_rate);
    hwlib::uart_putc_bit_banged_pin_custom_baudrate(response_packet.packet[7], tx_pin, baud_rate);
    hwlib::uart_putc_bit_banged_pin_custom_baudrate('\t', tx_pin, baud_rate);
    hwlib::uart_putc_bit_banged_pin_custom_baudrate(response_packet.packet[8], tx_pin, baud_rate);
    hwlib::uart_putc_bit_banged_pin_custom_baudrate(response_packet.packet[9], tx_pin, baud_rate);

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

// @brief Identify a fingerprint according to the steps to take in the datasheet
int Fingerprintsensor::identify_fingerprint() {
    auto green_led = hwlib::target::pin_out( hwlib::target::pins::d4 );
    auto red_led = hwlib::target::pin_out( hwlib::target::pins::d5 );

    red_led.set(1);
    control_led(true);
    while (check_finger_pressing_status()) {}
    capture_fingerprint("best");
    identification_1_N();
    control_led(false);
    green_led.set(1);
    green_led.set(0); red_led.set(0);

    if (debug) {
        hwlib::cout << "Identy fingerprint" << "\n";
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

    hwlib::wait_ms(300);

    red_led.set(1);
    while (check_finger_pressing_status()) {}
    capture_fingerprint("best");
    enrollment(2);
    green_led.set(1);
    while (!check_finger_pressing_status()) {}
    green_led.set(0); red_led.set(0);

    hwlib::wait_ms(300);
    
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