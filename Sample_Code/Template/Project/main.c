/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2019 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

/***********************************************************************************************************/
/* Website: http://www.nuvoton.com                                                                         */
/*  E-Mail : MicroC-8bit@nuvoton.com                                                                       */
/*  Date   : Jan/21/2019                                                                                   */
/***********************************************************************************************************/

/************************************************************************************************************/
/*  File Function: MS51 DEMO project                                                                        */
/************************************************************************************************************/
#include <stdio.h>
#include "MS51_16K.h"

//#define ENABLE_16MHz
#define ENABLE_24MHz

#if defined (ENABLE_16MHz)
#define SYS_CLOCK 								(16000000ul)
#elif defined (ENABLE_24MHz)
#define SYS_CLOCK 								(24000000ul)
#endif

#define MS_1000									(1000ul)
#define MS_500									(500ul)

uint8_t 	u8TH0_Tmp = 0;
uint8_t 	u8TL0_Tmp = 0;
uint8_t 	u8TH1_Tmp = 0;
uint8_t 	u8TL1_Tmp = 0;

xdata unsigned char ArrayData[50];
xdata unsigned char buffer[50];

//UART 0
bit BIT_TMP;
bit BIT_UART;

typedef enum{
	flag_uart0_receive = 0 ,
	
	/*---------------*/
	flag_write_read_test , 

	
	flag_DEFAULT	
}Flag_Index;

volatile uint32_t BitFlag = 0;
#define BitFlag_ON(flag)							(BitFlag|=flag)
#define BitFlag_OFF(flag)							(BitFlag&=~flag)
#define BitFlag_READ(flag)							((BitFlag&flag)?1:0)
#define ReadBit(bit)								(uint32_t)(1<<bit)

#define is_flag_set(idx)							(BitFlag_READ(ReadBit(idx)))
#define set_flag(idx,en)							( (en == 1) ? (BitFlag_ON(ReadBit(idx))) : (BitFlag_OFF(ReadBit(idx))))

/*----------------------------------------------------*/

char putchar (char c)  {
  while (!TI);
  TI = 0;
  return (SBUF = c);
}

void send_UARTString(uint8_t* Data)
{
	#if 1
	uint16_t i = 0;

	while (Data[i] != '\0')
	{
		#if 1
		SBUF = Data[i++];
		#else
		UART_Send_Data(UART0,Data[i++]);		
		#endif
	}

	#endif

	#if 0
	uint16_t i = 0;
	
	for(i = 0;i< (strlen(Data)) ;i++ )
	{
		UART_Send_Data(UART0,Data[i]);
	}
	#endif

	#if 0
    while(*Data)  
    {  
        UART_Send_Data(UART0, (unsigned char) *Data++);  
    } 
	#endif
}

void send_UARTASCII(uint16_t Temp)
{
    uint8_t print_buf[16];
    uint16_t i = 15, j;

    *(print_buf + i) = '\0';
    j = (uint16_t)Temp >> 31;
    if(j)
        (uint16_t) Temp = ~(uint16_t)Temp + 1;
    do
    {
        i--;
        *(print_buf + i) = '0' + (uint16_t)Temp % 10;
        (uint16_t)Temp = (uint16_t)Temp / 10;
    }
    while((uint16_t)Temp != 0);
    if(j)
    {
        i--;
        *(print_buf + i) = '-';
    }
    send_UARTString(print_buf + i);
}

void send_UARTHex(uint16_t u16Temp)
{
    uint8_t print_buf[16];
    uint32_t i = 15;
    uint32_t temp;

    *(print_buf + i) = '\0';
    do
    {
        i--;
        temp = u16Temp % 16;
        if(temp < 10)
            *(print_buf + i) = '0' + temp;
        else
            *(print_buf + i) = 'a' + (temp - 10) ;
        u16Temp = u16Temp / 16;
    }
    while(u16Temp != 0);
    send_UARTString(print_buf + i);
}

