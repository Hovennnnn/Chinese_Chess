#ifndef __CHECK_H__
#define __CHECK_H__

#include "stdint.h"

int8_t check(int8_t camp,int8_t *p);
int8_t generalLine();
int8_t judgePosition(int8_t pX,int8_t pY);
int8_t judgeWin(int8_t assassinIndex);
int8_t escapeFromCannon(int8_t index,int8_t assassinIndex);
uint8_t tryMove(uint8_t index,uint8_t x,uint8_t y);

#endif
