#ifndef TIMEMAN_H
  #define TIMEMAN_H

int      TimeManInit(void);
void     Get_time_counters(uint32_t *t);
uint32_t Time_diff_microseconds(uint32_t t2, uint32_t t1);
uint32_t Get_calibrating_time(void);



#endif // TIMEMAN_H



