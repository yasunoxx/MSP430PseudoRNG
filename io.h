/*
 * io.h -- MSP430 Pseudorandom Number Generator
 * by yasunoxx
 * ### Use mspgcc(4.6.3 or later) only !!! ###
 */

#define	LED1		BIT0
#define	LED2		BIT6
#define	LED_DIR		P1DIR
#define	LED_OUT		P1OUT
#define PRNG_DIR	P2DIR
#define PRNG_OUT	P2OUT

#define	BUTTON		BIT3
#define	BUTTON_OUT	P1OUT
#define	BUTTON_DIR	P1DIR
#define	BUTTON_IN	P1IN
#define	BUTTON_IE	P1IE
#define	BUTTON_IES	P1IES
#define	BUTTON_IFG	P1IFG
#define	BUTTON_REN	P1REN

/* io.c */
extern void InitializeLeds( void );
extern void InitializeButton( void );
extern void ConfigureAdcTempSensor( void );
extern unsigned short SampleAndConversionAdcTemp( void );
extern volatile unsigned short TempReg;
