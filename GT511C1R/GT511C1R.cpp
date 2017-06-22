#include "GT511C1R.hpp"
#include "unit_tests.hpp"

#include <cstring>
#include "hwlib.hpp"

#define debug 0
#define testing 0
#define ERROR_NO_VALID_RESPONSE -5

/* ==========================================================================================================================
Constructor, Deconstructor, Command packets & Response packets
*/ // =======================================================================================================================

/// @brief Constructor without parameters for testing purposes
GT511C1R::GT511C1R(): tx(hwlib::pin_out_dummy), rx(hwlib::pin_in_dummy) { hwlib::wait_ms(200); initialise(); }

/// @brief Constructor for GT511C1R
/// @param hwlib::pin_out & tx, pin used for sending data/commands
/// @param hwlib::pin_in & rx, pin used for recieving data/commands
GT511C1R::GT511C1R(hwlib::pin_out & tx, hwlib::pin_in & rx): tx(tx), rx(rx) { hwlib::wait_ms(200); initialise(); }

/// @brief Deconstructor for GT511C1R, terminates when program ends
GT511C1R::~GT511C1R(){ /* terminate(); */ }

/*
Constructors
*/

/// @brief Constructor for Command packet, after calculating the command packet is send
GT511C1R::Command_packet::Command_packet(double_word input_parameter, word input_command, int input_baud_rate, hwlib::pin_out & tx):
parameter(input_parameter), command(input_command), baud_rate(input_baud_rate), tx_pin(tx)
{
    set_checksum(calculate_checksum());
    send();
}

/// @brief Set parameter for command packet
/// @param double_word input_parameter, the parameter to set for the command packet
void GT511C1R::Command_packet::set_parameter(double_word input_parameter) {
    parameter = input_parameter;
}

/// @brief Set command for command packet
/// @param word input_command, the command to set for the command packet
void GT511C1R::Command_packet::set_command(word input_command) {
    command = input_command;
}

/// @brief Set checksum for command packet
/// @param word input_checksum, the checksum to set for the command packet
void GT511C1R::Command_packet::set_checksum(word input_checksum) {
    checksum = input_checksum;
}

/// @brief Calculate checksum for command packet
/// @return word temp_checksum, the calculated checksum
word GT511C1R::Command_packet::calculate_checksum() {
    word temp_checksum = 0;
    temp_checksum += start_code1;
    temp_checksum += start_code2;  
    temp_checksum += device_id;
    temp_checksum += parameter;
    temp_checksum += command;
    return temp_checksum;
}

