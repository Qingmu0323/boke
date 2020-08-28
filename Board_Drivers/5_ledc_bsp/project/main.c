

#include "main.h"




int main() 
{
    clk_enable();  //使能外设时钟

    led_init(); //初始化LED
    
    while(1)
    {
        led_off();  
        delay(1000);

        led_on();
        delay(1000);
    }

    return 0;
}