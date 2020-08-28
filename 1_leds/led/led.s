.global _start @全局标号

_start:
    /*使能所有外设时钟 */
    LDR R0 , =0x020c4068  @CCGR0 
    LDR R1 , =0xffffffff  @要想CCGR0写入的数据
    STR R1 , [R0]         @将R1的值写入到R0中
    
    LDR R0 , =0x020c406c  @CCGR1
    STR R1 ,[R0]

    LDR R0 , =0x020c4070  @CCGR1
    STR R1 ,[R0]

    LDR R0 , =0x020c4074  @CCGR1
    STR R1 ,[R0]

    LDR R0 , =0x020c4078  @CCGR1
    STR R1 ,[R0]

    LDR R0 , =0x020c407c  @CCGR1
    STR R1 ,[R0]

    LDR R0 , =0x020c4080  @CCGR1
    STR R1 ,[R0]

    /*配置 GPIO_I003  PIN的复用为GPIO
    * IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03 = 0101 =5
    * IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03寄存器的地址为0x020E_0068
    */
    LDR R0 , =0x020E0068  @IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03
    LDR R1 , =0x5          @要写入的数据
    STR R1 , [R0]          @将5写入IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03

    /*配置 IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO03的电器属性
    * IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO03的地址时0x020E_02F4
    * bit0 :    0 低速率
    * bit5-3:   110 R0/6  驱动能力
    * bit7-6:   10 100MHz速度
    * bit11:    0 关闭开路输出
    * bit12:    1 使能pull/keeper
    * bit15-14: 00 100K下拉
    * bit16:    0 关闭hys    
    * 向寄存器IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO03写入 0x10b0
    */

    LDR R0 , = 0x020E02F4
    LDR R1 , = 0x10b0
    STR R1 ,[R0]

    /*设置GPIO功能
     *设置GPIO1_GDIR寄存器  设置GPIO1_GPIO03为输出
     *寄存器GPIO_GDIR的地址是  0x0209C004
     * 设置GPIO1_GDIR寄存器bit3为1也就是GPIO1_GPIO03为输出
     */
    LDR R0 , = 0x0209C004
    LDR R1 , = 0x8
    STR R1 ,[R0]

    /*打开LED，也就是设置GPIO1_GPIO03为0 
     *GPIO1_DR 寄存器地址为0x0209C000
    */

    LDR R0 , = 0x0209C000
    LDR R1 , =0
    STR R1 ,[R0]

loop:
    b loop
