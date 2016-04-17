#include "app.h"

#define DISK_NAME "2:/"
#define STRINGS_SZ 64


typedef struct
{
  int    fd;
  uint32_t open_t;
  uint32_t rw_t;
  uint32_t close_t;
} T_file_stat_rec;

static  FATFS          fs; /* File system object */
static  FIL            f;   // File object
static  FRESULT        error;
static  char           *fname_tmpl;
static  char           *filename;
static  unsigned char  *fbuf;
static  unsigned char  *fbuf2;
static  char           *str;


static uint32_t        t1, t2;
static uint32_t        res;

uint32_t tcal;




/*------------------------------------------------------------------------------


 ------------------------------------------------------------------------------*/
int32_t Prepare_FatFS(void)
{

  if (f_mount(&fs, DISK_NAME, 0U))
  {
    PRINTF("Mount volume failed.\r\n");
    return -1;
  }

#if (_FS_RPATH >= 2U)
  error = f_chdrive(DISK_NAME);
  if (error)
  {
    PRINTF("Change drive failed.\r\n");
    return -1;
  }
#endif

#if _USE_MKFS
  PRINTF("\r\nMake file system......The time may be long if the card capacity is big.\r\n");
  if (f_mkfs(DISK_NAME, 1U, 0U))
  {
    PRINTF("Make file system failed.\r\n");
    return -1;
  }
#endif /* _USE_MKFS */
  return 0;
}
/*-------------------------------------------------------------------------------------------------------------
 Последовательно записываем количество файлов заданное в cbl->files_cnt  размером заданным в cbl->file_sz
 Потом последовательно проверяем их содержимое
 Потом последовательно стираем
-------------------------------------------------------------------------------------------------------------*/
int   FS_test1(T_fs_test *cbl)
{
  T_file_stat_rec     *ftm;

  int                  err_files = 0;
  int                  err_cnt;
  int                  i,  num;
  uint8_t              *bptr;

  uint32_t max_open_t  = 0;
  uint32_t min_open_t  = 0xFFFFFFFF;
  uint32_t max_rw_t    = 0;
  uint32_t min_rw_t    = 0xFFFFFFFF;
  uint32_t max_close_t = 0;
  uint32_t min_close_t = 0xFFFFFFFF;

  float avr_open_t = 0;
  float avr_rw_t = 0;
  float avr_close_t = 0;


  // Выделяем память для массивов
  fbuf = malloc(cbl->file_sz);
  if (fbuf == NULL)
  {
    PRINTF("\r\nUnnable allocate memory for file first buffer!\r\n");
    goto exit_;
  }
  fbuf2 = malloc(cbl->file_sz);
  if (fbuf2 == NULL)
  {
    PRINTF("\r\nUnnable allocate memory for file second buffer!\r\n");
    goto exit_;
  }

  ftm = (T_file_stat_rec *)malloc(cbl->files_cnt * sizeof(T_file_stat_rec));
  if (ftm == NULL)
  {
    PRINTF("\r\nUnnable allocate memory for array of statistic!\r\n");
    goto exit_;
  }

  str = (char *)malloc(STRINGS_SZ * 3);
  if (ftm == NULL)
  {
    PRINTF("\r\nUnnable allocate memory for strings!\r\n");
    goto exit_;
  }
  fname_tmpl = &str[64];
  filename   = &str[128];


  strcpy(fname_tmpl, "testf");

  PRINTF("\r\n--------- FatFS File System ---------\r\n");


  if (cbl->write_test == 1)
  {

    // Создаем и записываем количество файлов заданное переменной filecount
    PRINTF("-------------------- Writing -----------------\n\r");
    for (num = 0; num < cbl->files_cnt; num++)
    {


      sprintf(filename, "%s%d.BIN", fname_tmpl, num);


      Get_time_counters(&t1);
      error = f_open(&f, filename, (FA_WRITE + FA_CREATE_ALWAYS));
      Get_time_counters(&t2);
      ftm[num].open_t = Time_diff_microseconds(t2, t1);

      if (error)
      {
        PRINTF("File %s open to write error\n\r", filename);
        continue;
      }

      srand(num);
      bptr = (uint8_t *)fbuf;
      for (i = 0; i < cbl->file_sz; i++)
      {
        *bptr = rand();
        bptr++;
      }

      Get_time_counters(&t1);
      error = f_write(&f, (void *)fbuf, cbl->file_sz, &res);
      Get_time_counters(&t2);
      ftm[num].rw_t = Time_diff_microseconds(t2, t1);

      Get_time_counters(&t1);
      f_close(&f);
      Get_time_counters(&t2);
      ftm[num].close_t = Time_diff_microseconds(t2, t1);

      if ((res != cbl->file_sz) || error)
      {
        PRINTF("%s Write error. Size = %06d, writen = %d\n\r", filename, cbl->file_sz, res);
      }
      else
      {
        PRINTF("%s %06d %08d %08d %08d %.0f Kb/s\n\r", filename, cbl->file_sz, ftm[num].open_t, ftm[num].rw_t, ftm[num].close_t,  cbl->file_sz * 1000000.0 / ((float)ftm[num].rw_t * 1024.0));
      }


      err_cnt = 0;
      // Проверить файл сразу после записи
      error = f_open(&f, filename, (FA_READ));
      if (error)
      {
        PRINTF("File %s open to read error!\n\r", filename);
        continue;
      }

      error = f_read(&f, (void *)fbuf2, cbl->file_sz, &res);
      f_close(&f);
      {
        uint8_t *ptr1 = (uint8_t *)fbuf;
        uint8_t *ptr2 = (uint8_t *)fbuf2;
        for (i = 0; i < cbl->file_sz; i++)
        {
          if ((*ptr1) != (*ptr2))
          {
            PRINTF("File %s has data error at offset %08X (read -> %02X ,must be -> %02X)\n\r", filename, i, *ptr2, *ptr1);
            err_cnt++;
            if (err_cnt > 32) break; //goto exit_;
          }
          ptr1++;
          ptr2++;
        }
      }


    }

    // Подсчет статистики
    for (num = 0; num < cbl->files_cnt; num++)
    {

      if (ftm[num].open_t > max_open_t) max_open_t = ftm[num].open_t;
      if (ftm[num].open_t < min_open_t) min_open_t = ftm[num].open_t;
      if (ftm[num].rw_t > max_rw_t) max_rw_t = ftm[num].rw_t;
      if (ftm[num].rw_t < min_rw_t) min_rw_t = ftm[num].rw_t;
      if (ftm[num].close_t > max_close_t) max_close_t = ftm[num].close_t;
      if (ftm[num].close_t < min_close_t) min_close_t = ftm[num].close_t;

      avr_open_t  += ftm[num].open_t;
      avr_rw_t    += ftm[num].rw_t;
      avr_close_t += ftm[num].close_t;
    }

    PRINTF("Statistic for write operations.\n\r");
    PRINTF("Open max = %08d us Open min = %08d us, Open aver = %.0f us\n\r", max_open_t, min_open_t,  (float)avr_open_t / (float)cbl->files_cnt);
    PRINTF("Writ max = %08d us Writ min = %08d us, Writ aver = %.0f us\n\r", max_rw_t, min_rw_t,  (float)avr_rw_t / (float)cbl->files_cnt);
    PRINTF("Clos max = %08d us Clos min = %08d us, Clos aver = %.0f us\n\r", max_close_t, min_close_t,  (float)avr_close_t / (float)cbl->files_cnt);
    {
      float t = (float)avr_rw_t / (float)cbl->files_cnt;
      PRINTF("Averaged write speed  = %.0f Kbyte/s\n\r", cbl->file_sz * 1000000.0 / (t * 1024.0));
    }

  }



  // Читаем количество файлов заданное переменной filecount
  PRINTF("-------------------- Reading -----------------\n\r");
  for (num = 0; num < cbl->files_cnt; num++)
  {
    int add_cycle;

    err_cnt = 0;

//      if (num>100)
//      {
    sprintf(filename, "%s%d.BIN", fname_tmpl, num);

    // Организуем дополнительный цикл перепроверки чтения файла в случае появления ошибок
    for (add_cycle = 0; add_cycle < cbl->read_cicles; add_cycle++)
    {


      Get_time_counters(&t1);
      error = f_open(&f, filename, (FA_READ));
      Get_time_counters(&t2);
      ftm[num].open_t = Time_diff_microseconds(t2, t1);

      if (error)
      {
        PRINTF("File %s open to read error!\n\r", filename);
        break;
      }

      Get_time_counters(&t1);
      error = f_read(&f, (void *)fbuf, cbl->file_sz, &res);
      Get_time_counters(&t2);
      ftm[num].rw_t = Time_diff_microseconds(t2, t1);

      srand(num);
      bptr = (uint8_t *)fbuf;
      for (i = 0; i < cbl->file_sz; i++)
      {
        unsigned char b;
        b = rand();
        if ((*bptr) != b)
        {
          PRINTF("File %s has data error at offset %08X (read -> %02X ,must be -> %02X)\n\r", filename, i, *bptr, b);
          err_cnt++;
          if (err_cnt > 32) break; //goto exit_;
        }
        bptr++;
      }

      if (err_cnt > 0)
      {
        err_files++;
      }


      Get_time_counters(&t1);
      f_close(&f);
      Get_time_counters(&t2);
      ftm[num].close_t = Time_diff_microseconds(t2, t1);

      PRINTF("%s %06d %08d %08d %08d %.0f Kb/s\n\r", filename, cbl->file_sz, ftm[num].open_t, ftm[num].rw_t, ftm[num].close_t,  cbl->file_sz * 1000000.0 / ((float)ftm[num].rw_t * 1024.0));
    }
//      }

  }

  max_open_t  = 0;
  min_open_t  = 0xFFFFFFFF;
  max_rw_t    = 0;
  min_rw_t    = 0xFFFFFFFF;
  max_close_t = 0;
  min_close_t = 0xFFFFFFFF;

  avr_open_t = 0;
  avr_rw_t = 0;
  avr_close_t = 0;

  // Подсчет статистики
  for (num = 0; num < cbl->files_cnt; num++)
  {

    if (ftm[num].open_t > max_open_t) max_open_t = ftm[num].open_t;
    if (ftm[num].open_t < min_open_t) min_open_t = ftm[num].open_t;
    if (ftm[num].rw_t > max_rw_t) max_rw_t = ftm[num].rw_t;
    if (ftm[num].rw_t < min_rw_t) min_rw_t = ftm[num].rw_t;
    if (ftm[num].close_t > max_close_t) max_close_t = ftm[num].close_t;
    if (ftm[num].close_t < min_close_t) min_close_t = ftm[num].close_t;

    avr_open_t  += ftm[num].open_t;
    avr_rw_t    += ftm[num].rw_t;
    avr_close_t += ftm[num].close_t;
  }

  PRINTF("Statistic for read operations.\n\r");
  PRINTF("Open max = %08d us Open min = %08d us, Open aver = %.0f us\n\r", max_open_t, min_open_t,  (float)avr_open_t / (float)cbl->files_cnt);
  PRINTF("Read max = %08d us Read min = %08d us, Read aver = %.0f us\n\r", max_rw_t, min_rw_t,  (float)avr_rw_t / (float)cbl->files_cnt);
  PRINTF("Clos max = %08d us Clos min = %08d us, Clos aver = %.0f us\n\r", max_close_t, min_close_t,  (float)avr_close_t / (float)cbl->files_cnt);
  {
    float t = (float)avr_rw_t / (float)cbl->files_cnt;
    PRINTF("Averaged read speed  = %.0f Kbyte/s\n\r", cbl->file_sz * 1000000.0 / (t * 1024.0));
  }


  if (cbl->en_erase)
  {

    // Удаляем количество файлов заданное переменной filecount
    PRINTF("-------------------- Deleting -----------------\n\r");

    for (num = 0; num < cbl->files_cnt; num++)
    {
      sprintf(filename, "%s%d.BIN", fname_tmpl, num);

      Get_time_counters(&t1);
      f_unlink(filename);
      Get_time_counters(&t2);
      ftm[num].open_t = Time_diff_microseconds(t2, t1);

      PRINTF("%s %06d %08d us\n\r", filename, cbl->file_sz, ftm[num].open_t);

    }

    max_open_t  = 0;
    min_open_t  = 0xFFFFFFFF;

    avr_open_t = 0;

    // Подсчет статистики
    for (num = 0; num < cbl->files_cnt; num++)
    {

      if (ftm[num].open_t > max_open_t) max_open_t = ftm[num].open_t;
      if (ftm[num].open_t < min_open_t) min_open_t = ftm[num].open_t;

      avr_open_t  += ftm[num].open_t;
    }

    PRINTF("Statistic for delete operations.\n\r");
    PRINTF("Delet. max = %08d us Delet. min = %08d us, Delet. aver = %.0f us\n\r", max_open_t, min_open_t,  (float)avr_open_t / (float)cbl->files_cnt);
  }

  PRINTF("\n\rNumber of the damaged files = %d.\n\r", err_files);

exit_:
  if (fbuf)  free(fbuf);
  if (fbuf2) free(fbuf2);
  if (ftm)   free(ftm);
  if (str)   free(str);
  return 0;
}


