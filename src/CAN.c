#include "S32K144.h"
#include "CAN.h"

#define MESSAGES_BUFF	32
#define DATA_LENGTH_MB	8

void CAN0_init(clkSource_t clkSource, bitTime_t bitTime)
{
	uint32_t counter;
	const uint8_t words = MESSAGES_BUFF / DATA_LENGTH_MB;

	/*Enable the clock to CAN0*/
	PCC->PCCn[PCC_FlexCAN0_INDEX] |= PCC_PCCn_CGC_MASK;

	/*Disable the CAN module before selecting clock*/
	CAN0->MCR |= CAN_MCR_MDIS_MASK;

	if (OSCILLATOR_SRC == clkSource)
		/*Choose the Oscillator Clock 8MHz*/
		CAN0->CTRL1 &= ~CAN_CTRL1_CLKSRC_MASK;
	else
		/*Choose the Peripheral Clock*/
		CAN0->CTRL1 |= CAN_CTRL1_CLKSRC_MASK;

	/*Enable the CAN module*/
	CAN0->MCR &= ~CAN_MCR_MDIS_MASK;

	/*Wait for FRZACK to be frozen*/
	while (!((CAN0->MCR & CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT));

	/*Now we can change the register in CTRL1*/
	CAN0_setBitTime(bitTime);

	/*CAN has 32 MB(Message Buffers) of 8 Bytes data length, that is to say,
	 *with 32 MB there are 128 words, then all the MB are cleaned */
	for(counter = 0; counter < words; counter++)
		CAN0->RAMn[counter] = 0;

	/*For testing (Pending)*/
	for(counter = 0; counter < 16; counter++)
		CAN0->RXIMR[counter] = 0xFFFFFFFF;

	/*Wait for FRZACK to be unfrozen*/
	while ((CAN0->MCR && CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT);

	/*Wait for CAN Module to be ready*/
	while ((CAN0->MCR && CAN_MCR_NOTRDY_MASK) >> CAN_MCR_NOTRDY_SHIFT);

	return;
}


static void CAN0_setBitTime(bitTime_t bitTime)
{
	switch(bitTime)
	{
	case B10KHZ:
		break;
	case B20KHZ:
		break;
	case B50KHZ:
		break;
	case B125KHZ:
		break;
	case B250KHZ:
		break;
	case B500KHZ:
		break;
	case B800KHZ:
		break;
	case B1MHZ:
		break;
	default:
		break;
	}
	/*Configure the bit time*/
	//CAN0->CTRL1 |= bitTime;
}
