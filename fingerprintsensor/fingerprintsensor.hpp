#ifndef FINGERPRINTSENSOR_H
#define FINGERPRINTSENSOR_H

#include <iostream>
#include <map>
using namespace std;

/*
Fingerprintsensor
*/
class Fingerprintsensor {
protected:
    map< string, int > command_packet;
    map< string, int > response_packet;
    map< string, int > data_packet;
public:
    Fingerprintsensor();
};

#endif