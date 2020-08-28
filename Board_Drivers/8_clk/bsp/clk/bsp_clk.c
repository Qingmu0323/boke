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
/*
 * @description : 初始化系统时钟 528Mhz，并且设置 PLL2 和 PLL3 各个
 * PFD 时钟,所有的时钟频率均按照 I.MX6U 官方手册推荐的值.
 * @param : 无
 * @return : 无
 */
void imx6u_clkinit(void)
{
    /* 1、设置 ARM 内核时钟为 528MHz */
    /* 1.1、判断当使用哪个时钟源启动的，正常情况下是由 pll1_sw_clk 驱动的，而
    * pll1_sw_clk 有两个来源： pll1_main_clk 和 step_clk， 如果要
    * 让 I.MX6ULL 跑到 528M， 那必须选择 pll1_main_clk 作为 pll1 的时钟
    * 源。 如果我们要修改 pll1_main_clk 时钟的话就必须先将 pll1_sw_clk 从
    * pll1_main_clk 切换到 step_clk,当修改完以后再将 pll1_sw_clk 切换
    * 回 pll1_main_cl， step_clk 等于 24MHz。
    */

    unsigned int reg = 0;
    
    if((((CCM->CCSR)>>2) & 0x1) ==0)
    {
        CCM->CCSR &= ~(1 << 8); /* 配置 step_clk 时钟源为 24MHz OSC */
        CCM->CCSR |= (1 << 2);  /* 配置 pll1_sw_clk 时钟源为 step_clk */
    }

    /* 1.2、设置 pll1_main_clk 为 1056MHz,也就是 528*2=1056MHZ,
    * 因为 pll1_sw_clk 进 ARM 内核的时候会被二分频！
    * 配置 CCM_ANALOG->PLL_ARM 寄存器
    * bit13: 1 使能时钟输出
    * bit[6:0]: 88, 由公式： Fout = Fin * div_select / 2.0，
    * 1056=24*div_select/2.0, 得出： div_select=88。
    */
    
    CCM_ANALOG->PLL_ARM = (1 << 13) | ((88 << 0) & 0X7F);
    CCM->CCSR &= ~(1 << 2);   /* 将 pll_sw_clk 时钟切换回 pll1_main_clk */
    CCM->CACRR = 1;  /* ARM 内核时钟为 pll1_sw_clk/2=1056/2=528Mhz */


    /* 2、设置 PLL2(SYS PLL)各个 PFD */
    reg = CCM_ANALOG->PFD_528;
    reg &=  ~(0x3f3f3f3f);      /* 清除原来的设置 */
    reg |= (32<<24);            /* PLL2_PFD3=528*18/32=297Mhz */
    reg |= (24<<16);            /* PLL2_PFD2=528*18/24=396Mhz */
    reg |= (16<<8);             /* PLL2_PFD1=528*18/16=594Mhz */
    reg |= (27<<0);             /* PLL2_PFD0=528*18/27=352Mhz */
    CCM_ANALOG->PFD_528 = reg;  /* 设置 PLL2_PFD0~3 */

    /* 3、设置 PLL3(USB1)各个 PFD */
    reg =0;                         /* 清零reg */
    reg = CCM_ANALOG->PFD_480;      /* 清除原来的设置 */
    reg &= ~(0x3f3f3f3f);           
    reg |=  (19<<24);               /* PLL3_PFD3=480*18/19=454.74Mhz */
    reg |=  (17<<16);               /* PLL3_PFD2=480*18/17=508.24Mhz */
    reg |=  (16<<8);                /* PLL3_PFD1=480*18/16=540Mhz */
    reg |=  (12<<0);                /* PLL3_PFD0=480*18/12=720Mhz */
    CCM_ANALOG->PFD_480 = reg;      /* 设置 PLL3_PFD0~3 */

    /* 4、设置 AHB 时钟 最小 6Mhz， 最大 132Mhz */
    CCM->CBCMR &= ~(3<<18);         /* 清除设置*/
    CCM->CBCMR |= (1<<18);          /* pre_periph_clk=PLL2_PFD2=396MHz */     
    CCM->CBCDR &= ~(1<<25);         /* periph_clk=pre_periph_clk=396MHz */
    while(CCM->CDHIPR & (1 << 5));  /* 等待握手完成 */

    /* 修改 AHB_PODF 位的时候需要先禁止 AHB_CLK_ROOT 的输出，但是
    * 我没有找到关闭 AHB_CLK_ROOT 输出的的寄存器，所以就没法设置。
    * 下面设置 AHB_PODF 的代码仅供学习参考不能直接拿来使用！！
    * 内部 boot rom 将 AHB_PODF 设置为了 3 分频，即使我们不设置 AHB_PODF，
    * AHB_ROOT_CLK 也依旧等于 396/3=132Mhz。
    */
#if 0
    /* 要先关闭 AHB_ROOT_CLK 输出，否则时钟设置会出错 */
    CCM->CBCDR &= ~(7 << 10);       /* CBCDR 的 AHB_PODF 清零 */
    CCM->CBCDR |= (2 << 10);        /* AHB_PODF 3 分频， AHB_CLK_ROOT=132MHz */
    while(CCM->CDHIPR & (1 << 5));  /* 等待握手完成 */
#endif

    /* 5、设置 IPG_CLK_ROOT 最小 3Mhz，最大 66Mhz */
    CCM->CBCDR &= ~(3<<8);       /* CBCDR 的 IPG_PODF 清零 */
    CCM->CBCDR |= (1<<8);        /* IPG_PODF 2 分频， IPG_CLK_ROOT=66MHz */

    /* 6、设置 PERCLK_CLK_ROOT 时钟 */
    CCM->CSCMR1 &= ~(1<<6);         /* PERCLK_CLK_ROOT 时钟源为 IPG */
    CCM->CSCMR1 &= ~(63 << 0);

}

