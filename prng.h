/* prng.h -- MSP430 Pseudorandom Noise Generator
 * by yasunoxx <yasunoxx gmail com>
 */

#include <stdint.h>

extern void prng1bit( void );
extern uint8_t prng( void );
extern uint8_t md_prng( uint8_t a, uint8_t b, uint8_t c );
extern void init_prng( void );
