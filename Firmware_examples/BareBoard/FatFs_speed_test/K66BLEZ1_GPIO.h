#ifndef __INIT_PINS
  #define __INIT_PINS



  #ifdef __cplusplus
extern "C"
{
  #endif

#define     BIT(n) (1u << n)
#define     LSHIFT(v,n) (((unsigned int)(v) << n))

#define     LED_BIT BIT(1)
#define     LED_ON   PTA->PCOR = LED_BIT
#define     LED_OFF  PTA->PSOR = LED_BIT


int Init_pins(void);


  #ifdef __cplusplus
}
  #endif

#endif