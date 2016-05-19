#define _GLOBAL_
#include "main.h"


/*------------------------------------------------------------------------------



 \return int
 ------------------------------------------------------------------------------*/
int main()
{
  Init_sys_timer();

  UART4_Init(115200);
  UART_printf(VT100_CLEAR_AND_HOME"K66BLEZv1 started.\r\n");
  Printf_unique_identificator();
  UART_printf("\r\n");

  Set_LED_pattern(LED_2_BLINK, 0);
  DELAY_ms(5000);
  Set_LED_pattern(LED_BLINK, 0);
  for (;;)
  {
    DELAY_ms(20);
  }

}
