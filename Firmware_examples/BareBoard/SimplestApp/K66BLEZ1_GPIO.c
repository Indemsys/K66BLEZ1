#include "main.h"

typedef struct
{
  GPIO_MemMapPtr gpio;
  PORT_MemMapPtr port;
  unsigned char  pin_num;
  unsigned char  irqc; //  Interrupt Configuration
                       //  0000 Interrupt/DMA Request disabled.
                       //  0001 DMA Request on rising edge.
                       //  0010 DMA Request on falling edge.
                       //  0011 DMA Request on either edge.
                       //  0100 Reserved.
                       //  1000 Interrupt when logic zero.
                       //  1001 Interrupt on rising edge.
                       //  1010 Interrupt on falling edge.
                       //  1011 Interrupt on either edge.
                       //  1100 Interrupt when logic one.
  unsigned char  lock; //  if 1 Pin Control Register bits [15:0] are locked and cannot be updated until the next System Reset.
  unsigned char  mux;  //  Pin Mux Control
                       //  000 Pin Disabled (Analog).
                       //  001 Alternative 1 (GPIO).
                       //  010 Alternative 2 (chip specific).
                       //  011 Alternative 3 (chip specific).
                       //  100 Alternative 4 (chip specific).
                       //  101 Alternative 5 (chip specific).
                       //  110 Alternative 6 (chip specific).
                       //  111 Alternative 7 (chip specific / JTAG / NMI).
  unsigned char  DSE; // 0 Low drive strength is configured on the corresponding pin, if pin is configured as a digital output.
                      // 1 High drive strength is configured on the corresponding pin, if pin is configured as a digital output.
  unsigned char  SRE;  // 0 Fast slew rate is configured on the corresponding pin, if pin is configured as a digital output.
                       // 1 Slow slew rate is configured on the corresponding pin, if pin is configured as a digital output.
  unsigned char  ODE;  // 0 Open Drain output is disabled on the corresponding pin.
                       // 1 Open Drain output is enabled on the corresponding pin, provided pin is configured as a digital output.
  unsigned char  PFE;  // 0 Passive Input Filter is disabled on the corresponding pin.
                       // 1 Passive Input Filter is enabled on the corresponding pin.
  unsigned char  PUPD; // 00 Internal pull-up or pull-down resistor is not enabled on the corresponding pin.
                       // 10 Internal pull-down resistor is enabled on the corresponding pin, if the corresponding Port Pull Enable Register bit is set.
                       // 11 Internal pull-up resistor is enabled on the corresponding pin, if the corresponding Port Pull Enable Register bit is set.
  unsigned char  dir;  // 0 Pin is configured as general purpose input, if configured for the GPIO function
                       // 1 Pin is configured for general purpose output, if configured for the GPIO function
  unsigned char  init; // Init state

} T_IO_pins_configuration;

#define   ANAL          0  // Pin Disabled (Analog).
#define   ALT0          0  // Pin Disabled (Analog).
#define   GPIO          1  // Alternative 1 (GPIO).
#define   ALT1          1  // Alternative 1 (GPIO).
#define   ALT2          2  //
#define   ALT3          3  //
#define   ALT4          4  //
#define   ALT5          5  //
#define   ALT6          6  //
#define   ALT7          7  //

#define   DSE_LO        0 // 0 Low drive strength is configured on the corresponding pin, if pin is configured as a digital output.
#define   DSE_HI        1 // 1 High drive strength is configured on the corresponding pin, if pin is configured as a digital output.

#define   OD_DIS        0 // 0 Open Drain output is disabled on the corresponding pin.
#define   OD__EN        1 // 1 Open Drain output is enabled on the corresponding pin, provided pin is configured as a digital output.

#define   PFE_DIS       0 // 0 Passive Input Filter is disabled on the corresponding pin.
#define   PFE__EN       1 // 1 Passive Input Filter is enabled on the corresponding pin.

#define   FAST_SLEW     0 // 0 Fast slew rate is configured on the corresponding pin, if pin is configured as a digital output.
#define   SLOW_SLEW     1 // 1 Slow slew rate is configured on the corresponding pin, if pin is configured as a digital output.


#define   PUPD_DIS      0 // 00 Internal pull-up or pull-down resistor is not enabled on the corresponding pin.
#define   PULL__DN      2 // 10 Internal pull-down resistor is enabled on the corresponding pin, if the corresponding Port Pull Enable Register bit is set.
#define   PULL__UP      3 // 11 Internal pull-up resistor is enabled on the corresponding pin, if the corresponding Port Pull Enable Register bit is set.

#define   GP_INP        0 // 0 Pin is configured as general purpose input, if configured for the GPIO function
#define   GP_OUT        1 // 1 Pin is configured for general purpose output, if configured for the GPIO function

void Config_pin(const T_IO_pins_configuration pinc);

// ��������� ����� �����/������ ����� K66BLEZ1 �� ���� ���������������� MK66FN2M0VLQ18

