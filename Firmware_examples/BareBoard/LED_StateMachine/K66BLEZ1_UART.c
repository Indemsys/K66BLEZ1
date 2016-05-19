// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2016.01.24
// 21:06:29
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "main.h"


#define COMC_IN_BUF_SZ  64
#define COMC_OUT_BUF_SZ 64


static  uint8_t          uart_rx_buf[COMC_IN_BUF_SZ];
static  uint32_t         uart_rx_head;
static  uint32_t         uart_rx_tail;


static  uint8_t          uart_tx_buf[COMC_OUT_BUF_SZ];
static  uint32_t         uart_tx_head;
static  uint32_t         uart_tx_tail;

volatile uint8_t g_UART_receved_byte;
volatile uint8_t g_UART_tx_buf_empty;



/*------------------------------------------------------------------------------
 Тактирование UART от частоты шины

 ------------------------------------------------------------------------------*/
void UART4_Init(uint32_t speed)
{
  UART_MemMapPtr  UART  = UART4_BASE_PTR;
  SIM_MemMapPtr   SIM   = SIM_BASE_PTR;
  uint32_t mod;

  SIM->SCGC1 |= BIT(10); // Включаем тактирование UART4

  mod = CPU_BUS_CLK_HZ / (16 * speed);

  UART->BDH = 0
              + LSHIFT(0, 7) // LBKDIE  | LIN Break Detect Interrupt Enable (for LBKDIF)
              + LSHIFT(0, 6) // RXEDGIE | RxD Input Active Edge Interrupt Enable (for RXEDGIF)
              + LSHIFT(0, 5) // SBNS    | Stop Bit Number Select | 0 One stop bit.
              + LSHIFT((mod >> 8) & 0x1F, 0) // SBR     | Baud Rate Modulo Divisor.
  ;
  UART->BDL = mod & 0xFF;

  UART->C4 = ((CPU_BUS_CLK_HZ << 5)/ (16 * speed)) % 32;

  UART->C1 = 0
             + LSHIFT(0, 7) // LOOPS    | Loop Mode Select              | 0 Normal operation - RxD and TxD use separate pins.
             + LSHIFT(0, 6) // UARTSWAI | UART Stops in Wait Mode       | 0 UART clocks continue to run in Wait mode so the UART can be the source of an interrupt that wakes up the CPU.
             + LSHIFT(0, 5) // RSRC     | Receiver Source Select        | 0 Provided LOOPS is set, RSRC is cleared, selects internal loop back mode and the UART does not use the RxD pins.
             + LSHIFT(0, 4) // M        | 9-Bit or 8-Bit Mode Select    | 0 Normal - start + 8 data bits (lsb first) + stop.
             + LSHIFT(0, 3) // WAKE     | Receiver Wakeup Method Select | 0 Idle-line wake-up.
             + LSHIFT(0, 2) // ILT      | Idle Line Type Select         | 0 Idle character bit count starts after start bit.
             + LSHIFT(0, 1) // PE       | Parity Enable                 | 0 No hardware parity generation or checking.
             + LSHIFT(0, 0) // PT       | Parity Type                   | 0 Even parity.
  ;


  UART->S1 = 0 // All bits Read Only
             + LSHIFT(0, 7) // TDRE  | Transmit Data Register Empty Flag | 1 Transmit data register (buffer) empty.
             + LSHIFT(0, 6) // TC    | Transmission Complete Flag | 1 Transmitter idle (transmission activity complete).
             + LSHIFT(0, 5) // RDRF  | Receive Data Register Full Flag | 1 Receive data register full.
             + LSHIFT(0, 4) // IDLE  | Idle Line Flag | 1 Idle line was detected.
             + LSHIFT(0, 3) // OR    | Receiver Overrun Flag | 1 Receive overrun (new UART data lost).
             + LSHIFT(0, 2) // NF    | Noise Flag | 1 Noise detected in the received character in UART_D.
             + LSHIFT(0, 1) // FE    | Framing Error Flag | 1 Framing error
             + LSHIFT(0, 0) // PF    | Parity Error Flag | 1 Parity error.
  ;

  UART->S2 = 0
             + LSHIFT(0, 7) // LBKDIF   | LIN Break Detect Interrupt Flag | 1 LIN break character has been detected.
             + LSHIFT(0, 6) // RXEDGIF  | RxD Pin Active Edge Interrupt Flag | 1 An active edge on the receive pin has occurred.
             + LSHIFT(0, 5) // MSBF     | Most Significant Bit First | Setting this field reverses the order of the bits that are transmitted and received on the wire
             + LSHIFT(0, 4) // RXINV    | Receive Data Inversion | 1 Receive data inverted.
             + LSHIFT(0, 3) // RWUID    | Receive Wake Up Idle Detect | 1 During receive standby state (RWU = 1), S1[IDLE] gets set upon detection of an idle character.
             + LSHIFT(0, 2) // BRK13    | Break Character Generation Length | 1 Break character is transmitted with length of 13 bit times (if M = 0, SBNS = 0) or 14 (if M = 1, SBNS = 0 or M = 0, SBNS = 1) or 15 (if M = 1, SBNS = 1).
             + LSHIFT(0, 1) // LBKDE    | LIN Break Detection Enable | 0 Break detection is disabled.
             + LSHIFT(0, 0) // RAF      | Receiver Active Flag | 1 UART receiver active (RxD input not idle).
  ;

  UART->C3 = 0
             + LSHIFT(0, 7) // R8    | Ninth Data Bit for Receiver
             + LSHIFT(0, 6) // T8    | Ninth Data Bit for Transmitter
             + LSHIFT(0, 5) // TXDIR | TxD Pin Direction in Single-Wire Mode
             + LSHIFT(0, 4) // TXINV | Transmit Data Inversion | 1 Transmit data inverted.
             + LSHIFT(0, 3) // ORIE  | Overrun Interrupt Enable
             + LSHIFT(0, 2) // NEIE  | Noise Error Interrupt Enable
             + LSHIFT(0, 1) // FEIE  | Framing Error Interrupt Enable
             + LSHIFT(0, 0) // PEIE  | Parity Error Interrupt Enable
  ;

  UART->C2 = 0
             + LSHIFT(0, 7) // TIE  | Transmit Interrupt Enable for TDRE            | 1 Hardware interrupt requested when TDRE flag is 1.
             + LSHIFT(0, 6) // TCIE | Transmission Complete Interrupt Enable for TC | 1 Hardware interrupt requested when TC flag is 1.
             + LSHIFT(1, 5) // RIE  | Receiver Interrupt Enable for RDRF            | 1 Hardware interrupt requested when S1[RDRF] flag is 1.
             + LSHIFT(0, 4) // ILIE | Idle Line Interrupt Enable for IDLE           | 1 Hardware interrupt requested when S1[IDLE] flag is 1.
             + LSHIFT(1, 3) // TE   | Transmitter Enable                            | 1 Transmitter on.
             + LSHIFT(1, 2) // RE   | Receiver Enable                               | 1 Receiver on.
             + LSHIFT(0, 1) // RWU  | Receiver Wakeup Control                       | 1 UART receiver in standby waiting for wake-up condition.
             + LSHIFT(0, 0) // SBK  | Send Break                                    | 1 Queue break character(s) to be sent.
  ;

  NVIC_SetPriority(INT_UART4_RX_TX, 1); //
  NVIC_EnableIRQ(INT_UART4_RX_TX);

}

