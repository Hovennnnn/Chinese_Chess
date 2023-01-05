#include <stdio.h>
#include "displayChess.h"
#include "chess.h"
#include "font.h"
#include "stm32469i_discovery_lcd.h"


extern int8_t selectIndex;
extern int touch_flag;
extern I2C_HandleTypeDef hi2c1;
extern const unsigned char *Chinese48_pointers[];
extern const unsigned char gImage_board[];

u16 width = 480;    //屏幕宽度
u16 height = 800;   //屏幕高度
u8 offsetX = 30;   //棋盘距离屏幕左右两边缘的差距
u16 offsetY1 = 28;   //棋盘距离屏幕上边缘的差距
u16 offsetY2 = 298;  //棋盘距离屏幕下边缘的差距
u8 pieceR = 24;     //棋子的半径
u8 lengthCH = 48;   //棋子上的字的边长
u8 lengthCH2 = 64;  //底部提示字的边长
u16 sX = 28;        //提示字的起始位置的横坐标
u16 sY = 720;       //提示字的起始位置的纵坐标
u8 sizeENG = 24;    //英语提示的字形（12，16，24）



unsigned char * Libbuttom  = (u8*)GB_6464;//下部提示字的字库选择
u32 CANVAS_COLOR = LCD_COLOR_WHITE;

//将棋盘格点坐标转换成屏幕的像素坐标
u16 turnPiX[9];
u16 turnPiY[10];


void initUI() {
	BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER_BACKGROUND);
	BSP_LCD_SetTextColor(CANVAS_COLOR);
	BSP_LCD_DrawRect(0, 531, width, 269);

	BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER_FOREGROUND);
	BSP_LCD_SetBackColor(BACK_COLOR_TRANSPARENT);		// 设置背景色为透明
	BSP_LCD_Clear(BACK_COLOR_TRANSPARENT);

	u8 divX, divY, i;

	divX = (width - 2 * offsetX) / 8;            //方格的宽度
	divY = (height - offsetY1 - offsetY2) / 9;   //方格的高度


	for (i = 0; i <= 8; i++)        
	{
		turnPiX[i] = offsetX + i * divX;
	}
	for (i = 0; i <= 9; i++) {
		turnPiY[i] = offsetY1 + i * divY;
	}


	initPiece();        //画每一个棋子
}

//制作一个新的棋子
void makePiece(int8_t index) {
	u16 piX, piY;
	piX = turnPiX[piece[index].x];     //获取像素点坐标
	piY = turnPiY[piece[index].y];
	pieceWrite((u8) index, lengthCH);      //画棋子
}

//擦除旧棋子
void clearPiece(int8_t x, int8_t y) {
	u16 piX, piY, tempColor;

	piX = turnPiX[x];
	piY = turnPiY[y];

	BSP_LCD_SetTextColor(BACK_COLOR_TRANSPARENT); //直接变透明
	BSP_LCD_FillCircle(piX, piY, pieceR); //擦除旧的棋子的痕迹
	BSP_LCD_SetTextColor((u32)(0x00FFFFFF));
	BSP_LCD_DrawRect(piX - pieceR, piY - pieceR, pieceR << 1, pieceR << 1); //取消半透明红矩形
	BSP_LCD_DrawRect(piX - pieceR - 1, piY - pieceR - 1, (pieceR + 1) << 1, (pieceR + 1) << 1); //画一个半透明红矩形
}

//初始化32个棋子
void initPiece(void) {
	u8 i;
	for (i = 0; i <= 31; i++) {
		makePiece(i);
	}
}

//当getPath()函数里面检测到有触摸的时候会调用，将对应的棋子涂色
void selectPiece(u8 index) {
	u16 tempPix, tempPiy;
	tempPix = turnPiX[piece[index].x];
	tempPiy = turnPiY[piece[index].y];

	BSP_LCD_SetTextColor((u32)(0x7FFF0000));
	BSP_LCD_DrawRect(tempPix - pieceR, tempPiy - pieceR, pieceR << 1, pieceR << 1); //画一个半透明红矩形
	BSP_LCD_DrawRect(tempPix - pieceR - 1, tempPiy - pieceR - 1, (pieceR + 1) << 1, (pieceR + 1) << 1); //画一个半透明红矩形
}

//把半透明的选择矩形清除
void deSelectPiece(u8 index) {

	u16 tempPix, tempPiy;
	tempPix = turnPiX[piece[index].x];
	tempPiy = turnPiY[piece[index].y];
	BSP_LCD_SetTextColor(BACK_COLOR_TRANSPARENT);
	BSP_LCD_DrawRect(tempPix - pieceR, tempPiy - pieceR, pieceR << 1, pieceR << 1); //取消半透明红矩形
	BSP_LCD_DrawRect(tempPix - pieceR - 1, tempPiy - pieceR - 1, (pieceR + 1) << 1, (pieceR + 1) << 1); //画一个半透明红矩形
	pieceWrite(index, lengthCH);
}

