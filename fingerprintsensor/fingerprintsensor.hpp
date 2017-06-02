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
        int send();
        int recieve();
        int execute();
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