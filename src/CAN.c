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

#define MESSAGES_BUFF		(32)			/*Number of MB for CAN0*/
#define MESSAGES_BUFF_CAN12	(16)			/*Number of MB for CAN1 y CAN2*/
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
#define SYNC_SEGMENT		(1)				/*Synchronization Segment*/
#define SHIFT_PSEG1			(19)			/*Shift to Phase Segment 1*/
#define SHIFT_PSEG2			(16)			/*Shift to Phase Segment 2*/
#define SHIFT_RJW			(22)			/*Shift to Resyn Jump Width*/
#define SHIFT_SMP			(7)				/*Shift to Sampling bit*/
#define SHIFT_PRESDIV		(24)			/*Shift to Prescaler divisor*/

Rx_t	rx;		/*Structure of Rx*/

/*Setup the configurations of the frame between eight options*/
static void CAN_SetBitTime(PortCAN_t portCAN, clkSource_t clkSource, bitTime_t bitTime, Timing_t timing)
{
	uint32_t time_quanta;		/*Value of time quantum*/
	uint32_t FrequencyTimeQ;	/*Frequency time quantum*/
	uint32_t PresDiv;			/*Prescaler divisor*/
	uint32_t Pseg1;				/*Phase Segment 1*/
	uint32_t Pseg2;				/*Phase Segment 2*/
	uint32_t Proseg;			/*Propagation Segment*/
	uint32_t Rjw;				/*Resync Jump Width*/

	/*Sum all the segments to obtain the total time quantum*/
	time_quanta = timing.phaseSeg1 + timing.phaseSeg2 + timing.propSeg + SYNC_SEGMENT;

	/*Assign the Frequency of Time Quantum with each bit time required*/
	switch(bitTime)
	{
	case B10KHZ:
		FrequencyTimeQ = time_quanta * 10000;
		break;
	case B20KHZ:
		FrequencyTimeQ = time_quanta * 20000;
		break;
	case B50KHZ:
		FrequencyTimeQ = time_quanta * 50000;
		break;
	case B125KHZ:
		FrequencyTimeQ = time_quanta * 125000;
		break;
	case B250KHZ:
		FrequencyTimeQ = time_quanta * 250000;
		break;
	case B500KHZ:
		FrequencyTimeQ = time_quanta * 500000;
		break;
	case B800KHZ:
		FrequencyTimeQ = time_quanta * 800000;
		break;
	case B1MHZ:
		FrequencyTimeQ = time_quanta * 1000000;
		break;
	default:
		break;
	}

	/*Calculate the prescaler divisor*/
	PresDiv = (clkSource/FrequencyTimeQ) -  1;

	/*Calculate the phase segment 1*/
	Pseg1 = timing.phaseSeg1 + 1;

	/*Calculate the phase segment 2*/
	Pseg2 = timing.phaseSeg2 + 1;

	/*Calculate the propagation segment*/
	Proseg = timing.propSeg - 1;

	/*Calculate the resync jump width*/
	Rjw = timing.phaseSeg2 - 1;

	switch(portCAN)
	{
	case CAN_0:
		/*AssigN the Prescaler divisor*/
		CAN0->CTRL1 |= PresDiv << SHIFT_PRESDIV;

		/*Assign the Phase Segment 1*/
		CAN0->CTRL1 |= Pseg1 << SHIFT_PSEG1;

		/*Assign the Phase Segment 2*/
		CAN0->CTRL1 |= Pseg2 << SHIFT_PSEG2;

		/*Assign the Resyn Jump Width*/
		CAN0->CTRL1 |= Rjw << SHIFT_RJW;

		/*Assign the Propagation Segment*/
		CAN0->CTRL1 |= Proseg;

		/*Assign the Sampling bit*/
		CAN0->CTRL1 |= timing.bitSampling << SHIFT_SMP;
		break;
	case CAN_1:
		/*AssigN the Prescaler divisor*/
		CAN1->CTRL1 |= PresDiv << SHIFT_PRESDIV;

		/*Assign the Phase Segment 1*/
		CAN1->CTRL1 |= Pseg1 << SHIFT_PSEG1;

		/*Assign the Phase Segment 2*/
		CAN1->CTRL1 |= Pseg2 << SHIFT_PSEG2;

		/*Assign the Resyn Jump Width*/
		CAN1->CTRL1 |= Rjw << SHIFT_RJW;

		/*Assign the Propagation Segment*/
		CAN1->CTRL1 |= Proseg;

		/*Assign the Sampling bit*/
		CAN1->CTRL1 |= timing.bitSampling << SHIFT_SMP;
		break;
	case CAN_2:
		/*AssigN the Prescaler divisor*/
		CAN2->CTRL1 |= PresDiv << SHIFT_PRESDIV;

		/*Assign the Phase Segment 1*/
		CAN2->CTRL1 |= Pseg1 << SHIFT_PSEG1;

		/*Assign the Phase Segment 2*/
		CAN2->CTRL1 |= Pseg2 << SHIFT_PSEG2;

		/*Assign the Resyn Jump Width*/
		CAN2->CTRL1 |= Rjw << SHIFT_RJW;

		/*Assign the Propagation Segment*/
		CAN2->CTRL1 |= Proseg;

		/*Assign the Sampling bit*/
		CAN2->CTRL1 |= timing.bitSampling << SHIFT_SMP;
		break;
	default:
		break;
	}
}

