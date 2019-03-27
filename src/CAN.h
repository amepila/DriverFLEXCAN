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

/*Variables needed to configure the driver*/
typedef struct
{
	clkSource_t		clkSource;
	bitTime_t		bitTime;
} CAN0_Config_t;

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
 	 \param[in] Pointer with the configuration
 	 \return 	Void
 */
void CAN0_init(const CAN0_Config_t* CAN0_Config);


/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/
/*!
 	 \brief	 	Tx to send the information
 	 \param[in]	Data to send in the bus
 	 \return	Void
 */
void CAN0_Transmitter(uint32_t dataWord1, uint32_t dataWord2);

/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/
/*!
 	 \brief	 	Receive the data on different fields
 	 \param[in] References from variables where the data received is saved
 	 \return 	Void
 */
void CAN0_Receiver(uint32_t *data1, uint32_t *data2);

#endif /* CAN_H_ */
