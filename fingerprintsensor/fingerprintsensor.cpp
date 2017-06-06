#include "fingerprintsensor.hpp"

/*
Constructors
*/
Fingerprintsensor::Fingerprintsensor() {

    /*
    Command packet
    */
    command_packet["NotSet"]              = 0x00;
    command_packet["Open"]                = 0x01;
    command_packet["Close"]               = 0x02;
    command_packet["UsbInternalCheck"]    = 0x03;
    command_packet["ChangeBaudrate"]      = 0x04;
    command_packet["SetIAPMode"]          = 0x05;
    command_packet["CmosLed"]             = 0x12;
    command_packet["GetEnrollCount"]      = 0x20;
    command_packet["CheckEnrolled"]       = 0x21;
    command_packet["EnrollStart"]         = 0x22;
    command_packet["Enroll1"]             = 0x23;
    command_packet["Enroll2"]             = 0x24;
    command_packet["Enroll3"]             = 0x25;
    command_packet["IsPressFinger"]       = 0x26;
    command_packet["DeleteID"]            = 0x40;
    command_packet["DeleteAll"]           = 0x41;
    command_packet["Verify1_1"]           = 0x50;
    command_packet["Identify1_N"]         = 0x51;
    command_packet["VerifyTemplate1_1"]   = 0x52;
    command_packet["IdentifyTemplate1_N"] = 0x53;
    command_packet["CaptureFinger"]       = 0x60;
    command_packet["MakeTemplate"]        = 0x61;
    command_packet["GetImage"]            = 0x62;
    command_packet["GetRawImage"]         = 0x63;
    command_packet["GetTemplate"]         = 0x70;
    command_packet["SetTemplate"]         = 0x71;
    command_packet["GetDatabaseStart"]    = 0x72;
    command_packet["GetDatabaseEnd"]      = 0x73;
    command_packet["UpgradeFirmware"]     = 0x80;
    command_packet["UpgradeISOCDImage"]   = 0x81;
    command_packet["Ack"]                 = 0x30;
    command_packet["Nack"]                = 0x31;

    /*
    Response packet
    */
    response_packet["NO_ERROR"]                   = 0x0000;
    response_packet["NACK_TIMEOUT"]               = 0x1001;
    response_packet["NACK_INVALID_BAUDRATE"]      = 0x1002;
    response_packet["NACK_INVALID_POS"]           = 0x1003;
    response_packet["NACK_IS_NOT_USED"]           = 0x1004;
    response_packet["NACK_IS_ALREADY_USED"]       = 0x1005;
    response_packet["NACK_COMM_ERR"]              = 0x1006;
    response_packet["NACK_VERIFY_FAILED"]         = 0x1007;
    response_packet["NACK_IDENTIFY_FAILED"]       = 0x1008;
    response_packet["NACK_DB_IS_FULL"]            = 0x1009;
    response_packet["NACK_DB_IS_EMPTY"]           = 0x100A;
    response_packet["NACK_TURN_ERR"]              = 0x100B;
    response_packet["NACK_BAD_FINGER"]            = 0x100C;
    response_packet["NACK_ENROLL_FAILED"]         = 0x100D;
    response_packet["NACK_IS_NOT_SUPPORTED"]      = 0x100E;
    response_packet["NACK_DEV_ERR"]               = 0x100F;
    response_packet["NACK_CAPTURE_CANCELED"]      = 0x1010;
    response_packet["NACK_INVALID_PARAM"]         = 0x1011;
    response_packet["NACK_FINGER_IS_NOT_PRESSED"] = 0x1012;
    response_packet["INVALID"]                    = 0XFFFF;
    
}

/*
Communication Commands functions
*/
int Fingerprintsensor::initialise() {
    return 0;
}