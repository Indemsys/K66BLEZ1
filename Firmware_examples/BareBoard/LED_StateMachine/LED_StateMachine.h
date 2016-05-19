#ifndef LED_STATEMACHINE_H
  #define LED_STATEMACHINE_H


// ”правл€юща€ структура машины состо€ний управл€емой шаблоном
typedef struct
{
  uint32_t  init_state;
  uint32_t  counter;
  int32_t  *pattern_start_ptr;  // ”казатель на массив констант €вл€ющийс€ цепочкой состо€ний (шаблоном)
                                // ≈сли значение в массиве = 0xFFFFFFFF, то процесс обработки завершаетс€
                                // ≈сли значение в массиве = 0x00000000, то вернуть указатель на начало цепочки
  int32_t   *pttn_ptr;          // “екуща€ позици€ в цепочке состо€ний

} T_LED_ptrn;




void Set_LED_pattern(const int32_t *pttn, uint32_t n);
void LEDS_state_automat(void);


#define LED__ON  0
#define LED_OFF  1

#ifdef _GLOBAL_

//  Ўаблон состоит из массива груп слов.
//  ѕервое слово в группе - значение напр€жени€
//  ¬торое слово в группе - длительность интервала времени в дес€тках мс или специальный маркер остановки(0xFFFFFFFF) или цикла(0x00000000)

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



