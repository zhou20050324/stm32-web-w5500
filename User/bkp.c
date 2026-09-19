#include "bkp.h"

void BKP_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_PWREN|RCC_APB1ENR_BKPEN;

    PWR->CR |= PWR_CR_DBP;
}

void BKP_WriteData(uint16_t data)
{
    BKP->DR1 = data;
}

uint16_t BKP_ReadData(void)
{
    return BKP->DR1;
}

void BKP_Tamper_Init(void)
{
    BKP->CR |= (BKP_CR_TPAL | BKP_CR_TPE);
    BKP->CSR |= (BKP_CSR_CTE | BKP_CSR_CTI);

    BKP->CSR |= BKP_CSR_TPIE;

    //NVIC_EnableIRQ(TAMPER_IRQn);
}
