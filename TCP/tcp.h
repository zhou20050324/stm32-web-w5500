#ifndef __TCP_H__
#define __TCP_H__

#include "socket.h"
#include "eth.h"

#define SN 0

#define CLIENT 0
#define SERVER 1
#define ROLE SERVER

void TCP_ServerStart(void);

void TCP_RecvData(uint8_t buff[], uint16_t *len);

void TCP_SendData(uint8_t data[], uint16_t len);

void TCP_ClientStart(void);

#endif 