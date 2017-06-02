#ifndef FINGERPRINTSENSOR_H
#define FINGERPRINTSENSOR_H

#include <iostream>
#include <map>
using namespace std;

/*
Fingerprintsensor
*/
class Fingerprintsensor {

/*
Packet variables
*/
protected:
    map< string, int > command_packet;
    map< string, int > response_packet;
    map< string, int > data_packet;

/*
Constructors
*/
public:
    Fingerprintsensor();

/*
Communication functions
*/
namespace Communication {
    namespace Commands {
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
    }
    namespace Response {
    }
    namespace Data {
        int send();
        int recieve();
    }
}

};

#endif