void DataFlash_Test(void)
{
	static uint8_t cnt = 0;
	uint8_t i = 0;
	uint8_t res = 0;
	
	if (is_flag_set(flag_write_read_test))
	{
		set_flag(flag_write_read_test , Disable);

		send_UARTString("DATAFLASH_ARRAY TEST\r\n");
		
		//prepare data 
		for (i = 0 ; i < 50 ; i++)
		{
			ArrayData[i] = i + cnt*0x10;
		}

		Write_DATAFLASH_ARRAY(0x3F80,ArrayData,50);

		Read_DATAFLASH_ARRAY(0x3F80,buffer,50);

		for (i = 0 ; i < 50 ; i++)
		{
			send_UARTHex(buffer[i]);
			send_UARTString(",");
	        if ((i+1)%8 ==0)
	        {
	            send_UARTString("\r\n");
	        }		
		}
		send_UARTString("\r\n");

		send_UARTString("DATAFLASH_BYTE TEST\r\n");
		Write_DATAFLASH_BYTE (0x3F00,cnt);
		res = read_APROM_BYTE(0x3F00);
		
		send_UARTHex(res);
		send_UARTString("\r\n\r\n");

		for (i = 0 ; i < 50 ; i++)
		{
			buffer[i] = 0x00;		
		}

		send_UARTString("SPROM_DATAFLASH_ARRAY TEST\r\n");
		
		Write_SPROM_DATAFLASH_ARRAY(1,ArrayData,50);

		Read_SPROM_DATAFLASH_ARRAY(1,buffer,50);

		for (i = 0 ; i < 50 ; i++)
		{
			send_UARTHex(buffer[i]);
			send_UARTString(",");
	        if ((i+1)%8 ==0)
	        {
	            send_UARTString("\r\n");
	        }		
		}
		send_UARTString("\r\n");

		send_UARTString("Read_SPROM_BYTE TEST\r\n");
		res = Read_SPROM_BYTE(1);
		
		send_UARTHex(res);
		send_UARTString("\r\n");
		
		
		cnt++;
		send_UARTString("\r\n");
	}




}

void GPIO_Toggle_1(void)
{

}

void GPIO_Toggle_0(void)
{
	P12 ^=1 ;
}

void GPIO_Init(void)	//for test
{
    P12_PUSHPULL_MODE;	

}

void Timer2_Delay(unsigned long u32SYSCLK,unsigned int u16TMDIV, unsigned int u16CNT, unsigned long u32DLYUnit)
{
    unsigned char TL2TMP,TH2TMP;
  
    SFRS = 0x00;
    switch (u16TMDIV)
    {
      case 1:T2MOD&=0x8F; break;
      case 4:T2MOD&=0x8F;T2MOD|=0x10; break;
      case 16:T2MOD&=0x8F;T2MOD|=0x20; break;
      case 32:T2MOD&=0x8F;T2MOD|=0x30; break;
      case 64:T2MOD&=0x8F;T2MOD|=0x40; break;
      case 128:T2MOD&=0x8F;T2MOD|=0x50; break;
      case 256:T2MOD&=0x8F;T2MOD|=0x60; break;
      case 512:T2MOD&=0x8F;T2MOD|=0x70; break;
    }
    clr_T2CON_CMRL2;                                  //Timer 2 as auto-reload mode
    set_T2MOD_LDEN;
    set_T2MOD_CMPCR;                                  //Timer 2 value is auto-cleared as 0000H when a compare match occurs.
    TL2TMP = LOBYTE(65536-((u32SYSCLK/1000000)*u32DLYUnit/u16TMDIV));
    TH2TMP = HIBYTE(65536-((u32SYSCLK/1000000)*u32DLYUnit/u16TMDIV));
    while (u16CNT != 0)
    {
      TL2 = TL2TMP;
      TH2 = TH2TMP;
      set_T2CON_TR2;                                    //Start Timer2
      while (TF2!=1);            //Check Timer2 Time-Out Flag
      clr_T2CON_TF2;
      clr_T2CON_TR2;                                    //Stop Timer2
      u16CNT --;
    }
}

void Timer0_IRQHandler(void)
{
	static uint16_t CNT_GPIO = 0;
//	static uint16_t counter = 0;
	
	if (CNT_GPIO++ >= MS_1000)
	{		
		CNT_GPIO = 0;
		GPIO_Toggle_0();	//for test
//		printf("coutner : %d\r\n" , counter++);			

		set_flag(flag_write_read_test , Enable);
		
	}	
}

void Timer0_ISR(void) interrupt 1        // Vector @  0x0B
{
    TH0 = u8TH0_Tmp;
    TL0 = u8TL0_Tmp;
    clr_TCON_TF0;
	
	Timer0_IRQHandler();
}

void Timer0_Init(void)
{
	uint16_t res = 0;

	ENABLE_TIMER0_MODE1;

	#if defined (ENABLE_16MHz)
	u8TH0_Tmp = HIBYTE(TIMER_DIV12_VALUE_1ms_FOSC_160000);
	u8TL0_Tmp = LOBYTE(TIMER_DIV12_VALUE_1ms_FOSC_160000); 

	
	#elif defined (ENABLE_24MHz)
	u8TH0_Tmp = HIBYTE(TIMER_DIV12_VALUE_1ms_FOSC_240000);
	u8TL0_Tmp = LOBYTE(TIMER_DIV12_VALUE_1ms_FOSC_240000); 
	#endif
	
    TH0 = u8TH0_Tmp;
    TL0 = u8TL0_Tmp;

    ENABLE_TIMER0_INTERRUPT;                       //enable Timer0 interrupt
    ENABLE_GLOBAL_INTERRUPT;                       //enable interrupts
  
    set_TCON_TR0;                                  //Timer0 run
}


