#ifndef __K66BLEZ_UART_H
  #define __K66BLEZ_UART_H


void UART4_Init(uint32_t speed);
int  UART4_send_buffer(uint8_t *buf, int32_t len);
int  UART_wait_sym(uint32_t ticks);
int  UART_check_sym(void);

#endif // APP_UART_H



