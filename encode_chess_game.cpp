#include <string>
#include <vector>
#include <iostream>
#include <cstdint>

using namespace std;

typedef uint16_t ChessMove;

enum SpecialMove {
    NO_SPECIAL_MOVE = 0b00,
    EN_PASSANT = 0b01,
    CASTLING = 0b10,
    PROMOTING = 0b11
};

enum PromoteTo {
    KNIGHT = 0b00,
    BISHOP = 0b01,
    ROOK = 0b10,
    QUEEN = 0b11
};

struct MoveInfo {
    int from_file;
    int from_rank;
    int to_file;
    int to_rank;
    SpecialMove special_move;
    PromoteTo promote_to;
};

// Convert algebraic notation to move information
MoveInfo parse_move(string move) {
    MoveInfo info;
    info.special_move = NO_SPECIAL_MOVE;
    info.promote_to = QUEEN;

    // Extract the file and rank of the starting position
    info.from_file = move[0] - 'a';
    info.from_rank = move[1] - '1';

    // Extract the file and rank of the ending position
    info.to_file = move[2] - 'a';
    info.to_rank = move[3] - '1';

    // Check for special moves
    if (move.length() > 4) {
        if (move[4] == 'e') {
            info.special_move = EN_PASSANT;
        }
        else if (move[4] == 'o') {
            info.special_move = CASTLING;
        }
        else {
            info.special_move = PROMOTING;
            switch (move[4]) {
                case 'n':
                    info.promote_to = KNIGHT;
                    break;
                case 'b':
                    info.promote_to = BISHOP;
                    break;
                case 'r':
                    info.promote_to = ROOK;
                    break;
                case 'q':
                    info.promote_to = QUEEN;
                    break;
            }
        }
    }

    return info;
}

// Convert move information to ChessMove
ChessMove encode_move(MoveInfo info) {
    uint16_t encoded_move = 0;

    encoded_move |= info.from_file;
    encoded_move |= info.from_rank << 3;
    encoded_move |= info.to_file << 6;
    encoded_move |= info.to_rank << 9;
    encoded_move |= info.special_move << 12;
    encoded_move |= info.promote_to << 14;

    return encoded_move;
}

// Convert algebraic notation to an array of ChessMove
vector<ChessMove> algebraic_to_moves(string game) {
    vector<ChessMove> moves;

    // Split the game into individual moves
    size_t pos = 0;
    string delimiter = " ";
    while ((pos = game.find(delimiter)) != string::npos) {
        string move = game.substr(0, pos);
        game.erase(0, pos + delimiter.length());

        MoveInfo info = parse_move(move);
        ChessMove encoded_move = encode_move(info);
        moves.push_back(encoded_move);
    }

    return moves;
}


// Sample usage
int main() {
    string game = "e2e4 e7e5 g1f3 b8c6 f1b5 c8g4 b5c6 g4f3 d2d4 e5d4 e1g1 f8e7 c1f4 d8d5 f4d6 e7d6 d1d6 b7d6 b2b3 a8b8 c2c3 c7c5 b3c4 d5c4 d3c4 e8c8 c4c5 d6c5 c3c4 b8c8 a2a4 h7h5 h2h4 h8h7 h4h5 g7g5 h5g6 h7g6 f3g6 f7g6 f2f4 g6f5 e4f5 g8h8 f1e1 c8d8 e1e2 d8e8 e2d3 e8d8 c4c5 d8d7 c5c6 d7d6 c6c7 d6d5 c7c8q";

    vector<ChessMove> moves = algebraic_to_moves(game);

    // Print the encoded moves
    for (ChessMove move : moves) {
        cout << move << " ";
    }

    return 0;
}
