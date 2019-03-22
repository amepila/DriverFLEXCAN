#include "S32K144.h"
#include "CAN.h"

void CAN0_init(void)
{
	/*Enable the clock to CAN0*/
	PCC->PCCn[PCC_FlexCAN0_INDEX] |= PCC_PCCn_CGC_MASK;

	/*Disable the CAN module before selecting clock*/
	CAN0->MCR |= CAN_MCR_MDIS_MASK;

	/*Choose the Oscillator Clock 8MHz*/
	CAN0->CTRL1 &= ~CAN_CTRL1_CLKSRC_MASK;

	/*Enable the CAN module*/
	CAN0->MCR &= ~CAN_MCR_MDIS_MASK;

	return;
}

