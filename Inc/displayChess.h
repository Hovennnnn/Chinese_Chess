#ifndef __DISPLAYCHESS_H__
#define __DISPLAYCHESS_H__

//#include <sys.h>
#include <config.h>


//路径结构体
typedef struct 
{
    int8_t indexS; //玩家选定的棋子
    int8_t x;      //把选定的棋子进行移动，目的地的坐标（x，y）
    int8_t y;
}Path;



void initUI(void);
extern void debugUI(u8 i,u8 j,u8 k);
void initPiece(void);
void drawSelection(void);
void clearSelection(void);
void selectPiece(u8 index);
void deSelectPiece(u8 index);
void makePiece(int8_t index);
void clearPiece(int8_t x,int8_t y);
void getPath(Path *p);


extern u8 lengthCH;
extern u8 lengthCH2;
extern u8 pieceR;
extern u16 sX;
extern u16 sY;
extern u8 sizeENG;
extern unsigned char * Libbuttom;
extern u16 turnPiX[9];
extern u16 turnPiY[10];

#define BACK_COLOR_TRANSPARENT (u32)0x00FFFFFF

#endif
