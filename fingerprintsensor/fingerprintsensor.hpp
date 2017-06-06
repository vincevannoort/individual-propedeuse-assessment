#ifndef FINGERPRINTSENSOR_H
#define FINGERPRINTSENSOR_H

#include <iostream>
#include <map>
using namespace std;
typedef uint8_t byte;
typedef int16_t word;
typedef int32_t double_word;

/*
Fingerprintsensor
*/
class Fingerprintsensor {

protected:
    /*
    Packet variables
    */
    map< string, int > command_packet;
    map< string, int > response_packet;
    map< string, int > data_packet;

public:
    /*
    Constructor
    */
    Fingerprintsensor();

    /*
    Functions
    */
    int initialise();
    int send();
    int recieve();
    int execute();
    int control_led();
    int change_baud_rate();
    int get_enrolled_fingerprint_count();
    int check_enrollment_status();
    int start_enrollment();
    int enrollment();
    int check_finger_pressing_status();
    int delete_one_fingerprint();
    int delete_all_fingerprints();
    int verification_1_1();
    int indentification_1_N();
    int capture_fingerprint();
    int make_template();
    int set_security_level();
    int get_security_level();
    int terminate();

    /*
    Communication functions
    */
    class Command_packet {
        const byte start_code1 = 0x55;
        const byte start_code2 = 0xAA;
        const word device_id = 0x0001;
        double_word parameter;
        word command;
        word checksum;
        Command_packet();
    };
    class Response_packet {};
    class Data_packet {};
};

#endif