#include "Watchy_Chess.h"

#define FROM_FILE_MASK 0x0007u
#define FROM_FILE_OFFSET 0u

#define FROM_RANK_MASK 0x0038u
#define FROM_RANK_OFFSET 3u

#define TO_FILE_MASK 0x01C0u
#define TO_FILE_OFFSET 6u

#define TO_RANK_MASK 0x0E00u
#define TO_RANK_OFFSET 9u



#define SPECIAL_MOVE_MASK 0x3000u
#define SPECIAL_MOVE_OFFSET 12u

#define NO_SPECIAL_MOVE 0u
#define EN_PASSANT 1u
#define CASTLING 2u
#define PROMOTING 3u


#define PROMOTE_TO_MASK 0xC000u
#define PROMOTE_TO_OFFSET 14u



#define PIECE_TYPE_MASK 0x07u
#define PIECE_TYPE_OFFSET 0u

#define PAWN 0u
#define KNIGHT 1u
#define BISHOP 2u
#define ROOK 3u
#define QUEEN 4u
#define KING 5u

#define PIECE_COLOR_MASK 0x08u
#define PIECE_COLOR_OFFSET 3u

#define BLACK 0u
#define WHITE 1u

#define IS_EMPTY_MASK 0x10u
#define IS_EMPTY_OFFSET 4u

#define NOT_EMPTY 0u
#define EMPTY 1u

/*

Files horizontal
Ranks vertical
------------------------BLACK


-----2--------------
-----1--------------
-----0--------------
File####---0---1---2
----Rank------------


------------------------WHITE


bits 0-2  --  from_file
bits 3-5  --  from_rank

bits 6-8  --  to_file
bits 9-11  --  to_rank



bits 12-13  --  special_move

special_move = 00 = NO_SPECIAL_MOVE
       = 01 = EN_PASSANT  ~~  pawn's final position will be encoded in to_file/to_rank
       = 10 = CASTLING  ~~  king's move will be encoded
       = 11 = PROMOTING

bits 14-15  --  promote_to

promote_to is only valid if special_move = PROMOTING

promote_to = 00 = KNIGHT
      = 01 = BISHOP
      = 10 = ROOK
      = 11 = QUEEN

*/
typedef uint16_t ChessMove;


/*

each element of the array will have the structure

bits 0-2  --  piece_type

piece_type = 000 = PAWN
      = 001 = KNIGHT
      = 010 = BISHOP
      = 011 = ROOK
      = 100 = QUEEN
      = 101 = KING

bit 3  --  piece_color

piece_color = 0 = BLACK
`      = 1 = WHITE

bit 4  --  is_empty

is_empty = 0 = NOT_EMPTY
     = 1 = EMPTY (if empty, bits 0-3 are ignored)

*/
typedef uint8_t Board[8][8];

const Board reset_board = {
	{8*WHITE+ROOK, 8*WHITE+KNIGHT, 8*WHITE+BISHOP, 8*WHITE+QUEEN, 8*WHITE+KING, 8*WHITE+BISHOP, 8*WHITE+KNIGHT, 8*WHITE+ROOK},
	{8*WHITE+PAWN, 8*WHITE+PAWN, 8*WHITE+PAWN, 8*WHITE+PAWN, 8*WHITE+PAWN, 8*WHITE+PAWN, 8*WHITE+PAWN, 8*WHITE+PAWN},
	{EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET},
	{EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET},
	{EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET},
	{EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET, EMPTY<<IS_EMPTY_OFFSET},
	{8*BLACK+PAWN, 8*BLACK+PAWN, 8*BLACK+PAWN, 8*BLACK+PAWN, 8*BLACK+PAWN, 8*BLACK+PAWN, 8*BLACK+PAWN, 8*BLACK+PAWN},
	{8*BLACK+ROOK, 8*BLACK+KNIGHT, 8*BLACK+BISHOP, 8*BLACK+QUEEN, 8*BLACK+KING, 8*BLACK+BISHOP, 8*BLACK+KNIGHT, 8*BLACK+ROOK}
};


ChessMove moves[] = { 50956, 51508, 51021, 51044, 50821, 51195, 49476, 51313, 51290, 52094, 50502, 52191, 50379, 51693, 51157, 51631, 51551, 51890, 51086, 52071, 49543, 51306, 51151, 52134, 51679, 51630, 50499, 53165, 51010, 52070, 50305, 51389, 51410, 49773, 51933, 49570, 51484, 49161, 49925, 51769, 52645, 52988, 52053, 52094 };