/*------------------------------------------------------------------------------
  Записываем файл, проверяем ешо содержание, стираем

 \param cbl

 \return int
 ------------------------------------------------------------------------------*/
int   FS_test2(T_fs_test *cbl)
{
  int                  i;
  int                  num;

  unsigned int         open_w_t;  // Время открытия файла на запись
  unsigned int         write_t;   // Время записи файла
  unsigned int         close_w_t; // Время закрытия файла на запись

  unsigned int         open_r_t;  // Время открытия файла на запись
  unsigned int         read_t;    // Время записи файла
  unsigned int         close_r_t; // Время закрытия файла на запись

  unsigned int         delete_t;  // Время удаления файла



  // Выделяем память для массивов
  fbuf = malloc(cbl->file_sz);
  if (fbuf == NULL)
  {
    PRINTF("\r\nUnnable allocate memory for file first buffer!\r\n");
    goto exit_;
  }
  fbuf2 = malloc(cbl->file_sz);
  if (fbuf2 == NULL)
  {
    PRINTF("\r\nUnnable allocate memory for file second buffer!\r\n");
    goto exit_;
  }

  str = (char *)malloc(STRINGS_SZ * 3);
  if (str == NULL)
  {
    PRINTF("\r\nUnnable allocate memory for strings!\r\n");
    goto exit_;
  }
  fname_tmpl = &str[64];
  filename   = &str[128];

  strcpy(fname_tmpl, "F_");

  PRINTF("\r\n--------- FatFS File System test 2 ---------\r\n");


  for (num = 0; num < cbl->files_cnt; num++)
  {

    srand(num);

    // Создаем имя файла
    sprintf(filename, "%07d.BIN", num);



    // Создаем файл
    Get_time_counters(&t1);
    error = f_open(&f, filename, (FA_WRITE + FA_CREATE_ALWAYS));
    Get_time_counters(&t2);
    open_w_t = Time_diff_microseconds(t2, t1);

    if (error)
    {
      PRINTF("\r\nFile opening for write error!\r\n");
      goto exit_;
    }
    // Заполняем буфер случайными числами
    for (i = 0; i < cbl->file_sz; i++)
    {
      fbuf[i] = rand();
    }

    // Записываем файл
    Get_time_counters(&t1);
    error = f_write(&f, (void *)fbuf, cbl->file_sz, &res);
    Get_time_counters(&t2);
    write_t = Time_diff_microseconds(t2, t1);
    if ((res != cbl->file_sz) || error)
    {
      PRINTF("\r\nWrite size error (%d)!\r\n", res);
      goto exit_;
    }

    // Закрываем файл
    Get_time_counters(&t1);
    f_close(&f);
    Get_time_counters(&t2);
    close_w_t = Time_diff_microseconds(t2, t1);

    // Открываем файл на чтение
    Get_time_counters(&t1);
    error = f_open(&f, filename, (FA_READ));
    Get_time_counters(&t2);
    open_r_t = Time_diff_microseconds(t2, t1);

    if (error)
    {
      PRINTF("\r\nFile opening for read error!\r\n");
      goto exit_;
    }


    // Читаем файл
    Get_time_counters(&t1);
    error = f_read(&f, (void *)fbuf2, cbl->file_sz, &res);
    Get_time_counters(&t2);
    read_t = Time_diff_microseconds(t2, t1);

    if ((res != cbl->file_sz) || error)
    {
      PRINTF("\r\nRead size error (%d)!\r\n", res);
      goto exit_;
    }

    // Закрываем файл
    Get_time_counters(&t1);
    f_close(&f);
    Get_time_counters(&t2);
    close_r_t = Time_diff_microseconds(t2, t1);


    Get_time_counters(&t1);
    error = f_unlink(filename);
    Get_time_counters(&t2);
    delete_t = Time_diff_microseconds(t2, t1);

    if (error)
    {
      PRINTF("\r\nDeleting  error (%d)!\r\n", res);
      goto exit_;
    }

    // Проверяем прочитанные данные
    for (i = 0; i < cbl->file_sz; i++)
    {
      if (fbuf[i] != fbuf2[i])
      {
        PRINTF("\r\nFile error %02X = %02X at %d\r\n", fbuf[i], fbuf2[i], i);
        goto exit_;
      }
    }
    PRINTF("%s o:%07d w:%07d c:%07d o:%07d r:%07d c:%07d d:%07d\r\n",
           filename,
           open_w_t,
           write_t,
           close_w_t,
           open_r_t,
           read_t,
           close_r_t,
           delete_t
          );

  }

exit_:
  if (fbuf)  free(fbuf);
  if (fbuf2) free(fbuf2);
  if (str)   free(str);
  return 0;
}

