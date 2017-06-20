/* prng.h -- MSP430 Pseudorandom Noise Generator
 * by yasunoxx <yasunoxx gmail com>
 */

#ifndef u_char
#define u_char unsigned char
#endif

#ifndef u_short
#define u_short unsigned short
#endif

#ifndef u_int
#define u_int unsigned long
#endif

extern void prng6bit( void );
extern void prng1bit( void );
extern u_char prng( void );
extern u_char md_prng( u_char a, u_char b, u_char c );
extern void init_prng( void );
