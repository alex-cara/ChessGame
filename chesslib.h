#define WHITE 0
#define BLACK 1
#define EMPTY 3

struct Piece
{
	int color; // WHITE, BLACK, EMPTY
	char icon; // 'P', 'Q', 'R', 'B', 'N', ' '
	int moves; // number of moves
};

struct atkInfo 
{
	int isAttack; // under attack by another piece
	int x; // x coordinate of the attacking piece
	int y; // x coordinate of the attacking piece
};

struct Move
{
    int x1; // start x
    int y1; // start y
    int x2; // end x
    int y2; // end y
    int pawn_prom;
};

int checkMoveValidity(struct Piece board[][8], struct Piece **pieceTracker,
                      struct Move *pmove, int turn);

int checkForMate(struct Piece board[][8], int pieceColor);
