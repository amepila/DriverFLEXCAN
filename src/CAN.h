/**
 *	\file	CAN.h
 *	\brief
 *			This is the header of driver in baremetal environment of
 *			CAN protocol, the driver includes the functions INIT,
 *			TRANSMITTER and RECEIVER.
 *	\author ACE TEAM
 *			Andres Hernandez
 *			Carem Bernabe
 *			Eric Guedea
 *	\date	27/03/2019
 */

#ifndef CAN_H_
#define CAN_H_

#define SBC_MC33903 	/*Transceiver CAN*/

/*Port CAN*/
typedef enum {CAN_0, CAN_1, CAN_2} PortCAN_t;

/*Clock source variable*/
typedef enum {OSCILLATOR_SRC, PERIPHERAL_SRC} clkSource_t;

/*Time of the frame*/
typedef enum
{
	B10KHZ,
	B20KHZ,
	B50KHZ,
	B125KHZ,
	B250KHZ,
	B500KHZ,
	B800KHZ,
	B1MHZ
} bitTime_t;

typedef struct
{
	uint8_t		propSeg;		/*Propagation Segment*/
	uint8_t		phaseSeg1;		/*Phase Segment 1*/
	uint8_t		phaseSeg2;		/*Phase Segment 2*/
	uint8_t		bitSampling;	/*CAN bit sampling*/
} Timing_t;

/*Variables needed to configure the driver*/
typedef struct
{
	bitTime_t	bitTime;		/*Bit time of CAN frame*/
	clkSource_t clkSource;		/*Source clock*/
	Timing_t	timing;			/*Timing to CAN bus*/
} CAN_Config_t;

/*Variables needed to Rx*/
typedef struct
{
	uint32_t  RxCode;              /* Received message buffer code */
	uint32_t  RxID;                /* Received message ID */
	uint32_t  RxLength;            /* Received message number of data bytes */
	uint32_t  RxData[2];           /* Received message data */
	uint32_t  RxTimeStamp;         /* Received message time */
} Rx_t;

/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/
/*!
 	 \brief	 	Configure the CAN driver
 	 \param[in] CAN Port and Pointer with the configuration
 	 \return 	Void
 */
void CAN_init(PortCAN_t portCAN, const CAN_Config_t* CAN_Config);


/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/
/*!
 	 \brief	 	Tx to send the information
 	 \param[in]	CAN Port and Data to send in the bus
 	 \return	Void
 */
void CAN_Transmitter(PortCAN_t portCAN, uint32_t dataWord1, uint32_t dataWord2);

/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/
/*!
 	 \brief	 	Receive the data on different fields
 	 \param[in] CAN Port and References from variables where the data received is saved
 	 \return 	Void
 */
void CAN_Receiver(PortCAN_t portCAN, uint32_t *data1, uint32_t *data2);

#endif /* CAN_H_ */
