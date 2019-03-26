
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


void CAN0_init(clkSource_t clkSource, bitTime_t bitTime);
void CAN_Transmitter(void);
void CAN_Receiver(void);

#endif /* CAN_H_ */
