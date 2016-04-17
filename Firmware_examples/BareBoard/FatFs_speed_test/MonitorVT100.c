#include "app.h"

#define CNTLQ      0x11
#define CNTLS      0x13
#define DEL        0x7F
#define BACKSPACE  0x08
//#define CR         0x0D  конфликт имен с регистрами периферии
#define LF         0x0A
#define ESC        0x1B

#define COL        80   /* Maximum column size       */

#define EDSTLEN    20





/*------------------------------------------------------------------------------


 ------------------------------------------------------------------------------*/
void Send_buf(uint8_t *str, uint32_t sz)
{
  uint32_t i;

  for (i = 0; i < sz;i++)
  {
    PUTCHAR(str[i]);
  }
}
/*----------------------------------------------------------------------------
 *      Line Editor
 *---------------------------------------------------------------------------*/
int Get_string(char *lp, int n)
{
  int     cnt = 0;
  int8_t  c;

  do
  {
    c = GETCHAR();
    if (c >= 0)
    {
      switch (c)
      {
      case CNTLQ:                          /* ignore Control S/Q             */
      case CNTLS:
        break;

      case BACKSPACE:
      case DEL:
        if (cnt == 0)
        {
          break;
        }
        cnt--;                             /* decrement count                */
        lp--;                              /* and line VOID*               */
        /* echo backspace                 */
        PRINTF("\x008 \x008");
        break;
      case ESC:
        *lp = 0;                           /* ESC - stop editing line        */
        return (-1);
      default:
        PRINTF("*");
        *lp = c;                           /* echo and store character       */
        lp++;                              /* increment line VOID*         */
        cnt++;                             /* and count                      */
        break;
      }
    }
  }
  while (cnt < n - 1 && c != 0x0d);      /* check limit and line feed      */
  *lp = 0;                                 /* mark end of string             */
  return (0);
}


/*-------------------------------------------------------------------------------------------------------------
  Очистка экрана монитора
-------------------------------------------------------------------------------------------------------------*/
void VT100_clr_screen(void)
{
  PRINTF(VT100_CLEAR_AND_HOME);
}

/*-------------------------------------------------------------------------------------------------------------
     Установка курсора в заданную позицию
-------------------------------------------------------------------------------------------------------------*/
void VT100_set_cursor_pos(uint8_t row, uint8_t col)
{
  PRINTF("\033[%.2d;%.2dH", row, col);
}

/*-------------------------------------------------------------------------------------------------------------
     Вывод строки в заданную позицию
-------------------------------------------------------------------------------------------------------------*/
void VT100_send_str_to_pos(uint8_t *str, uint8_t row, uint8_t col)
{
  PRINTF("\033[%.2d;%.2dH", row, col);
  Send_buf(str, strlen((char *)str));
}

/*-------------------------------------------------------------------------------------------------------------
    Находим позицию начала строки для расположения ее по центру экрана
-------------------------------------------------------------------------------------------------------------*/
uint8_t Find_str_center(uint8_t *str)
{
  int16_t l = 0;
  while (*(str + l) != 0) l++; // Находим длину строки
  return (COLCOUNT - l) / 2;
}



/*-------------------------------------------------------------------------------------------------------------
  Прием строки
-------------------------------------------------------------------------------------------------------------*/
int32_t Mon_input_line(char *buf, int buf_len, int row, char *instr)
{

  int     i;
  int8_t  b;
  int     res;
  uint8_t bs_seq[] = { 0x08, 0x020, 0x08, 0 };

  i = 0;
  VT100_set_cursor_pos(row, 0);
  PRINTF(VT100_CLL_FM_CRSR);
  PRINTF(">");

  if (instr != 0)
  {
    i = strlen(instr);
    if (i < buf_len)
    {
      PRINTF(instr);
    }
    else i = 0;
  }

  do
  {
    b = GETCHAR();
    if (b < 0)
    {
      res = -1;
      goto exit_;
    };

    if (b == 0x08)
    {
      if (i > 0)
      {
        i--;
        Send_buf(bs_seq, sizeof(bs_seq));
      }
    }
    else if (b != 0x0D && b != 0x0A && b != 0)
    {
      Send_buf((uint8_t*)&b, 1);
      buf[i] = b;           /* String[i] value set to alpha */
      i++;
      if (i >= buf_len)
      {
        res = -1;
        goto exit_;
      };
    }
  }
  while ((b != 0x0D) && (i < COL));

  res = 0;
  buf[i] = 0;                     /* End of string set to NUL */
exit_:

  VT100_set_cursor_pos(row, 0);
  PRINTF(VT100_CLL_FM_CRSR);

  return (res);
}


