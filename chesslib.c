#include <stdio.h>
#include <stdlib.h>

#include "chesslib.h"

int checkSquareSafety(struct Piece board[][8],  int xCoord, int yCoord,
                      int pieceColor);

void makeMove(struct Piece board[][8], struct Move *pmove) // Moves the piece and clears the old square.
{
	board[pmove->x2][pmove->y2] = board[pmove->x1][pmove->y1];
    board[pmove->x1][pmove->y1].icon = ' ';
	board[pmove->x1][pmove->y1].color = EMPTY;
    board[pmove->x1][pmove->y1].moves = 0;
}

void promotePawn(struct Piece board[][8], struct Move *pmove) //Changes pawn type
{
    const char *prom_icon = " QRBN";
    board[pmove->x2][pmove->y2].icon = prom_icon[pmove->pawn_prom];

}

void castle(struct Piece board[][8], struct Move *pmove) // Makes the castle move
{
	struct Piece currentPiece = board[pmove->x1][pmove->y1];
	struct Piece newSquare = board[pmove->x2][pmove->y2];
    struct Piece empty = {3, ' ', 0};
    int row = (currentPiece.color == WHITE) ? 0 : 7;
    
    if (pmove->x2 == 0)
    {
        board[2][row] = currentPiece;
        board[3][row] = newSquare;
        board[1][row] = empty;
        board[0][row] = empty;
		board[4][row] = empty;
    }
    else if (pmove->x2 == 7)
    {
		board[6][row] = currentPiece;
		board[5][row] = newSquare;
        board[4][row] = empty;
        board[7][row] = empty;
    }
}

int checkCastling(struct Piece board[][8], struct Move *pmove) // Checks if castling is possible
{
	struct Piece currentPiece = board[pmove->x1][pmove->y1];
	struct Piece newSquare = board[pmove->x2][pmove->y2];
	if (currentPiece.color == WHITE && newSquare.color == WHITE) //Sees which side is castling
    {
		if (newSquare.icon == 'R' && (newSquare.moves == 0 && //Makes sure castling is possible
                                      currentPiece.moves == 0))
        {
			if (pmove->x2 == 0) { // Checks which side to castle
				if ((board[1][0].icon == ' ' &&  board[2][0].icon == ' ' &&
                     board[3][0].icon == ' ') &&
					!(checkSquareSafety(board, 2, 0, WHITE) &&
					checkSquareSafety(board, 3, 0, WHITE))
                    )
                {
                    return 1;
                }
			}
            else if (pmove->x2 == 7)
            {
				if ((board[6][0].icon == ' ' && board[5][0].icon == ' ') &&
                    !(checkSquareSafety(board, 6, 0, WHITE) &&
					checkSquareSafety(board, 5, 0, WHITE))
                    )
                {
					return 1;
				}
			}
		}
	}
	else if (currentPiece.color == BLACK && newSquare.color == BLACK)
    {
		if (newSquare.icon == 'R' && (newSquare.moves == 0 &&
                                      currentPiece.moves == 0))
        {
			if (pmove->x2 == 0)
            {
				if ((board[1][7].icon == ' ' &&
					board[2][7].icon == ' ' &&
					board[3][7].icon == ' ') &&
					!(checkSquareSafety(board, 2, 7, BLACK) &&
					checkSquareSafety(board, 3, 7, BLACK)
					))
                {
					return 1;
				}
			}
			else if (pmove->x2 == 7)
            {
				if ((board[6][7].icon == ' ' && board[5][7].icon == ' ') &&
					!(checkSquareSafety(board, 6, 7, BLACK) &&
					checkSquareSafety(board, 5, 7, BLACK)
					))
                {
					return 1;
				}
			}
		}
	}
	return 0;
}

int checkPawnMove(struct Piece board[][8], struct Piece **pieceTracker,
                  struct Move *pmove) // Checks if pawn move is valid
{
    struct Piece currentPiece = board[pmove->x1][pmove->y1];
    struct Piece newSquare = board[pmove->x2][pmove->y2];
    int one, two, enpassant_row;
    
    if (currentPiece.color == WHITE)
    {
        one = 1;
        two = 2;
        enpassant_row = 4;
    }
    else
    {
        one = -1;
        two = -2;
        enpassant_row = 3;
    }
    
