#include "fingerprintsensor.hpp"

#include <cstring>
#include "hwlib.hpp"

/*
Constructor & Deconstructor
*/

// @brief Constructor for Fingerprintsensor
// @param hwlib::pin_out & tx, pin used for sending data/commands
// @param hwlib::pin_in & rx, pin used for recieving data/commands
Fingerprintsensor::Fingerprintsensor(hwlib::pin_out & tx, hwlib::pin_in & rx, hwlib::pin_out & power): tx(tx), rx(rx), power(power) { hwlib::wait_ms(200); }

// @brief Deconstructor for Fingerprintsensor, terminates when program ends
Fingerprintsensor::~Fingerprintsensor(){ /* terminate(); */ }

/*
Constructors
*/

// @brief Constructor for Command packet
Fingerprintsensor::Command_packet::Command_packet(double_word input_parameter, word input_command) {
    set_parameter(input_parameter);
    set_command(input_command);
    set_checksum(calculate_checksum());
    send(9600); // @TEMPORARY HARDCODED
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
    auto rx_pin = hwlib::target::pin_in( hwlib::target::pins::d19 );
    auto led = hwlib::target::pin_out( hwlib::target::pins::d3 );
    byte packet[12];
    byte r_packet[12];
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

    led.set(1);
    for (const byte & packet_byte : packet) {
        hwlib::uart_putc_bit_banged_pin_custom_baudrate(packet_byte, tx_pin, input_baud_rate);
    }
    led.set(0);

    /*
    Debugging RESPONSE PACKET
    */
    // packet_byte = hwlib::uart_getc_bit_banged_pin(rx_pin);
    for (byte & packet_byte : r_packet) {
        char c = 0;        
        const auto bit_cel = ( ( 1000L * 1000L ) / 9600 );

        led.set(1);
        while( rx_pin.get() ){}
        led.set(0);
        hwlib::cout << "recieved start bit. \n";

        led.set(1);
        auto t = hwlib::now_us();
        t += bit_cel + ( bit_cel / 2 );
        while( hwlib::now_us() < t ){};
        led.set(0);
        hwlib::cout << "Waited until halfway the first data bit. \n";

        led.set(1);
        for( uint_fast8_t i = 0; i < 8; ++i ) {
            c = c >> 1;            
            if( rx_pin.get() ){ c = c | 0x80; }
            t+= bit_cel;
            while( hwlib::now_us() < t ){};
        }   
        led.set(0);
        hwlib::cout << "Recieved 8 data bits. \n";

        packet_byte = c;
        hwlib::cout << "\n";
    }
    led.set(0);
}

// @brief Constructor for Response packet
Fingerprintsensor::Response_packet::Response_packet() {
    recieve(9600); // @TEMPORARY HARDCODED
}

// @brief Recieve command which polls for a response, then acquires the needed data
int Fingerprintsensor::Response_packet::recieve(int input_baud_rate) {
    auto rx_pin = hwlib::target::pin_in( hwlib::target::pins::d19 );
    auto led = hwlib::target::pin_out( hwlib::target::pins::d3 );
    byte packet[12];

    for (byte & packet_byte : packet) {
        packet_byte = hwlib::uart_getc_bit_banged_pin(rx_pin);
    }

    return 0;
}

/*
Communication Commands functions
*/

// @brief Initialise the fingerprint sensor
int Fingerprintsensor::initialise() {
    Fingerprintsensor::Command_packet command_packet(0x00, ((word) command_packet_data::Open));
    // Fingerprintsensor::Response_packet response_packet;

    if (debug) {
        hwlib::cout << "Initialise" << "\n";
        display << "\f" << "Initialise" << hwlib::flush;
    } 
    return 0;
}

// @brief Control the led
// @param bool on, true for turning the led on and false for turning it off
int Fingerprintsensor::control_led(bool on) {
    double input_parameter;
    if (on) { input_parameter = 0x01; } 
    else {  input_parameter = 0x00; }
    Fingerprintsensor::Command_packet command_packet(input_parameter, ((word) command_packet_data::CmosLed));

    if (debug) {
        hwlib::cout << "Control led" << "\n";
        display << "\f" << "Control led" << hwlib::flush;
    } 
    return 0;
}

// @brief Change the internal baud rate of the fingerprint sensor
// @param int baud_rate, baud rate to set (between 9600 - 115200)
int Fingerprintsensor::change_baud_rate(int input_baud_rate) {
    Fingerprintsensor::Command_packet command_packet(input_baud_rate, ((word) command_packet_data::ChangeBaudrate));
    baud_rate = input_baud_rate;

    if (debug) {
        hwlib::cout << "Baud rate: " << baud_rate << " checksum = " << command_packet.calculate_checksum();
        display << "\f" << "Baud rate" << hwlib::flush;
    }
    return 0;
}

