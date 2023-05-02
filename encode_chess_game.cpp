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
    string game = "e2e4 e7e5 f2f4 e5f4 f1c4 d8h4 e1f1 b7b5 c4b5 g8f6 g1f3 h4h6 d2d3 f6h5 f3h4 h6g5 h4f5 c7c6 g2g4 h5f6 h1g1 c6b5 h2h4 g5g6 h4h5 g6g5 d1f3 f6g8 c1f4 g5f6 b1c3 f8c5 c3d5 f6b2 f4d6 c5g1 e4e5 b2a1 f1e2 b8a6 f5g7 e8d8 f3f6 g8f6 d6e7";

    vector<ChessMove> moves = algebraic_to_moves(game);

    // Print the encoded moves
    for (ChessMove move : moves) {
        cout << move << " ";
    }

    return 0;
}
