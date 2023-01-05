#include <stdint.h>
#include "chess.h"
#include "displayChess.h"
#include "check.h"

int8_t TURN = 0;   // turn这里是轮番的意思，决定现在是哪一方动子了
int8_t undoEn = 0; // 悔棋允许标志位，只能悔一步器，移子之后使能，悔棋之后禁止

ChangeBuffer changeBuffer2 =
    {
        -2,
        0,
        0,
        0,
        0,
        0};

// 棋子结构体数组初始化
PieceTypedef piece[32] =
    {
        {'車', 0, 0, blackCAMP, car, ALIVE},      // 0
        {'馬', 1, 0, blackCAMP, horse, ALIVE},    // 1
        {'象', 2, 0, blackCAMP, elephant, ALIVE}, // 2
        {'士', 3, 0, blackCAMP, guard, ALIVE},    // 3
        {'将', 4, 0, blackCAMP, general, ALIVE},  // 4
        {'士', 5, 0, blackCAMP, guard, ALIVE},    // 5
        {'象', 6, 0, blackCAMP, elephant, ALIVE}, // 6
        {'馬', 7, 0, blackCAMP, horse, ALIVE},    // 7
        {'車', 8, 0, blackCAMP, car, ALIVE},      // 8
        {'砲', 1, 2, blackCAMP, cannon, ALIVE},   // 9
        {'砲', 7, 2, blackCAMP, cannon, ALIVE},   // 10
        {'卒', 0, 3, blackCAMP, soldier, ALIVE},  // 11
        {'卒', 2, 3, blackCAMP, soldier, ALIVE},  // 12
        {'卒', 4, 3, blackCAMP, soldier, ALIVE},  // 13
        {'卒', 6, 3, blackCAMP, soldier, ALIVE},  // 14
        {'卒', 8, 3, blackCAMP, soldier, ALIVE},  // 15
        {'兵', 0, 6, redCAMP, soldier, ALIVE},    // 16
        {'兵', 2, 6, redCAMP, soldier, ALIVE},    // 17
        {'兵', 4, 6, redCAMP, soldier, ALIVE},    // 18
        {'兵', 6, 6, redCAMP, soldier, ALIVE},    // 19
        {'兵', 8, 6, redCAMP, soldier, ALIVE},    // 20
        {'炮', 1, 7, redCAMP, cannon, ALIVE},     // 21
        {'炮', 7, 7, redCAMP, cannon, ALIVE},     // 22
        {'车', 0, 9, redCAMP, car, ALIVE},        // 23
        {'马', 1, 9, redCAMP, horse, ALIVE},      // 24
        {'相', 2, 9, redCAMP, elephant, ALIVE},   // 25
        {'仕', 3, 9, redCAMP, guard, ALIVE},      // 26
        {'帅', 4, 9, redCAMP, general, ALIVE},    // 27
        {'仕', 5, 9, redCAMP, guard, ALIVE},      // 28
        {'相', 6, 9, redCAMP, elephant, ALIVE},   // 29
        {'马', 7, 9, redCAMP, horse, ALIVE},      // 30
        {'车', 8, 9, redCAMP, car, ALIVE}         // 31
};

// 棋盘数组，实时记录棋盘上棋子分布位置
int8_t pieceTable[9][10] =
    {
        {0, -1, -1, 11, -1, -1, 16, -1, -1, 23},
        {1, -1, 9, -1, -1, -1, -1, 21, -1, 24},
        {2, -1, -1, 12, -1, -1, 17, -1, -1, 25},
        {3, -1, -1, -1, -1, -1, -1, -1, -1, 26},
        {4, -1, -1, 13, -1, -1, 18, -1, -1, 27},
        {5, -1, -1, -1, -1, -1, -1, -1, -1, 28},
        {6, -1, -1, 14, -1, -1, 19, -1, -1, 29},
        {7, -1, 10, -1, -1, -1, -1, 22, -1, 30},
        {8, -1, -1, 15, -1, -1, 20, -1, -1, 31}};