/// @brief Send the command which should be initialized from this packet via the UART protocol
/// @param int input_baud_rate, controls the speed of the protocol
void GT511C1R::Command_packet::send() {
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

/// @brief Constructor for Response packet, tries to recieve the response packet after setup
GT511C1R::Response_packet::Response_packet(int input_baud_rate, hwlib::pin_in & rx): 
baud_rate(input_baud_rate), rx_pin(rx)
{
    recieve();
}

/// @brief Function for getting parameter data of the protected variable
double_word GT511C1R::Response_packet::get_parameter_data() {
    return parameter;
}

/// @brief Function for getting response data of the protected variable
word GT511C1R::Response_packet::get_response_data() {
    return response;
}

/// @brief Recieve command which polls for a response, then acquires the needed data
void GT511C1R::Response_packet::recieve() {
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
        switch( (double_word)parameter ) {
            case (double_word)GT511C1R::response_packet_data::NO_ERROR:
                hwlib::cout << "NO_ERROR" << "\n"; break;
            case (double_word)GT511C1R::response_packet_data::NACK_TIMEOUT:
                hwlib::cout << "TIMEOUT" << "\n"; break;
            case (double_word)GT511C1R::response_packet_data::NACK_INVALID_BAUDRATE:
                hwlib::cout << "INVALID_BAUDRATE" << "\n"; break;
            case (double_word)GT511C1R::response_packet_data::NACK_INVALID_POS:
                hwlib::cout << "INVALID_POS" << "\n"; break;
            case (double_word)GT511C1R::response_packet_data::NACK_IS_NOT_USED:
                hwlib::cout << "IS_NOT_USED" << "\n"; break;
            case (double_word)GT511C1R::response_packet_data::NACK_IS_ALREADY_USED:
                hwlib::cout << "IS_ALREADY_USED" << "\n"; break;
            case (double_word)GT511C1R::response_packet_data::NACK_COMM_ERR:
                hwlib::cout << "COMM_ERR" << "\n"; break;
            case (double_word)GT511C1R::response_packet_data::NACK_VERIFY_FAILED:
                hwlib::cout << "VERIFY_FAILED" << "\n"; break;
            case (double_word)GT511C1R::response_packet_data::NACK_IDENTIFY_FAILED:
                hwlib::cout << "IDENTIFY_FAILED" << "\n"; break;
            case (double_word)GT511C1R::response_packet_data::NACK_DB_IS_FULL:
                hwlib::cout << "DB_IS_FULL" << "\n"; break;
            case (double_word)GT511C1R::response_packet_data::NACK_DB_IS_EMPTY:
                hwlib::cout << "DB_IS_EMPTY" << "\n"; break;
            case (double_word)GT511C1R::response_packet_data::NACK_TURN_ERR:
                hwlib::cout << "TURN_ERR" << "\n"; break;
            case (double_word)GT511C1R::response_packet_data::NACK_BAD_FINGER:
                hwlib::cout << "BAD_FINGER" << "\n"; break;
            case (double_word)GT511C1R::response_packet_data::NACK_ENROLL_FAILED:
                hwlib::cout << "ENROLL_FAILED" << "\n"; break;
            case (double_word)GT511C1R::response_packet_data::NACK_IS_NOT_SUPPORTED:
                hwlib::cout << "IS_NOT_SUPPORTED" << "\n"; break;
            case (double_word)GT511C1R::response_packet_data::NACK_DEV_ERR:
                hwlib::cout << "DEV_ERR" << "\n"; break;
            case (double_word)GT511C1R::response_packet_data::NACK_CAPTURE_CANCELED:
                hwlib::cout << "CAPTURE_CANCELED" << "\n"; break;
            case (double_word)GT511C1R::response_packet_data::NACK_INVALID_PARAM:
                hwlib::cout << "INVALID_PARAM" << "\n"; break;
            case (double_word)GT511C1R::response_packet_data::NACK_FINGER_IS_NOT_PRESSED:
                hwlib::cout << "FINGER_IS_NOT_PRESSED" << "\n"; break;
            case (double_word)GT511C1R::response_packet_data::INVALID:
                hwlib::cout << "INVALID" << "\n"; break;
            default:
                hwlib::cout << "\f" << "NOTHING" << "\n";
        }
    }
}

/* ==========================================================================================================================
Communication Commands functions
*/ // =======================================================================================================================

/// @brief Initialise the fingerprint sensor
/// @return int parameter, may be used for debugging
int GT511C1R::initialise() {
    GT511C1R::Command_packet command_packet(0x00, ((word) command_packet_data::Open), baud_rate, tx);
    GT511C1R::Response_packet response_packet(baud_rate, rx);

    if (debug) {
        hwlib::cout << "Initialise" << "\n";
    }
    if (testing) {
        TEST_check_initialise_command_packet(command_packet.packet);
        TEST_check_initialise_response_packet(response_packet.packet);
    }
    return response_packet.get_parameter_data();
}

/// @brief Control the led
/// @param bool on, true for turning the led on and false for turning it off
/// @return int parameter, may be used for debugging
int GT511C1R::control_led(bool on) {
    double input_parameter;
    if (on) { input_parameter = 0x01; } 
    else {  input_parameter = 0x00; }
    GT511C1R::Command_packet command_packet(input_parameter, ((word) command_packet_data::CmosLed), baud_rate, tx);
    GT511C1R::Response_packet response_packet(baud_rate, rx);

    if (debug) {
        hwlib::cout << "Control led" << "\n";
    } 

    if (testing) {
        if (on) {
            TEST_check_led_on_command_packet(command_packet.packet);
            TEST_check_led_on_response_packet(response_packet.packet);
        } else {
            TEST_check_led_off_command_packet(command_packet.packet);
            TEST_check_led_off_response_packet(response_packet.packet);
        }
    }
    return response_packet.get_parameter_data();
}