    if (pmove->y1 + one == pmove->y2 && pmove->x1 == pmove->x2)
    {
        // move forward without capturing opponent:
        if (newSquare.color == EMPTY)
        {
            makeMove(board, pmove);
            return 1;
        }
    }
	else if (pmove->y1 + two == pmove->y2 && pmove->y2 == 7 - enpassant_row) 
	{
		if (newSquare.color == EMPTY)
		{
			makeMove(board, pmove);
			return 1;
		}
	}
    else if (pmove->y1 + one == pmove->y2 &&
             (pmove->x1 + 1 == pmove->x2 || pmove->x1 - 1 == pmove->x2))
    {
        // diagonal move
        if (newSquare.color != currentPiece.color && newSquare.icon != ' ')
        {
            // with capture:
            makeMove(board, pmove);
            return 1;
        }
        else if (&(board[pmove->x2][pmove->y1]) == *pieceTracker &&
                 ((*pieceTracker)->icon == 'P' && (*pieceTracker)->moves == 1)
                 && pmove->y1 == enpassant_row)
        {
            // en passant:
            makeMove(board, pmove);
            (*pieceTracker)->icon = ' ';
            (*pieceTracker)->color = EMPTY;
            (*pieceTracker)->moves = 0;
            return 1;
        }
    }
    
	return 0;
}

// checkBishopMove: Tests if bishop move to square
int checkBishopMove(struct Piece board[][8], struct Move *pmove)
{
    int i, j;
    struct Piece currentPiece = board[pmove->x1][pmove->y1];
    struct Piece newSquare = board[pmove->x2][pmove->y2];
	if (!(pmove->x1 - pmove->y1 == pmove->x2 - pmove->y2 || 
          pmove->x1 + pmove->y1 == pmove->x2 + pmove->y2))
    {
		return 0;
	}
	if (currentPiece.color == newSquare.color)
	{
		return 0;
	}
    
	for (i = pmove->x1 + 1, j = pmove->y1 + 1; i <= pmove->x2; i++, j++)
	{
		if (board[i][j].color != EMPTY && i < pmove->x1)
		{
			break;
		}
		else if (i == pmove->x2)
		{
			return 1;
		}
	}

	for (i = pmove->x1 + 1, j = pmove->y1 - 1; i <= pmove->x2; i++, j--)
	{
		if (board[i][j].color != EMPTY && i > pmove->x1)
		{
			break;
		}
		else if (i == pmove->x2)
		{
			return 1;
		}
	}
	
	for (i = pmove->x1 - 1, j = pmove->y1 + 1; i >= pmove->x2; i--, j++)
	{
		if (board[i][j].color != EMPTY && i > pmove->x2)
		{
			break;
		}
		else if (i == pmove->x2)
		{
			return 1;
		}
	}
	
	for (i = pmove->x1 - 1, j = pmove->y1 - 1; i >= pmove->x2; i--, j--)
	{
		if (board[i][j].color != EMPTY && i < pmove->x1)
		{	
			break;
		}
		else if (i == pmove->x2)
		{
			return 1;
		}
	}
	
	return 0;
}
// checkKnightMove: Tests if valid knight move.
int checkKnightMove(struct Piece board[][8], struct Move *pmove) 
{
    struct Piece currentPiece = board[pmove->x1][pmove->y1];
    struct Piece newSquare = board[pmove->x2][pmove->y2];
    int move_x, move_y;

    if (currentPiece.color == newSquare.color) return 0;
    
    move_x = abs(pmove->x1 - pmove->x2);
    move_y = abs(pmove->y1 - pmove->y2);
    
    if (((move_x == 1) && (move_y == 2)) ||
        ((move_x == 2) && (move_y == 1))) return 1;

    return 0;
}
// checkRookMove: Tests if valid rook move.
int checkRookMove(struct Piece board[][8], struct Move *pmove) // Checks if rook move to square.
{
    int start, end;
    struct Piece currentPiece = board[pmove->x1][pmove->y1];
    struct Piece newSquare = board[pmove->x2][pmove->y2];

    if (currentPiece.color == newSquare.color) return 0;
    
	if (pmove->y2 == pmove->y1)
	{
        if (pmove->x2 > pmove->x1)
        {
            start = pmove->x1;
            end = pmove->x2;
        }
        else
        {
            start = pmove->x2;
            end = pmove->x1;
        }
        
        if (start + 1 == end) return 1;

		for (int i = start + 1; i <= end; i++)
		{
			if (board[i][pmove->y1].color != EMPTY) return 0;
		}
        return 1;
	}
    
    if (pmove->x2 == pmove->x1)
    {
        if (pmove->y2 > pmove->y1)
        {
            start = pmove->y1;
            end = pmove->y2;
        }
        else
        {
            start = pmove->y2;
            end = pmove->y1;
        }
        
        if (start + 1 == end) return 1;

        for (int i = start + 1; i <= end; i++)
        {
            if (board[pmove->x1][i].color != EMPTY) return 0;
        }
        return 1;
    }
    
    return 0;
}
// checkQueenMove: Tests if valid queen move.
int checkQueenMove(struct Piece board[][8], struct Move *pmove) 
{
    if (checkRookMove(board, pmove) || checkBishopMove(board, pmove))
    {
        return 1;
    }
    return 0;
}
// checkKingMove: Tests if valid king move.
int checkKingMove(struct Piece board[][8], struct Move *pmove)
{
    struct Piece currentPiece = board[pmove->x1][pmove->y1];
    struct Piece newSquare = board[pmove->x2][pmove->y2];
	if (abs(pmove->x1 - pmove->x2) <= 1 && abs(pmove->y1 - pmove->y2) <= 1 &&
        currentPiece.color != newSquare.color)
	{
        return 1;
	}
	return 0;
}
// checkKnightMove: Locates king
void findKing(struct Piece board[][8], int pieceColor, int *x, int *y)
{
	for (int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			if (board[i][j].color == pieceColor && board[i][j].icon == 'K') 
			{
				*x = i;
				*y = j;
				return;
			}		
		}
	}
    
	return;
}


