#include "bsp_clk.h"

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

