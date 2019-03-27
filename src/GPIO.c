/**
 *	\file	GPIO.c
 *	\brief
 *			This is the source of GPIO in baremetal environment
 *			where the watch dog and ports are disabled and enabled
 *			respectively.
 *	\author ACE TEAM
 *			Andres Hernandez
 *			Carem Bernabe
 *			Eric Guedea
 *	\date	27/03/2019
 */

#include "S32K144.h"           /* include peripheral declarations S32K144 */
#include "clock_and_modes.h"
#include "GPIO.h"

/*Unlock the watchdog*/
void WDOG_disable (void)
{
  WDOG->CNT=0xD928C520; 	/* Unlock watchdog */
  WDOG->TOVAL=0x0000FFFF;	/* Maximum timeout value */
  WDOG->CS = 0x00002100;    /* Disable watchdog */
}

/*Activate the needed clock*/
void PORT_init (void)
{
  PCC->PCCn[PCC_PORTE_INDEX] |= PCC_PCCn_CGC_MASK; 	/* Enable clock for PORTE */
  PORTE->PCR[4] |= PORT_PCR_MUX(5); 				/* Port E4: MUX = ALT5, CAN0_RX */
  PORTE->PCR[5] |= PORT_PCR_MUX(5); 				/* Port E5: MUX = ALT5, CAN0_TX */
#ifdef SBC_MC33903 /* If board has MC33904, SPI pin config. is required */
  PCC->PCCn[PCC_PORTB_INDEX] |= PCC_PCCn_CGC_MASK; 	/* Enable clock for PORTB */
  PORTB->PCR[14] |= PORT_PCR_MUX(3); 				/* Port B14: MUX = ALT3, LPSPI1_SCK */
  PORTB->PCR[15] |= PORT_PCR_MUX(3); 				/* Port B15: MUX = ALT3, LPSPI1_SIN */
  PORTB->PCR[16] |= PORT_PCR_MUX(3); 				/* Port B16: MUX = ALT3, LPSPI1_SOUT */
  PORTB->PCR[17] |= PORT_PCR_MUX(3); 				/* Port B17: MUX = ALT3, LPSPI1_PCS3 */
#endif
}

/*Delay that costumized count*/
void delay(uint32_t count)
{
	uint32_t counter;

	for(counter = 0; counter < count; counter++)
	{
	}
}
