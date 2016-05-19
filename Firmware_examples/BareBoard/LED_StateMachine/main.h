#ifndef __MAIN_H
  #define __MAIN_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <intrinsics.h>

#define CPU_CORE_CLK_HZ 180000000ul  // „астота тактировани€ €дра
#define CPU_BUS_CLK_HZ  60000000ul   // „астота тактировани€ шины

#define SYS_TIMER_TICKS (CPU_CORE_CLK_HZ/100ul) // ѕерезагрузка каждые 10 мс

#include "MK65F18.h"
#include "K66BLEZ1_GPIO.h"
#include "K66BLEZ1_INIT_SYS.h"
#include "K66BLEZ1_UART.h"
#include "VT100.h"
#include "Print_utils.h"
#include "LED_StateMachine.h"

#define     BIT(n) (1u << n)
#define     LSHIFT(v,n) (((unsigned int)(v) << n))

  #define  DELAY_1us    Delay_m7(25)           // 1.011     мкс при частоте 180 ћ√ц
  #define  DELAY_4us    Delay_m7(102)          // 4.005     мкс при частоте 180 ћ√ц
  #define  DELAY_8us    Delay_m7(205)          // 8.011     мкс при частоте 180 ћ√ц
  #define  DELAY_32us   Delay_m7(822)          // 32.005    мкс при частоте 180 ћ√ц
  #define  DELAY_ms(x)  Delay_m7(25714*x-1)    // 999.95*N  мкс при частоте 180 ћ√ц

extern void Delay_m7(int cnt); // «адержка на (cnt+1)*7 тактов . ѕередача нул€ недопускаетс€




#endif
