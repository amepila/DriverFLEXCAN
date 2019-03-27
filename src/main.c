#include "S32K144.h"
#include "CAN.h"
#include "LPSPI.h"
#include "GPIO.h"
#include "clock_and_modes.h"

int main(void)
{
	WDOG_disable();
	SOSC_init_8MHz();       /* Initialize system oscillator for 8 MHz xtal */
	SPLL_init_160MHz();     /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	NormalRUNmode_80MHz();  /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */

	CAN0_init(OSCILLATOR_SRC, B500KHZ);         /* Init FlexCAN0 */
	PORT_init();             /* Configure ports */

	#ifdef SBC_MC33903 /* SPI and transceiver initialization is required */
	  LPSPI1_init_master(); /* Initialize LPSPI1 for communication with MC33903 */
	  LPSPI1_init_MC33903(); /* Configure SBC via SPI for CAN transceiver operation */
	#endif

	  CAN_Transmitter(); /* Transmit one message */
	  for(;;)
	  {
#if 1
		  CAN_Transmitter ();
		  delay();
		  CAN_Receiver ();
		  delay();

#endif
#if 0
		  if ((CAN0->IFLAG1 >> 4) & 1)  /* If CAN 0 MB 4 flag is set (received msg), read MB4 */
			  CAN_Receiver ();
		  if ((CAN0->IFLAG1 >> 0) & 1) /* If CAN 0 MB 0 flag is set (transmit done), transmit */
			  CAN_Transmitter(); /* Transmit message again */
#endif
	  }
	return 0;
}
