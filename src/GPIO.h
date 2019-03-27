/**
 *	\file	GPIO.h
 *	\brief
 *			This is the source of GPIO in baremetal environment
 *			where the watch dog and ports are disabled and enabled
 *			respectively.
 *	\author ACE TEAM
 *			Andres Hernandez
 *			Carem Bernabe
 *			Eric Guedea
 *	\date	27/03/2019
 */
#ifndef GPIO_H_
#define GPIO_H_

#include "CAN.h"

typedef enum{PORT_A, PORT_B, PORT_C, PORT_D, PORT_E} Port_t;

/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/
/*!
 	 \brief	 	Disable the watchdog
 	 \param[in] Void
 	 \return 	Void
 */
void WDOG_disable (void);

/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/
/*!
 	 \brief	 	Configure the needed clock
 	 \param[in] Port of CAN and peripheral port
 	 \return 	Void
 */
void PORT_init (PortCAN_t portCAN, Port_t port);

/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/
/*!
 	 \brief	 	Configure a delay
 	 \param[in] Count
 	 \return 	Void
 */
void delay(uint32_t count);

#endif /* GPIO_H_ */