/*------------------------------------------------------------------------------


 \param _isrParameter
 ------------------------------------------------------------------------------*/
void UART4_RX_TX_IRQHandler(void)
{
  UART_MemMapPtr  UART  = UART4_BASE_PTR;
  unsigned char stat;
  unsigned int  tmp;
  // Определяем источник прерывания
  stat = UART->S1;

  if (stat & BIT(5))
  {
    // Получили байт, сохраняем в кольцевом буфере
    tmp = uart_rx_head;
    uart_rx_buf[uart_rx_head] = UART->D;
    uart_rx_head++;
    if (uart_rx_head >= COMC_IN_BUF_SZ) uart_rx_head = 0;
    if (uart_rx_head == uart_rx_tail) uart_rx_head = tmp; // Если происходит переполнение то не сдвигать указатель буфера
    g_UART_receved_byte = 1;
  }

  if ((stat & BIT(7)) && (UART->C2 & BIT(7)))
  {
    UART->D =  uart_tx_buf[uart_tx_tail];
    uart_tx_tail++;
    if (uart_tx_tail >= COMC_OUT_BUF_SZ) uart_tx_tail = 0;
    if (uart_tx_head == uart_tx_tail)
    {
      UART->C2 &= ~BIT(7); // Запретим прерывания от передатчика если нечего передавать
      g_UART_tx_buf_empty = 1;
    }
  }
}

/*------------------------------------------------------------------------------
  Ожидаем прихода символа заданное колическтво тиков - ticks


 \return int  Возвращает -1 если символа не дождались
 ------------------------------------------------------------------------------*/
int32_t UART_wait_sym(uint32_t ticks)
{
  int32_t b;
  if (uart_rx_head != uart_rx_tail)
  {
    b = uart_rx_buf[uart_rx_tail];
    uart_rx_tail++;
    if (uart_rx_tail >= COMC_IN_BUF_SZ) uart_rx_tail = 0;
    return b;
  }
  else
  {
    while (ticks != 0)
    {
      if (uart_rx_head != uart_rx_tail)
      {
        b = uart_rx_buf[uart_rx_tail];
        uart_rx_tail++;
        if (uart_rx_tail >= COMC_IN_BUF_SZ) uart_rx_tail = 0;
        return b;
      }
      ticks--;
    }

  }
  return -1;
}

/*------------------------------------------------------------------------------
 Проверить не появилось ли входящих символов

 \return int  возвращает -1 если символов нет и 1 если есть символы
 ------------------------------------------------------------------------------*/
int32_t UART_check_sym(void)
{
  if (uart_rx_head == uart_rx_tail) return -1;
  return 1;
}
/*------------------------------------------------------------------------------
  Послать буфер в коммуникационный канал
  Строка отправляется в кольцевой буффер
  Управление в процедуре остается пока все данные не будут переписаны в кольцевой буфер
 ------------------------------------------------------------------------------*/
int UART4_send_buffer(uint8_t *buf, int32_t len)
{
  UART_MemMapPtr  UART  = UART4_BASE_PTR;

  do
  {
    if ((uart_tx_head == uart_tx_tail) && (UART->C2 & BIT(7)))
    {
      // Буфер передачи весь заполнен и идет передача, ожидать освобождения буфера
      while (g_UART_tx_buf_empty == 0);
    }
    g_UART_tx_buf_empty = 0;

    uart_tx_buf[uart_tx_head] = *buf++;
    uart_tx_head++;
    if (uart_tx_head >= COMC_OUT_BUF_SZ) uart_tx_head = 0;
    len--;
    UART->C2 |= BIT(7); // Разрешим прерывания от передатчика
  }
  while (len > 0);

  return 0;
}