//TODO: These are static, will put them as static later.
// isKingSurronded: Tests if king can move to any square.
int isKingSurronded(struct Piece board[][8], int turn)
{
    int kx, ky, x1, x2, y1, y2, i, j;
    
	findKing(board, turn, &kx, &ky);
    x1 = (kx > 0) ? kx - 1 : 0;
    x2 = (kx < 7) ? kx + 1 : 7;
    y1 = (ky > 0) ? ky - 1 : 0;
    y2 = (ky < 7) ? ky + 1 : 7;
    
    for (i = x1; i <= x2; i++)
        for (j = y1; j <= y2; j++)
            if ((i != kx || j != ky) && board[i][j].color != turn &&
                !checkSquareSafety(board, i, j, turn))
            {
                return 0;
            }
    return 1;
}

// Tests if king is underattack
int revealsKing(struct Piece board[][8], int x1, int y1, int x2, int y2)
{
	struct Piece next_board[8][8];
    struct Move move;
    int kx, ky;
    
    move.x1 = x1;
    move.y1 = y1;
    move.x2 = x2;
    move.y2 = y2;
    
    int pieceColor = board[x1][y1].color;
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
            next_board[i][j] = board[i][j];
	makeMove(next_board, &move);
	findKing(next_board, pieceColor, &kx, &ky);
	if (checkSquareSafety(next_board, kx, ky, pieceColor))
	{
		return 1;
	}
	return 0;
}

// kingAttack: Tests to see if enemy king is attacking a square
int kingAttack(struct Piece board[][8], int xCoord, int yCoord, int pieceColor)
{
    int x1 = (xCoord > 0) ? xCoord - 1 : 0;
    int x2 = (xCoord < 7) ? xCoord + 1 : 7;
    int y1 = (yCoord > 0) ? yCoord - 1 : 0;
    int y2 = (yCoord < 7) ? yCoord + 1 : 7;
    
    for (int i = x1; i <= x2; i++)
    {
        for (int j = y1; j <= y2; j++)
        {
            if (i == xCoord && j == yCoord)
            {
                continue;
            }
            if (board[i][j].color != pieceColor && board[i][j].icon == 'K')
            {
                return 1;
            }
        }
    }
    return 0;
}
// kingAttack: Tests to see if enemy king is attacking a square.
struct atkInfo knightAttack(struct Piece board[][8], int xCoord, int yCoord,
                            int pieceColor)
{
    struct atkInfo knightLoc = {0, -1, -1};
    int move_x[8] = {-2, -2, -1, -1,  1, 1,  2, 2};
    int move_y[8] = {-1,  1, -2,  2, -2, 2, -1, 1};
    int x, y;
    
    for (int i = 0; i < 8; i++)
    {
        x = xCoord + move_x[i];
        y = yCoord + move_y[i];
        
        if (x >= 0 && x <= 7 && y >= 0 && y <= 7 &&
            board[x][y].color != pieceColor && board[x][y].icon == 'N')
        {
            knightLoc.isAttack = 1;
            knightLoc.x = x;
            knightLoc.y = y;
            return knightLoc;
        }
    }
    return knightLoc;
}