/// @brief Change the internal baud rate of the fingerprint sensor
/// @param int baud_rate, baud rate to set (between 9600 - 115200)
/// @return int parameter, may be used for debugging
int GT511C1R::change_baud_rate(int input_baud_rate) {
    GT511C1R::Command_packet command_packet(input_baud_rate, ((word) command_packet_data::ChangeBaudrate), baud_rate, tx);
    GT511C1R::Response_packet response_packet(baud_rate, rx);
    baud_rate = input_baud_rate;

    if (debug) {
        hwlib::cout << "Baud rate" << "\n";
    }
    return response_packet.get_parameter_data();
}

/// @brief Get enrolled fingerprint count
/// @return int parameter, used for enrolling as the count is returned
int GT511C1R::get_enrolled_fingerprint_count() {
    GT511C1R::Command_packet command_packet(0x00, ((word) command_packet_data::GetEnrollCount), baud_rate, tx);
    GT511C1R::Response_packet response_packet(baud_rate, rx);

    if (debug) {
        hwlib::cout << "Get count" << "\n";
    } 

    return response_packet.get_parameter_data();
}

/// @brief Check status of fingerprint id
/// @param int id, value between 0 - 19
/// @return int parameter, may be used for debugging
int GT511C1R::check_enrollment_status(int id) {
    GT511C1R::Command_packet command_packet(id, ((word) command_packet_data::CheckEnrolled), baud_rate, tx);
    GT511C1R::Response_packet response_packet(baud_rate, rx);

    if (debug) {
        hwlib::cout << "Check status" << "\n";
    }

    if (response_packet.get_response_data() == (double_word) command_packet_data::Ack) {
        return response_packet.get_parameter_data();
    } else if (response_packet.get_parameter_data() == (double_word) response_packet_data::NACK_INVALID_POS 
        || response_packet.get_parameter_data() == (double_word) response_packet_data::NACK_IS_NOT_USED) {
        return -1;
    } else { return ERROR_NO_VALID_RESPONSE; }
}

/// @brief Start a fingerprint enrollment
/// @param int id, value between 0 - 19
/// @return int parameter, may be used for debugging
int GT511C1R::start_enrollment() {
    double input_parameter = get_enrolled_fingerprint_count();
    GT511C1R::Command_packet command_packet(input_parameter, ((word) command_packet_data::EnrollStart), baud_rate, tx);
    GT511C1R::Response_packet response_packet(baud_rate, rx);

    if (debug) {
        hwlib::cout << "Start enrollment" << "\n";
    }

    if (response_packet.get_response_data() == (double_word) command_packet_data::Ack) {
        return input_parameter;
    } else if (response_packet.get_parameter_data() == (double_word) response_packet_data::NACK_DB_IS_FULL 
        || response_packet.get_parameter_data() == (double_word) response_packet_data::NACK_INVALID_POS 
        || response_packet.get_parameter_data() == (double_word) response_packet_data::NACK_IS_ALREADY_USED) {
        return -1;
    } else { return ERROR_NO_VALID_RESPONSE; }
}

/// @brief Function for each of the 3 enrollments based on the parameter
/// @param int template, control over the enrollment number
/// @return int parameter, returns 1 if the enrollment was okay
int GT511C1R::enrollment(int template_number) {
    word input_command;
    if (template_number == 1) {
        input_command = ((word) command_packet_data::Enroll1);
    } else if ( template_number == 2) {
        input_command = ((word) command_packet_data::Enroll2);
    } else if ( template_number == 3) {
        input_command = ((word) command_packet_data::Enroll3);
    }
    GT511C1R::Command_packet command_packet(0x00, input_command, baud_rate, tx);
    GT511C1R::Response_packet response_packet(baud_rate, rx);

    if (debug) {
        hwlib::cout << "Enrollment" << "\n";
    }

    if (response_packet.get_response_data() == (double_word) command_packet_data::Ack) {
        return 1;
    } else if (response_packet.get_parameter_data() == (double_word) response_packet_data::NACK_ENROLL_FAILED 
        || response_packet.get_parameter_data() == (double_word) response_packet_data::NACK_BAD_FINGER) {
        return -1;
    } else { return ERROR_NO_VALID_RESPONSE; }
    return 0;
}

