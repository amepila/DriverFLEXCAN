/**
 *	\file	main.c
 *	\brief
 *			This is the main of driver where the driver is
 *			tested with the transmission and reception.
 *	\author ACE TEAM
 *			Andres Hernandez
 *			Carem Bernabe
 *			Eric Guedea
 *	\date	27/03/2019
 */

#include "S32K144.h"
#include "CAN.h"
#include "LPSPI.h"
#include "GPIO.h"
#include "clock_and_modes.h"

#define DATA_WORD_1			(0xA5112233)	/*Data word 1 to transmit*/
#define DATA_WORD_2			(0x44556677)	/*Data word 2 to transmit*/


/*Pointer that saves the information about the configuration about the CAN frame*/
const CAN0_Config_t	CAN0_Config =
{
	OSCILLATOR_SRC,		/*Source clock*/
	B500KHZ,			/*Bit time*/
	{7,					/*Propagation Segment*/
	4,					/*Phase 1 Segment*/
	4,					/*Phase 2 Segment*/
	1}					/*Sampling bit*/
};

int main(void)
{
	WDOG_disable();					/*Disable the watchdog*/
	ClockConfig();					/*Configure the clock*/

	CAN0_init(&CAN0_Config);		/* Init FlexCAN0 */
	PORT_init();             		/* Configure ports */

#ifdef SBC_MC33903 					/* SPI and transceiver initialization is required */
	  LPSPI1_init_master(); 		/* Initialize LPSPI1 for communication with MC33903 */
	  LPSPI1_init_MC33903(); 		/* Configure SBC via SPI for CAN transceiver operation */
#endif

	  uint32_t *dataReceived1;		/*Data to save the information from RX*/
	  uint32_t *dataReceived2;		/*Data to save the information from RX*/

	  for(;;)
	  {
		  delay(5000);
		  CAN0_Transmitter(DATA_WORD_1, DATA_WORD_2);
		  CAN0_Receiver (dataReceived1, dataReceived2);
	  }

	return 0;
}