void UART0_Process(uint8_t res)
{
	if (res > 0x7F)
	{
		return;
	}

	if (is_flag_set(flag_uart0_receive))
	{
		set_flag(flag_uart0_receive , Disable);

		#if 0

		res -= 0x30;
		I2Cx_Master_example(res);
		
		#else
//		printf("0x%2X\r\n" , res);
		switch(res)
		{
			case '1' :

				break;
		}	
		#endif
	}
}

void Serial_ISR (void) interrupt 4 
{
    if (RI)
    {   
	  set_flag(flag_uart0_receive , Enable);
	  UART0_Process(SBUF);

      clr_SCON_RI;                                         // Clear RI (Receive Interrupt).
    }
    if  (TI)
    {
      if(!BIT_UART)
      {
          TI = 0;
      }
    }
}

void UART0_Init(void)
{
	unsigned long u32Baudrate = 115200;
	
	P06_QUASI_MODE;    //Setting UART pin as Quasi mode for transmit
	
	#if 1
	SCON = 0x50;          //UART0 Mode1,REN=1,TI=1
	set_PCON_SMOD;        //UART0 Double Rate Enable
	T3CON &= 0xF8;        //T3PS2=0,T3PS1=0,T3PS0=0(Prescale=1)
	set_T3CON_BRCK;        //UART0 baud rate clock source = Timer3

	#if defined (ENABLE_16MHz)
	RH3    = HIBYTE(65536 - (1000000/u32Baudrate)-1);  
	RL3    = LOBYTE(65536 - (1000000/u32Baudrate)-1);  
	#elif defined (ENABLE_24MHz)
	RH3    = HIBYTE(65536 - (SYS_CLOCK/16/u32Baudrate));  
	RL3    = LOBYTE(65536 - (SYS_CLOCK/16/u32Baudrate));  
	#endif
	
	set_T3CON_TR3;         //Trigger Timer3
	set_IE_ES;

	ENABLE_GLOBAL_INTERRUPT;

	set_SCON_TI;
	BIT_UART=1;
	#else	
	
	SCON = 0x50;       			//UART0 Mode1,REN=1,TI=1
	TMOD |= 0x20;      		//Timer1 Mode1
	set_PCON_SMOD;          	//UART0 Double Rate Enable
	set_CKCON_T1M;
	clr_T3CON_BRCK;          	//Serial port 0 baud rate clock source = Timer1
	TH1 = 256 - (SYS_CLOCK/16/u32Baudrate);

	set_TCON_TR1;
	ENABLE_UART0_INTERRUPT;

	ENABLE_GLOBAL_INTERRUPT;
	
	set_SCON_TI;
	BIT_UART=1;
	
	#endif
}



#if defined (ENABLE_16MHz)
void MODIFY_HIRC_16(void)
{
    unsigned char data hircmap0,hircmap1;
    set_CHPCON_IAPEN;
    IAPAL = 0x30;
    IAPAH = 0x00;
    IAPCN = READ_UID;
    set_IAPTRG_IAPGO;
    hircmap0 = IAPFD;
    IAPAL = 0x31;
    IAPAH = 0x00;
    set_IAPTRG_IAPGO;
    hircmap1 = IAPFD;
    clr_CHPCON_IAPEN;
    TA=0XAA;
    TA=0X55;
    RCTRIM0 = hircmap0;
    TA=0XAA;
    TA=0X55;
    RCTRIM1 = hircmap1;
}

#elif defined (ENABLE_24MHz)
void MODIFY_HIRC_24(void)
{
    unsigned char data hircmap0,hircmap1;
/* Check if power on reset, modify HIRC */
    if (PCON&SET_BIT4)
    {
        set_CHPCON_IAPEN;
        IAPAL = 0x38;
        IAPAH = 0x00;
        IAPCN = READ_UID;
        set_IAPTRG_IAPGO;
        hircmap0 = IAPFD;
        IAPAL = 0x39;
        IAPAH = 0x00;
        set_IAPTRG_IAPGO;
        hircmap1 = IAPFD;
        clr_CHPCON_IAPEN;
        TA=0XAA;
        TA=0X55;
        RCTRIM0 = hircmap0;
        TA=0XAA;
        TA=0X55;
        RCTRIM1 = hircmap1;
        clr_CHPCON_IAPEN;
    }
}

#endif

void SYS_Init(void)
{
	#if defined (ENABLE_16MHz)
	MODIFY_HIRC_16();
	#else
    MODIFY_HIRC_24();
	#endif

    ALL_GPIO_QUASI_MODE;
    ENABLE_GLOBAL_INTERRUPT;                // global enable bit	
}

void main (void) 
{
	
    SYS_Init();

    UART0_Init();
							
	GPIO_Init();					
				
	Timer0_Init();
	
    while(1)
    {
		DataFlash_Test();

    }
}



