#include   "app.h"




/*-------------------------------------------------------------------------------------------------------------

 -------------------------------------------------------------------------------------------------------------*/
int TimeManInit(void)
{

  SIM->SCGC6 |= BIT(23); // PIT | PIT clock gate control | 1 Clock is enabled.

  PIT->MCR =
             LSHIFT(0, 1) | // MDIS | Module Disable  | 1 Clock for PIT Timers is disabled.
             LSHIFT(1, 0) | // FRZ  | Freeze          | 1 Timers are stopped in debug mode.
             0;

  // Таймер 0 генерирует сигнал каждую микросекунду для таймера 1
  PIT->CHANNEL[1].LDVAL = 0xFFFFFFFF; //

  PIT->CHANNEL[1].TCTRL =
                          LSHIFT(1, 2) | // CHN |  Chain Mode                 | 1 Timer is chained to previous timer.
                          LSHIFT(0, 1) | // TIE | Timer Interrupt Enable Bit. | 0 Interrupt requests from Timer n are disabled.
                          LSHIFT(1, 0) | // TEN | Timer Enable Bit.           | 1 Timer n is active.
                          0;

  // Таймер 0 генерирует сигнал каждую микросекунду для таймера 1
  PIT->CHANNEL[0].LDVAL = 90 - 1; //

  PIT->CHANNEL[0].TCTRL =
                          LSHIFT(0, 1) | // TIE | Timer Interrupt Enable Bit. | 0 Interrupt requests from Timer n are disabled.
                          LSHIFT(1, 0) | // TEN | Timer Enable Bit.           | 1 Timer n is active.
                          0;

  return 0;
}

/*-------------------------------------------------------------------------------------------------------------
 Получить значение времени
 -------------------------------------------------------------------------------------------------------------*/
void Get_time_counters(uint32_t *t)
{
  *t = PIT->CHANNEL[1].CVAL;
}

/*-------------------------------------------------------------------------------------------------------------
 Вычисление разницы во времени в мкс
 -------------------------------------------------------------------------------------------------------------*/
uint32_t Time_diff_microseconds(uint32_t t2, uint32_t t1)
{

  return t1 - t2;
}

/*-------------------------------------------------------------------------------------------------------------

 -------------------------------------------------------------------------------------------------------------*/
uint32_t Get_calibrating_time(void)
{
  uint32_t t1, t2;

  Get_time_counters(&t1);
  DELAY_ms(100);
  //us_Delay(2079999u);
  Get_time_counters(&t2);

  return Time_diff_microseconds(t2, t1);
}