/// @brief Check if a finger sits on the GT511C1R
/// @return int parameter, 0 if finger is pressed, 1 if not
int GT511C1R::check_finger_pressing_status() {
    GT511C1R::Command_packet command_packet(0x00, ((word) command_packet_data::IsPressFinger), baud_rate, tx);
    GT511C1R::Response_packet response_packet(baud_rate, rx);

    if (debug) {
        hwlib::cout << "Check fingerpress" << "\n";
    }

    return response_packet.get_parameter_data();
}

/// @brief Delete one fingerprint based on a id
/// @param int id, id to delete from the GT511C1R
/// @return int parameter, 1 if succesfull, -1 if not
int GT511C1R::delete_one_fingerprint(int id) {
    GT511C1R::Command_packet command_packet(id, ((word) command_packet_data::DeleteID), baud_rate, tx);
    GT511C1R::Response_packet response_packet(baud_rate, rx);

    if (debug) {
        hwlib::cout << "Delete one fingerprint" << "\n";
    }

    if (response_packet.get_response_data() == (double_word) command_packet_data::Ack) {
        return 1;
    } else if (response_packet.get_parameter_data() == (double_word) response_packet_data::NACK_DB_IS_EMPTY) {
        return -1;
    } else { return ERROR_NO_VALID_RESPONSE; }
}

/// @brief Delete one fingerprint based on a id
/// @param int id, id to delete from the GT511C1R
/// @return int parameter, 1 if succesfull, -1 if not
int GT511C1R::delete_all_fingerprints() {
    GT511C1R::Command_packet command_packet(0x00, ((word) command_packet_data::DeleteAll), baud_rate, tx);
    GT511C1R::Response_packet response_packet(baud_rate, rx);

    if (debug) {
        hwlib::cout << "Delete all fingerprints" << "\n";
    }

    if (response_packet.get_response_data() == (double_word) command_packet_data::Ack) {
        return 1;
    } else if (response_packet.get_parameter_data() == (double_word) response_packet_data::NACK_DB_IS_EMPTY) {
        return -1;
    } else { return ERROR_NO_VALID_RESPONSE; }
}

/// @brief Verify a fingerprint based on id
/// @param int id, id to verify a fingerprint with
/// @return int parameter, 1 if succesfull, -1 if not
int GT511C1R::verification_1_1(int id) {
    GT511C1R::Command_packet command_packet(id, ((word) command_packet_data::Verify1_1), baud_rate, tx);
    GT511C1R::Response_packet response_packet(baud_rate, rx);

    if (debug) {
        hwlib::cout << "Verification 1:1" << "\n";
    }

    if (response_packet.get_response_data() == (double_word) command_packet_data::Ack) {
        return 1;
    } else if (response_packet.get_parameter_data() == (double_word) response_packet_data::NACK_INVALID_POS 
        || response_packet.get_parameter_data() == (double_word) response_packet_data::NACK_IS_NOT_USED
        || response_packet.get_parameter_data() == (double_word) response_packet_data::NACK_VERIFY_FAILED) {
        return -1;
    } else { return ERROR_NO_VALID_RESPONSE; }
}

/// @brief Verify a fingerprint on all existing fingerprints
/// @return int parameter, id of the identificated fingerprint, -1 if not
int GT511C1R::identification_1_N() {
    GT511C1R::Command_packet command_packet(0x00, ((word) command_packet_data::Identify1_N), baud_rate, tx);
    GT511C1R::Response_packet response_packet(baud_rate, rx);


    if (debug) {
        hwlib::cout << "Identification 1:N" << "\n";
    }

    if (response_packet.get_response_data() == (double_word) command_packet_data::Ack) {
        return (int) response_packet.get_parameter_data();
    } else if (response_packet.get_parameter_data() == (double_word) response_packet_data::NACK_IDENTIFY_FAILED 
        || response_packet.get_parameter_data() == (double_word) response_packet_data::NACK_DB_IS_EMPTY) {
        return -1;
    } else { return ERROR_NO_VALID_RESPONSE; }
}

