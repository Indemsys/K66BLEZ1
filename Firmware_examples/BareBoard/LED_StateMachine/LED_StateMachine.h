#ifndef LED_STATEMACHINE_H
  #define LED_STATEMACHINE_H


// ����������� ��������� ������ ��������� ����������� ��������
typedef struct
{
  uint32_t  init_state;
  uint32_t  counter;
  int32_t  *pattern_start_ptr;  // ��������� �� ������ �������� ���������� �������� ��������� (��������)
                                // ���� �������� � ������� = 0xFFFFFFFF, �� ������� ��������� �����������
                                // ���� �������� � ������� = 0x00000000, �� ������� ��������� �� ������ �������
  int32_t   *pttn_ptr;          // ������� ������� � ������� ���������

} T_LED_ptrn;




void Set_LED_pattern(const int32_t *pttn, uint32_t n);
void LEDS_state_automat(void);


#define LED__ON  0
#define LED_OFF  1

#ifdef _GLOBAL_

//  ������ ������� �� ������� ���� ����.
//  ������ ����� � ������ - �������� ����������
//  ������ ����� � ������ - ������������ ��������� ������� � �������� �� ��� ����������� ������ ���������(0xFFFFFFFF) ��� �����(0x00000000)

const int32_t   LED_BLINK[] =
{
  LED__ON, 10,
  LED_OFF, 10,
  0, 0
};


const int32_t   LED_2_BLINK[] =
{
  LED__ON, 5,
  LED_OFF, 5,
  LED__ON, 5,
  LED_OFF, 35,
  0, 0
};

const int32_t   LED_3_BLINK[] =
{
  LED__ON, 5,
  LED_OFF, 5,
  LED__ON, 5,
  LED_OFF, 5,
  LED__ON, 5,
  LED_OFF, 35,
  0, 0
};

#else

extern const int32_t   LED_BLINK[];
extern const int32_t   LED_2_BLINK[];
extern const int32_t   LED_3_BLINK[];

#endif



#endif // LED_STATEMACHINE_H



