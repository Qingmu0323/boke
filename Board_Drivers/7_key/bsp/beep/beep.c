#include "beep.h"

/*初始化蜂鸣器*/
void init_beep(void)
{
    gpio_pin_config_t beep_config;

    IOMUXC_SetPinMux(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01,0);  /*复用为GPIO*/

    /*
    *bit 16:0 HYS 关闭
    *bit [15:14]: 00 默认下拉
    *bit [13]: 0 kepper 功能
    *bit [12]: 1 pull/keeper 使能
    *bit [11]: 0 关闭开路输出
    *bit [7:6]: 10 速度 100Mhz
    *bit [5:3]: 110 R0/6 驱动能力
    bit [0]: 0 低转换率
    */
    IOMUXC_SetPinConfig(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01,0x10b0);/*设置其电器属性*/

    /*GPIO初始化*/
    beep_config.direction=kGPIO_DigitalOutput;
    beep_config.outputLogic=1;

    gpio_init(GPIO5,1,&beep_config);

}

/*蜂鸣器控制*/
void beep_switch(int status)
{
    if(status == ON)
        gpio_pinwrite(GPIO5,1,0);
    else if(status == OFF)  
        gpio_pinwrite(GPIO5,1,1);
}