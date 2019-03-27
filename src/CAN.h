
#ifndef CAN_H_
#define CAN_H_

#define SBC_MC33903 /* SBC requires SPI init + max 1MHz bit rate */

typedef enum {OSCILLATOR_SRC, PERIPHERAL_SRC} clkSource_t;
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
	uint32_t  RxCode;              /* Received message buffer code */
	uint32_t  RxID;                /* Received message ID */
	uint32_t  RxLength;            /* Received message number of data bytes */
	uint32_t  RxData[2];           /* Received message data */
	uint32_t  RxTimeStamp;         /* Received message time */
} Rx_t;

void CAN0_init(clkSource_t clkSource, bitTime_t bitTime);
void CAN0_Transmitter(void);
void CAN0_Receiver(void);

#endif /* CAN_H_ */
