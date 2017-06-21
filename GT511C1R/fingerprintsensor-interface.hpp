#ifndef FINGERPRINTSENSORINTERFACE_H
#define FINGERPRINTSENSORINTERFACE_H

#include <stdint.h>
#include "hwlib.hpp"

using namespace std;
typedef uint8_t byte;
typedef int16_t word;
typedef int32_t double_word;

/*
GT511C1R
*/
// @brief Class for the fingeprintsensor interface
class fingeprintsensor_interface {
public:
    /*
    Control functions
    */
    virtual int identify_fingerprint() = 0;
    virtual int identify_fingerprint_try_check_finger_pressing_status_once() = 0;
    virtual int register_fingerprint() = 0;
};

#endif