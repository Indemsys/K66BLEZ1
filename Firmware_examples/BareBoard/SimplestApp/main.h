#ifndef __MAIN_H
  #define __MAIN_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "MK65F18.h"
#include "K66BLEZ1_GPIO.h"
#include "K66BLEZ1_INIT_SYS.h"

#define     BIT(n) (1u << n)
#define     LSHIFT(v,n) (((unsigned int)(v) << n))

  #define  DELAY_1us    Delay_m7(25)           // 1.011     ��� ��� ������� 180 ���
  #define  DELAY_4us    Delay_m7(102)          // 4.005     ��� ��� ������� 180 ���
  #define  DELAY_8us    Delay_m7(205)          // 8.011     ��� ��� ������� 180 ���
  #define  DELAY_32us   Delay_m7(822)          // 32.005    ��� ��� ������� 180 ���
  #define  DELAY_ms(x)  Delay_m7(25714*x-1)    // 999.95*N  ��� ��� ������� 180 ���

extern void Delay_m7(int cnt); // �������� �� (cnt+1)*7 ������ . �������� ���� �������������

#endif
