#ifndef __UDP_H__
#define __UDP_H__

#include "eth.h"
#include "socket.h"


#define SN 0

void UDP_Start(void);

void UDP_RecvData(uint8_t *rxBuff, uint16_t *rxLen, uint8_t *srcIP, uint16_t *srcPort);

void UDP_SendData(uint8_t *txBuff, uint16_t txLen, uint8_t *dstIP, uint16_t dstPort);
#endif