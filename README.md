MSP430PseudoRNG
===============

TI MSP430 PseudoRNG(Random Number Generator) for MSP430-LaunchPad, or MSP430G2553.

Add new GCC(Mitto Systems Limited - msp430-gcc 9.2.0.50) support.
https://www.ti.com/tool/MSP430-GCC-OPENSOURCE

If you use GCC 4.6.3, you must edit Makefile.

for usage:

    make
    mspdebug rf2500
      > prog main.elf
      > exit

1bit Pseudorandom Number Output is P1.0 .