// kingAttack: Tests to see if enemy king is attacking a square.
struct atkInfo rookAndQueenAttack(struct Piece board[][8],
                                  int xCoord, int yCoord, int pieceColor)
{
    struct atkInfo rookLoc = {0, -1, -1};
	for (int i = xCoord + 1; i < 8; i++)
	{
		if ((board[i][yCoord].icon == 'R' || board[i][yCoord].icon == 'Q') &&
            board[i][yCoord].color != pieceColor)
		{
			rookLoc.isAttack = 1;
			rookLoc.x = i;
			rookLoc.y = yCoord;
			return rookLoc;
		}
		else if (board[i][yCoord].icon != ' ') 
		{
			break;
		}
	}
	for (int i = xCoord - 1; i >= 0; i--)
	{
		if ((board[i][yCoord].icon == 'R' || board[i][yCoord].icon == 'Q') &&
            board[i][yCoord].color != pieceColor)
		{
			rookLoc.isAttack = 1;
			rookLoc.x = i;
			rookLoc.y = yCoord;
			return rookLoc;
		}
		else if (board[i][yCoord].icon != ' ')
		{
			break;
		}
	}
	for (int i = yCoord + 1; i < 8; i++)
	{
		if ((board[xCoord][i].icon == 'R' || board[i][yCoord].icon == 'Q') &&
            board[i][yCoord].color != pieceColor)
		{
			rookLoc.isAttack = 1;
			rookLoc.x = xCoord;
			rookLoc.y = i;
			return rookLoc;
		}
		else if (board[xCoord][i].icon != ' ')
		{
			break;
		}
	}
	for (int i = yCoord - 1; i >= 0; i--)
	{
		if ((board[xCoord][i].icon == 'R' || board[xCoord][i].icon == 'Q') &&
            board[xCoord][i].color != pieceColor)
		{
			rookLoc.isAttack = 1;
			rookLoc.x = xCoord;
			rookLoc.y = i;
			return rookLoc;
		}
		else if (board[xCoord][i].icon != ' ')
		{
			break;
		}
	}

    return rookLoc;
} 

// bishopAttack: Tests to see if enemy bishop is attacking a square.
struct atkInfo bishopAndQueenAttack(struct Piece board[][8],
                                    int xCoord, int yCoord, int pieceColor)
{
    int i, j;
    struct atkInfo bishopLoc = {0, -1, -1};
	for (i = xCoord + 1, j = yCoord + 1; i < 8 && j < 8; i++, j++)
	{
		if ((board[i][j].icon == 'B' || board[i][j].icon == 'Q') &&
            board[i][j].color != pieceColor)
		{
			bishopLoc.isAttack = 1;
			bishopLoc.x = i;
			bishopLoc.y = j;
			return bishopLoc;
		}
		else if (board[i][j].icon != ' ')
		{
			break;
		}
	}
	for (i = xCoord - 1, j = yCoord + 1; i >= 0 && j < 8; i--, j++)
	{
		if ((board[i][j].icon == 'B' || board[i][j].icon == 'Q') &&
            board[i][j].color != pieceColor)
		{
			bishopLoc.isAttack = 1;
			bishopLoc.x = i;
			bishopLoc.y = j;
			return bishopLoc;
		}
		else if (board[i][j].icon != ' ')
		{
			break;
		}
	}
	for (i = xCoord + 1, j = yCoord - 1; i < 8 && j >= 0; i++, j--)
	{
		if ((board[i][j].icon == 'B' || board[i][j].icon == 'Q') &&
            board[i][j].color != pieceColor)
		{
			bishopLoc.isAttack = 1;
			bishopLoc.x = i;
			bishopLoc.y = j;
			return bishopLoc;
		}
		else if (board[i][j].icon != ' ')
		{
			break;
		}
	}
	for (i = xCoord - 1, j = yCoord - 1; i >= 0 && j >= 0; i--, j--)
	{
		if ((board[i][j].icon == 'B' || board[i][j].icon == 'Q') &&
            board[i][j].color != pieceColor)
		{
			bishopLoc.isAttack = 1;
			bishopLoc.x = i;
			bishopLoc.y = j;
			return bishopLoc;
		}
		else if (board[i][j].icon != ' ')
		{
			break;
		}
	}

    return bishopLoc;
}

