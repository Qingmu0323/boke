#include "bsp_gpio.h"


/*
 * @description : GPIO 初始化。
 * @param - base : 要初始化的 GPIO 组。
 * @param - pin : 要初始化 GPIO 在组内的编号。
 * @param - config : GPIO 配置结构体。
 * @return : 无
 */

void gpio_init(GPIO_Type *base,int pin, gpio_pin_config_t *config)
{
    if(config->direction == kGPIO_DigitalInput)  /*输入*/
    {
        base->GDIR &= ~(1 << pin);

    }
    else  /*输出*/
    {
        base->GDIR |= (1 << pin);
        gpio_pinwrite(base,pin,config->outputLogic); /*默认输出电平*/
    }
    
}

/*
 * @description : 读取指定 GPIO 的电平值 。
 * @param – base : 要读取的 GPIO 组。
 * @param - pin : 要读取的 GPIO 脚号。
 * @return : 无
 */

int gpio_pinread(GPIO_Type *base,int pin)
{
    return (((base->DR) >> pin) & 0x1);
}

/*
 * @description : 指定 GPIO 输出高或者低电平 。
 * @param – base : 要输出的的 GPIO 组。
 * @param - pin : 要输出的 GPIO 脚号。
 * * @param – value : 要输出的电平， 1 输出高电平， 0 输出低低电平
 * @return : 无
 */

void gpio_pinwrite(GPIO_Type *base,int pin,int value)
{
    if(value == 0U)
    {
        base->DR &= ~(1U<<pin);  /*输出低电平*/

    }
    else
    {
        base->DR |= (1U << pin);  /*输出高电平*/
    }
    

}