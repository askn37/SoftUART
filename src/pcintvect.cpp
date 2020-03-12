/***************
 *
 * pcintvect - PCINTn_vect helper utilities
 *
 * target architectures: Atmel AVR (ATmega 328P, 1284P and other)
 *
 * release site: https://github.com/askn37/Futilities
 * maintainer: askn https://twitter.com/askn37
 *
 */

#ifndef SoftwareSerial_h
#if defined(digitalPinToPCICRbit)
#include <Arduino.h>
#include "pcintvect.h"

volatile void __empty (void) {}
volatile void (*__pcint_vect[4])(void) = {__empty, __empty, __empty, __empty};

#if defined(PCINT0_vect)
// ISR(PCINT0_vect, __attribute__ ((weak))) { __pcint_vect[0](); }
ISR(PCINT0_vect) { __pcint_vect[0](); }
#endif
#if defined(PCINT1_vect)
// ISR(PCINT1_vect, __attribute__ ((weak))) { __pcint_vect[1](); }
ISR(PCINT1_vect) { __pcint_vect[1](); }
#endif
#if defined(PCINT2_vect)
// ISR(PCINT2_vect, __attribute__ ((weak))) { __pcint_vect[2](); }
ISR(PCINT2_vect) { __pcint_vect[2](); }
#endif
#if defined(PCINT3_vect)
// ISR(PCINT3_vect, __attribute__ ((weak))) { __pcint_vect[3](); }
ISR(PCINT3_vect) { __pcint_vect[3](); }
#endif

void attachPCInterrupt (uint8_t interruptPin, void (*userFunc)(void)) {
    int _pcint = digitalPinToPCICRbit(interruptPin);
    __pcint_vect[_pcint & 3] = userFunc != NULL
		? (volatile void (*)(void)) userFunc
		: (volatile void (*)(void)) __empty;
    *digitalPinToPCMSK(interruptPin) |= _BV(digitalPinToPCMSKbit(interruptPin));
    PCICR |= _BV(_pcint);
}

void detachPCInterrupt (uint8_t interruptPin, bool removeAll) {
    int _pcint = digitalPinToPCICRbit(interruptPin);
	if (removeAll) {
		__pcint_vect[_pcint & 3] = (volatile void (*)(void)) __empty;
		*digitalPinToPCMSK(interruptPin) = 0;
		PCICR &= ~3;
	}
	else {
		*digitalPinToPCMSK(interruptPin) &= ~_BV(digitalPinToPCMSKbit(interruptPin));
		PCICR &= ~_BV(_pcint);
	}
}
#endif
#endif

// end of code
