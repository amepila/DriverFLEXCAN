#include "S32K144.h"
#include "CAN.h"

#define MESSAGES_BUFF		(32)			/*Number of MB*/
#define WORDS_PER_MB		(4)				/*Words per Messages Buffer*/
#define RAM_LENGTH			(128)			/*Length of the RAM*/
#define MAX_DATA			(2)				/*Maximum of words with data*/
#define MB_FILT				(16)			/*Messages Buffer filtered*/
#define CHECK_ID			(0xFFFFFFFF)	/*Check all IDs for MB*/
#define CHECK_ALL_ID		(0x1FFFFFFF)	/*Global acceptance mask*/
#define RX_MB4				(16)			/*Message Buffer 4 for RX*/
#define CODE_FIELD_4		(0x04000000)	/*Code field of the Control and Status*/
#define RX_ID_WORD			(0x14440000)	/*Word with ID for RX*/
#define CANFD_NOT_USED		(0x0000001F)	/*Prior equal to zero*/
#define CLEAN_MB0			(0x00000001)	/*Clean the flag of MB0*/

/*Structure of Rx*/
Rx_t	rx;

static void CAN0_SetBitTime(bitTime_t bitTime)
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
	CAN0->CTRL1 |= 0x00DB0006;
}

void CAN0_init(clkSource_t clkSource, bitTime_t bitTime)
{
	uint32_t counter;

	/*Total of words in RAM*/
	const uint8_t words = MESSAGES_BUFF * WORDS_PER_MB;

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
	while (!((CAN0->MCR & CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT)){}

	/*Now we can change the register in CTRL1*/
	CAN0_SetBitTime(bitTime);

	/*FIFO is disabled*/
	CAN0->MCR &= ~ CAN_MCR_RFEN_MASK;

	/*Self reception is enabled*/
	CAN0->MCR &= ~ CAN_MCR_SRXDIS_MASK;

	/*The RAM is cleaned*/
	for(counter = 0; counter < RAM_LENGTH; counter++)
		CAN0->RAMn[counter] = 0;

	/*Check all IDs*/
	for(counter = 0; counter < MB_FILT; counter++)
		CAN0->RXIMR[counter] = CHECK_ID;

	/*Global acceptance mask to check all the IDs*/
	CAN0->RXMGMASK = CHECK_ALL_ID;

	/*Assign the code field to the MB4*/
	CAN0->RAMn[RX_MB4] = CODE_FIELD_4;

	/*Assign the standard ID 0x511 to the next word of MB4*/
	CAN0->RAMn[RX_MB4 + 1] = RX_ID_WORD;

	/*CAN FD is not used*/
	CAN0->MCR = CANFD_NOT_USED;

	/*Wait for FRZACK to be unfrozen*/
	while ((CAN0->MCR && CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT){}

	/*Wait for CAN Module to be ready*/
	while ((CAN0->MCR && CAN_MCR_NOTRDY_MASK) >> CAN_MCR_NOTRDY_SHIFT){}

	return;
}

void CAN0_Transmitter(void)
{
	/* Clear CAN 0 MB 0 flag without clearing others*/
	CAN0->IFLAG1 = CLEAN_MB0;

	/* MB0 word 2: data word 0 */
	CAN0->RAMn[ 0*WORDS_PER_MB + 2] = 0xA5112233;

	/* MB0 word 3: data word 1 */
	CAN0->RAMn[ 0*WORDS_PER_MB + 3] = 0x44556677;

	/* MB0 word 1: Tx msg with STD ID 0x555 */
	CAN0->RAMn[ 0*WORDS_PER_MB + 1] = 0x15540000;
	CAN0->RAMn[ 0*WORDS_PER_MB + 0] = 0x0C400000 | 8 <<CAN_WMBn_CS_DLC_SHIFT; /* MB0 word 0: */
	                                                /* EDL,BRS,ESI=0: CANFD not used */
	                                                /* CODE=0xC: Activate msg buf to transmit */
	                                                /* IDE=0: Standard ID */
	                                                /* SRR=1 Tx frame (not req'd for std ID) */
	                                                /* RTR = 0: data, not remote tx request frame*/
	                                                /* DLC = 8 bytes */
}

void CAN0_Receiver(void)/* Receive msg from ID 0x556 using msg buffer 4 */
{
	  uint8_t counter;
	  uint32_t dummy;

	  /* If CAN 0 MB 4 flag is set (received msg), read MB4 */
	  /* Read CODE field */
	  rx.RxCode   = (CAN0->RAMn[4*WORDS_PER_MB] 	& 0x07000000) >> 24;
	  rx.RxID     = (CAN0->RAMn[4*WORDS_PER_MB+1] & CAN_WMBn_ID_ID_MASK)  >> CAN_WMBn_ID_ID_SHIFT ;
	  rx.RxLength = (CAN0->RAMn[4*WORDS_PER_MB] 	& CAN_WMBn_CS_DLC_MASK) >> CAN_WMBn_CS_DLC_SHIFT;

	  /* Read two words of data (8 bytes) */
	  for (counter = 0; counter < MAX_DATA; counter++) {
		rx.RxData[counter] = CAN0->RAMn[4*DATA_LENGTH_MB + 2 + counter];
	  }
	  rx.RxTimeStamp = (CAN0->RAMn[ 0*WORDS_PER_MB + 0] & 0x000FFFF);

	  /* Read TIMER to unlock message buffers */
	  dummy = CAN0->TIMER;
	  /* Clear CAN 0 MB 4 flag without clearing others*/
	  CAN0->IFLAG1 = 0x00000010;
}
