#include "imx6ul.h"
/*使能外设时钟*/
void clk_enable(void)
{
    CCM->CCGR0 =0xFFFFFFFF;
    CCM->CCGR1 =0xFFFFFFFF;
    CCM->CCGR2 =0xFFFFFFFF;
    CCM->CCGR3 =0xFFFFFFFF;
    CCM->CCGR4 =0xFFFFFFFF;
    CCM->CCGR5 =0xFFFFFFFF;
    CCM->CCGR6 =0xFFFFFFFF;

}

/*初始化LED灯*/
void led_init(void)
{

    IOMUX_SW_MUX->GPIO1_IO03=0x5; /*复用为GPIO--IO03 */


    IOMUX_SW_PAD->GPIO1_IO03=0x10B0;/*设置GPIO__IO03电器属性*/

    GPIO1->GDIR=0x8;//设置为输出

    GPIO1->DR=0x0;

}
/*短延时*/
void delay_short(volatile unsigned int n)
{
    while(n--){}

}
/*
 * 延时  一次循环大概是1ms 在主频396MHz
 * n:延时ms数
*/
void delay(volatile unsigned int n)
{
    while (n--)
    {
        delay_short(0x7ff);
    }
    
}
/*打开LED灯*/
void  led_on(void)
{
    GPIO1->DR&= ~(1<<3); //bit3清零

}
/*关闭LED灯*/
void led_off(void )
{
    GPIO1->DR |= (1<<3);  //bit3置1
}
int main() 
{
    clk_enable();  //使能外设时钟

    led_init(); //初始化LED
    
    while(1)
    {
        led_off();  
        delay(500);

        led_on();
        delay(500);
    }

    return 0;
}