/*-----------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------*/
void Edit_integer_val(uint32_t row, uint32_t *value, uint32_t minv, uint32_t maxv)
{
  char   str[32];
  char   buf[32];
  uint32_t tmpv;
  sprintf(str, "%d", *value);
  if (Mon_input_line(buf, 31, row, str) == 0)
  {
    if (sscanf(buf, "%d", &tmpv) == 1)
    {
      if (tmpv > maxv) tmpv = maxv;
      if (tmpv < minv) tmpv = minv;
      *value = tmpv;
    }
  }
}
/*-----------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------*/
void Edit_float_val(uint32_t row, float *value, float minv, float maxv)
{
  char   str[32];
  char   buf[32];
  float tmpv;
  sprintf(str, "%f", *value);
  if (Mon_input_line(buf, 31, row, str) == 0)
  {
    if (sscanf(buf, "%f", &tmpv) == 1)
    {
      if (tmpv > maxv) tmpv = maxv;
      if (tmpv < minv) tmpv = minv;
      *value = tmpv;
    }
  }
}

/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
uint8_t BCD2ToBYTE(uint8_t value)
{
  uint32_t tmp;
  tmp = ((value & 0xF0) >> 4) * 10;
  return(uint8_t)(tmp + (value & 0x0F));
}


/*-----------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------*/
static unsigned int  Print_MFS_test_header(T_fs_test *p)
{
  PRINTF(VT100_CLEAR_AND_HOME);
  PRINTF(" ===  FatFS System Test ===\n\r");
  PRINTF("Press 'A'- test 1, 'B'- test 2, 'D'- test 3\n\r");
  PRINTF("'Q' - existing files read test\n\r");
  PRINTF("----------------------------------------------------------------------\n\r");
  PRINTF("<F>ormat = %d, <E>rase = %d, Read <I>terat. = %d, Files <C>nt= %d, File s<Z>. = %d\n\r", p->en_format, p->en_erase, p->read_cicles, p->files_cnt, p->file_sz);
  PRINTF("----------------------------------------------------------------------\n\r");
  return 6;
}

/*-----------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------*/
void Do_FatFS_test(void)
{
  uint8_t             b;
  uint8_t             row;



  T_fs_test cbl;

  cbl.en_erase = 0;
  cbl.en_format = 0;
  cbl.file_sz = 8192;
  cbl.files_cnt = 100;
  cbl.read_cicles = 1;
  row = Print_MFS_test_header(&cbl);

//  cbl.info = 0;
//  cbl.write_test = 1;
//  FS_test2(&cbl);
//  PRINTF("\n\r\n\r");


  do
  {
    b = GETCHAR();
    if (b>0)
    {
      switch (b)
      {
      case 'F':
      case 'f':
        cbl.en_format ^= 1;
        Print_MFS_test_header(&cbl);
        break;
      case 'E':
      case 'e':
        cbl.en_erase ^= 1;
        Print_MFS_test_header(&cbl);
        break;
      case 'I':
      case 'i':
        Print_MFS_test_header(&cbl);
        Edit_integer_val(row + 1, &cbl.read_cicles, 1, 10000);
        Print_MFS_test_header(&cbl);
        break;
      case 'C':
      case 'c':
        Print_MFS_test_header(&cbl);
        Edit_integer_val(row + 1, &cbl.files_cnt, 1, 1000000000);
        Print_MFS_test_header(&cbl);
        break;
      case 'Z':
      case 'z':
        Print_MFS_test_header(&cbl);
        Edit_integer_val(row + 1, &cbl.file_sz, 1, 50000);
        Print_MFS_test_header(&cbl);
        break;
      case 'A':
      case 'a':
        cbl.info = 0;
        cbl.write_test = 1;
        FS_test1(&cbl);
        PRINTF("\n\r\n\r");
        break;
      case 'B':
      case 'b':
        FS_test2(&cbl);
        PRINTF("\n\r\n\r");
        break;
      case 'D':
      case 'd':
        FS_test3(&cbl);
        PRINTF("\n\r\n\r");
        break;
      case 'Q':
      case 'q':
        cbl.info = 0;
        cbl.write_test = 0;
        FS_test1(&cbl);
        PRINTF("\n\r\n\r");
        break;
      case 'S':
      case 's':
        cbl.info = 1;
        cbl.write_test = 1;
        FS_test1(&cbl);
        PRINTF("\n\r\n\r");
        break;
      default:
        Print_MFS_test_header(&cbl);
        break;

      }
    }
  }
  while (1);

}




