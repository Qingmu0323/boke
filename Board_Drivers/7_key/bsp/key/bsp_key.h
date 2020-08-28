#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "imx6ul.h"
#include "bsp_delay.h"

/*定义按键值*/
enum keyvalue {
    KEY_NONE =0,
    KEY_VALUE ,
};

/*函数声明*/
void key_init(void);
int key_get_value(void);

#endif // !__BSP_KEY_H