/// @brief Capture a fingerprint on the GT511C1R
/// @param int quality, controls the quality taken with the GT511C1R
/// @return int parameter, 1 if succesfull, -1 if not
int GT511C1R::capture_fingerprint(const char* quality) {
    double input_parameter;
    if (strcmp(quality, "fast")) {
        input_parameter = 0x00;
    } else if (strcmp(quality, "best")) {
        input_parameter = 0x01;
    }
    GT511C1R::Command_packet command_packet(input_parameter, ((word) command_packet_data::CaptureFinger), baud_rate, tx);
    GT511C1R::Response_packet response_packet(baud_rate, rx);

    if (debug) {
        hwlib::cout << "Capture" << "\n";
    }

    if (response_packet.get_response_data() == (double_word) command_packet_data::Ack) {
        return 1;
    } else if (response_packet.get_parameter_data() == (double_word) response_packet_data::NACK_FINGER_IS_NOT_PRESSED) {
        return -1;
    } else { return ERROR_NO_VALID_RESPONSE; }
}

/// @brief Terminate/close the fingerprint sensor
/// @return int parameter, 1 if succesfull
int GT511C1R::terminate() {
    GT511C1R::Command_packet command_packet(0x00, ((word) command_packet_data::Close), baud_rate, tx);
    GT511C1R::Response_packet response_packet(baud_rate, rx);

    if (debug) {
        hwlib::cout << "Terminate" << "\n";
    }

    if (response_packet.get_response_data() == (double_word) command_packet_data::Ack) {
        return 1;
    } else { return ERROR_NO_VALID_RESPONSE; }
}

/* ==========================================================================================================================
Control Commands functions
*/ // =======================================================================================================================

/// @brief Identify a fingerprint according to the steps to take in the datasheet
int GT511C1R::identify_fingerprint() {
    auto green_led = hwlib::target::pin_out( hwlib::target::pins::d4 );
    auto red_led = hwlib::target::pin_out( hwlib::target::pins::d5 );
    int identification_id;

    red_led.set(1);
    control_led(true);
    while (check_finger_pressing_status()) {}
    capture_fingerprint("best");
    identification_id = identification_1_N();
    control_led(false);
    green_led.set(1);
    green_led.set(0); red_led.set(0);

    if (debug) {
        hwlib::cout << "Identify fingerprint" << "\n";
    }
    return identification_id;
}

/// @brief Identify a fingerprint for one try according to the steps to take in the datasheet
int GT511C1R::identify_fingerprint_try_check_finger_pressing_status_once() {
    int identification_id = -1;

    // Check if a finger is pressed once, if so capture the finger, otherwise do nothing
    control_led(true);
    if (!check_finger_pressing_status()) {
        capture_fingerprint("best");
        identification_id = identification_1_N();
    }
    control_led(false);

    if (debug) {
        hwlib::cout << "Identify fingerprint once" << "\n";
    }
    return identification_id;
}

/// @brief Register a fingerprint according to the steps to take in the datasheet
int GT511C1R::register_fingerprint() {
    auto green_led = hwlib::target::pin_out( hwlib::target::pins::d4 );
    auto red_led = hwlib::target::pin_out( hwlib::target::pins::d5 );
    int enrollment_outcome1, enrollment_outcome2, enrollment_outcome3;
    start_enrollment();

    // Enrollment 1
    red_led.set(1);
    control_led(true);
    while (check_finger_pressing_status()) {}
    capture_fingerprint("best");
    enrollment_outcome1 = enrollment(1);
    green_led.set(1);
    while (!check_finger_pressing_status()) {}
    green_led.set(0); red_led.set(0);

    hwlib::wait_ms(50);

    // Enrollment 2
    red_led.set(1);
    while (check_finger_pressing_status()) {}
    capture_fingerprint("best");
    enrollment_outcome2 = enrollment(2);
    green_led.set(1);
    while (!check_finger_pressing_status()) {}
    green_led.set(0); red_led.set(0);

    hwlib::wait_ms(50);

    // Enrollment 3
    red_led.set(1);
    while (check_finger_pressing_status()) {}
    capture_fingerprint("best");
    enrollment_outcome3 = enrollment(3);
    green_led.set(1);
    control_led(false);
    green_led.set(0); red_led.set(0);

    if (debug) {
        hwlib::cout << "Registered fingerprint" << "\n";
    }

    if (enrollment_outcome1 == 1 && enrollment_outcome2 == 1 && enrollment_outcome3) {
        return 1;
    } else {
        return -1;
    }
}