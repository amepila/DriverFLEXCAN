#include "S32K144.h"
#include "CAN.h"

#define MESSAGES_BUFF	32
#define DATA_LENGTH_MB	4

uint32_t  RxCODE;              /* Received message buffer code */
uint32_t  RxID;                /* Received message ID */
uint32_t  RxLENGTH;            /* Recieved message number of data bytes */
uint32_t  RxDATA[2];           /* Received message data (2 words) */
uint32_t  RxTIMESTAMP;         /* Received message time */

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
	CAN0->CTRL1 |= 0x00DB0006;
}

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
	CAN0->MCR &= ~ CAN_MCR_RFEN_MASK;

	/*CAN has 32 MB(Message Buffers) of 4 Bytes data length, that is to say,
	 *with 32 MB there are 128 words, then all the MB are cleaned */
	for(counter = 0; counter < 128; counter++)
		CAN0->RAMn[counter] = 0;

	/*For testing (Pending)*/
	for(counter = 0; counter < 16; counter++)
		CAN0->RXIMR[counter] = 0xFFFFFFFF;

	CAN0->RXMGMASK = 0xFFFFFF;
	CAN0->RAMn[4*DATA_LENGTH_MB] = 0x04000000;

	/* Node A receives msg with std ID 0x511 */
	CAN0->RAMn[4*DATA_LENGTH_MB+1] = 0x14440000; /* Msg Buf 4, word 1: Standard ID = 0x111 */

	/* PRIO = 0: CANFD not used */
	CAN0->MCR = 0x0000001F;       /* Negate FlexCAN 1 halt state for 32 MBs */

	/*Wait for FRZACK to be unfrozen*/
	while ((CAN0->MCR && CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT);

	/*Wait for CAN Module to be ready*/
	while ((CAN0->MCR && CAN_MCR_NOTRDY_MASK) >> CAN_MCR_NOTRDY_SHIFT);

	return;
}

void CAN_Transmitter(void)
{
	/* Clear CAN 0 MB 0 flag without clearing others*/
	CAN0->IFLAG1 = 0x00000001;

	/* MB0 word 2: data word 0 */
	CAN0->RAMn[ 0*DATA_LENGTH_MB + 2] = 0xA5112233;

	/* MB0 word 3: data word 1 */
	CAN0->RAMn[ 0*DATA_LENGTH_MB + 3] = 0x44556677;

	/* MB0 word 1: Tx msg with STD ID 0x555 */
	CAN0->RAMn[ 0*DATA_LENGTH_MB + 1] = 0x15540000;
	CAN0->RAMn[ 0*DATA_LENGTH_MB + 0] = 0x0C400000 | 8 <<CAN_WMBn_CS_DLC_SHIFT; /* MB0 word 0: */
	                                                /* EDL,BRS,ESI=0: CANFD not used */
	                                                /* CODE=0xC: Activate msg buf to transmit */
	                                                /* IDE=0: Standard ID */
	                                                /* SRR=1 Tx frame (not req'd for std ID) */
	                                                /* RTR = 0: data, not remote tx request frame*/
	                                                /* DLC = 8 bytes */
}

void CAN_Receiver(void)/* Receive msg from ID 0x556 using msg buffer 4 */
{
	  uint8_t j;
	  uint32_t dummy;

	  /* If CAN 0 MB 4 flag is set (received msg), read MB4 */
	  /* Read CODE field */
	  RxCODE   = (CAN0->RAMn[4*DATA_LENGTH_MB] & 0x07000000) >> 24;
	  RxID     = (CAN0->RAMn[4*DATA_LENGTH_MB+1] & CAN_WMBn_ID_ID_MASK)  >> CAN_WMBn_ID_ID_SHIFT ;
	  RxLENGTH = (CAN0->RAMn[4*DATA_LENGTH_MB] & CAN_WMBn_CS_DLC_MASK) >> CAN_WMBn_CS_DLC_SHIFT;

	  /* Read two words of data (8 bytes) */
	  for (j=0; j<2; j++) {
		RxDATA[j] = CAN0->RAMn[4*DATA_LENGTH_MB + 2 + j];
	  }
	  RxTIMESTAMP = (CAN0->RAMn[ 0*DATA_LENGTH_MB + 0] & 0x000FFFF);

	  /* Read TIMER to unlock message buffers */
	  dummy = CAN0->TIMER;
	  /* Clear CAN 0 MB 4 flag without clearing others*/
	  CAN0->IFLAG1 = 0x00000010;
}
