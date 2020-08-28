
#include "main.h"

int main() 
{
    clk_enable();  //使能外设时钟

    led_init(); //初始化LED

    init_beep();//初始化蜂鸣器
    
    while(1)
    {
        led_off();  
        beep_switch(OFF);
        delay(1000);

        led_on();
        beep_switch(ON);
        delay(1000);
    }

    return 0;
}