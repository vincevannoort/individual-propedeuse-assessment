#include "realtimeclock.hpp"

#include <stdint.h>
#include "hwlib.hpp"

#define LSBFIRST 1

namespace {

enum Register {
  kSecondReg       = 0,
  kMinuteReg       = 1,
  kHourReg         = 2,
  kDateReg         = 3,
  kMonthReg        = 4,
  kDayReg          = 5,
  kYearReg         = 6,
  kWriteProtectReg = 7,

  // The RAM register space follows the clock register space.
  kRamAddress0     = 32
};

enum Command {
  kClockBurstRead  = 0xBF,
  kClockBurstWrite = 0xBE,
  kRamBurstRead    = 0xFF,
  kRamBurstWrite   = 0xFE
};

// Establishes and terminates a three-wire SPI session.
// For the interface, in addition to power, only three wires are required: 
// CE (RST), 
// I / O (line Data), 
// and SCLK (serial clock).
class SPISession {
 public:
  // SPISession(const int ce_pin, const int io_pin, const int sclk_pin)
  SPISession(hwlib::pin_out & ce_pin, hwlib::pin_in_out & io_pin, hwlib::pin_out & sclk_pin): 
  ce_pin_(ce_pin), io_pin_(io_pin), sclk_pin_(sclk_pin) {
    sclk_pin_.set(0); // digitalWrite(sclk_pin_, LOW);
    ce_pin_.set(1); // digitalWrite(ce_pin_, HIGH);
    hwlib::wait_us( 4 ); // delayMicroseconds(4);  // tCC
  }
  ~SPISession() {
    ce_pin_.set(0); // digitalWrite(ce_pin_, LOW);
    hwlib::wait_us( 4 ); // delayMicroseconds(4);  // tCWH
  }

 private:
  hwlib::pin_out & ce_pin_; // const int ce_pin_;
  hwlib::pin_in_out & io_pin_; // const int io_pin_;
  hwlib::pin_out & sclk_pin_; // const int sclk_pin_;
};

// Returns the decoded decimal value from a binary-coded decimal (BCD) byte.
// Assumes 'bcd' is coded with 4-bits per digit, with the tens place digit in
// the upper 4 MSBs.
uint8_t bcdToDec(const uint8_t bcd) {
  return (10 * ((bcd & 0xF0) >> 4) + (bcd & 0x0F));
}

// Returns the binary-coded decimal of 'dec'. Inverse of bcdToDec.
uint8_t decToBcd(const uint8_t dec) {
  const uint8_t tens = dec / 10;
  const uint8_t ones = dec % 10;
  return (tens << 4) | ones;
}

// Returns the hour in 24-hour format from the hour register value.
uint8_t hourFromRegisterValue(const uint8_t value) {
  uint8_t adj;
  if (value & 128)  // 12-hour mode
    adj = 12 * ((value & 32) >> 5);
  else           // 24-hour mode
    adj = 10 * ((value & (32 + 16)) >> 4);
  return (value & 15) + adj;
}

}  // namespace

Time::Time(const uint16_t yr, const uint8_t mon, const uint8_t date,
           const uint8_t hr, const uint8_t min, const uint8_t sec,
           const Day day) {
  this->yr   = yr;
  this->mon  = mon;
  this->date = date;
  this->hr   = hr;
  this->min  = min;
  this->sec  = sec;
  this->day  = day;
}

DS1302::DS1302(hwlib::pin_out & ce_pin, hwlib::pin_in_out & io_pin, hwlib::pin_out & sclk_pin):
ce_pin_(ce_pin), io_pin_(io_pin), sclk_pin_(sclk_pin) {

  // pinMode(ce_pin, OUTPUT);
  // pinMode(sclk_pin, OUTPUT);
}

// void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val)
// void shiftOut(hwlib::pin_out & dataPin, hwlib::pin_out & clockPin, uint8_t bitOrder, uint8_t val)
// {
//   uint8_t i;
//   for (i = 0; i < 8; i++) {
//     if (bitOrder == LSBFIRST) {
//       dataPin.set(!!(val & (1 << i)));  // digitalWrite(dataPin, !!(val & (1 << i)));
//     } else {
//       dataPin.set(!!(val & (1 << (7 - i))));  // digitalWrite(dataPin, !!(val & (1 << (7 - i))));
//     }

//     clockPin.set(1); // digitalWrite(clockPin, HIGH);
//     clockPin.set(0); // digitalWrite(clockPin, LOW);            
//   }
// }

