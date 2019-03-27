/**
 *	\file	CAN.h
 *	\brief
 *			This is the source of driver in baremetal environment of
 *			CAN protocol, the driver includes the functions INIT,
 *			TRANSMITTER and RECEIVER.
 *	\author ACE TEAM
 *			Andres Hernandez
 *			Carem Bernabe
 *			Eric Guedea
 *	\date	27/03/2019
 */

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
#define ENABLE_RX			(0x04000000)	/*Code field of the Control and Status*/
#define RX_ID_WORD			(0x14440000)	/*Word with ID for RX*/
#define CANFD_NOT_USED		(0x0000001F)	/*Prior equal to zero*/
#define CLEAN_MB0			(0x00000001)	/*Clean the flag of MB0*/
#define TX_MB0				(0)				/*Message Buffer 0 for TX*/
#define DISABLE_RX			(0x00000000)	/*Disable the RX*/

#define TX_ID_WORD			(0x15540000)	/*Word with ID for TX*/
#define DLC_LENGTH			(8)				/*Length of DLC in Bytes*/
#define CODE_FIELD_TX		(0x0C000000)	/*Code to enable the transmission of MB*/
#define SRR_TX				(0x400000)		/*Set the TX frame*/

#define SHIFT_CODE_RX		(24)			/*Shift to obtain the code of RX*/
#define CODE_MASK_RX		(0x07000000)	/*Mask to obtain the code of RX*/
#define RX_DATA_MB4			(18)			/*Position of data in MB4*/
#define TIME_STAMP_RX		(0x000FFFF)		/*Mask to obtain the time stamp*/
#define MB4_CLEAN_FLAG		(0x00000010)	/*Mask to clean the MB4 flag*/


Rx_t	rx;		/*Structure of Rx*/

/*Setup the configurations of the frame betweeen eight options*/
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

/*Setup the CAN0 with a selectable clock*/
void CAN0_init(const CAN0_Config_t* CAN0_Config)
{
	uint32_t counter;

	/*Total of words in RAM*/
	const uint8_t words = MESSAGES_BUFF * WORDS_PER_MB;

	/*Enable the clock to CAN0*/
	PCC->PCCn[PCC_FlexCAN0_INDEX] |= PCC_PCCn_CGC_MASK;

	/*Disable the CAN module before selecting clock*/
	CAN0->MCR |= CAN_MCR_MDIS_MASK;


	if (OSCILLATOR_SRC == CAN0_Config->clkSource)
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
	CAN0_SetBitTime(CAN0_Config->bitTime);

	/*Loopback is enabled*/
	CAN0->CTRL1 |= CAN_CTRL1_LPB_MASK;

	/*FIFO is disabled*/
	CAN0->MCR &= ~ CAN_MCR_RFEN_MASK;

	/*Self reception is enabled*/
	CAN0->MCR &= ~ CAN_MCR_SRXDIS_MASK;

	/*Check all IDs*/
	for(counter = 0; counter < MB_FILT; counter++)
		CAN0->RXIMR[counter] = CHECK_ID;

	/*Global acceptance mask to check all the IDs*/
	CAN0->RXMGMASK = CHECK_ALL_ID;

	/*Disable the RX*/
	CAN0->RAMn[RX_MB4] = DISABLE_RX;

	/*Assign the standard ID to the next word of MB4*/
	CAN0->RAMn[RX_MB4 + 1] = TX_ID_WORD;

	/*Enable the RX*/
	CAN0->RAMn[RX_MB4] = ENABLE_RX;

	/*CAN FD is not used*/
	CAN0->MCR = CANFD_NOT_USED;

	/*Wait for FRZACK to be unfrozen*/
	while ((CAN0->MCR && CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT);

	/*Wait for CAN Module to be ready*/
	while ((CAN0->MCR && CAN_MCR_NOTRDY_MASK) >> CAN_MCR_NOTRDY_SHIFT);
}

/*Transmit the data through of channel CAN 0 with two data */
void CAN0_Transmitter(uint32_t dataWord1, uint32_t dataWord2)
{
	/*Clean MB0 flag*/
	CAN0->IFLAG1 = CLEAN_MB0;

	/*Data word 1 in the third position of MB0*/
	CAN0->RAMn[TX_MB0 + 2] = dataWord1;

	/*Data word 2 in the fourth position of MB0*/
	CAN0->RAMn[TX_MB0 + 3] = dataWord2;

	/* Standard ID 0x555, first position of MB0*/
	CAN0->RAMn[TX_MB0 + 1] = TX_ID_WORD;

	/*SETUP OF THE TX*/
	/*Set the length of DLC*/
	CAN0->RAMn[TX_MB0 + 0] |= (DLC_LENGTH << CAN_WMBn_CS_DLC_SHIFT);

	/*Assign the code to transmit MB*/
	CAN0->RAMn[TX_MB0 + 0] |= CODE_FIELD_TX;

	/*Set TX frame*/
	CAN0->RAMn[TX_MB0 + 0] |= SRR_TX;
}

/*Receive the data though the channel and only is received two data*/
void CAN0_Receiver(uint32_t *data1, uint32_t *data2)
{
	uint8_t counter;
	uint32_t dummy;

	/*Obtain the code of RX with the first word of MB4*/
	rx.RxCode   = (CAN0->RAMn[RX_MB4] & CODE_MASK_RX) >> SHIFT_CODE_RX;

	/*Obtain the ID of RX with the next word of MB4*/
	rx.RxID     = (CAN0->RAMn[RX_MB4 + 1] & CAN_WMBn_ID_ID_MASK)  >> CAN_WMBn_ID_ID_SHIFT;

	/*Obtain the length of RX with the first word of MB4*/
	rx.RxLength = (CAN0->RAMn[RX_MB4] & CAN_WMBn_CS_DLC_MASK) >> CAN_WMBn_CS_DLC_SHIFT;

	/*Read the data received*/
	for (counter = 0; counter < MAX_DATA; counter++)
	  rx.RxData[counter] = CAN0->RAMn[RX_DATA_MB4 + counter];

	/*Save the data received*/
	data1 = rx.RxData[0];
	data2 = rx.RxData[1];

	/*Obtain the time stamp*/
	rx.RxTimeStamp = (CAN0->RAMn[RX_MB4] & TIME_STAMP_RX);

	/*Unlock message buffers*/
	dummy = CAN0->TIMER;

	/*Clean MB4 flag*/
	CAN0->IFLAG1 = MB4_CLEAN_FLAG;
}