//获得玩家选择的移子的路径
void getPath(Path *p) {
	u8 pressX, pressY;		// 存储移动后的格点
	int8_t getFlag, index;
	u8 divX, divY;			//方格的宽和高

	divX = (width - 2 * offsetX) / 8;
	divY = (height - offsetY1 - offsetY2) / 9;

	selectIndex = -2;
	getFlag = 0;
	u8 buf[10] = { 0 }; //用来存放读取触摸屏寄存器数据的buffer
	while (1) {
		if (touch_flag) {
			u8 len;
			u8 uart_buf[20];
			touch_flag = 0;
			if (HAL_I2C_Mem_Read(&hi2c1, 0x54, 0x03, 1, buf, 4, 1000) != HAL_OK)	continue;

			u16 xPos = (u16) (((buf[0] & 0xf) << 8) + buf[1]); // 触摸点的像素x坐标
			u16 yPos = (u16) (((buf[2] & 0xf) << 8) + buf[3]); // 触摸点的像素y坐标
			u8 status = (u8) (buf[0] >> 6);
			if(status == 0x01 || status == 0x03) continue; //如果是手指抬起的中断，则直接跳过

			pressX = (xPos - offsetX + 0.5 * divX) / divX; //将触屏位置的像素坐标改为棋局的格点坐标（pressX，pressY）
			pressY = (yPos - offsetY1 + 0.5 * divY) / divY;

			if (pressX > 8 || pressY > 9) continue;			//超出范围，重新选取位置


			index = pieceTable[pressX][pressY];

			if (!getFlag)			//还没有选中棋子
			{
				if (index != -1 && piece[index].camp == TURN) // 如果选中了棋子，并且当前是该方下棋，则选中合理
				{
					selectPiece(index);
					selectIndex = index;
					getFlag = 1;
				}
			}
			else //已经选中了棋子，要选择将要移动到的位置了
			{
				if (index != -1 && piece[index].camp == TURN) //如果此时又点击了己方棋子
					{
					deSelectPiece(selectIndex); //把之前选中的棋子取消选中
					if (index != selectIndex) //第二次选中的和之前的不是同一个，则把第二次当作选中的棋子
							{
						selectIndex = index;
						selectPiece(index);
					} else
						//第二次选中的和之前的是同一个，重新选择棋子
						getFlag = 0;
				} else //成功获取到了玩家输入的路径，赋值到p指向的对象，然后返回
				{
					p->indexS = selectIndex;
					p->x = pressX;
					p->y = pressY;
					return;
				}
			}

//			HAL_Delay(500);

			while (status == 0x00 || status == 0x02) //等待手指离开屏幕
			{
				if (HAL_I2C_Mem_Read(&hi2c1, 0x54, 0x03, 1, buf, 1, 1000) != HAL_OK)
					continue;
				status = (u8) (buf[0] >> 6);
			}
		}
	}
}

//在给定的坐标填充棋子
void pieceWrite(uint8_t index, uint8_t length) {
	u16 x, y, camp;
	u16 ex, ey;
	u8 index1, i, j;
	u8 byteColNum; //一列的字节数
	u16 byteNum;
	unsigned int GBK;

	x = turnPiX[piece[index].x];
	y = turnPiY[piece[index].y];
	GBK = piece[index].name;
	camp = piece[index].camp;

	index1 = 0;

	while (GBK != Chinese48[index1] && index1 < 32) {
		index1++;
	}

	if (camp)               //如果是黑色阵营
	{
		ex = x - 0.5 * length;
		ey = y - 0.5 * length;
		for (i = 0; i < length; ++i) {
			for (j = 0; j < length; ++j) {
				u16 bytes_to_pass = (j * length + i) * 4;
				unsigned char *gImage = Chinese48_pointers[index1];
				u32 color0 = (u32) gImage[bytes_to_pass] << 24;
				u32 color1 = (u32) gImage[bytes_to_pass + 1] << 16;
				u32 color2 = (u32) gImage[bytes_to_pass + 2] << 8;
				u32 color3 = (u32) gImage[bytes_to_pass + 3];
				u32 color = color0 + color1 + color2 + color3;
				BSP_LCD_DrawPixel(ex + i, ey + j, color);
			}
		}
	} else                  //如果是红色阵营，颜色要变，棋子显示的方向也变
	{
		ex = x - 0.5 * length;
		ey = y - 0.5 * length;
		for (i = 0; i < length; ++i) {
			for (j = 0; j < length; ++j) {
				u16 bytes_to_pass = (j * length + i) * 4;
				unsigned char *gImage = Chinese48_pointers[index1];
				u32 color0 = (u32) gImage[bytes_to_pass] << 24;
				u32 color1 = (u32) gImage[bytes_to_pass + 1] << 16;
				u32 color2 = (u32) gImage[bytes_to_pass + 2] << 8;
				u32 color3 = (u32) gImage[bytes_to_pass + 3];
				u32 color = color0 + color1 + color2 + color3;
				BSP_LCD_DrawPixel(ex + i, ey + j, color);
			}
		}
	}
}

//写屏幕底部提示作用的汉字
void write_Chinese(u16 x,u16 y,u16 GBK,u32 color,uint8_t * lib,uint8_t length)
{
	int8_t index1,i;
	u16 j;
	u8 byteColNum; //一列的字节数
	u16 byteNum;
	index1 = 0;

    byteColNum = (length/8.0) + ((length%8)?1:0);
	byteNum = length * byteColNum;   //一个字所用的字节数

	while(GBK != (u16)Chinese64[index1])  //寻找GBK对应的汉字在Chinese64[]里的哪一个位置
	{
		index1++;
	}

	for(j=0;j<=byteNum;j++)
	{
		for(i=7;i>=0;i--)
		{
			if( lib[index1*byteNum+j] >> i & 0x01)
				BSP_LCD_DrawPixel(x,y,color);
			else
				BSP_LCD_DrawPixel(x,y,BACK_COLOR_TRANSPARENT);
			y++;
		}
		if((j+1)%byteColNum == 0)     //显示完一列之后，列数+1，显示下一列，行数复位到开始值
		{
			x++;
			y = y-length;
		}

	}
}

