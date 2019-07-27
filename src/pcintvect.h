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
#ifndef __PCINTVECT_H
#define __PCINTVECT_H

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void attachPCInterrupt (uint8_t, void (*)(void) = NULL);
extern void detachPCInterrupt (uint8_t, bool = false);

#ifdef __cplusplus
}
#endif

#endif
#endif

// end of header