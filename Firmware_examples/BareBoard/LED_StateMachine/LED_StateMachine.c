// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2016.05.19
// 14:44:12
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "main.h"



#define LEDS_CNT  1

T_LED_ptrn ledsm_cbl[LEDS_CNT];

/*------------------------------------------------------------------------------


 ------------------------------------------------------------------------------*/
static void Set_LED_voltage(uint8_t val, uint8_t num)
{
  if (val == 0)
    GPIOA_PCOR = BIT(1);
  else
    GPIOA_PSOR = BIT(1);
}
/*-------------------------------------------------------------------------------------------------------------
  ������������� ������� ��� ������ ��������� ������� �� ���������

  ������ ������� �� ������� ���� ����.
  ������ ����� � ������ - �������� ����������
  ������ ����� � ������ - ������������ ��������� ������� � �������� ��
    �������� ������ 0x00000000 - �������� ������� � ������ �������
    �������� ������ 0xFFFFFFFF - �������� ���������� ���������

  n - ������ ���������� (0..1)
-------------------------------------------------------------------------------------------------------------*/
void Set_LED_pattern(const int32_t *pttn, uint32_t n)
{
  __disable_interrupt();
  if ((pttn != 0) && (ledsm_cbl[n].pattern_start_ptr != (int32_t *)pttn))
  {
    ledsm_cbl[n].pattern_start_ptr = (int32_t *)pttn;
    ledsm_cbl[n].pttn_ptr = (int32_t *)pttn;
    Set_LED_voltage(*ledsm_cbl[n].pttn_ptr, n);
    ledsm_cbl[n].pttn_ptr++;
    ledsm_cbl[n].counter = *ledsm_cbl[n].pttn_ptr;
    ledsm_cbl[n].pttn_ptr++;
  }
  __enable_interrupt();
}


/*------------------------------------------------------------------------------
   ������� ��������� �����������

 ------------------------------------------------------------------------------*/
void LEDS_state_automat(void)
{
  uint32_t        duration;
  uint32_t        voltage;
  uint32_t        n;


  for (n = 0; n < LEDS_CNT; n++)
  {
    // ���������� ���������� ������� ���������
    if (ledsm_cbl[n].counter) // ������������ ������ ������ ���� ������� �� �������
    {
      ledsm_cbl[n].counter--;
      if (ledsm_cbl[n].counter == 0)  // ������ ��������� ������� ��� ��������� ��������
      {
        if (ledsm_cbl[n].pattern_start_ptr != 0) // ��������� ���� �� ����������� ������
        {
          voltage = *ledsm_cbl[n].pttn_ptr;        // ������� �������� ����������
          ledsm_cbl[n].pttn_ptr++;
          duration = *ledsm_cbl[n].pttn_ptr;       // ������� ������������ ���������
          ledsm_cbl[n].pttn_ptr++;                 // ������� �� ��������� ������� �������
          if (duration != 0xFFFFFFFF)
          {
            if (duration == 0)  // ������������ ������ 0 �������� ������� ��������� �������� �� ������ ������� � ��������� �������
            {
              ledsm_cbl[n].pttn_ptr = ledsm_cbl[n].pattern_start_ptr;
              voltage = *ledsm_cbl[n].pttn_ptr;
              ledsm_cbl[n].pttn_ptr++;
              ledsm_cbl[n].counter = *ledsm_cbl[n].pttn_ptr;
              ledsm_cbl[n].pttn_ptr++;
              Set_LED_voltage(voltage, n);
            }
            else
            {
              ledsm_cbl[n].counter = duration;
              Set_LED_voltage(voltage, n);
            }
          }
          else
          {
            // �������� ������� � ����� ������� ��������� ��������� ��������
            Set_LED_voltage(voltage, n);
            ledsm_cbl[n].counter = 0;
            ledsm_cbl[n].pattern_start_ptr = 0;
          }
        }
        else
        {
          // ���� ��� ������� �������� ���������� �� ���������
          Set_LED_voltage(0, n);
        }
      }
    }
  }
}

