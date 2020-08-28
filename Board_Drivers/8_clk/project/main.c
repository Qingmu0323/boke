
#include "main.h"

int main() 
{
    int i=0;
    int keyvalue=0;   
    unsigned char led_status= OFF;
    unsigned char beep_status= OFF;

    imx6u_clkinit(); /* 初始化系统时钟 */   
    clk_enable();  //使能外设时钟
    led_init(); //初始化LED
    init_beep();//初始化蜂鸣器
    key_init(); //初始化key
    
    while(1)
    {
        keyvalue = key_get_value();
        if(keyvalue)
        {
            switch (keyvalue)
            {
            case KEY_VALUE:
                beep_status=!beep_status;
                beep_switch(beep_status);
                break;
            
            default:
                break;
            }
        }
        
        i++;
        if(i == 50)
        {
            i=0;
            led_status=!led_status;
            led_switch(LED0,led_status);
        }
        delay(10);

    }

    return 0;
}