void WatchyChess::drawWatchFace()
{
    uint8_t last_file;
    uint8_t last_rank;
    uint16_t temp;

    if (move_counter >= 44)
    {
        memcpy(chess_board, reset_board, 8 * 8);
        move_counter = 0;
    }

    temp = make_move(moves[move_counter]);
    move_counter++;

    display.fillScreen(GxEPD_WHITE);

    render_board();
    // draw border to visually keep track of last move played
    last_file = temp >> 8;
    last_rank = temp % 256;
    draw_border(last_file, last_rank);




    display.setTextColor(GxEPD_BLACK);
    display.setFont(&DSEG7_Classic_Bold_25);
    display.setCursor(0, 195);
    int displayHour;
    if(HOUR_12_24==12){
      displayHour = ((currentTime.Hour+11)%12)+1;
    } else {
      displayHour = currentTime.Hour;
    }
    if(displayHour < 10){
        display.print("0");
    }
    display.print(displayHour);
    display.print(":");
    if(currentTime.Minute < 10){
        display.print("0");
    }
    display.println(currentTime.Minute);

    //display.fillCircle(180, 20, 10, GxEPD_BLACK);


    // reset step counter at midnight
    if (currentTime.Hour == 0 && currentTime.Minute == 0){
      sensor.resetStepCounter();
    }
    uint32_t stepCount = sensor.getCounter();
    display.setCursor(130, 190);
    display.setFont(&DSEG7_Classic_Regular_15);
    display.println(stepCount);


}

void WatchyChess::draw_border(uint8_t file, uint8_t rank)
{
    display.drawRect(file * 20 + 1, 140 - rank * 20 + 1, 18, 18, GxEPD_BLACK);
}

uint16_t WatchyChess::make_move(ChessMove move)
{
    uint8_t to_file = (move & TO_FILE_MASK) >> TO_FILE_OFFSET;
    uint8_t to_rank = (move & TO_RANK_MASK) >> TO_RANK_OFFSET;
    uint8_t from_file = (move & FROM_FILE_MASK) >> FROM_FILE_OFFSET;
    uint8_t from_rank = (move & FROM_RANK_MASK) >> FROM_RANK_OFFSET;

    switch ((move & SPECIAL_MOVE_MASK) >> SPECIAL_MOVE_OFFSET)
    {
    case NO_SPECIAL_MOVE:
    {
        chess_board[to_rank][to_file] = chess_board[from_rank][from_file];
        chess_board[from_rank][from_file] = EMPTY << IS_EMPTY_OFFSET;
        return (to_file << 8) + to_rank;
    }
    break;
    default:
    {
        /* nothing to do */
    }
    break;
    }
}

void WatchyChess::render_square(uint8_t file, uint8_t row)
{
    display.drawBitmap(row * 20, 140 - file * 20, (file+row) % 2 == 0 ? gray_square : white_square, 20, 20, GxEPD_BLACK);
    if (((chess_board[file][row] & IS_EMPTY_MASK) >> IS_EMPTY_OFFSET) == EMPTY)
    {
        /* nothing to do */
    }
    else
    {
        if (((chess_board[file][row] & PIECE_COLOR_MASK) >> PIECE_COLOR_OFFSET) == WHITE)
        {
            switch (((chess_board[file][row] & PIECE_TYPE_MASK) >> PIECE_TYPE_OFFSET))
            {
            case PAWN:
            {
                display.drawBitmap(row * 20, 140 - file * 20, other_white_pawn, 20, 20, GxEPD_BLACK);
            }
            break;
            case KNIGHT:
            {
                display.drawBitmap(row * 20, 140 - file * 20, white_knight, 20, 20, GxEPD_BLACK);
            }
            break;
            case BISHOP:
            {
                display.drawBitmap(row * 20, 140 - file * 20, white_bishop, 20, 20, GxEPD_BLACK);
            }
            break;
            case ROOK:
            {
                display.drawBitmap(row * 20, 140 - file * 20, white_rook, 20, 20, GxEPD_BLACK);
            }
            break;
            case QUEEN:
            {
                display.drawBitmap(row * 20, 140 - file * 20, white_queen, 20, 20, GxEPD_BLACK);
            }
            break;
            case KING:
            {
                display.drawBitmap(row * 20, 140 - file * 20, white_king, 20, 20, GxEPD_BLACK);
            }
            break;
            default:
            {
                /* nothing to do */
            }
            break;
            }
        }
        else
        {
            switch (((chess_board[file][row] & PIECE_TYPE_MASK) >> PIECE_TYPE_OFFSET))
            {
            case PAWN:
            {
                display.drawBitmap(row * 20, 140 - file * 20, other_black_pawn, 20, 20, GxEPD_BLACK);
            }
            break;
            case KNIGHT:
            {
                display.drawBitmap(row * 20, 140 - file * 20, black_knight, 20, 20, GxEPD_BLACK);
            }
            break;
            case BISHOP:
            {
                display.drawBitmap(row * 20, 140 - file * 20, other_black_bishop, 20, 20, GxEPD_BLACK);
            }
            break;
            case ROOK:
            {
                display.drawBitmap(row * 20, 140 - file * 20, black_rook, 20, 20, GxEPD_BLACK);
            }
            break;
            case QUEEN:
            {
                display.drawBitmap(row * 20, 140 - file * 20, black_queen, 20, 20, GxEPD_BLACK);
            }
            break;
            case KING:
            {
                display.drawBitmap(row * 20, 140 - file * 20, black_king, 20, 20, GxEPD_BLACK);
            }
            break;
            default:
            {
                /* nothing to do */
            }
            break;
            }
        }
    }
}

void WatchyChess::render_board()
{
    for (uint8_t file = 0; file < 8; file++)
    {
        for (uint8_t row = 0; row < 8; row++)
        {
            render_square(file, row);
        }
    }
}