// 以下7个函数分别对应于7种棋子的的路径判断
int8_t car(int8_t x1, int8_t y1, int8_t x2, int8_t y2, int8_t Camp) //
{
    int8_t index, i, temp;

    if (x1 != x2 && y1 != y2) // 不在同一直线上，不能移
        return 0;
    if (x1 == x2 && y1 == y2)
        return 0;

    index = pieceTable[x2][y2];
    if (index != -1 && piece[index].camp == Camp) // 目的地上已经有我方棋子，不能移
        return 0;

    if (x2 < 0 || y2 < 0 || x2 > 8 || y2 > 9)
        return 0;

    i = 1;
    if (x1 == x2) // 纵向移动
    {
        if (y1 > y2) // 如果y1>y2,将两值互换，保证y2是更大地那个
        {
            temp = y1;
            y1 = y2;
            y2 = temp;
        }
        while (pieceTable[x1][y1 + i] == -1) // 判断y1和y2之间有无其它棋子，有的话不能移
        {
            if (y1 + i == y2)
                break;
            i++;
        }
        if (y1 + i != y2)
            return 0;
    }
    else // 横向移动，原理同上
    {
        if (x1 > x2)
        {
            temp = x1;
            x1 = x2;
            x2 = temp;
        }
        while (pieceTable[x1 + i][y1] == -1)
        {
            if (x1 + i == x2)
                break;
            i++;
        }
        if (x1 + i != x2)
            return 0;
    }
    return 1;
}

int8_t horse(int8_t x1, int8_t y1, int8_t x2, int8_t y2, int8_t Camp) //(x1,y1):源坐标(x2,y2):目的坐标
{
    int8_t dx, dy, index, x3, y3;
    dx = x2 - x1;
    dy = y2 - y1;

    if (dx * dy != 2 && dx * dy != -2) // 马走日
        return 0;

    index = pieceTable[x2][y2];
    if (index != -1 && piece[index].camp == Camp)
        return 0;

    x3 = x1 + dx / 2;
    y3 = y1 + dy / 2;
    if (pieceTable[x3][y3] != -1)
        return 0;

    if (x2 < 0 || y2 < 0 || x2 > 8 || y2 > 9)
        return 0;

    return 1;
}

int8_t elephant(int8_t x1, int8_t y1, int8_t x2, int8_t y2, int8_t Camp)
{
    int8_t index, dx, dy;
    dx = x2 - x1;
    dy = y2 - y1;

    if ((dx != 2 && dx != -2) || (dy != 2 && dy != -2)) // x,y方向移动的距离一定是2
        return 0;

    index = pieceTable[x1 + dx / 2][y1 + dy / 2]; // 这里查看象脚处是否有棋子，有的话不能移动
    if (index != -1)
        return 0;

    if (x2 < 0 || y2 < 0 || x2 > 8 || y2 > 9)
        return 0;

    if (Camp) // 象不能过河
    {
        if (y2 > 4)
            return 0;
    }
    else
    {
        if (y2 < 5)
            return 0;
    }

    index = pieceTable[x2][y2];
    if (index != -1 && piece[index].camp == Camp) // 目的地有己方棋子，不能移
        return 0;

    return 1;
}

// 士
int8_t guard(int8_t x1, int8_t y1, int8_t x2, int8_t y2, int8_t Camp)
{
    int8_t index, dx, dy;
    dx = x2 - x1;
    dy = y2 - y1;

    if (x2 < 3 || x2 > 5)
        return 0;

    if (dx * dx + dy * dy != 2)
        return 0;

    if (Camp)
    {
        if (y2 > 2 || y2 < 0)
            return 0;
    }
    else
    {
        if (y2 < 7 || y2 > 9)
            return 0;
    }

    index = pieceTable[x2][y2];
    if (index != -1 && piece[index].camp == Camp)
        return 0;

    return 1;
}

// 将(帅)
int8_t general(int8_t x1, int8_t y1, int8_t x2, int8_t y2, int8_t Camp)
{
    int8_t index, dx, dy;
    dx = x2 - x1;
    dy = y2 - y1;

    index = pieceTable[x2][y2];
    if (index != -1 && piece[index].camp == Camp)
        return 0;

    if (x2 < 3 || x2 > 5)
        return 0;

    if (dx * dx + dy * dy != 1)
        return 0;

    if (Camp)
    {
        if (y2 > 2 || y2 < 0)
            return 0;
    }
    else
    {
        if (y2 < 7 || y2 > 9)
            return 0;
    }
    return 1;
}

