/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TM1637_CONF_H
#define __TM1637_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm8s_gpio.h"

// Configuration.

//#define TM1637_GPIO_PORT       		GPIOC
//#define TM1637_GPIO_PORT_CLK   		RCC_AHB1Periph_GPIOC
#define TM1637_GPIO_PORT_ENCLK()	RCC_AHB1PeriphClockCmd(TM1637_GPIO_PORT_CLK, ENABLE)
#define TM1637_CLK_PIN       		GPIO_Init(GPIOC,(GPIO_Pin_TypeDef)(GPIO_PIN_6));
#define TM1637_CLK_PIN_HIGH()    	GPIO_SetBits(TM1637_GPIO_PORT, TM1637_CLK_PIN)
#define TM1637_CLK_PIN_LOW()        GPIO_ResetBits(TM1637_GPIO_PORT, TM1637_CLK_PIN)
#define TM1637_DIO_PIN       		GPIO_Init(GPIOC,(GPIO_Pin_TypeDef)(GPIO_PIN_5));
#define TM1637_DIO_PIN_HIGH() 		GPIO_SetBits(TM1637_GPIO_PORT, TM1637_DIO_PIN)
#define TM1637_DIO_PIN_LOW()        GPIO_ResetBits(TM1637_GPIO_PORT, TM1637_DIO_PIN)
#define TM1637_DIO_READ()			(GPIO_ReadInputDataBit(TM1637_GPIO_PORT, TM1637_DIO_PIN) > 0 ? 1 : 0)

#ifdef __cplusplus
}
#endif
#endif /* __TM1637_CONF_H */
