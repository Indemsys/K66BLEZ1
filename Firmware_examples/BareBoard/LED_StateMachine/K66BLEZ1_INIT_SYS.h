#ifndef __K66BLEZ_INIT_SYS
  #define __K66BLEZ_INIT_SYS

void       Init_cpu(void);
void       Get_unique_identificator(unsigned int *uid);
void       NVIC_EnableIRQ(int32_t irq_index);
void       NVIC_SetPriority(int32_t irq_index, uint32_t priority);
void       Init_sys_timer(void);
void       SysTick_Config(uint32_t ticks);
uint64_t   Get_time_us(void);

#endif