// pawnAttack: Tests to see if enemy pawn is attacking a square.
struct atkInfo pawnAttack(struct Piece board[][8], int xCoord, int yCoord,
                          int pieceColor)
{
    struct atkInfo pawnLoc = {0, -1, -1};
    if (board[xCoord + 1][yCoord + 1].icon == 'P' &&
        board[xCoord + 1][yCoord + 1].color == 1 - pieceColor)
    {
        pawnLoc.isAttack = 1;
        pawnLoc.x = xCoord + 1;
        pawnLoc.y = yCoord + 1;
        return pawnLoc;
    }
    else if (board[xCoord - 1][yCoord + 1].icon == 'P' &&
             board[xCoord - 1][yCoord + 1].color == 1 - pieceColor)
    {
        pawnLoc.isAttack = 1;
        pawnLoc.x = xCoord - 1;
        pawnLoc.y = yCoord + 1;
        return pawnLoc;
    }

	return pawnLoc;
}

//Turns coordinates into move struct
struct Move CreateMove(int x1, int y1, int x2, int y2)
{
    struct Move move;
    move.x1 = x1;
    move.x2 = x2;
    move.y1 = y1;
    move.y2 = y2;
    return move;
}

//Checks if any piece can move to that square
int blockSquare(struct Piece board[][8], int xCoord, int yCoord, int pieceColor)
{
    struct atkInfo storeKnight, storeBishop, storeRook;
	int oppositeColor = pieceColor == WHITE ? BLACK : WHITE;
    
	storeKnight = knightAttack(board, xCoord, yCoord, oppositeColor);
	storeBishop = bishopAndQueenAttack(board, xCoord, yCoord, oppositeColor);
	storeRook = rookAndQueenAttack(board, xCoord, yCoord, oppositeColor);
    
	if (storeKnight.isAttack &&
        !revealsKing(board, storeKnight.x, storeKnight.y, xCoord, yCoord))
        return 1;

	if (storeBishop.isAttack &&
        !revealsKing(board, storeBishop.x, storeBishop.y, xCoord, yCoord))
        return 1;

    if (storeRook.isAttack &&
        !revealsKing(board, storeRook.x, storeRook.y, xCoord, yCoord))
        return 1;

    if (pieceColor == WHITE)
    {
		if (board[xCoord][yCoord - 1].icon == 'P' &&
            board[xCoord][yCoord - 1].color == WHITE)
		{
			if (!revealsKing(board, xCoord, yCoord - 1, xCoord, yCoord))
                return 1;
		}
		else if (board[xCoord][yCoord - 2].icon == 'P' &&
                 board[xCoord][yCoord - 2].color == WHITE && yCoord == 3)
		{
            if (!revealsKing(board, xCoord, yCoord - 2, xCoord, yCoord))
                return 1;
		}
	}
	else if (pieceColor == BLACK)
	{
		if (board[xCoord][yCoord + 1].icon == 'P' &&
            board[xCoord][yCoord + 1].color == BLACK)
		{
            if (!revealsKing(board, xCoord, yCoord + 1, xCoord, yCoord))
                return 1;
		}
		else if (board[xCoord][yCoord + 2].icon == 'P' &&
                 board[xCoord][yCoord + 2].color == BLACK && yCoord == 4)
		{
            if (!revealsKing(board, xCoord, yCoord + 2, xCoord, yCoord))
                return 1;
		}
	}

	return 0;
}

