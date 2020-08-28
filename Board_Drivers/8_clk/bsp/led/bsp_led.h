#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "imx6ul.h"

#define LED0 0

void  led_on(void);
void led_off(void );
void led_init(void);
void led_switch(int led, int status);

#endif /* __BSP_LED_H */
