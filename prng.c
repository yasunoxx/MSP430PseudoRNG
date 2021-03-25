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

#define Hibyte 1
#define Midbyte 2
#define Lobyte 3

union uni_ShiftReg {
    u_int i;
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

u_char Prng_LoopCount;
u_char Prng_OutBuf;
const unsigned char WaveBuf[ 12 ] = { 0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

void prng6bit( void )
{
	unsigned char buf;

	Prng_OutBuf <<= 1;
	Prng_OutBuf |= prng();

	if( Prng_LoopCount >= 5 )
	{
		buf = PRNG_OUT;
		PRNG_OUT = buf & 0x0C0;
		PRNG_OUT |= ( Prng_OutBuf & 0x3F );
		Prng_LoopCount = 0;
	} else {
		Prng_LoopCount++;
	}
}

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

	if( 0 == md_prng( prng(), prn_b, prn_c ) )
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
    u_char carry;

    carry = ShiftReg.b.carry;
    ShiftReg.i <<= 1;
    ShiftReg.b2.carry = carry;

    if( carry == 1 )
    {
        ShiftReg.c[ Lobyte ] ^= 0x1A;
    }

    return ShiftReg.b2.retbit;
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
    } else if( prn_b == 1 && prn_c == 1 )
    {
        return 1;
    }
    return 0;
}

void init_prng( void )
{
	ShiftReg.i = 0x00FFFFFF;
	Prng_LoopCount = 0;
	PRNG_DIR = 0x3F;
}