// blockBishopAttack: Checks if the bishop attack can be blocked.
int blockBishopAttack(struct Piece board[][8], int xCoord, int yCoord,
                      int kingColor)
{
    int x, y, kx, ky;
	
    findKing(board, kingColor, &kx, &ky);

	if (xCoord > kx && yCoord > ky)
    {
		for (x = kx + 1, y = ky + 1; x < xCoord; x++, y++)
			if (blockSquare(board, x, y, kingColor)) return 1;
    }
	else if (xCoord > kx && yCoord < ky)
	{
		for (x = kx + 1, y = ky - 1; x < xCoord; x++, y--)
			if (blockSquare(board, x, y, kingColor)) return 1;
	}
	else if (xCoord < kx && yCoord > ky)
	{
		for (x = kx - 1, y = ky + 1; x > xCoord; x--, y++)
			if (blockSquare(board, x, y, kingColor)) return 1;
	}
	else if (xCoord < kx && yCoord < ky)
	{
		for (x = kx - 1, y = ky - 1; x > xCoord; x--, y--)
			if (blockSquare(board, x, y, kingColor)) return 1;
    }
	return 0;
}

// blockRookAttack: Checks if the rook attack can be blocked.
int blockRookAttack(struct Piece board[][8], int xCoord, int yCoord,
                    int kingColor)
{
    int x, y, kx, ky;
	findKing(board, kingColor, &kx, &ky);

	if (xCoord > kx && yCoord == ky)
	{
		for (x = kx + 1; x < xCoord; x++)
			if (blockSquare(board, x, ky, kingColor)) return 1;
	}
	else if (xCoord < kx && yCoord == ky)
	{
		for (x = kx - 1; x < xCoord; x--)
			if (blockSquare(board, x, ky, kingColor)) return 1;
	}
	else if (xCoord == kx && yCoord > ky)
	{
		for (y = kx + 1; y > yCoord; y++)
			if (blockSquare(board, ky, y, kingColor)) return 1;
	}
	else if (xCoord == kx && yCoord < ky)
	{
		for (y = ky - 1; y < yCoord; y--)
            if (blockSquare(board, kx, y, kingColor)) return 1;
	}
	return 0;
}

// checkSquareSafety: Sees if square is underattack
int checkSquareSafety(struct Piece board[][8], int xCoord, int yCoord, int turn)
{
	if (knightAttack(board, xCoord, yCoord, turn).isAttack) return 1;
	if (rookAndQueenAttack(board, xCoord, yCoord, turn).isAttack) return 1;
    if (bishopAndQueenAttack(board, xCoord, yCoord, turn).isAttack) return 1;
    if (pawnAttack(board, xCoord, yCoord, turn).isAttack) return 1;
	if (kingAttack(board, xCoord, yCoord, turn)) return 1;
	return 0;
}

// checkForMate: looks to see if king in mate.
int checkForMate(struct Piece board[][8], int pieceColor)
{
	int i, x, y, kx, ky, attacks = 0, oppositeColor = 1 - pieceColor;
    struct atkInfo knightDefense, bishopDefense, rookDefense, pawnDefense;
    struct atkInfo storePiece[4];

	findKing(board, pieceColor, &kx, &ky);
    storePiece[0] = bishopAndQueenAttack(board, kx, ky, pieceColor);
    storePiece[1] = rookAndQueenAttack(board, kx, ky, pieceColor);
    storePiece[2] = knightAttack(board, kx, ky, pieceColor);
    storePiece[3] = pawnAttack(board, kx, ky, pieceColor);
    
    for (i = 0; i < 4; i++)
    
		attacks += storePiece[i].isAttack; 
	// If king has squares to move to not checkmate
    if (!isKingSurronded(board, pieceColor))
        return 0;
    // If king is not under check
	if (!attacks) return 0;
	// If king is in double check and can't move checkmate
	if (attacks == 2)
        return (isKingSurronded(board, pieceColor)) ? 1 : 0;
   
    
    for (i = 0; i < 4; i++)
    {
        if (storePiece[i].isAttack)
        {
            x = storePiece[i].x;
            y = storePiece[i].y;
			// Sees if any pieces can attack the attacking piece.
            knightDefense = knightAttack(board, x, y, oppositeColor);
            bishopDefense = bishopAndQueenAttack(board, x, y, oppositeColor);
            rookDefense = rookAndQueenAttack(board, x, y, oppositeColor);
            pawnDefense = pawnAttack(board, x, y, oppositeColor);
            // TODO: This should return 0 since the attack was blocked
            if (i == 0 && blockBishopAttack(board, x, y, pieceColor)) continue;
            if (i == 1 && blockRookAttack(board, x, y, pieceColor)) continue;
            // Sees if taking attacking piece will reveal the king.
            if (knightDefense.isAttack &&
                !revealsKing(board, knightDefense.x, knightDefense.y, x, y))
            {
                return 0;
            }
            if (bishopDefense.isAttack &&
                !revealsKing(board, bishopDefense.x, bishopDefense.y, x, y))
            {
                return 0;
            }
            if (rookDefense.isAttack &&
                !revealsKing(board, rookDefense.x, rookDefense.y, x, y))
            {
                return 0;
            }
            if (pawnDefense.isAttack &&
                !revealsKing(board, pawnDefense.x, pawnDefense.y, x, y))
            {
                return 0;
            }
            return 1;
        }
    }

	return 0;
}

