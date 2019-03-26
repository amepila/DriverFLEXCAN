#ifndef LPSPI_H_
#define LPSPI_H_

void LPSPI1_init_master (void);
void LPSPI1_init_MC33903 (void);
void LPSPI1_transmit_16bits (uint16_t);
uint16_t LPSPI1_receive_16bits (void);

#endif /* LPSPI_H_ */
