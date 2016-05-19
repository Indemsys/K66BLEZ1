// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2016.05.19
// 11:04:56
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "main.h"

#define UART_STR_SZ  128

uint8_t  uart_str[UART_STR_SZ+1];


/*------------------------------------------------------------------------------
  Выводим форматированную строку в UART
 \param BufferIndex
 \param sFormat
 ------------------------------------------------------------------------------*/
void UART_printf(const uint8_t *fmt, ...)
{
  int32_t    n;
  va_list    ap;
  va_start(ap, fmt);
  n = vsnprintf((char*)uart_str, UART_STR_SZ, (char *)fmt, ap);
  va_end(ap);
  UART4_send_buffer(uart_str, n);
}


/*------------------------------------------------------------------------------


 ------------------------------------------------------------------------------*/
void Printf_unique_identificator(void)
{
  unsigned int uid[4];

  Get_unique_identificator(uid);

  UART_printf("UID = %08X %08X %08X %08X", uid[0], uid[1], uid[2], uid[3]);

}