/*Setup the CAN with a selectable clock*/
void CAN_init(PortCAN_t portCAN, const CAN_Config_t* CAN_Config)
{
	uint32_t counter;

	switch(portCAN)
	{
	case CAN_0:
		/*Enable the clock to CAN0*/
		PCC->PCCn[PCC_FlexCAN0_INDEX] |= PCC_PCCn_CGC_MASK;

		/*Disable the CAN module before selecting clock*/
		CAN0->MCR |= CAN_MCR_MDIS_MASK;


		if (OSCILLATOR_SRC == CAN_Config->clkSource)
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
		CAN_SetBitTime(portCAN, CAN_Config->clkSource, CAN_Config->bitTime, CAN_Config->timing);

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
		break;

	case CAN_1:
		/*Enable the clock to CAN1*/
		PCC->PCCn[PCC_FlexCAN1_INDEX] |= PCC_PCCn_CGC_MASK;

		/*Disable the CAN module before selecting clock*/
		CAN1->MCR |= CAN_MCR_MDIS_MASK;


		if (OSCILLATOR_SRC == CAN_Config->clkSource)
			/*Choose the Oscillator Clock 8MHz*/
			CAN1->CTRL1 &= ~CAN_CTRL1_CLKSRC_MASK;
		else
			/*Choose the Peripheral Clock*/
			CAN1->CTRL1 |= CAN_CTRL1_CLKSRC_MASK;


		/*Enable the CAN module*/
		CAN1->MCR &= ~CAN_MCR_MDIS_MASK;

		/*Wait for FRZACK to be frozen*/
		while (!((CAN1->MCR & CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT));

		/*Now we can change the register in CTRL1*/
		CAN_SetBitTime(portCAN, CAN_Config->clkSource, CAN_Config->bitTime, CAN_Config->timing);

		/*Loopback is enabled*/
		CAN1->CTRL1 |= CAN_CTRL1_LPB_MASK;

		/*FIFO is disabled*/
		CAN1->MCR &= ~ CAN_MCR_RFEN_MASK;

		/*Self reception is enabled*/
		CAN1->MCR &= ~ CAN_MCR_SRXDIS_MASK;

		/*Check all IDs*/
		for(counter = 0; counter < MB_FILT; counter++)
			CAN1->RXIMR[counter] = CHECK_ID;

		/*Global acceptance mask to check all the IDs*/
		CAN1->RXMGMASK = CHECK_ALL_ID;

		/*Disable the RX*/
		CAN1->RAMn[RX_MB4] = DISABLE_RX;

		/*Assign the standard ID to the next word of MB4*/
		CAN1->RAMn[RX_MB4 + 1] = TX_ID_WORD;

		/*Enable the RX*/
		CAN1->RAMn[RX_MB4] = ENABLE_RX;

		/*CAN FD is not used*/
		CAN1->MCR = CANFD_NOT_USED;

		/*Wait for FRZACK to be unfrozen*/
		while ((CAN1->MCR && CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT);

		/*Wait for CAN Module to be ready*/
		while ((CAN1->MCR && CAN_MCR_NOTRDY_MASK) >> CAN_MCR_NOTRDY_SHIFT);
		break;

	case CAN_2:
		/*Enable the clock to CAN0*/
		PCC->PCCn[PCC_FlexCAN2_INDEX] |= PCC_PCCn_CGC_MASK;

		/*Disable the CAN module before selecting clock*/
		CAN2->MCR |= CAN_MCR_MDIS_MASK;


		if (OSCILLATOR_SRC == CAN_Config->clkSource)
			/*Choose the Oscillator Clock 8MHz*/
			CAN2->CTRL1 &= ~CAN_CTRL1_CLKSRC_MASK;
		else
			/*Choose the Peripheral Clock*/
			CAN2->CTRL1 |= CAN_CTRL1_CLKSRC_MASK;


		/*Enable the CAN module*/
		CAN2->MCR &= ~CAN_MCR_MDIS_MASK;

		/*Wait for FRZACK to be frozen*/
		while (!((CAN2->MCR & CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT));

		/*Now we can change the register in CTRL1*/
		CAN_SetBitTime(portCAN, CAN_Config->clkSource, CAN_Config->bitTime, CAN_Config->timing);

		/*Loopback is enabled*/
		CAN2->CTRL1 |= CAN_CTRL1_LPB_MASK;

		/*FIFO is disabled*/
		CAN2->MCR &= ~ CAN_MCR_RFEN_MASK;

		/*Self reception is enabled*/
		CAN2->MCR &= ~ CAN_MCR_SRXDIS_MASK;

		/*Check all IDs*/
		for(counter = 0; counter < MB_FILT; counter++)
			CAN2->RXIMR[counter] = CHECK_ID;

		/*Global acceptance mask to check all the IDs*/
		CAN2->RXMGMASK = CHECK_ALL_ID;

		/*Disable the RX*/
		CAN2->RAMn[RX_MB4] = DISABLE_RX;

		/*Assign the standard ID to the next word of MB4*/
		CAN2->RAMn[RX_MB4 + 1] = TX_ID_WORD;

		/*Enable the RX*/
		CAN2->RAMn[RX_MB4] = ENABLE_RX;

		/*CAN FD is not used*/
		CAN2->MCR = CANFD_NOT_USED;

		/*Wait for FRZACK to be unfrozen*/
		while ((CAN2->MCR && CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT);

		/*Wait for CAN Module to be ready*/
		while ((CAN2->MCR && CAN_MCR_NOTRDY_MASK) >> CAN_MCR_NOTRDY_SHIFT);
		break;

	default:
		break;
	}
}

/*Transmit the data through of channel CAN 0 with two data */
void CAN_Transmitter(PortCAN_t portCAN, uint32_t dataWord1, uint32_t dataWord2)
{
	switch(portCAN)
	{
	case CAN_0:
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
		break;

	case CAN_1:
		/*Clean MB0 flag*/
		CAN1->IFLAG1 = CLEAN_MB0;

		/*Data word 1 in the third position of MB0*/
		CAN1->RAMn[TX_MB0 + 2] = dataWord1;

		/*Data word 2 in the fourth position of MB0*/
		CAN1->RAMn[TX_MB0 + 3] = dataWord2;

		/* Standard ID 0x555, first position of MB0*/
		CAN1->RAMn[TX_MB0 + 1] = TX_ID_WORD;

		/*SETUP OF THE TX*/
		/*Set the length of DLC*/
		CAN1->RAMn[TX_MB0 + 0] |= (DLC_LENGTH << CAN_WMBn_CS_DLC_SHIFT);

		/*Assign the code to transmit MB*/
		CAN1->RAMn[TX_MB0 + 0] |= CODE_FIELD_TX;

		/*Set TX frame*/
		CAN1->RAMn[TX_MB0 + 0] |= SRR_TX;
		break;

	case CAN_2:
		/*Clean MB0 flag*/
		CAN2->IFLAG1 = CLEAN_MB0;

		/*Data word 1 in the third position of MB0*/
		CAN2->RAMn[TX_MB0 + 2] = dataWord1;

		/*Data word 2 in the fourth position of MB0*/
		CAN2->RAMn[TX_MB0 + 3] = dataWord2;

		/* Standard ID 0x555, first position of MB0*/
		CAN2->RAMn[TX_MB0 + 1] = TX_ID_WORD;

		/*SETUP OF THE TX*/
		/*Set the length of DLC*/
		CAN2->RAMn[TX_MB0 + 0] |= (DLC_LENGTH << CAN_WMBn_CS_DLC_SHIFT);

		/*Assign the code to transmit MB*/
		CAN2->RAMn[TX_MB0 + 0] |= CODE_FIELD_TX;

		/*Set TX frame*/
		CAN2->RAMn[TX_MB0 + 0] |= SRR_TX;
		break;
	default:
		break;
	}
}

/*Receive the data though the channel and only is received two data*/
void CAN_Receiver(PortCAN_t portCAN, uint32_t *data1, uint32_t *data2)
{
	uint8_t counter;
	uint32_t dummy;

	switch(portCAN)
	{
	case CAN_0:
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
		break;

	case CAN_1:
		/*Obtain the code of RX with the first word of MB4*/
		rx.RxCode   = (CAN1->RAMn[RX_MB4] & CODE_MASK_RX) >> SHIFT_CODE_RX;

		/*Obtain the ID of RX with the next word of MB4*/
		rx.RxID     = (CAN1->RAMn[RX_MB4 + 1] & CAN_WMBn_ID_ID_MASK)  >> CAN_WMBn_ID_ID_SHIFT;

		/*Obtain the length of RX with the first word of MB4*/
		rx.RxLength = (CAN1->RAMn[RX_MB4] & CAN_WMBn_CS_DLC_MASK) >> CAN_WMBn_CS_DLC_SHIFT;

		/*Read the data received*/
		for (counter = 0; counter < MAX_DATA; counter++)
		  rx.RxData[counter] = CAN1->RAMn[RX_DATA_MB4 + counter];

		/*Save the data received*/
		data1 = rx.RxData[0];
		data2 = rx.RxData[1];

		/*Obtain the time stamp*/
		rx.RxTimeStamp = (CAN1->RAMn[RX_MB4] & TIME_STAMP_RX);

		/*Unlock message buffers*/
		dummy = CAN1->TIMER;

		/*Clean MB4 flag*/
		CAN1->IFLAG1 = MB4_CLEAN_FLAG;
		break;

	case CAN_2:
		/*Obtain the code of RX with the first word of MB4*/
		rx.RxCode   = (CAN2->RAMn[RX_MB4] & CODE_MASK_RX) >> SHIFT_CODE_RX;

		/*Obtain the ID of RX with the next word of MB4*/
		rx.RxID     = (CAN2->RAMn[RX_MB4 + 1] & CAN_WMBn_ID_ID_MASK)  >> CAN_WMBn_ID_ID_SHIFT;

		/*Obtain the length of RX with the first word of MB4*/
		rx.RxLength = (CAN2->RAMn[RX_MB4] & CAN_WMBn_CS_DLC_MASK) >> CAN_WMBn_CS_DLC_SHIFT;

		/*Read the data received*/
		for (counter = 0; counter < MAX_DATA; counter++)
		  rx.RxData[counter] = CAN2->RAMn[RX_DATA_MB4 + counter];

		/*Save the data received*/
		data1 = rx.RxData[0];
		data2 = rx.RxData[1];

		/*Obtain the time stamp*/
		rx.RxTimeStamp = (CAN2->RAMn[RX_MB4] & TIME_STAMP_RX);

		/*Unlock message buffers*/
		dummy = CAN2->TIMER;

		/*Clean MB4 flag*/
		CAN2->IFLAG1 = MB4_CLEAN_FLAG;
		break;
	default:
		break;
	}
}
