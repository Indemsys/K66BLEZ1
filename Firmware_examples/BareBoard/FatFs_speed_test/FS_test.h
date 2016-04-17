#ifndef __FS_TEST
  #define __FS_TEST

typedef struct
{
  unsigned char en_format;
  unsigned char en_erase;
  unsigned int  read_cicles; // Количество дополнительных повторов чтения файла
  unsigned int  files_cnt;
  unsigned int  file_sz;
  unsigned char write_test;
  unsigned char info;

} T_fs_test;

int32_t  Prepare_FatFS(void);

int   FS_test1(T_fs_test *cbl);
int   FS_test2(T_fs_test *cbl);
int   FS_test3(T_fs_test *cbl);



  #define  DELAY_1us    Delay_m7(25)           // 1.011     мкс при частоте 180 МГц
  #define  DELAY_4us    Delay_m7(102)          // 4.005     мкс при частоте 180 МГц
  #define  DELAY_8us    Delay_m7(205)          // 8.011     мкс при частоте 180 МГц
  #define  DELAY_32us   Delay_m7(822)          // 32.005    мкс при частоте 180 МГц
  #define  DELAY_ms(x)  Delay_m7(25714*x-1)    // 999.95*N  мкс при частоте 180 МГц

extern void Delay_m7(int cnt); // Задержка на (cnt+1)*7 тактов . Передача нуля недопускается


#endif
