#ifndef __FONT_H
#define __FONT_H 	   
 


extern const unsigned int Chinese48[];
extern const unsigned int Chinese64[];
extern const unsigned char GB_6464[][512]; 
#define PIECE_LENGTH 9216
#define BOARD_LENGTH 1019520 // 480*531*4
extern const unsigned char gImage_ba[PIECE_LENGTH], gImage_bb[PIECE_LENGTH], gImage_bc[PIECE_LENGTH],gImage_bk[PIECE_LENGTH],gImage_bn[PIECE_LENGTH],gImage_bp[PIECE_LENGTH],gImage_br[PIECE_LENGTH];
extern const unsigned char gImage_ra[PIECE_LENGTH], gImage_rb[PIECE_LENGTH], gImage_rc[PIECE_LENGTH],gImage_rk[PIECE_LENGTH],gImage_rn[PIECE_LENGTH],gImage_rp[PIECE_LENGTH],gImage_rr[PIECE_LENGTH];
extern const unsigned char gImage_board[BOARD_LENGTH];
extern const unsigned char *Chinese48_pointers[];

#endif