/*------------------------------------------------------------------------------
 Открываем файл и записываем туда непрерывно блоки
 количество блоков задано в cbl->files_cnt
 размер блоков задан cbl->file_sz
 Повторяем для второго файла

 \param cbl

 \return int
 ------------------------------------------------------------------------------*/
int   FS_test3(T_fs_test *cbl)
{
  int                  i,k;
  int                  num;

  unsigned int         write_t;   // Время записи файла


  // Выделяем память для массивов
  fbuf = malloc(cbl->file_sz);
  if (fbuf == NULL)
  {
    PRINTF("\r\nUnnable allocate memory for file first buffer!\r\n");
    goto exit_;
  }

  str = (char *)malloc(STRINGS_SZ * 3);
  if (str == NULL)
  {
    PRINTF("\r\nUnnable allocate memory for strings!\r\n");
    goto exit_;
  }
  fname_tmpl = &str[64];
  filename   = &str[128];

  strcpy(fname_tmpl, "F_");

  PRINTF("\r\n--------- FatFS File System test 3 ---------\r\n");

  for (num = 0; num < 2; num++)
  {
    // Создаем имя файла
    sprintf(filename, "%s%07d.BIN", fname_tmpl, num);

    error = f_open(&f, filename, (FA_WRITE + FA_CREATE_ALWAYS));
    if (error)
    {
      PRINTF("\r\nFile %s opening for write error!\r\n", filename);
      goto exit_;
    }
    else
    {
      PRINTF("\r\nStarted writing to file  %s.\r\n", filename);
    }

    srand(0);

    for (i = 0; i < cbl->files_cnt; i++)
    {
      // Заполняем буфер случайными числами
      for (k = 0; k < cbl->file_sz; k++)
      {
        fbuf[k] = rand();
      }

      // Записываем файл
      Get_time_counters(&t1);
      error = f_write(&f, (void *)fbuf, cbl->file_sz, &res);
      Get_time_counters(&t2);
      write_t = Time_diff_microseconds(t2, t1);
      if ((res != cbl->file_sz) || error)
      {
        PRINTF("\r\nWrite size error (%d)!\r\n", res);
        goto exit_;
      }
      PRINTF("%07d %07d us\r\n", i, write_t);

    }
    f_close(&f);

  }

exit_:
  if (fbuf)  free(fbuf);
  if (str)   free(str);
  return 0;

}

/*------------------------------------------------------------------------------



 \return int
 ------------------------------------------------------------------------------*/
int main(void)
{
  uint32_t i;

  //BOARD_InitPins();
  //BOARD_BootClockRUN();
  Init_pins();
  BOARD_BootClockHSRUN();
  BOARD_InitDebugConsole();
  MPU_Enable(MPU, false);

  LED_OFF;

  for (i = 0; i < 10;i++)
  {
    LED_ON;
    DELAY_ms(1);
    LED_OFF;
    DELAY_ms(1);
  }

  TimeManInit();
  tcal = Get_calibrating_time(  );
  PRINTF("FatFS spped test.\r\n");
  PRINTF("-------------------------------------------------------------------\r\n");
  PRINTF("Time measurement calibrating result = %d\r\n", tcal);
  PRINTF("-------------------------------------------------------------------\r\n");


  if (Prepare_FatFS() != 0)
  {
    PRINTF("FatFS initialisation error!\r\n");
    return 0;
  }
  Do_FatFS_test();


}