// @brief Get enrolled fingerprint count
int Fingerprintsensor::get_enrolled_fingerprint_count() {
    Fingerprintsensor::Command_packet command_packet(0x00, ((word) command_packet_data::GetEnrollCount));

    if (debug) {
        hwlib::cout << "Get count" << "\n";
        display << "\f" << "Get count" << hwlib::flush;
    } 
    return 0;
}

// @brief Check status of fingerprint id
// @param int id, value between 0 - 19
int Fingerprintsensor::check_enrollment_status(int id) {
    Fingerprintsensor::Command_packet command_packet(id, ((word) command_packet_data::CheckEnrolled));

    if (debug) {
        hwlib::cout << "Check status" << "\n";
        display << "\f" << "Check status" << hwlib::flush;
    }
    return 0;
}

// @brief Start a fingerprint enrollment
// @param int id, value between 0 - 19
int Fingerprintsensor::start_enrollment(int id) {
    double input_parameter = get_enrolled_fingerprint_count();
    Fingerprintsensor::Command_packet command_packet(input_parameter, ((word) command_packet_data::EnrollStart));

    if (debug) {
        hwlib::cout << "Start enrollment" << "\n";
        display << "\f" << "Start enrollment" << hwlib::flush;
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
    Fingerprintsensor::Command_packet command_packet(0x00, input_command);

    if (debug) {
        hwlib::cout << "Enrollment" << "\n";
        display << "\f" << "Enrollment" << hwlib::flush;
    }
    return 0;
}

// @brief Check if a finger sits on the fingerprintsensor
int Fingerprintsensor::check_finger_pressing_status() {
    Fingerprintsensor::Command_packet command_packet(0x00, ((word) command_packet_data::IsPressFinger));

    if (debug) {
        hwlib::cout << "Check fingerpress" << "\n";
        display << "\f" << "Check fingerpress" << hwlib::flush;
    }
    return 0;
}

// @brief Delete one fingerprint based on a id
// @param int id, id to delete from the fingerprintsensor
int Fingerprintsensor::delete_one_fingerprint(int id) {
    Fingerprintsensor::Command_packet command_packet(id, ((word) command_packet_data::DeleteID));

    if (debug) {
        hwlib::cout << "Delete one fingerprint" << "\n";
        display << "\f" << "Delete one fingerprint" << hwlib::flush;
    }
    return 0;
}

// @brief Delete one fingerprint based on a id
// @param int id, id to delete from the fingerprintsensor
int Fingerprintsensor::delete_all_fingerprints() {
    Fingerprintsensor::Command_packet command_packet(0x00, ((word) command_packet_data::DeleteAll));

    if (debug) {
        hwlib::cout << "Delete all fingerprints" << "\n";
        display << "\f" << "Delete all fingerprints" << hwlib::flush;
    }
    return 0;
}

// @brief Verify a fingerprint based on id
// @param int id, id to verify a fingerprint with
int Fingerprintsensor::verification_1_1(int id) {
    Fingerprintsensor::Command_packet command_packet(id, ((word) command_packet_data::Verify1_1));

    if (debug) {
        hwlib::cout << "Verification 1:1" << "\n";
        display << "\f" << "Verification 1:1" << hwlib::flush;
    }
    return 0;
}

// @brief Verify a fingerprint on all existing fingerprints
int Fingerprintsensor::identification_1_N() {
    Fingerprintsensor::Command_packet command_packet(0x00, ((word) command_packet_data::Identify1_N));

    if (debug) {
        hwlib::cout << "Verification 1:N" << "\n";
        display << "\f" << "Verification 1:N" << hwlib::flush;
    }
    return 0;
}

// @brief Capture a fingerprint on the fingerprintsensor
// @param int quality, controls the quality taken with the fingerprintsensor
int Fingerprintsensor::capture_fingerprint(char quality[]) {
    double input_parameter;
    if (strcmp(quality, "fast")) {
        input_parameter = 0x00;
    } else if (strcmp(quality, "best")) {
        input_parameter = 0x01;
    }
    Fingerprintsensor::Command_packet command_packet(input_parameter, ((word) command_packet_data::CaptureFinger));

    if (debug) {
        hwlib::cout << "Capture" << "\n";
        display << "\f" << "Capture" << hwlib::flush;
    }
    return 0;
}

// @brief Terminate/close the fingerprint sensor
int Fingerprintsensor::terminate() {
    Fingerprintsensor::Command_packet command_packet(0x00, ((word) command_packet_data::Close));

    if (debug) {
        hwlib::cout << "Terminate" << "\n";
        display << "\f" << "Terminate" << hwlib::flush;
    }
    return 0;
}