/* prng.c -- MSP430 Pseudorandom Number Generator
 * by yasunoxx <yasunoxx gmail com>
*/

/*
  Original source (C)2017 by EDN Japan
  http://ednjapan.com/edn/articles/1704/06/news026.html

; *********************************************************************************
;  LISTING 1 - 12C508 ASSEMBLY CODE FOR PSEUDORANDOM-NOISE GENERATOR
; "Single IC Forms pseudorandom-noise source," EDN, March 21, 2002, pg 98
; *********************************************************************************
;
;    Pseudorandom Noise Generator
;    12/12/01 Stephen J. Ploss
;

    LIST p=12C508A

#include "P12C508A.INC"

    __CONFIG _CP_OFF & _WDT_OFF & _IntRC_OSC & _MCLRE_OFF

    cblock 0x07

    Lobyte
    Midbyte
    Hibyte

    endc

start
    movlw 0xC0

    option
    movlw 0xFE
    tris GPIO

    ; Start the PN Generator
main
    movlw 0xFF
    movwf Lobyte
    movwf Midbyte
    movwf Hibyte

loop
    rlf Lobyte, F
    rlf Midbyte, F
    rlf Hibyte, F
    movlw 0x1A
    btfsc STATUS, C
    xorwf Lobyte, F

    movf Hibyte, W
    movwf GPIO
    goto loop

    END
*/

#ifdef GCC_VERSION_463
#include <legacymsp430.h>
#else
#include <msp430g2553.h>
#endif
#include "prng.h"
#include "io.h"

#define Topbyte 0
#define Hibyte 1
#define Midbyte 2
#define Lobyte 3

/*
volatile static union uni_ShiftReg {
    u_int i:32;
    struct str_s {
        u_short sh;
        u_short sl;
    } s;
    struct str_b {
        u_short pad:7;
        u_char carry:1;
        u_int reg:24;
    } b;
    struct str_b2 {
        u_int pad:15;
        u_char retbit:1;
        u_int pad2:15;
        u_char carry:1;
    } b2;
    u_char c[ 4 ];
} ShiftReg;
*/
volatile static struct {
    u_char c[ 4 ];
} ShiftReg;

void prng1bit( void )
{
#ifdef MAJORITY_DECISION
    unsigned char prn_b = 0, prn_c = 0;
	  unsigned short tempreg;

	  if( ( ADC10CTL1 & ( 0x0FFFF - ADC10BUSY ) ) != 0 )
	  {
		    tempreg = SampleAndConversionAdcTemp();
		    tempreg >>= 8;
		    if( ( tempreg & 0x0001 ) == 0 )
		    {
		        prn_b = 0;
		    } else {
            prn_b = 1;
        }
    }

    if( ( BUTTON_IN & BUTTON ) == 0 )
    {
        prn_c = 0;
    } else {
        prn_c = 1;
    }

    if( 0 != md_prng( prng(), prn_b, prn_c ) )
#else
    u_char val;

    val = prng();
    if( val != 0 )
#endif
    {
        LED_OUT |= LED1;
    } else {
        LED_OUT &= ~LED1;
    }
}

u_char prng( void )
{
    u_char carry, carry2;

    // Lobyte
    carry = ShiftReg.c[ Lobyte ] & 0x80;  // get Lobyte Carry
    if( carry != 0 )
    {
        carry = 0x01;
    } else {
        carry = 0;
    }
    ShiftReg.c[ Lobyte ] <<= 1;
    ShiftReg.c[ Lobyte ] |= ShiftReg.c[ Topbyte ];// Store previous Hibyte Carry to Lobyte

    // Midbyte
    carry2 = ShiftReg.c[ Midbyte ] & 0x80; // get Midbyte Carry
    if( carry2 != 0 )
    {
        carry2 = 0x01;
    } else {
        carry2 = 0;
    }
    ShiftReg.c[ Midbyte ] <<= 1;
    ShiftReg.c[ Midbyte ] |= carry;       // Store Lobyte Carry to Midbyte

    // Hibyte
    carry = ShiftReg.c[ Hibyte ] & 0x80;  // get Hibyte Carry
    if( carry != 0 )
    {
        carry = 0x01;
    } else {
        carry = 0;
    }
    ShiftReg.c[ Hibyte ] <<= 1;
    ShiftReg.c[ Hibyte ] |= carry2;        // Store Midbyte Carry to Hibyte
    ShiftReg.c[ Topbyte ] = carry;

    if( carry != 0 )
    {
        ShiftReg.c[ Lobyte ] ^= 0x1A;
    }

    return ( ShiftReg.c[ Hibyte ] & 0x01 );
}

/* md_prng() -- majority decision */
u_char md_prng( prn_a, prn_b, prn_c )
u_char prn_a, prn_b, prn_c;
{
    if( prn_a == 1 )
    {
        if( prn_b == 1 || prn_c == 1 )
        {
            return 1;
        }
    }
    else if( prn_b == 1 && prn_c == 1 )
    {
        return 1;
    }
    return 0;
}

void init_prng( void )
{
    ShiftReg.c[ Topbyte ] = 0x00;
    ShiftReg.c[ Hibyte ] = 0x0FF;
    ShiftReg.c[ Midbyte ] = 0x0FF;
    ShiftReg.c[ Lobyte ] = 0x0FF;
//    LED_OUT |= LED1;  // Runup
//    while( 1 );
}