const T_IO_pins_configuration K66BLEZ1_pins_conf[] =
{
//  gpio          port            num  irqc  lock  mux   DSE     SRE        ODE     PFE      PUPD      dir     init
  { PTA_BASE_PTR, PORTA_BASE_PTR,   0,   0,   0,   ALT7, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // JTCLK/SWC            50 # PTA0 # Default=(JTAG_TCLK/SWD_CLK/EZP_CLK)  ALT0=(TSI0_CH1)  ALT1=(PTA0)  ALT2=(UART0_CTS_b/UART0_COL_b)  ALT3=(FTM0_CH5)  ALT4=()  ALT5=(LPUART0_CTS_b)  ALT6=()  ALT7=(JTAG_TCLK/SWD_CLK)  EZPort=(EZP_CLK)
  { PTA_BASE_PTR, PORTA_BASE_PTR,   1,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_OUT,   1 }, // JTDI (LED)           51 # PTA1 # Default=(JTAG_TDI/EZP_DI)  ALT0=(TSI0_CH2)  ALT1=(PTA1)  ALT2=(UART0_RX)  ALT3=(FTM0_CH6)  ALT4=(I2C3_SDA)  ALT5=(LPUART0_RX)  ALT6=()  ALT7=(JTAG_TDI)  EZPort=(EZP_DI)
  { PTA_BASE_PTR, PORTA_BASE_PTR,   2,   0,   0,   ALT7, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // JTDO/SWO             52 # PTA2 # Default=(JTAG_TDO/TRACE_SWO/EZP_DO)  ALT0=(TSI0_CH3)  ALT1=(PTA2)  ALT2=(UART0_TX)  ALT3=(FTM0_CH7)  ALT4=(I2C3_SCL)  ALT5=(LPUART0_TX)  ALT6=()  ALT7=(JTAG_TDO/TRACE_SWO)  EZPort=(EZP_DO)
  { PTA_BASE_PTR, PORTA_BASE_PTR,   3,   0,   0,   ALT7, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // JTMS/SWD             53 # PTA3 # Default=(JTAG_TMS/SWD_DIO)  ALT0=(TSI0_CH4)  ALT1=(PTA3)  ALT2=(UART0_RTS_b)  ALT3=(FTM0_CH0)  ALT4=()  ALT5=(LPUART0_RTS_b)  ALT6=()  ALT7=(JTAG_TMS/SWD_DIO)  EZPort=()
  { PTA_BASE_PTR, PORTA_BASE_PTR,   4,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      54 # PTA4/LLWU_P3 # Default=(NMI_b/EZP_CS_b)  ALT0=(TSI0_CH5)  ALT1=(PTA4/LLWU_P3)  ALT2=()  ALT3=(FTM0_CH1)  ALT4=()  ALT5=()  ALT6=()  ALT7=(NMI_b)  EZPort=(EZP_CS_b)
  { PTA_BASE_PTR, PORTA_BASE_PTR,   5,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      55 # PTA5 # Default=(DISABLED)  ALT0=()  ALT1=(PTA5)  ALT2=(USB0_CLKIN)  ALT3=(FTM0_CH2)  ALT4=(RMII0_RXER/MII0_RXER)  ALT5=(CMP2_OUT)  ALT6=(I2S0_TX_BCLK)  ALT7=(JTAG_TRST_b)  EZPort=()
  { PTA_BASE_PTR, PORTA_BASE_PTR,   6,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      58 # PTA6 # Default=(DISABLED)  ALT0=()  ALT1=(PTA6)  ALT2=()  ALT3=(FTM0_CH3)  ALT4=()  ALT5=(CLKOUT)  ALT6=()  ALT7=(TRACE_CLKOUT)  EZPort=()
  { PTA_BASE_PTR, PORTA_BASE_PTR,   7,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      59 # PTA7 # Default=(ADC0_SE10)  ALT0=(ADC0_SE10)  ALT1=(PTA7)  ALT2=()  ALT3=(FTM0_CH4)  ALT4=()  ALT5=(RMII0_MDIO/MII0_MDIO)  ALT6=()  ALT7=(TRACE_D3)  EZPort=()
  { PTA_BASE_PTR, PORTA_BASE_PTR,   8,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      60 # PTA8 # Default=(ADC0_SE11)  ALT0=(ADC0_SE11)  ALT1=(PTA8)  ALT2=()  ALT3=(FTM1_CH0)  ALT4=()  ALT5=(RMII0_MDC/MII0_MDC)  ALT6=(FTM1_QD_PHA/TPM1_CH0)  ALT7=(TRACE_D2)  EZPort=()
  { PTA_BASE_PTR, PORTA_BASE_PTR,   9,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      61 # PTA9 # Default=(DISABLED)  ALT0=()  ALT1=(PTA9)  ALT2=()  ALT3=(FTM1_CH1)  ALT4=(MII0_RXD3)  ALT5=()  ALT6=(FTM1_QD_PHB/TPM1_CH1)  ALT7=(TRACE_D1)  EZPort=()
  { PTA_BASE_PTR, PORTA_BASE_PTR,  10,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      62 # PTA10/LLWU_P22 # Default=(DISABLED)  ALT0=()  ALT1=(PTA10/LLWU_P22)  ALT2=()  ALT3=(FTM2_CH0)  ALT4=(MII0_RXD2)  ALT5=()  ALT6=(FTM2_QD_PHA/TPM2_CH0)  ALT7=(TRACE_D0)  EZPort=()
  { PTA_BASE_PTR, PORTA_BASE_PTR,  11,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      63 # PTA11/LLWU_P23 # Default=(DISABLED)  ALT0=()  ALT1=(PTA11/LLWU_P23)  ALT2=()  ALT3=(FTM2_CH1)  ALT4=(MII0_RXCLK)  ALT5=(I2C2_SDA)  ALT6=(FTM2_QD_PHB/TPM2_CH1)  ALT7=()  EZPort=()
  { PTA_BASE_PTR, PORTA_BASE_PTR,  12,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      64 # PTA12 # Default=(CMP2_IN0)  ALT0=(CMP2_IN0)  ALT1=(PTA12)  ALT2=(CAN0_TX)  ALT3=(FTM1_CH0)  ALT4=(RMII0_RXD1/MII0_RXD1)  ALT5=(I2C2_SCL)  ALT6=(I2S0_TXD0)  ALT7=(FTM1_QD_PHA/TPM1_CH0)  EZPort=()
  { PTA_BASE_PTR, PORTA_BASE_PTR,  13,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      65 # PTA13/LLWU_P4 # Default=(CMP2_IN1)  ALT0=(CMP2_IN1)  ALT1=(PTA13/LLWU_P4)  ALT2=(CAN0_RX)  ALT3=(FTM1_CH1)  ALT4=(RMII0_RXD0/MII0_RXD0)  ALT5=(I2C2_SDA)  ALT6=(I2S0_TX_FS)  ALT7=(FTM1_QD_PHB/TPM1_CH1)  EZPort=()
  { PTA_BASE_PTR, PORTA_BASE_PTR,  14,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      66 # PTA14 # Default=(DISABLED)  ALT0=()  ALT1=(PTA14)  ALT2=(SPI0_PCS0)  ALT3=(UART0_TX)  ALT4=(RMII0_CRS_DV/MII0_RXDV)  ALT5=(I2C2_SCL)  ALT6=(I2S0_RX_BCLK)  ALT7=(I2S0_TXD1)  EZPort=()
  { PTA_BASE_PTR, PORTA_BASE_PTR,  15,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      67 # PTA15 # Default=(CMP3_IN1)  ALT0=(CMP3_IN1)  ALT1=(PTA15)  ALT2=(SPI0_SCK)  ALT3=(UART0_RX)  ALT4=(RMII0_TXEN/MII0_TXEN)  ALT5=()  ALT6=(I2S0_RXD0)  ALT7=()  EZPort=()
  { PTA_BASE_PTR, PORTA_BASE_PTR,  16,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      68 # PTA16 # Default=(CMP3_IN2)  ALT0=(CMP3_IN2)  ALT1=(PTA16)  ALT2=(SPI0_SOUT)  ALT3=(UART0_CTS_b/UART0_COL_b)  ALT4=(RMII0_TXD0/MII0_TXD0)  ALT5=()  ALT6=(I2S0_RX_FS)  ALT7=(I2S0_RXD1)  EZPort=()
  { PTA_BASE_PTR, PORTA_BASE_PTR,  17,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      69 # PTA17 # Default=(ADC1_SE17)  ALT0=(ADC1_SE17)  ALT1=(PTA17)  ALT2=(SPI0_SIN)  ALT3=(UART0_RTS_b)  ALT4=(RMII0_TXD1/MII0_TXD1)  ALT5=()  ALT6=(I2S0_MCLK)  ALT7=()  EZPort=()
  { PTA_BASE_PTR, PORTA_BASE_PTR,  18,   0,   0,   ALT0, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // EXTAL                72 # PTA18 # Default=(EXTAL0)  ALT0=(EXTAL0)  ALT1=(PTA18)  ALT2=()  ALT3=(FTM0_FLT2)  ALT4=(FTM_CLKIN0)  ALT5=()  ALT6=()  ALT7=(TPM_CLKIN0)  EZPort=()
  { PTA_BASE_PTR, PORTA_BASE_PTR,  19,   0,   0,   ALT0, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // XTAL                 73 # PTA19 # Default=(XTAL0)  ALT0=(XTAL0)  ALT1=(PTA19)  ALT2=()  ALT3=(FTM1_FLT0)  ALT4=(FTM_CLKIN1)  ALT5=()  ALT6=(LPTMR0_ALT1)  ALT7=(TPM_CLKIN1)  EZPort=()
  { PTA_BASE_PTR, PORTA_BASE_PTR,  24,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      75 # PTA24 # Default=(CMP3_IN4)  ALT0=(CMP3_IN4)  ALT1=(PTA24)  ALT2=()  ALT3=()  ALT4=(MII0_TXD2)  ALT5=()  ALT6=(FB_A29)  ALT7=()  EZPort=()
  { PTA_BASE_PTR, PORTA_BASE_PTR,  25,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      76 # PTA25 # Default=(CMP3_IN5)  ALT0=(CMP3_IN5)  ALT1=(PTA25)  ALT2=()  ALT3=()  ALT4=(MII0_TXCLK)  ALT5=()  ALT6=(FB_A28)  ALT7=()  EZPort=()
  { PTA_BASE_PTR, PORTA_BASE_PTR,  26,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      77 # PTA26 # Default=(DISABLED)  ALT0=()  ALT1=(PTA26)  ALT2=()  ALT3=()  ALT4=(MII0_TXD3)  ALT5=()  ALT6=(FB_A27)  ALT7=()  EZPort=()
  { PTA_BASE_PTR, PORTA_BASE_PTR,  27,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      78 # PTA27 # Default=(DISABLED)  ALT0=()  ALT1=(PTA27)  ALT2=()  ALT3=()  ALT4=(MII0_CRS)  ALT5=()  ALT6=(FB_A26)  ALT7=()  EZPort=()
  { PTA_BASE_PTR, PORTA_BASE_PTR,  28,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      79 # PTA28 # Default=(DISABLED)  ALT0=()  ALT1=(PTA28)  ALT2=()  ALT3=()  ALT4=(MII0_TXER)  ALT5=()  ALT6=(FB_A25)  ALT7=()  EZPort=()
  { PTA_BASE_PTR, PORTA_BASE_PTR,  29,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      80 # PTA29 # Default=(DISABLED)  ALT0=()  ALT1=(PTA29)  ALT2=()  ALT3=()  ALT4=(MII0_COL)  ALT5=()  ALT6=(FB_A24)  ALT7=()  EZPort=()

  { PTB_BASE_PTR, PORTB_BASE_PTR,   0,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      81 # PTB0/LLWU_P5 # Default=(ADC0_SE8/ADC1_SE8/TSI0_CH0)  ALT0=(ADC0_SE8/ADC1_SE8/TSI0_CH0)  ALT1=(PTB0/LLWU_P5)  ALT2=(I2C0_SCL)  ALT3=(FTM1_CH0)  ALT4=(RMII0_MDIO/MII0_MDIO)  ALT5=(SDRAM_CAS_b)  ALT6=(FTM1_QD_PHA/TPM1_CH0)  ALT7=()  EZPort=()
  { PTB_BASE_PTR, PORTB_BASE_PTR,   1,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      82 # PTB1 # Default=(ADC0_SE9/ADC1_SE9/TSI0_CH6)  ALT0=(ADC0_SE9/ADC1_SE9/TSI0_CH6)  ALT1=(PTB1)  ALT2=(I2C0_SDA)  ALT3=(FTM1_CH1)  ALT4=(RMII0_MDC/MII0_MDC)  ALT5=(SDRAM_RAS_b)  ALT6=(FTM1_QD_PHB/TPM1_CH1)  ALT7=()  EZPort=()
  { PTB_BASE_PTR, PORTB_BASE_PTR,   2,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      83 # PTB2 # Default=(ADC0_SE12/TSI0_CH7)  ALT0=(ADC0_SE12/TSI0_CH7)  ALT1=(PTB2)  ALT2=(I2C0_SCL)  ALT3=(UART0_RTS_b)  ALT4=(ENET0_1588_TMR0)  ALT5=(SDRAM_WE)  ALT6=(FTM0_FLT3)  ALT7=()  EZPort=()
  { PTB_BASE_PTR, PORTB_BASE_PTR,   3,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      84 # PTB3 # Default=(ADC0_SE13/TSI0_CH8)  ALT0=(ADC0_SE13/TSI0_CH8)  ALT1=(PTB3)  ALT2=(I2C0_SDA)  ALT3=(UART0_CTS_b/UART0_COL_b)  ALT4=(ENET0_1588_TMR1)  ALT5=(SDRAM_CS0_b)  ALT6=(FTM0_FLT0)  ALT7=()  EZPort=()
  { PTB_BASE_PTR, PORTB_BASE_PTR,   4,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      85 # PTB4 # Default=(ADC1_SE10)  ALT0=(ADC1_SE10)  ALT1=(PTB4)  ALT2=()  ALT3=()  ALT4=(ENET0_1588_TMR2)  ALT5=(SDRAM_CS1_b)  ALT6=(FTM1_FLT0)  ALT7=()  EZPort=()
  { PTB_BASE_PTR, PORTB_BASE_PTR,   5,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      86 # PTB5 # Default=(ADC1_SE11)  ALT0=(ADC1_SE11)  ALT1=(PTB5)  ALT2=()  ALT3=()  ALT4=(ENET0_1588_TMR3)  ALT5=()  ALT6=(FTM2_FLT0)  ALT7=()  EZPort=()
  { PTB_BASE_PTR, PORTB_BASE_PTR,   6,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      87 # PTB6 # Default=(ADC1_SE12)  ALT0=(ADC1_SE12)  ALT1=(PTB6)  ALT2=()  ALT3=()  ALT4=()  ALT5=(FB_AD23/SDRAM_D23)  ALT6=()  ALT7=()  EZPort=()
  { PTB_BASE_PTR, PORTB_BASE_PTR,   7,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // INT from LDC1000     88 # PTB7 # Default=(ADC1_SE13)  ALT0=(ADC1_SE13)  ALT1=(PTB7)  ALT2=()  ALT3=()  ALT4=()  ALT5=(FB_AD22/SDRAM_D22)  ALT6=()  ALT7=()  EZPort=()
  { PTB_BASE_PTR, PORTB_BASE_PTR,   8,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      89 # PTB8 # Default=(DISABLED)  ALT0=()  ALT1=(PTB8)  ALT2=()  ALT3=(UART3_RTS_b)  ALT4=()  ALT5=(FB_AD21/SDRAM_D21)  ALT6=()  ALT7=()  EZPort=()
  { PTB_BASE_PTR, PORTB_BASE_PTR,   9,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_OUT,   1 }, // SPI1_PCS1            90 # PTB9 # Default=(DISABLED)  ALT0=()  ALT1=(PTB9)  ALT2=(SPI1_PCS1)  ALT3=(UART3_CTS_b)  ALT4=()  ALT5=(FB_AD20/SDRAM_D20)  ALT6=()  ALT7=()  EZPort=()
  { PTB_BASE_PTR, PORTB_BASE_PTR,  10,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      91 # PTB10 # Default=(ADC1_SE14)  ALT0=(ADC1_SE14)  ALT1=(PTB10)  ALT2=(SPI1_PCS0)  ALT3=(UART3_RX)  ALT4=()  ALT5=(FB_AD19/SDRAM_D19)  ALT6=(FTM0_FLT1)  ALT7=()  EZPort=()
  { PTB_BASE_PTR, PORTB_BASE_PTR,  11,   0,   0,   ALT2, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // SPI1_SCK             92 # PTB11 # Default=(ADC1_SE15)  ALT0=(ADC1_SE15)  ALT1=(PTB11)  ALT2=(SPI1_SCK)  ALT3=(UART3_TX)  ALT4=()  ALT5=(FB_AD18/SDRAM_D18)  ALT6=(FTM0_FLT2)  ALT7=()  EZPort=()
  { PTB_BASE_PTR, PORTB_BASE_PTR,  16,   0,   0,   ALT2, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // SPI1_SOUT            95 # PTB16 # Default=(TSI0_CH9)  ALT0=(TSI0_CH9)  ALT1=(PTB16)  ALT2=(SPI1_SOUT)  ALT3=(UART0_RX)  ALT4=(FTM_CLKIN0)  ALT5=(FB_AD17/SDRAM_D17)  ALT6=(EWM_IN)  ALT7=(TPM_CLKIN0)  EZPort=()
  { PTB_BASE_PTR, PORTB_BASE_PTR,  17,   0,   0,   ALT2, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // SPI1_SIN             96 # PTB17 # Default=(TSI0_CH10)  ALT0=(TSI0_CH10)  ALT1=(PTB17)  ALT2=(SPI1_SIN)  ALT3=(UART0_TX)  ALT4=(FTM_CLKIN1)  ALT5=(FB_AD16/SDRAM_D16)  ALT6=(EWM_OUT_b)  ALT7=(TPM_CLKIN1)  EZPort=()
  { PTB_BASE_PTR, PORTB_BASE_PTR,  18,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      97 # PTB18 # Default=(TSI0_CH11)  ALT0=(TSI0_CH11)  ALT1=(PTB18)  ALT2=(CAN0_TX)  ALT3=(FTM2_CH0)  ALT4=(I2S0_TX_BCLK)  ALT5=(FB_AD15/SDRAM_A23)  ALT6=(FTM2_QD_PHA/TPM2_CH0)  ALT7=()  EZPort=()
  { PTB_BASE_PTR, PORTB_BASE_PTR,  19,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      98 # PTB19 # Default=(TSI0_CH12)  ALT0=(TSI0_CH12)  ALT1=(PTB19)  ALT2=(CAN0_RX)  ALT3=(FTM2_CH1)  ALT4=(I2S0_TX_FS)  ALT5=(FB_OE_b)  ALT6=(FTM2_QD_PHB/TPM2_CH1)  ALT7=()  EZPort=()
  { PTB_BASE_PTR, PORTB_BASE_PTR,  20,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      99 # PTB20 # Default=(DISABLED)  ALT0=()  ALT1=(PTB20)  ALT2=(SPI2_PCS0)  ALT3=()  ALT4=()  ALT5=(FB_AD31/SDRAM_D31)  ALT6=(CMP0_OUT)  ALT7=()  EZPort=()
  { PTB_BASE_PTR, PORTB_BASE_PTR,  21,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      100 # PTB21 # Default=(DISABLED)  ALT0=()  ALT1=(PTB21)  ALT2=(SPI2_SCK)  ALT3=()  ALT4=()  ALT5=(FB_AD30/SDRAM_D30)  ALT6=(CMP1_OUT)  ALT7=()  EZPort=()
  { PTB_BASE_PTR, PORTB_BASE_PTR,  22,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      101 # PTB22 # Default=(DISABLED)  ALT0=()  ALT1=(PTB22)  ALT2=(SPI2_SOUT)  ALT3=()  ALT4=()  ALT5=(FB_AD29/SDRAM_D29)  ALT6=(CMP2_OUT)  ALT7=()  EZPort=()
  { PTB_BASE_PTR, PORTB_BASE_PTR,  23,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      102 # PTB23 # Default=(DISABLED)  ALT0=()  ALT1=(PTB23)  ALT2=(SPI2_SIN)  ALT3=(SPI0_PCS5)  ALT4=()  ALT5=(FB_AD28/SDRAM_D28)  ALT6=(CMP3_OUT)  ALT7=()  EZPort=()

  { PTC_BASE_PTR, PORTC_BASE_PTR,   0,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      103 # PTC0 # Default=(ADC0_SE14/TSI0_CH13)  ALT0=(ADC0_SE14/TSI0_CH13)  ALT1=(PTC0)  ALT2=(SPI0_PCS4)  ALT3=(PDB0_EXTRG)  ALT4=(USB0_SOF_OUT)  ALT5=(FB_AD14/SDRAM_A22)  ALT6=(I2S0_TXD1)  ALT7=()  EZPort=()
  { PTC_BASE_PTR, PORTC_BASE_PTR,   1,   0,   0,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // FTM0_CH0(Solenoid)   104 # PTC1/LLWU_P6 # Default=(ADC0_SE15/TSI0_CH14)  ALT0=(ADC0_SE15/TSI0_CH14)  ALT1=(PTC1/LLWU_P6)  ALT2=(SPI0_PCS3)  ALT3=(UART1_RTS_b)  ALT4=(FTM0_CH0)  ALT5=(FB_AD13/SDRAM_A21)  ALT6=(I2S0_TXD0)  ALT7=()  EZPort=()
  { PTC_BASE_PTR, PORTC_BASE_PTR,   2,   0,   0,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // FTM0_CH1(LDC1000 clk)105 # PTC2 # Default=(ADC0_SE4b/CMP1_IN0/TSI0_CH15)  ALT0=(ADC0_SE4b/CMP1_IN0/TSI0_CH15)  ALT1=(PTC2)  ALT2=(SPI0_PCS2)  ALT3=(UART1_CTS_b)  ALT4=(FTM0_CH1)  ALT5=(FB_AD12/SDRAM_A20)  ALT6=(I2S0_TX_FS)  ALT7=()  EZPort=()
  { PTC_BASE_PTR, PORTC_BASE_PTR,   3,   0,   0,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      106 # PTC3/LLWU_P7 # Default=(CMP1_IN1)  ALT0=(CMP1_IN1)  ALT1=(PTC3/LLWU_P7)  ALT2=(SPI0_PCS1)  ALT3=(UART1_RX)  ALT4=(FTM0_CH2)  ALT5=(CLKOUT)  ALT6=(I2S0_TX_BCLK)  ALT7=()  EZPort=()
  { PTC_BASE_PTR, PORTC_BASE_PTR,   4,   0,   0,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      109 # PTC4/LLWU_P8 # Default=(DISABLED)  ALT0=()  ALT1=(PTC4/LLWU_P8)  ALT2=(SPI0_PCS0)  ALT3=(UART1_TX)  ALT4=(FTM0_CH3)  ALT5=(FB_AD11/SDRAM_A19)  ALT6=(CMP1_OUT)  ALT7=()  EZPort=()
  { PTC_BASE_PTR, PORTC_BASE_PTR,   5,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      110 # PTC5/LLWU_P9 # Default=(DISABLED)  ALT0=()  ALT1=(PTC5/LLWU_P9)  ALT2=(SPI0_SCK)  ALT3=(LPTMR0_ALT2)  ALT4=(I2S0_RXD0)  ALT5=(FB_AD10/SDRAM_A18)  ALT6=(CMP0_OUT)  ALT7=(FTM0_CH2)  EZPort=()
  { PTC_BASE_PTR, PORTC_BASE_PTR,   6,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      111 # PTC6/LLWU_P10 # Default=(CMP0_IN0)  ALT0=(CMP0_IN0)  ALT1=(PTC6/LLWU_P10)  ALT2=(SPI0_SOUT)  ALT3=(PDB0_EXTRG)  ALT4=(I2S0_RX_BCLK)  ALT5=(FB_AD9/SDRAM_A17)  ALT6=(I2S0_MCLK)  ALT7=()  EZPort=()
  { PTC_BASE_PTR, PORTC_BASE_PTR,   7,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      112 # PTC7 # Default=(CMP0_IN1)  ALT0=(CMP0_IN1)  ALT1=(PTC7)  ALT2=(SPI0_SIN)  ALT3=(USB0_SOF_OUT)  ALT4=(I2S0_RX_FS)  ALT5=(FB_AD8/SDRAM_A16)  ALT6=()  ALT7=()  EZPort=()
  { PTC_BASE_PTR, PORTC_BASE_PTR,   8,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      113 # PTC8 # Default=(ADC1_SE4b/CMP0_IN2)  ALT0=(ADC1_SE4b/CMP0_IN2)  ALT1=(PTC8)  ALT2=()  ALT3=(FTM3_CH4)  ALT4=(I2S0_MCLK)  ALT5=(FB_AD7/SDRAM_A15)  ALT6=()  ALT7=()  EZPort=()
  { PTC_BASE_PTR, PORTC_BASE_PTR,   9,   0,   0,   ALT3, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      114 # PTC9 # Default=(ADC1_SE5b/CMP0_IN3)  ALT0=(ADC1_SE5b/CMP0_IN3)  ALT1=(PTC9)  ALT2=()  ALT3=(FTM3_CH5)  ALT4=(I2S0_RX_BCLK)  ALT5=(FB_AD6/SDRAM_A14)  ALT6=(FTM2_FLT0)  ALT7=()  EZPort=()
  { PTC_BASE_PTR, PORTC_BASE_PTR,  10,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      115 # PTC10 # Default=(ADC1_SE6b)  ALT0=(ADC1_SE6b)  ALT1=(PTC10)  ALT2=(I2C1_SCL)  ALT3=(FTM3_CH6)  ALT4=(I2S0_RX_FS)  ALT5=(FB_AD5/SDRAM_A13)  ALT6=()  ALT7=()  EZPort=()
  { PTC_BASE_PTR, PORTC_BASE_PTR,  11,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      116 # PTC11/LLWU_P11 # Default=(ADC1_SE7b)  ALT0=(ADC1_SE7b)  ALT1=(PTC11/LLWU_P11)  ALT2=(I2C1_SDA)  ALT3=(FTM3_CH7)  ALT4=(I2S0_RXD1)  ALT5=(FB_RW_b)  ALT6=()  ALT7=()  EZPort=()
  { PTC_BASE_PTR, PORTC_BASE_PTR,  12,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      117 # PTC12 # Default=(DISABLED)  ALT0=()  ALT1=(PTC12)  ALT2=()  ALT3=(UART4_RTS_b)  ALT4=(FTM_CLKIN0)  ALT5=(FB_AD27/SDRAM_D27)  ALT6=(FTM3_FLT0)  ALT7=(TPM_CLKIN0)  EZPort=()
  { PTC_BASE_PTR, PORTC_BASE_PTR,  13,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      118 # PTC13 # Default=(DISABLED)  ALT0=()  ALT1=(PTC13)  ALT2=()  ALT3=(UART4_CTS_b)  ALT4=(FTM_CLKIN1)  ALT5=(FB_AD26/SDRAM_D26)  ALT6=()  ALT7=(TPM_CLKIN1)  EZPort=()
  { PTC_BASE_PTR, PORTC_BASE_PTR,  14,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      119 # PTC14 # Default=(DISABLED)  ALT0=()  ALT1=(PTC14)  ALT2=()  ALT3=(UART4_RX)  ALT4=()  ALT5=(FB_AD25/SDRAM_D25)  ALT6=()  ALT7=()  EZPort=()
  { PTC_BASE_PTR, PORTC_BASE_PTR,  15,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      120 # PTC15 # Default=(DISABLED)  ALT0=()  ALT1=(PTC15)  ALT2=()  ALT3=(UART4_TX)  ALT4=()  ALT5=(FB_AD24/SDRAM_D24)  ALT6=()  ALT7=()  EZPort=()
  { PTC_BASE_PTR, PORTC_BASE_PTR,  16,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      123 # PTC16 # Default=(DISABLED)  ALT0=()  ALT1=(PTC16)  ALT2=(CAN1_RX)  ALT3=(UART3_RX)  ALT4=(ENET0_1588_TMR0)  ALT5=(FB_CS5_b/FB_TSIZ1/FB_BE23_16_BLS15_8_b/SDRAM_DQM2)  ALT6=()  ALT7=()  EZPort=()
  { PTC_BASE_PTR, PORTC_BASE_PTR,  17,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      124 # PTC17 # Default=(DISABLED)  ALT0=()  ALT1=(PTC17)  ALT2=(CAN1_TX)  ALT3=(UART3_TX)  ALT4=(ENET0_1588_TMR1)  ALT5=(FB_CS4_b/FB_TSIZ0/FB_BE31_24_BLS7_0_b/SDRAM_DQM3)  ALT6=()  ALT7=()  EZPort=()
  { PTC_BASE_PTR, PORTC_BASE_PTR,  18,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      125 # PTC18 # Default=(DISABLED)  ALT0=()  ALT1=(PTC18)  ALT2=()  ALT3=(UART3_RTS_b)  ALT4=(ENET0_1588_TMR2)  ALT5=(FB_TBST_b/FB_CS2_b/FB_BE15_8_BLS23_16_b/SDRAM_DQM1)  ALT6=()  ALT7=()  EZPort=()
  { PTC_BASE_PTR, PORTC_BASE_PTR,  19,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      126 # PTC19 # Default=(DISABLED)  ALT0=()  ALT1=(PTC19)  ALT2=()  ALT3=(UART3_CTS_b)  ALT4=(ENET0_1588_TMR3)  ALT5=(FB_CS3_b/FB_BE7_0_BLS31_24_b/SDRAM_DQM0)  ALT6=(FB_TA_b)  ALT7=()  EZPort=()

  { PTD_BASE_PTR, PORTD_BASE_PTR,   0,   0,   0,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      127 # PTD0/LLWU_P12 # Default=(DISABLED)  ALT0=()  ALT1=(PTD0/LLWU_P12)  ALT2=(SPI0_PCS0)  ALT3=(UART2_RTS_b)  ALT4=(FTM3_CH0)  ALT5=(FB_ALE/FB_CS1_b/FB_TS_b)  ALT6=()  ALT7=()  EZPort=()
  { PTD_BASE_PTR, PORTD_BASE_PTR,   1,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      128 # PTD1 # Default=(ADC0_SE5b)  ALT0=(ADC0_SE5b)  ALT1=(PTD1)  ALT2=(SPI0_SCK)  ALT3=(UART2_CTS_b)  ALT4=(FTM3_CH1)  ALT5=(FB_CS0_b)  ALT6=()  ALT7=()  EZPort=()
  { PTD_BASE_PTR, PORTD_BASE_PTR,   2,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      129 # PTD2/LLWU_P13 # Default=(DISABLED)  ALT0=()  ALT1=(PTD2/LLWU_P13)  ALT2=(SPI0_SOUT)  ALT3=(UART2_RX)  ALT4=(FTM3_CH2)  ALT5=(FB_AD4/SDRAM_A12)  ALT6=()  ALT7=(I2C0_SCL)  EZPort=()
  { PTD_BASE_PTR, PORTD_BASE_PTR,   3,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      130 # PTD3 # Default=(DISABLED)  ALT0=()  ALT1=(PTD3)  ALT2=(SPI0_SIN)  ALT3=(UART2_TX)  ALT4=(FTM3_CH3)  ALT5=(FB_AD3/SDRAM_A11)  ALT6=()  ALT7=(I2C0_SDA)  EZPort=()
  { PTD_BASE_PTR, PORTD_BASE_PTR,   4,   0,   0,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      131 # PTD4/LLWU_P14 # Default=(DISABLED)  ALT0=()  ALT1=(PTD4/LLWU_P14)  ALT2=(SPI0_PCS1)  ALT3=(UART0_RTS_b)  ALT4=(FTM0_CH4)  ALT5=(FB_AD2/SDRAM_A10)  ALT6=(EWM_IN)  ALT7=(SPI1_PCS0)  EZPort=()
  { PTD_BASE_PTR, PORTD_BASE_PTR,   5,   0,   0,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      132 # PTD5 # Default=(ADC0_SE6b)  ALT0=(ADC0_SE6b)  ALT1=(PTD5)  ALT2=(SPI0_PCS2)  ALT3=(UART0_CTS_b/UART0_COL_b)  ALT4=(FTM0_CH5)  ALT5=(FB_AD1/SDRAM_A9)  ALT6=(EWM_OUT_b)  ALT7=(SPI1_SCK)  EZPort=()
  { PTD_BASE_PTR, PORTD_BASE_PTR,   6,   0,   0,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      133 # PTD6/LLWU_P15 # Default=(ADC0_SE7b)  ALT0=(ADC0_SE7b)  ALT1=(PTD6/LLWU_P15)  ALT2=(SPI0_PCS3)  ALT3=(UART0_RX)  ALT4=(FTM0_CH6)  ALT5=(FB_AD0)  ALT6=(FTM0_FLT0)  ALT7=(SPI1_SOUT)  EZPort=()
  { PTD_BASE_PTR, PORTD_BASE_PTR,   7,   0,   0,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      136 # PTD7 # Default=(DISABLED)  ALT0=()  ALT1=(PTD7)  ALT2=(CMT_IRO)  ALT3=(UART0_TX)  ALT4=(FTM0_CH7)  ALT5=(SDRAM_CKE)  ALT6=(FTM0_FLT1)  ALT7=(SPI1_SIN)  EZPort=()
  { PTD_BASE_PTR, PORTD_BASE_PTR,   8,   0,   0,   ALT2, DSE_HI, FAST_SLEW, OD__EN, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // SCL                  137 # PTD8/LLWU_P24 # Default=(DISABLED)  ALT0=()  ALT1=(PTD8/LLWU_P24)  ALT2=(I2C0_SCL)  ALT3=()  ALT4=()  ALT5=(LPUART0_RX)  ALT6=(FB_A16)  ALT7=()  EZPort=()
  { PTD_BASE_PTR, PORTD_BASE_PTR,   9,   0,   0,   ALT2, DSE_HI, FAST_SLEW, OD__EN, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // SDA                  138 # PTD9 # Default=(DISABLED)  ALT0=()  ALT1=(PTD9)  ALT2=(I2C0_SDA)  ALT3=()  ALT4=()  ALT5=(LPUART0_TX)  ALT6=(FB_A17)  ALT7=()  EZPort=()
  { PTD_BASE_PTR, PORTD_BASE_PTR,  10,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_OUT,   0 }, // PSEL                 139 # PTD10 # Default=(DISABLED)  ALT0=()  ALT1=(PTD10)  ALT2=()  ALT3=()  ALT4=()  ALT5=(LPUART0_RTS_b)  ALT6=(FB_A18)  ALT7=()  EZPort=()
  { PTD_BASE_PTR, PORTD_BASE_PTR,  11,   0,   0,   ALT2, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      140 # PTD11/LLWU_P25 # Default=(DISABLED)  ALT0=()  ALT1=(PTD11/LLWU_P25)  ALT2=(SPI2_PCS0)  ALT3=()  ALT4=(SDHC0_CLKIN)  ALT5=(LPUART0_CTS_b)  ALT6=(FB_A19)  ALT7=()  EZPort=()
  { PTD_BASE_PTR, PORTD_BASE_PTR,  12,   0,   0,   ALT2, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // MKW40_SPI1_SCK       141 # PTD12 # Default=(DISABLED)  ALT0=()  ALT1=(PTD12)  ALT2=(SPI2_SCK)  ALT3=(FTM3_FLT0)  ALT4=(SDHC0_D4)  ALT5=()  ALT6=(FB_A20)  ALT7=()  EZPort=()
  { PTD_BASE_PTR, PORTD_BASE_PTR,  13,   0,   0,   ALT2, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // MKW40_SPI1_SIN       142 # PTD13 # Default=(DISABLED)  ALT0=()  ALT1=(PTD13)  ALT2=(SPI2_SOUT)  ALT3=()  ALT4=(SDHC0_D5)  ALT5=()  ALT6=(FB_A21)  ALT7=()  EZPort=()
  { PTD_BASE_PTR, PORTD_BASE_PTR,  14,   0,   0,   ALT2, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // MKW40_SPI1_SOUT      143 # PTD14 # Default=(DISABLED)  ALT0=()  ALT1=(PTD14)  ALT2=(SPI2_SIN)  ALT3=()  ALT4=(SDHC0_D6)  ALT5=()  ALT6=(FB_A22)  ALT7=()  EZPort=()
  { PTD_BASE_PTR, PORTD_BASE_PTR,  15,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // INT                  144 # PTD15 # Default=(DISABLED)  ALT0=()  ALT1=(PTD15)  ALT2=(SPI2_PCS1)  ALT3=()  ALT4=(SDHC0_D7)  ALT5=()  ALT6=(FB_A23)  ALT7=()  EZPort=()

  { PTE_BASE_PTR, PORTE_BASE_PTR,   0,   0,   0,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // SD_D1                1 # PTE0 # Default=(ADC1_SE4a)  ALT0=(ADC1_SE4a)  ALT1=(PTE0)  ALT2=(SPI1_PCS1)  ALT3=(UART1_TX)  ALT4=(SDHC0_D1)  ALT5=(TRACE_CLKOUT)  ALT6=(I2C1_SDA)  ALT7=(RTC_CLKOUT)  EZPort=()
  { PTE_BASE_PTR, PORTE_BASE_PTR,   1,   0,   0,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // SD_D0                2 # PTE1/LLWU_P0 # Default=(ADC1_SE5a)  ALT0=(ADC1_SE5a)  ALT1=(PTE1/LLWU_P0)  ALT2=(SPI1_SOUT)  ALT3=(UART1_RX)  ALT4=(SDHC0_D0)  ALT5=(TRACE_D3)  ALT6=(I2C1_SCL)  ALT7=(SPI1_SIN)  EZPort=()
  { PTE_BASE_PTR, PORTE_BASE_PTR,   2,   0,   0,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // SD_CLK               3 # PTE2/LLWU_P1 # Default=(ADC1_SE6a)  ALT0=(ADC1_SE6a)  ALT1=(PTE2/LLWU_P1)  ALT2=(SPI1_SCK)  ALT3=(UART1_CTS_b)  ALT4=(SDHC0_DCLK)  ALT5=(TRACE_D2)  ALT6=()  ALT7=()  EZPort=()
  { PTE_BASE_PTR, PORTE_BASE_PTR,   3,   0,   0,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // SD_CMD               4 # PTE3 # Default=(ADC1_SE7a)  ALT0=(ADC1_SE7a)  ALT1=(PTE3)  ALT2=(SPI1_SIN)  ALT3=(UART1_RTS_b)  ALT4=(SDHC0_CMD)  ALT5=(TRACE_D1)  ALT6=()  ALT7=(SPI1_SOUT)  EZPort=()
  { PTE_BASE_PTR, PORTE_BASE_PTR,   4,   0,   0,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // SD_D3                7 # PTE4/LLWU_P2 # Default=(DISABLED)  ALT0=()  ALT1=(PTE4/LLWU_P2)  ALT2=(SPI1_PCS0)  ALT3=(UART3_TX)  ALT4=(SDHC0_D3)  ALT5=(TRACE_D0)  ALT6=()  ALT7=()  EZPort=()
  { PTE_BASE_PTR, PORTE_BASE_PTR,   5,   0,   0,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // SD_D2                8 # PTE5 # Default=(DISABLED)  ALT0=()  ALT1=(PTE5)  ALT2=(SPI1_PCS2)  ALT3=(UART3_RX)  ALT4=(SDHC0_D2)  ALT5=()  ALT6=(FTM3_CH0)  ALT7=()  EZPort=()
  { PTE_BASE_PTR, PORTE_BASE_PTR,   6,   0,   0,   ALT6, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      9 # PTE6/LLWU_P16 # Default=(DISABLED)  ALT0=()  ALT1=(PTE6/LLWU_P16)  ALT2=(SPI1_PCS3)  ALT3=(UART3_CTS_b)  ALT4=(I2S0_MCLK)  ALT5=()  ALT6=(FTM3_CH1)  ALT7=(USB0_SOF_OUT)  EZPort=()
  { PTE_BASE_PTR, PORTE_BASE_PTR,   7,   0,   0,   ALT6, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      10 # PTE7 # Default=(DISABLED)  ALT0=()  ALT1=(PTE7)  ALT2=()  ALT3=(UART3_RTS_b)  ALT4=(I2S0_RXD0)  ALT5=()  ALT6=(FTM3_CH2)  ALT7=()  EZPort=()
  { PTE_BASE_PTR, PORTE_BASE_PTR,   8,   0,   0,   ALT6, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      11 # PTE8 # Default=(DISABLED)  ALT0=()  ALT1=(PTE8)  ALT2=(I2S0_RXD1)  ALT3=()  ALT4=(I2S0_RX_FS)  ALT5=(LPUART0_TX)  ALT6=(FTM3_CH3)  ALT7=()  EZPort=()
  { PTE_BASE_PTR, PORTE_BASE_PTR,   9,   0,   0,   ALT6, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      12 # PTE9/LLWU_P17 # Default=(DISABLED)  ALT0=()  ALT1=(PTE9/LLWU_P17)  ALT2=(I2S0_TXD1)  ALT3=()  ALT4=(I2S0_RX_BCLK)  ALT5=(LPUART0_RX)  ALT6=(FTM3_CH4)  ALT7=()  EZPort=()
  { PTE_BASE_PTR, PORTE_BASE_PTR,  10,   0,   0,   ALT7, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // USB_HS_DI            13 # PTE10/LLWU_P18 # Default=(DISABLED)  ALT0=()  ALT1=(PTE10/LLWU_P18)  ALT2=(I2C3_SDA)  ALT3=()  ALT4=(I2S0_TXD0)  ALT5=(LPUART0_CTS_b)  ALT6=(FTM3_CH5)  ALT7=(USB1_ID)  EZPort=()
  { PTE_BASE_PTR, PORTE_BASE_PTR,  11,   0,   0,   ALT6, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      14 # PTE11 # Default=(DISABLED)  ALT0=()  ALT1=(PTE11)  ALT2=(I2C3_SCL)  ALT3=()  ALT4=(I2S0_TX_FS)  ALT5=(LPUART0_RTS_b)  ALT6=(FTM3_CH6)  ALT7=()  EZPort=()
  { PTE_BASE_PTR, PORTE_BASE_PTR,  12,   0,   0,   ALT6, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      15 # PTE12 # Default=(DISABLED)  ALT0=()  ALT1=(PTE12)  ALT2=()  ALT3=()  ALT4=(I2S0_TX_BCLK)  ALT5=()  ALT6=(FTM3_CH7)  ALT7=()  EZPort=()
  { PTE_BASE_PTR, PORTE_BASE_PTR,  24,   0,   0,   ALT3, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // Debug port           45 # PTE24 # Default=(ADC0_SE17)  ALT0=(ADC0_SE17)  ALT1=(PTE24)  ALT2=(CAN1_TX)  ALT3=(UART4_TX)  ALT4=()  ALT5=(I2C0_SCL)  ALT6=(EWM_OUT_b)  ALT7=()  EZPort=()
  { PTE_BASE_PTR, PORTE_BASE_PTR,  25,   0,   0,   ALT3, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // Debug port           46 # PTE25/LLWU_P21 # Default=(ADC0_SE18)  ALT0=(ADC0_SE18)  ALT1=(PTE25/LLWU_P21)  ALT2=(CAN1_RX)  ALT3=(UART4_RX)  ALT4=()  ALT5=(I2C0_SDA)  ALT6=(EWM_IN)  ALT7=()  EZPort=()
  { PTE_BASE_PTR, PORTE_BASE_PTR,  26,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, //                      47 # PTE26 # Default=(DISABLED)  ALT0=()  ALT1=(PTE26)  ALT2=(ENET_1588_CLKIN)  ALT3=(UART4_CTS_b)  ALT4=()  ALT5=()  ALT6=(RTC_CLKOUT)  ALT7=(USB0_CLKIN)  EZPort=()
  { PTE_BASE_PTR, PORTE_BASE_PTR,  27,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_OUT,   1 }, //                      48 # PTE27 # Default=(DISABLED)  ALT0=()  ALT1=(PTE27)  ALT2=()  ALT3=(UART4_RTS_b)  ALT4=()  ALT5=()  ALT6=()  ALT7=()  EZPort=()
  { PTE_BASE_PTR, PORTE_BASE_PTR,  28,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_OUT,   0 }, //                      49 # PTE28 # Default=(DISABLED)  ALT0=()  ALT1=(PTE28)  ALT2=()  ALT3=()  ALT4=()  ALT5=()  ALT6=()  ALT7=()  EZPort=()


};


/*------------------------------------------------------------------------------

 ------------------------------------------------------------------------------*/
void Config_pin(const T_IO_pins_configuration pinc)
{
  pinc.port->PCR[pinc.pin_num] = LSHIFT(pinc.irqc, 16) |
                                 LSHIFT(pinc.lock, 15) |
                                 LSHIFT(pinc.mux, 8) |
                                 LSHIFT(pinc.DSE, 6) |
                                 LSHIFT(pinc.ODE, 5) |
                                 LSHIFT(pinc.PFE, 4) |
                                 LSHIFT(pinc.SRE, 2) |
                                 LSHIFT(pinc.PUPD, 0);

  if ( pinc.init == 0 ) pinc.gpio->PCOR = LSHIFT(1, pinc.pin_num);
  else pinc.gpio->PSOR = LSHIFT(1, pinc.pin_num);
  pinc.gpio->PDDR = (pinc.gpio->PDDR & ~LSHIFT(1, pinc.pin_num)) | LSHIFT(pinc.dir, pinc.pin_num);
}


/*------------------------------------------------------------------------------

 ------------------------------------------------------------------------------*/
int Init_pins(void)
{
  int i;

  // �������� ������������ �� ���� ������
  SIM_SCGC5 |=   SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;

  for (i = 0; i < (sizeof(K66BLEZ1_pins_conf) / sizeof(K66BLEZ1_pins_conf[0])); i++)
  {
    Config_pin(K66BLEZ1_pins_conf[i]);
  }

  return 0;
}



