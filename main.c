#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "chesslib.h"

void ResetBoard(struct Piece board[][8]);
void PrintBoard(struct Piece board[][8], int turn, int color);
struct Move GetMove(struct Piece board[][8], int turn);

int main(void) {
    int valid_move = 0;
    struct Piece chess_board[8][8];
    struct Move move;
    struct Piece *lastMove = NULL;
	int player_turn = WHITE;
	int checkMate = 0;
	char userColor[128];
	int colorChoice;

	printf("Do you want to use a colored board (if console supports it)? Y or N:");
	fgets(userColor, sizeof(userColor), stdin);
	colorChoice = (userColor[0] == 'Y' || userColor[0] == 'y') ? 1 : 0;

	printf("\nThis chess program will be using the ICCF notation\n");
	printf("https://en.wikipedia.org/wiki/ICCF_numeric_notation \n");
	printf("For example to move white's pawn at the start you would put 1214.\n");

    ResetBoard(chess_board);
    PrintBoard(chess_board, player_turn, colorChoice);
    
	while (!checkMate)
    {
        do {
            move = GetMove(chess_board, player_turn);
            if (move.x1 < 0)
            {
                printf(" Try again1.\n");
				valid_move = 0;
                continue;
            }
            valid_move = checkMoveValidity(chess_board, &lastMove, &move,
                                           player_turn);
            if (!valid_move)
                printf(" Try again2.\n");
        } while (!valid_move);
        
        player_turn = 1 - player_turn;
        PrintBoard(chess_board, player_turn, colorChoice);
		
		checkMate = checkForMate(chess_board, BLACK);
    };
	printf("Checkmate!\n");

	return 0;
}

void ResetBoard(struct Piece board[][8])
{
    int i, j;
    
    // Fill entire board except first and last raw's icons:
    for (i = 0; i < 8; i++)
    {
        for (j = 2; j < 6; j++)
        {
            board[i][j].color = EMPTY;
            board[i][j].icon = ' ';
            board[i][j].moves = 0;
        }

        board[i][1].color = WHITE;
        board[i][1].icon = 'P'; // Pawn
        board[i][1].moves = 0;

        board[i][0].color = WHITE;
        board[i][0].moves = 0;

        board[i][6].color = BLACK;
        board[i][6].icon = 'P';
        board[i][6].moves = 0;
        
        board[i][7].color = BLACK;
        board[i][7].moves = 0;
    }
    
    // Fill first and last row icons only:
    for (i = 0; i < 2; i++)
    {
        board[0][7 - 7 * i].icon = 'R';  // Rook
        board[1][7 - 7 * i].icon = 'N';  // Knight
        board[2][7 - 7 * i].icon = 'B';  // Bishop
        board[3][7 - 7 * i].icon = 'Q';  // Queen
        board[4][7 - 7 * i].icon = 'K';  // King
        board[5][7 - 7 * i].icon = 'B';
        board[6][7 - 7 * i].icon = 'N';
        board[7][7 - 7 * i].icon = 'R';
    }
}

void PrintPiece(struct Piece board[][8], int i, int j, int color)
{
    const char *squareColor, *pieceColor;
	if (color)
	{
		squareColor = (i + j) % 2 == 0 ? "\033[48;5;214m" : "\033[48;5;222m";
		pieceColor = board[i][j].color == WHITE ? "\033[38;5;1m" : "\033[38;5;0m";
		printf("%s%s\033[1m%c ", squareColor, pieceColor, board[i][j].icon);
	}
	else
	{
		printf("%c ", board[i][j].icon);
	}
}

void PrintBoard(struct Piece board[][8], int turn, int color)
{
    // int turn can be WHITE (0) or BLACK (1)
    int i, j;
    
    printf("\n");

    if (turn == WHITE)
    {
        for (j = 7; j >= 0; j--)
        {
            printf(" %1d ", j + 1); // row label
            for (i = 0; i < 8; i++)
                PrintPiece(board, i, j, color);
            printf("\033[m\n");
        }
        
        // print column labels:
        printf("   1 2 3 4 5 6 7 8\n\n");
    }
    else
    {
        for (j = 0; j < 8; j++)
        {
            printf(" %1d ", j + 1); // row label
            for (i = 7; i >= 0; i--)
                PrintPiece(board, i, j, color);
            printf("\033[m\n");
        }
        
        // print column labels:
        printf("   8 7 6 5 4 3 2 1\n\n");
    }
}

struct Move GetMove(struct Piece board[][8], int turn)
{
    int coordinates[5] = {-1, -1, -1, -1, 0}; // 4 coordinates and 1 pawn
                                              // promotion code
    int i = 0;
    int n = 0;
    char str[16];  // at least 5 + 2 (new line and null char)
    struct Move move = {-1, -1, -1, -1, 0}; // bad move
    
    printf("%s's turn. Enter your move: ", turn ? "Black" : "White");
    fgets(str, 20, stdin);
    
    while (i < 16 && str[i] != '\0' && n < 5)
    {
        if (isspace(str[i]))
        {
            i++;
            continue;
        }
        else if (isdigit(str[i]))
        {
            coordinates[n] = str[i] - '0';
            if (n < 4)
            {
                if (coordinates[n] < 1 || coordinates[n] > 8)
                {
                    printf("ERROR: Move coordinates not within the board.");
                    return move;
                }
            }
            else if (coordinates[n] < 1 || coordinates[n] > 4)
            {
                printf("ERROR: Pawn promotion code is invalid.");
                return move;
            }
            n++;
            i++;
        }
    }
    
    // make sure there are at least 4 coordinates:
    if (n < 4)
    {
        printf("ERROR: Too few move coordinates.");
        return move;
    }

	// require pawn promotion?
	if (((turn && coordinates[3] == 1) || (!turn && coordinates[3] == 8))
		&& board[coordinates[0] - 1][coordinates[1] - 1].icon == 'P' && n == 4)
	{
		printf("ERROR: Pawn promotion is required!");
		return move;
	}

	move.x1 = coordinates[0] - 1;
	move.y1 = coordinates[1] - 1;
	move.x2 = coordinates[2] - 1;
	move.y2 = coordinates[3] - 1;
	move.pawn_prom = coordinates[4];

    return move;
}
