#include "can.h"


static void CAN_FilterConfig(void);

// 初始化
void CAN_Init(void)
{
    // 1. 开启时钟：CAN、GPIO、AFIO
    RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;


    // 2.PA11、PA12  CAN1_TX、CAN1_RX
    

    // 3. GPIO工作模式定义：
    // PA11 - 浮空输入 MODE 00，CNF 01
    GPIOA->CRH &= ~GPIO_CRH_MODE11;
    GPIOA->CRH &= ~GPIO_CRH_CNF11_1;
    GPIOA->CRH |= GPIO_CRH_CNF11_0;
    
    // PA12 - 复用推挽输出, MODE 11，CNF 10
    GPIOA->CRH |= GPIO_CRH_MODE12;
    GPIOA->CRH |= GPIO_CRH_CNF12_1;
    GPIOA->CRH &= ~GPIO_CRH_CNF12_0;

    // 4. CAN 初始化基本配置
    // 4.1 进入初始化模式
    CAN1->MCR |= CAN_MCR_INRQ;
    while ((CAN1->MSR & CAN_MSR_INAK) == 0)
    {}

    // 4.2 退出睡眠模式
    CAN1->MCR &= ~CAN_MCR_SLEEP;
    while ((CAN1->MSR & CAN_MSR_SLAK) != 0)
    {}

    // 4.3 自动离线管理
    CAN1->MCR |= CAN_MCR_ABOM;

    // 4.4 自动唤醒管理
    CAN1->MCR |= CAN_MCR_AWUM;

    // 4.5 配置环回静默测试模式
    CAN1->BTR |= CAN_BTR_SILM;
    CAN1->BTR |= CAN_BTR_LBKM;

    // 4.6 配置位时序
    // 4.6.1 配置波特率分频器，36分频，Tq = 1us
    CAN1->BTR &= ~CAN_BTR_BRP;
    CAN1->BTR |= (35 << 0);

    // 4.6.2 配置BS1和BS2的时间长度
    CAN1->BTR &= ~CAN_BTR_TS1;
    CAN1->BTR |= (2 << 16);

    CAN1->BTR &= ~CAN_BTR_TS2;
    CAN1->BTR |= (5 << 20);

    // 4.6.3 再同步跳跃宽度
    CAN1->BTR &= ~CAN_BTR_SJW;
    CAN1->BTR |= (1 << 24);

    // 4.7 退出初始化模式
    CAN1->MCR &= ~CAN_MCR_INRQ;
    while ((CAN1->MSR & CAN_MSR_INAK) != 0)
    {}

    // 5. CAN过滤器配置
    CAN_FilterConfig();
}

// 定义静态函数，进行过滤器配置
static void CAN_FilterConfig(void)
{
    // 1. 进入初始化模式
    CAN1->FMR |= CAN_FMR_FINIT;

    // 2. 配置过滤器工作模式：0 - 屏蔽位模式
    CAN1->FM1R &= ~CAN_FM1R_FBM0;

    // 3. 配置位宽：1 - 32位
    CAN1->FS1R |= CAN_FS1R_FSC0;

    // 4. 设置关联的FIFO：FIFO0
    CAN1->FFA1R &= ~CAN_FFA1R_FFA0;

    // 5. 设置过滤器组0的ID寄存器：FR1
    CAN1->sFilterRegister[0].FR1 = 0x06e << 21;

    // 6. 设置过滤器组0的掩码寄存器：FR2 = 0，不做过滤，全部接收
    CAN1->sFilterRegister[0].FR2 = 0x000 << 21;

    // 7. 激活过滤器组0
    CAN1->FA1R |= CAN_FA1R_FACT0;

    // 8. 退出初始化模式
    CAN1->FMR &= ~CAN_FMR_FINIT;
}

// 发送报文：标准ID、数据、长度（使用发送邮箱0）
void CAN_SendMsg(uint16_t stdID, uint8_t * data, uint8_t len)
{
    // 1. 等待发送邮箱0为空
    while ( (CAN1->TSR & CAN_TSR_TME0) == 0)
    {
    }

    // 2. 包装要发送的数据帧
    // 2.1 设置标准ID
    CAN1->sTxMailBox[0].TIR &= ~CAN_TI0R_STID;
    CAN1->sTxMailBox[0].TIR |= stdID << 21;

    // 2.2 设置为标准帧
    CAN1->sTxMailBox[0].TIR &= ~CAN_TI0R_IDE;

    // 2.3 设置为数据帧
    CAN1->sTxMailBox[0].TIR &= ~CAN_TI0R_RTR;

    // 2.4 设置数据长度
    CAN1->sTxMailBox[0].TDTR &= ~CAN_TDT0R_DLC;
    CAN1->sTxMailBox[0].TDTR |= len << 0;

    // 2.5 设置数据
    // 先清零寄存器
    CAN1->sTxMailBox[0].TDLR = 0;
    CAN1->sTxMailBox[0].TDHR = 0;

    // 循环处理每一个字节数据
    for (uint8_t i = 0; i < len; i++)
    {
        // 判断前4个字节，放到TDLR中
        if (i < 4)
        {
            CAN1->sTxMailBox[0].TDLR |= data[i] << (i * 8);
        } 
        // 后4个字节，放到TDHR中
        else
        {
            CAN1->sTxMailBox[0].TDHR |= data[i] << ((i - 4) * 8);
        }
    }

    // 3. 请求发送数据帧
    CAN1->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;

    // 4. 等待发送完成
    while ((CAN1->TSR & CAN_TSR_TXOK0) == 0)
    {
    }
}

// 接收报文：结构体数组，数组长度（从FIFO0中读取）
void CAN_ReceiveMsg(RxMsg rxMsg[], uint8_t * msgCount)
{
    // 1. 获取FIFO0的报文个数，通过指针返回
    * msgCount = (CAN1->RF0R & CAN_RF0R_FMP0) >> 0;

    // 2. 循环读取每一个报文
    for (uint8_t i = 0; i < *msgCount; i++)
    {
        // 定义指针，指向当前保存报文的数据对象
        RxMsg * msg = &rxMsg[i];

        // 2.1 读取ID
        msg->stdID = (CAN1->sFIFOMailBox[0].RIR >> 21) & 0x7ff;

        // 2.2 读取数据长度
        msg->len = (CAN1->sFIFOMailBox[0].RDTR >> 0) & 0x0f;
        
        // 2.3 读取数据
        uint32_t low = CAN1->sFIFOMailBox[0].RDLR;
        uint32_t high = CAN1->sFIFOMailBox[0].RDHR;

        for (uint8_t j = 0; j < msg->len; j++)
        {
            // 如果是前4个字节，就从RDLR中提取
            if (j < 4)
            {
                msg->data[j] = (low >> (8 * j)) & 0xff;
            }      
            // 如果是后4个字节，就从RDHR中提取
            else
            {
                msg->data[j] = (high >> (8 * (j - 4))) & 0xff;
            }
        }

        // 2.4 释放FIFO0，出队，再读取下一个报文
        CAN1->RF0R |= CAN_RF0R_RFOM0;
    }
}