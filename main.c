
#include "stm8s.h"
#include "tm1637.h"
//#include "mystm8sxxx.h"
#define SPEAKER_GPIO_PORT		GPIOD
#define SPEAKER_GPIO_PIN		GPIO_PIN_3
#define SPEAKER_ON()    		GPIO_WriteHigh(SPEAKER_GPIO_PORT, SPEAKER_GPIO_PIN)
#define SPEAKER_OFF()        	GPIO_WriteLow(SPEAKER_GPIO_PORT, SPEAKER_GPIO_PIN)

void delay(uint32_t time);

volatile uint32_t f_cpu=0;

void beep(void) {
	SPEAKER_ON();
	delay(100000);
	SPEAKER_OFF();
}

void main(void)
{
	clk_config_16MHz_hse();
	f_cpu=CLK_GetClockFreq();

	delay_using_timer4_init();

        //khai bao TIMER dem
        TIMbase_Configuration();
        
        printf("Test TM1637\r\n");
	TM1637_init();
	TM1637_display_colon(true);
	TM1637_display_digit(TM1637_SET_ADR_00H, 0);
	TM1637_display_digit(TM1637_SET_ADR_01H, 0);
	TM1637_display_digit(TM1637_SET_ADR_02H, 0);
	TM1637_display_digit(TM1637_SET_ADR_03H, 0);
	
	while (1) {
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) != 0) {
			delay_ms(10);
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) != 0) {
				display_minute = 0;
				display_second = 10;
				SetMinute(display_minute);
				SetSecond(display_second);
				counting_flag = 1;
			}
		}

		if (GetSecond() || GetMinute()) {
			TM1637_display_digit(TM1637_SET_ADR_03H, GetSecond() % 10);
			TM1637_display_digit(TM1637_SET_ADR_02H, GetSecond() / 10);
			TM1637_display_digit(TM1637_SET_ADR_01H, GetMinute() % 10);
			TM1637_display_digit(TM1637_SET_ADR_00H, GetMinute() / 10);
			TM1637_display_colon(false);
			delay_ms(250);
			TM1637_display_colon(true);
			delay_ms(250);
			LEDS_TOGGLE;
		} else {
			TM1637_display_digit(TM1637_SET_ADR_03H, 0);
		}
	}
}

void TIMbase_Configuration(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = ((SystemCoreClock/2)/1000000)-1;     // frequency = 1000000
	TIM_TimeBaseStructure.TIM_Period = 1000 - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM4, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/* User Function */

void delay(uint32_t time)
{
  //1000000 = 2s hiha
  while(time--);
}


#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