void DS1302::writeOut(const uint8_t value) {
  // pinMode(io_pin_, OUTPUT);
  // This assumes that shiftOut is 'slow' enough for the DS1302 to read the
  // bits. The datasheet specifies that SCLK must be in its high and low states
  // for at least 0.25us at 5V or 1us at 2V. Experimentally, a 16MHz Arduino
  // seems to spend ~4us high and ~12us low when shifting.
  // spi_bus_.write_and_read(); // shiftOut(io_pin_, sclk_pin_, LSBFIRST, value);
  // shiftOut(io_pin_OUT, sclk_pin_, LSBFIRST, value);

  uint8_t i;

  for (i = 0; i < 8; i++)  {
      io_pin_.set(!!(value & (1 << i))); // digitalWrite(dataPin, !!(val & (1 << i)));
      sclk_pin_.set(1); // digitalWrite(clockPin, HIGH);
      sclk_pin_.set(0); // digitalWrite(clockPin, LOW);        
  }
}

uint8_t DS1302::readIn() {
  uint8_t input_value = 0;
  uint8_t bit = 0;
  // pinMode(io_pin_, INPUT);

  // Bits from the DS1302 are output on the falling edge of the clock
  // cycle. This method is called after a previous call to writeOut() or
  // readIn(), which will have already set the clock low.
  for (int i = 0; i < 8; ++i) {
    bit = io_pin_.get(); // bit = digitalRead(io_pin_);
    input_value |= (bit << i);  // Bits are read LSB first.

    // See the note in writeOut() about timing. digitalWrite() is slow enough to
    // not require extra delays for tCH and tCL.
    sclk_pin_.set(1); // digitalWrite(sclk_pin_, HIGH);
    sclk_pin_.set(0); // digitalWrite(sclk_pin_, LOW);
  }

  return input_value;
}

uint8_t DS1302::readRegister(const uint8_t reg) {
  const SPISession s(ce_pin_, io_pin_, sclk_pin_);

  const uint8_t cmd_byte = (0x81 | (reg << 1));
  writeOut(cmd_byte);
  return readIn();
}

void DS1302::writeRegister(const uint8_t reg, const uint8_t value) {
  const SPISession s(ce_pin_, io_pin_, sclk_pin_);

  const uint8_t cmd_byte = (0x80 | (reg << 1));
  writeOut(cmd_byte);
  writeOut(value);
}

void DS1302::writeProtect(const bool enable) {
  writeRegister(kWriteProtectReg, (enable << 7));
}

void DS1302::halt(const bool enable) {
  uint8_t sec = readRegister(kSecondReg);
  sec &= ~(1 << 7);
  sec |= (enable << 7);
  writeRegister(kSecondReg, sec);
}

Time DS1302::time() {
  const SPISession s(ce_pin_, io_pin_, sclk_pin_);

  Time t(2099, 1, 1, 0, 0, 0, Time::kSunday);
  writeOut(kClockBurstRead);
  t.sec = bcdToDec(readIn() & 0x7F);
  t.min = bcdToDec(readIn());
  t.hr = hourFromRegisterValue(readIn());
  t.date = bcdToDec(readIn());
  t.mon = bcdToDec(readIn());
  t.day = static_cast<Time::Day>(bcdToDec(readIn()));
  t.yr = 2000 + bcdToDec(readIn());
  return t;
}

void DS1302::time(const Time t) {
  // We want to maintain the Clock Halt flag if it is set.
  const uint8_t ch_value = readRegister(kSecondReg) & 0x80;

  const SPISession s(ce_pin_, io_pin_, sclk_pin_);

  writeOut(kClockBurstWrite);
  writeOut(ch_value | decToBcd(t.sec));
  writeOut(decToBcd(t.min));
  writeOut(decToBcd(t.hr));
  writeOut(decToBcd(t.date));
  writeOut(decToBcd(t.mon));
  writeOut(decToBcd(static_cast<uint8_t>(t.day)));
  writeOut(decToBcd(t.yr - 2000));
  // All clock registers *and* the WP register have to be written for the time
  // to be set.
  writeOut(0);  // Write protection register.
}

void DS1302::writeRam(const uint8_t address, const uint8_t value) {
  if (address >= kRamSize) {
    return;
  }

  writeRegister(kRamAddress0 + address, value);
}

uint8_t DS1302::readRam(const uint8_t address) {
  if (address >= kRamSize) {
    return 0;
  }

  return readRegister(kRamAddress0 + address);
}

void DS1302::writeRamBulk(const uint8_t* const data, int len) {
  if (len <= 0) {
    return;
  }
  if (len > kRamSize) {
    len = kRamSize;
  }

  const SPISession s(ce_pin_, io_pin_, sclk_pin_);

  writeOut(kRamBurstWrite);
  for (int i = 0; i < len; ++i) {
    writeOut(data[i]);
  }
}

void DS1302::readRamBulk(uint8_t* const data, int len) {
  if (len <= 0) {
    return;
  }
  if (len > kRamSize) {
    len = kRamSize;
  }

  const SPISession s(ce_pin_, io_pin_, sclk_pin_);

  writeOut(kRamBurstRead);
  for (int i = 0; i < len; ++i) {
    data[i] = readIn();
  }
}