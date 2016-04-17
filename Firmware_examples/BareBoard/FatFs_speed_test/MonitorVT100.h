#ifndef __MONITORVT100
  #define __MONITORVT100

  #define COLCOUNT 80
  #define ROWCOUNT 24

/* cursor motion */
  #define VT100_CURSOR_DN   "\033D"  //
  #define VT100_CURSOR_DN_L "\033E"
  #define VT100_CURSOR_UP   "\033M"
  #define VT100_CURSOR_HOME "\033[H"
  #define VT100_CURSOR_N_UP "\033[%dA"  /* printf argument: lines */
  #define VT100_CURSOR_N_RT "\033[%dC"  /* printf argument: cols  */
  #define VT100_CURSOR_N_LT "\033[%dD"  /* printf argument: cols  */
  #define VT100_CURSOR_N_DN "\033[%dB"  /* printf argument: lines */
  #define VT100_CURSOR_SET  "\033[%d;%dH" /* printf arguments: row, col */

/* erasing the screen */
  #define VT100_CLR_FM_CRSR "\033[J"
  #define VT100_CLR_TO_CRSR "\033[1J"
  #define VT100_CLR_SCREEN  "\033[2J"

/* erasing current line */
  #define VT100_CLL_FM_CRSR "\033[K"
  #define VT100_CLL_TO_CRSR "\033[1K"
  #define VT100_CLR_LINE    "\033[2K"

/* inserting and deleting */
  #define VT100_INS_CHARS   "\033[%d"   /* printf argument: cols */
  #define VT100_DEL_CHARS   "\033[%dP"  /* printf argument: cols */
  #define VT100_INS_LINES   "\033[%dL"  /* printf argument: cols */
  #define VT100_DEL_LINES   "\033[%dM"  /* printf argument: cols */

/* character attributes */
  #define VT100_NORMAL      "\033[m"
  #define VT100_ALL_OFF     "\033[0m"
  #define VT100_BOLD_ON     "\033[1m"
  #define VT100_BOLD_OFF    "\033[22m"
  #define VT100_UNDERL_ON   "\033[4m"
  #define VT100_UNDERL_OFF  "\033[24m"
  #define VT100_BLINK_ON    "\033[5m"
  #define VT100_BLINK_OFF   "\033[25m"
  #define VT100_REVERSE_ON  "\033[7m"
  #define VT100_REVERSE_OFF "\033[27m"
  #define VT100_INVIS_ON    "\033[8m"
  #define VT100_INVIS_OFF   "\033[28m"

/* screen attributes */
  #define VT100_ECHO_ON     "\033[12l"
  #define VT100_ECHO_OFF    "\033[12h"
  #define VT100_WRAP_ON     "\033[?7l"
  #define VT100_WRAP_OFF    "\033[?7h"
  #define VT100_CURSOR_ON   "\033[?25h"
  #define VT100_CURSOR_OFF  "\033[?25l"
  #define VT100_ENQ_SIZE    "\033[?92l" /* response: "\033[?%d,%dc" rows, cols */

  #define VT100_CLEAR_AND_HOME "\033[2J\033[H\033[m\033[?25l"


  #define DASH_LINE "----------------------------------------------------------------------"
  #define SCR_ITEMS_VERT_OFFS 8
  #define SCR_ITEMS_HOR_OFFS  1




typedef struct
{
  uint8_t               curr_pos;                      // Текущая позиция в области редактирования
  uint8_t               beg_pos;                       // Начальная позиция области редактирования

  volatile int        par_indx;                      // Индекс редактируемого параметра
  uint8_t               firstrow;                      // Позиция первой  строки области редактирования переменной
  uint8_t               rowcount;                      // Количество строк в области редактирования
  uint8_t               current_row;
  uint8_t               current_col;
  uint8_t               current_pos;
  uint8_t               ed_enter_cnt;                  // Счетчик нажатий Enter при редактировании, требуется нажать 2-а раза. Обход проблемы Telnet клиентов



} T_monitor_cbl;


void         VT100_clr_screen(void);
void         VT100_send_str_to_pos(uint8_t *str, uint8_t row, uint8_t col);
uint8_t      Find_str_center(uint8_t *str);
void         VT100_send_str_to_pos(uint8_t *str, uint8_t row, uint8_t col);

void         VT100_set_cursor_pos(uint8_t row, uint8_t col);

void         Set_monitor_func(void (*func)(unsigned char));
void         Goto_main_menu(void);
void         Return_to_prev_menu(void);
void         Execute_menu_func(uint8_t b);
void         Display_menu(void);

int32_t      Mon_input_line(char *buf, int buf_len, int row, char *instr);
void         Edit_integer_val(uint32_t row, uint32_t *value, uint32_t minv, uint32_t maxv);
void         Edit_float_val(uint32_t row, float *value, float minv, float maxv);

void         Do_FatFS_test(void);
#endif