// 炮
int8_t cannon(int8_t x1, int8_t y1, int8_t x2, int8_t y2, int8_t Camp)
{
    int8_t index, i, temp, pieceN, temp1;
    temp1 = pieceTable[x2][y2];
    pieceN = 0;

    if (x1 != x2 && y1 != y2) // 如果不在同一条直线上，不能移
        return 0;
    if (x1 == x2 && y1 == y2) // 原地移动
        return 0;
    if (x2 < 0 || y2 < 0 || x2 > 8 || y2 > 9)
        return 0;

    index = pieceTable[x2][y2];
    i = 1;

    if (x1 == x2) // 纵向移动
    {
        if (y1 > y2) // 如果y1>y2，将两值交换，保证y2是更大的那个
        {
            temp = y1;
            y1 = y2;
            y2 = temp;
        }
        for (i = 1; y1 + i < y2; i++) // y1和y2之间所有的位置（不包括y1，y2）都轮询一遍
        {
            if (pieceTable[x1][y1 + i] != -1) // 有棋子则pieceN++，以此统计两位置之间的棋子数
                pieceN++;
        }
    }
    else // 横向移动
    {
        if (x1 > x2)
        {
            temp = x1;
            x1 = x2;
            x2 = temp;
        }
        for (i = 1; x1 + i < x2; i++)
        {
            if (pieceTable[x1 + i][y1] != -1)
                pieceN++;
        }
    }

    if (pieceN > 1) // 此时pieceN表示源位置与目的位置之间有多少个棋子
        return 0;

    if ((pieceN == 1 && temp1 == -1) || (pieceN == 0 && temp1 != -1))
        return 0;

    if (index != -1 && piece[index].camp == Camp)
        return 0;

    return 1;
}

// 兵
int8_t soldier(int8_t x1, int8_t y1, int8_t x2, int8_t y2, int8_t Camp)
{
    int8_t index, dx, dy;
    dx = x2 - x1;
    dy = y2 - y1;

    if (dy * dy + dx * dx != 1)
        return 0;

    if (x2 < 0 || y2 < 0 || x2 > 8 || y2 > 9)
        return 0;

    if (Camp)
    {
        if (dy < 0) // 兵不能后退
            return 0;
        if (y1 <= 4 && dx != 0) // 兵没有过河
            return 0;
    }
    else
    {
        if (dy > 0)
            return 0;
        if (y1 >= 5 && dx != 0)
            return 0;
    }

    index = pieceTable[x2][y2];
    if (index != -1 && piece[index].camp == Camp)
        return 0;

    return 1;
}

// 移动棋子后的时候要把所移的棋子的信息和棋局数组进行更新
void change(int8_t index, int8_t x, int8_t y, ChangeBuffer *p)
{
    int8_t index2;

    p->indexSrc = index; // 把移动过程的数据保存到buffer里
    p->AX = piece[index].x;
    p->AY = piece[index].y;
    p->BX = x;
    p->BY = y;
    index2 = pieceTable[x][y];
    p->indexDes = index2;

    pieceTable[piece[index].x][piece[index].y] = -1; // 把棋子移走后，没有棋子索引用1表示
    if (index2 != -1)                                // 杀死目的地棋子
    {
        piece[index2].status = DEATH;
    }
    pieceTable[x][y] = index;
    piece[index].x = x;
    piece[index].y = y;
}

// 把change的所有效果回退
void backChange(ChangeBuffer *p)
{
    int8_t index, index2;
    index = p->indexSrc;
    index2 = p->indexDes;

    piece[index].x = p->AX;
    piece[index].y = p->AY;

    if (p->indexDes != -1)
    {
        piece[p->indexDes].status = ALIVE;
    }

    pieceTable[p->AX][p->AY] = index;
    pieceTable[p->BX][p->BY] = index2;
}
