#include <xc.h>
#include "config.h"
#include "hal.h"

/* Define core clock frequency */
#define _XTAL_FREQ 1000000

/* setup for NRTPS tick support */
volatile uint64_t ms_ticks = 0;

void Timer0_Init(void)
{
    T0CONbits.TMR0ON = 0;   /* Stop timer */
    T0CONbits.T08BIT = 0;   /* 16-bit mode */
    T0CONbits.T0CS   = 0;   /* Internal instruction cycle clock (FOSC/4) */
    T0CONbits.PSA    = 1;   /* Prescaler is BYPASSED ( 1:1 ratio ) */

    /* Load preload value for 1 ms period (65286 = 0xFF06) */
    TMR0H = 0xFF;
    TMR0L = 0x06;

    T0CONbits.TMR0ON = 1;   /* Start timer */
    INTCONbits.TMR0IE = 1;  /* Enable Timer0 interrupt */
    INTCONbits.GIE   = 1;   /* Enable global interrupts */
}

void __interrupt() high_priority_isr(void) {
    if (INTCONbits.TMR0IF) {
        /* Reload timer high byte first, then low byte */
        TMR0H = 0xFF;
        TMR0L = 0x06;
        ms_ticks++;         /* Increment 64-bit tick counter */
        INTCONbits.TMR0IF = 0; /* Clear flag */
    }
}

uint64_t hal_get_sys_tick( void )
{
    uint64_t current_ticks;
    INTCONbits.TMR0IE = 0; /* Disable interrupt to read safely */
    current_ticks = ms_ticks;
    INTCONbits.TMR0IE = 1; /* Enable interrupt */
    return ( current_ticks );
}

/* NRTPS support calls hal_config first to configure hardware */
void hal_config( void )
{
    /* internal clock set to 1MHz */
    /* IRCF bits: 100 = 1 MHz internal block (or adjust based on postscaler) */
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 0;
    OSCCONbits.IRCF0 = 0;

    /* Select Internal Oscillator block as system clock */
    OSCCONbits.SCS1  = 1;
    OSCCONbits.SCS0  = 0;

    /* Init 1 ms tick timer */
    Timer0_Init();

    /* Init Binary Status Indicator */
    LED_port &= ~LED_pin; /* set LED pin as output */
    set_status_indicator( OFF ); /* Turn OFF LED */

}

/* NRTPS support calls hal_init to start background tasks needed by the hardware */
void hal_init( void )
{

}

/* Binary Status Indicator */
void set_status_indicator( bool state )
{
    if ( ON == state )
    {
        LED_latch |= LED_pin; /* turn on the LED */
    }
    else
    {
        LED_latch &= ~LED_pin; /* turn off the LED */
    }
}
