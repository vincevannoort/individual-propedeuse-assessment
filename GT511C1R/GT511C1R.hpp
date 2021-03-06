#ifndef GT511C1R_H
#define GT511C1R_H

#include <stdint.h>
#include "hwlib.hpp"
#include "fingerprintsensor-interface.hpp"

using namespace std;
typedef uint8_t byte;
typedef int16_t word;
typedef int32_t double_word;

/*
GT511C1R
*/
// @brief Class for the GT-511C1R fingeprintsensor
class GT511C1R : public fingeprintsensor_interface {
public:
    /*
    Constructor
    */
    GT511C1R();
    GT511C1R(hwlib::pin_out & tx, hwlib::pin_in & rx);
    ~GT511C1R();

protected:
    /*
    Pins used for UART communication
    */
    hwlib::pin_out & tx;
    hwlib::pin_in & rx;
    int baud_rate = 9600;


    /*
    Command packet
    */
    enum class command_packet_data : word {
        NotSet              = 0x00,
        Open                = 0x01,
        Close               = 0x02,
        UsbInternalCheck    = 0x03,
        ChangeBaudrate      = 0x04,
        SetIAPMode          = 0x05,
        CmosLed             = 0x12,
        GetEnrollCount      = 0x20,
        CheckEnrolled       = 0x21,
        EnrollStart         = 0x22,
        Enroll1             = 0x23,
        Enroll2             = 0x24,
        Enroll3             = 0x25,
        IsPressFinger       = 0x26,
        DeleteID            = 0x40,
        DeleteAll           = 0x41,
        Verify1_1           = 0x50,
        Identify1_N         = 0x51,
        VerifyTemplate1_1   = 0x52,
        IdentifyTemplate1_N = 0x53,
        CaptureFinger       = 0x60,
        MakeTemplate        = 0x61,
        GetImage            = 0x62,
        GetRawImage         = 0x63,
        GetTemplate         = 0x70,
        SetTemplate         = 0x71,
        GetDatabaseStart    = 0x72,
        GetDatabaseEnd      = 0x73,
        UpgradeFirmware     = 0x80,
        UpgradeISOCDImage   = 0x81,
        Ack                 = 0x30,
        Nack                = 0x31
    };

    /*
    Response packet
    */
    enum class response_packet_data : double_word {
        NO_ERROR                   = 0x0000,
        NACK_TIMEOUT               = 0x1001,
        NACK_INVALID_BAUDRATE      = 0x1002,
        NACK_INVALID_POS           = 0x1003,
        NACK_IS_NOT_USED           = 0x1004,
        NACK_IS_ALREADY_USED       = 0x1005,
        NACK_COMM_ERR              = 0x1006,
        NACK_VERIFY_FAILED         = 0x1007,
        NACK_IDENTIFY_FAILED       = 0x1008,
        NACK_DB_IS_FULL            = 0x1009,
        NACK_DB_IS_EMPTY           = 0x100A,
        NACK_TURN_ERR              = 0x100B,
        NACK_BAD_FINGER            = 0x100C,
        NACK_ENROLL_FAILED         = 0x100D,
        NACK_IS_NOT_SUPPORTED      = 0x100E,
        NACK_DEV_ERR               = 0x100F,
        NACK_CAPTURE_CANCELED      = 0x1010,
        NACK_INVALID_PARAM         = 0x1011,
        NACK_FINGER_IS_NOT_PRESSED = 0x1012,
        INVALID                    = 0XFFFF
    };

public:
    /*
    Classes
    */

    // @brief Command packet for sending commands, which consists of 12 bytes
    class Command_packet {
    public:
        byte packet[12];

    protected:
        const byte start_code1 = 0x55;
        const byte start_code2 = 0xAA;
        const word device_id = 0x0001;
        double_word parameter;
        word command;
        word checksum;
        int baud_rate;
        hwlib::pin_out & tx_pin;

    public:
        Command_packet(double_word input_parameter, word input_command, int input_baud_rate, hwlib::pin_out & tx);
        void set_parameter(double_word input_parameter);
        void set_command(word input_command);
        void set_checksum(word input_checksum);
        word calculate_checksum();
        void send();
    };

    // @brief Response packet for recieving commands, 12 bytes being recieved
    class Response_packet {
    public:
        byte packet[12];

    protected:
        const byte response_code1 = 0x55;
        const byte response_code2 = 0xAA;
        const word device_id = 0x00001;
        double_word parameter;
        word response;
        word checksum;
        int baud_rate;
        hwlib::pin_in & rx_pin;
        
    public:
        Response_packet(int input_baud_rate, hwlib::pin_in & rx);
        double_word get_parameter_data();
        word get_response_data();
        void recieve();
    };

    // @brief Data packet for sending and recieving
    class Data_packet {};

    /*
    Communication functions
    */
    int initialise();
    int control_led(bool on);
    int change_baud_rate(int baud_rate);
    int get_enrolled_fingerprint_count();
    int check_enrollment_status(int id);
    int start_enrollment();
    int enrollment(int template_number);
    int check_finger_pressing_status();
    int delete_one_fingerprint(int id);
    int delete_all_fingerprints();
    int verification_1_1(int id);
    int identification_1_N();
    int capture_fingerprint(const char* input);
    int terminate();

    /*
    Control functions
    */
    int identify_fingerprint();
    int identify_fingerprint_try_check_finger_pressing_status_once();
    int register_fingerprint();
};

#endif