/***************
 *
 * SoftUART - Multi-instance software serial library for Arduino/Wiring
 *
 * target architectures: Atmel AVR (ATmega 328P, 1284P and other)
 *
 * release site: https://github.com/askn37/SoftUART
 * maintainer: askn https://twitter.com/askn37
 *
 */

#ifdef SoftwareSerial_h
#error Collision include SoftwareSerial.h <=> SoftUART.h
#else
#ifndef __SOFTUART_H
#define __SOFTUART_H

#include <inttypes.h>
#include <Stream.h>
#include "pcintvect.h"

// Microcontrollers that only make use of INTs
#if defined(__AVR_ATmega8__)   || defined(__AVR_ATmega16__)   || \
    defined(__AVR_ATmega32__)  || defined(__AVR_ATmega64__)   || \
    defined(__AVR_ATmega128__) || defined(__AVR_ATmega8515__) || \
    defined(__AVR_ATmega8535__)
#define INT_ONLY
// Microcontrollers that make use of INTs and PCINTs
#elif defined(__AVR_ATmega162__) || defined(__AVR_ATmega640__)  || \
    defined(__AVR_ATmega1280__)  || defined(__AVR_ATmega1281__) || \
    defined(__AVR_ATmega2560__)  || defined(__AVR_ATmega2561__)
#define INT_AND_PCINT
// Microcontrollers that only make use of PCINTs
#else
    #define PCINT_ONLY
#endif

/******************************************************************************
* Definitions
******************************************************************************/

#ifndef _SS_MAX_RX_BUFF
#define _SS_MAX_RX_BUFF 64 // RX buffer size
#endif

#ifndef GCC_VERSION
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

class SoftUART : public Stream {
private:
    // per object data
    int8_t _receivePin;
    uint8_t _receiveBitMask;
    volatile uint8_t *_receivePortRegister;
    uint8_t _transmitBitMask;
    volatile uint8_t *_transmitPortRegister;
    volatile uint8_t *_pcint_maskreg;
    uint8_t _pcint_maskvalue;

    // Expressed as 4-cycle delays (must never be 0!)
    uint16_t _rx_delay_centering;
    uint16_t _rx_delay_intrabit;
    uint16_t _rx_delay_stopbit;
    uint16_t _tx_delay;

    uint16_t _buffer_overflow:1;
    uint16_t _inverse_logic:1;

    // static data
    static uint8_t _receive_buffer[_SS_MAX_RX_BUFF];
    static volatile uint8_t _receive_buffer_tail;
    static volatile uint8_t _receive_buffer_head;
    static SoftUART *active_object;

    // private methods
    inline void recv () __attribute__((__always_inline__));
    uint8_t rx_pin_read ();
    void setTX (int8_t transmitPin);
    void setRX (int8_t receivePin);
    inline void setRxIntMsk (bool enable) __attribute__((__always_inline__));

    // Return num - sub, or 1 if the result would be < 1
    static uint16_t subtract_cap (uint16_t num, uint16_t sub);

    // private static method for timing
    static inline void tunedDelay (uint16_t delay);

public:
    // public methods
    SoftUART (int8_t receivePin, int8_t transmitPin, bool inverse_logic = false);
    ~SoftUART ();
    void begin (long speed);
    bool listen ();
    void end ();
    bool isListening () { return this == active_object; }
    bool stopListening ();
    bool overflow () {
        bool ret = _buffer_overflow;
        if (ret) _buffer_overflow = false;
        return ret;
    }
    int peek ();

    virtual size_t write (uint8_t byte);
    virtual int read ();
    virtual int available ();
    virtual void flush ();
    operator bool () { return true; }

    using Print::write;

    // public only for easy access by interrupt handlers
    static inline void handle_interrupt () __attribute__((__always_inline__));
};

// Arduino 0012 workaround
#undef int
#undef char
#undef long
#undef byte
#undef float
#undef abs
#undef round

#endif
#endif
// end of header
