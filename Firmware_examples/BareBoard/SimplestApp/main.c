#include "main.h"

int main()
{
  for (;;)
  {
    GPIOA_PTOR = BIT(1);
    DELAY_ms(20);
  }
}
