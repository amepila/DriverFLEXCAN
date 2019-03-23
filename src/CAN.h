
#ifndef CAN_H_
#define CAN_H_

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

#endif /* CAN_H_ */