int checkMoveValidity(struct Piece board[][8], struct Piece **pieceTracker,
                      struct Move *pmove, int turn)
{
	struct Piece currentPiece = board[pmove->x1][pmove->y1];
	struct Piece *newSquare = &(board[pmove->x2][pmove->y2]);
    
    // check that there is an actual piece at specified location:
    if (currentPiece.color == EMPTY)
    {
        printf("ERROR: There is no piece at specified position.");
        return 0;
    }
    
    // check that the piece has correct color for this turn:
    if (currentPiece.color != turn)
    {
        if (turn == WHITE)
        {
            printf("ERROR: It's whites' turn. You are trying to move a black"\
                " piece.");
        }
        else
        {
            printf("ERROR: It's blacks' turn. You are trying to move a white"\
                   " piece.");
        }
        return 0;
    }
    
	// checks if moving the piece will reveal king
	if (revealsKing(board, pmove->x1, pmove->y1, pmove->x2, pmove->y2))
    {
        printf("ERROR: Move reveals King.");
		return 0;
	}
	
	// this checks if that move matches the piece type
	if (currentPiece.icon == 'P')
    {
		if (checkPawnMove(board, pieceTracker, pmove))
        {
			(*pieceTracker) = newSquare;
			newSquare->moves++;
			if (pmove->y2 == 0 || pmove->y2 == 7)
            {
				promotePawn(board, pmove);
			}
            else if (pmove->pawn_prom)
            {
                printf("ERROR: Pawn cannot be promoted yet.");
                return 0;
            }
			return 1;
		}
	}
	else if (currentPiece.icon == 'K')
    {
		if (checkKingMove(board, pmove))
		{
			makeMove(board, pmove);
			pieceTracker = &newSquare;
			newSquare->moves++;
			return 1;
		}
		if (checkCastling(board, pmove))
		{
			castle(board, pmove);
			pieceTracker = &newSquare;
			return 1;
		}
	}
    else if (currentPiece.icon == 'B')
    {
        if (checkBishopMove(board, pmove))
        {
            makeMove(board, pmove);
            pieceTracker = &newSquare;
            newSquare->moves++;
            return 1;
        }
    }
	else if (currentPiece.icon == 'Q') {
		if (checkQueenMove(board, pmove))
		{
			makeMove(board, pmove);
			pieceTracker = &newSquare;
			newSquare->moves++;
			return 1;
		}
	}
	else if (currentPiece.icon == 'N') {
		if (checkKnightMove(board, pmove))
		{
			makeMove(board, pmove);
			pieceTracker = &newSquare;
			newSquare->moves++;
			return 1;
		}
	}
	else if (currentPiece.icon == 'R') {
		if (checkRookMove(board, pmove))
		{
			makeMove(board, pmove);
			pieceTracker = &newSquare;
			newSquare->moves++;
			return 1;
		}
	}
    
    printf("ERROR: Requested move is not allowed.");
	return 0;
}
