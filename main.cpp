#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include <utility>
#include <cstdlib>

// CONVERT 2D ARRAY CO-ORDINATES TO APPROPRIATE ALGEBRAIC NOTATION
std::string convertToAlgebraicNotationPos(int y, int x) {
    // Calculate the rank and file characters and concatenate them into a string
    return std::string(1, 'a' + x) + std::to_string(8 - y);
}

// DATA STRUCTURE TO STORE LEGAL MOVES FOR EACH PIECE
using LegalMovesMap = std::map<std::string, std::vector<std::string>>;

// A MAP OF THE SQUARES THAT WHITE OCCUPIES
int whitePieceOccupation[8][8];
// A MAP OF THE SQUARES THAT BLACK OCCUPIES
int blackPieceOccupation[8][8];

// A MAP OF THE SQUARES THAT WHITE OCCUPIES VIRTUALLY
int virtual_whitePieceOccupation[8][8];
// A MAP OF THE SQUARES THAT BLACK OCCUPIES VIRTUALLY
int virtual_blackPieceOccupation[8][8];

// A MAP OF THE SQUARES THAT WHITE OCCUPIES VIRTUALLY
int hyper_virtual_whitePieceOccupation[8][8];
// A MAP OF THE SQUARES THAT BLACK OCCUPIES VIRTUALLY
int hyper_virtual_blackPieceOccupation[8][8];

int ambiguityMap[8][8];

// MANAGING CASTLING RIGHTS
bool whiteCastlingRightsKingside = true;
bool whiteCastlingRightsQueenside = true;
bool whiteCanCastleKingside = false;
bool whiteCanCastleQueenside = false;

bool blackCastlingRightsKingside = true;
bool blackCastlingRightsQueenside = true;
bool blackCanCastleKingside = false;
bool blackCanCastleQueenside = false;

// STORES LIST OF MOVES
std::vector<std::string> movesLog;

std::vector<std::string> whiteRookMoves;
std::vector<std::string> blackRookMoves;

bool isInputValid(const std::string& userInput, const LegalMovesMap& legalMovesMap) {
    for (const auto& entry : legalMovesMap) {
        const std::vector<std::string>& legalMoves = entry.second;
        for (const std::string& move : legalMoves) {
            if (userInput == move) {
                return true; // User input matches a legal move
            }
        }
    }
    return false; // User input doesn't match any legal move
}

class Piece {
public:
    Piece(std::string color, int y, int x) : color(color), y(y), x(x) {}

    // DESTRUCTOR
    virtual ~Piece() {}
    
    // Functions to retrieve type, color, and position
    virtual std::string getType() const {
        return "Piece"; // Override this in derived classes
    }

    std::string getColor() const {
        return color;
    }

    int getRowIndex() const {
        return y;
    }

    int getColIndex() const {
        return x;
    }

    std::string getPosition() const {
        return convertToAlgebraicNotationPos(y, x);
    }

    void setRow(int newY) {
        y = newY;
    }

    void setCol(int newX) {
        x = newX;
    }

protected:
    std::string color;
    int x, y;
};

class Pawn : public Piece {
public:
    Pawn(std::string color, int y, int x) : Piece(color, y, x) {}
    // Implement Pawn-specific move logic here

    // Modify the generatePawnLegalMoves function to use LegalMovesMap
    void generatePawnLegalMoves(LegalMovesMap& legalMovesMap, const std::string chessboard[8][8], bool whiteTurn) {
        // Define legalMoves within the function
        std::vector<std::string> legalMoves;

        if (whiteTurn) {
            // FORWARD PAWN MOVES
            if (isEmptySquare(y - 1, x, chessboard)) {
                // Add the move one square forward
                legalMoves.push_back(convertToAlgebraicNotationPos(y - 1, x));

                // PAWN CAN MOVE TWICE FORWARD IF IT HASN'T MOVED YET
                if (y == 6 && color == "White") {
                    if (isEmptySquare(y - 2, x, chessboard)) {
                        // Add the move two squares forward
                        legalMoves.push_back(convertToAlgebraicNotationPos(y - 2, x));
                    }
                }
            }
            // CHECK DIAGONAL CAPTURES FOR WHITE PAWNS
            if (x > 0 && chessboard[y - 1][x - 1] != "-" && chessboard[y - 1][x - 1][0] == 'B') {
                // Add the diagonal capture move
                legalMoves.push_back(std::string(1, 'a' + x) + "x" + convertToAlgebraicNotationPos(y - 1, x - 1));
            }

            if (x < 7 && chessboard[y - 1][x + 1] != "-" && chessboard[y - 1][x + 1][0] == 'B') {
                // Add the diagonal capture move
                legalMoves.push_back(std::string(1, 'a' + x) + "x" + convertToAlgebraicNotationPos(y - 1, x + 1));
            }
        } else {
            if (isEmptySquare(y + 1, x, chessboard)) {
                // Add the move one square forward
                legalMoves.push_back(convertToAlgebraicNotationPos(y + 1, x));

                // If the pawn is on its starting rank (2 for white, 7 for black), it can move two squares forward
                if (y == 1 && color == "Black") {
                    if (isEmptySquare(y + 2, x, chessboard)) {
                        // Add the move two squares forward
                        legalMoves.push_back(convertToAlgebraicNotationPos(y + 2, x));
                    }
                }
            }
            // CHECK DIAGONAL CAPTURES FOR BLACK PAWNS
            if (x > 0 && chessboard[y + 1][x - 1] != "-" && chessboard[y + 1][x - 1][0] == 'W') {
                // Add the diagonal capture move
                legalMoves.push_back(std::string(1, 'a' + x) + "x" + convertToAlgebraicNotationPos(y + 1, x - 1));
            }

            if (x < 7 && chessboard[y + 1][x + 1] != "-" && chessboard[y + 1][x + 1][0] == 'W') {
                // Add the diagonal capture move
                legalMoves.push_back(std::string(1, 'a' + x) + "x" + convertToAlgebraicNotationPos(y + 1, x + 1));
            }
        }

        // Store legal moves in the corresponding map
        legalMovesMap[getPosition()] = legalMoves;
    }

    // Override getType() to return "Pawn"  
    std::string getType() const override {
        return "Pawn";
    }
private:
    bool isEmptySquare(int y, int x, const std::string chessboard[8][8]) {
        return chessboard[y][x] == "-";
    }
};

class Knight : public Piece {
public:
    Knight(std::string color, int y, int x) : Piece(color, y, x) {}
    // Implement Pawn-specific move logic here

    void generateKnightLegalMoves(LegalMovesMap& legalMovesMap, const std::string chessboard[8][8], bool whiteTurn) {
        // Define legalMoves within the function
        std::vector<std::string> legalMoves;

        // Knight move offsets
        int offsetX[8] = {1, 2, 2, 1, -1, -2, -2, -1};
        int offsetY[8] = {-2, -1, 1, 2, 2, 1, -1, -2};

        // Get the current position of the knight
        int currentY = getRowIndex();
        int currentX = getColIndex();

        // Check each possible knight move
        for (int i = 0; i < 8; i++) {
            int newY = currentY + offsetY[i];
            int newX = currentX + offsetX[i];

            // Check if the move is within the board boundaries
            if (newY >= 0 && newY < 8 && newX >= 0 && newX < 8) {
                // Check if the target square is empty or contains an opponent's piece
                if (chessboard[newY][newX] == "-") {
                    legalMoves.push_back("N" + convertToAlgebraicNotationPos(newY, newX)); // Knight move
                } else {
                    if (whiteTurn) {
                        if (chessboard[newY][newX][0] == 'B') {
                            legalMoves.push_back("Nx" + convertToAlgebraicNotationPos(newY, newX)); // Knight move
                        }
                    } else {
                        if (chessboard[newY][newX][0] == 'W') {
                            legalMoves.push_back("Nx" + convertToAlgebraicNotationPos(newY, newX)); // Knight move
                        }
                    }
                }
            }
        }
        // Store legal moves in the corresponding map
        legalMovesMap[getPosition()] = legalMoves;
    }

    // Override getType() to return "Knight"
    std::string getType() const override {
        return "Knight";
    }
};

class Bishop : public Piece {
public:
    Bishop(std::string color, int y, int x) : Piece(color, y, x) {}
    // Implement Pawn-specific move logic here

    void generateBishopLegalMoves(LegalMovesMap& legalMovesMap, const std::string chessboard[8][8], bool whiteTurn) {
        // Define legalMoves within the function
        std::vector<std::string> legalMoves;

        // Offset arrays for diagonal moves
        int offsetX[4] = {1, 1, -1, -1};
        int offsetY[4] = {-1, 1, 1, -1};

        // Iterate through all diagonal directions
        for (int i = 0; i < 4; i++) {
            int newY = getRowIndex();
            int newX = getColIndex();

            while (true) {
                newY += offsetY[i];
                newX += offsetX[i];

                // Check if the new position is out of bounds
                if (newY < 0 || newY >= 8 || newX < 0 || newX >= 8) {
                    break; // Out of bounds, stop in this direction
                }

                // If the square is empty, it's a legal move
                if (chessboard[newY][newX] == "-") {
                    legalMoves.push_back("B" + convertToAlgebraicNotationPos(newY, newX));
                }
                
                // If the square is occupied by an opponent's piece, it's a capture move
                else if (chessboard[newY][newX][0] != getColor()[0]) {
                    legalMoves.push_back("Bx" + convertToAlgebraicNotationPos(newY, newX));
                    break; // Capture move, stop in this direction after capturing
                }

                // If the square is occupied by any piece (including your own), stop in this direction
                else {
                    break;
                }
            }
        }

        // Store legal moves in the corresponding map
        legalMovesMap[getPosition()] = legalMoves;
    }

    // Override getType() to return "Bishop"
    std::string getType() const override {
        return "Bishop";
    }
};

class Rook : public Piece {
public:
    Rook(std::string color, int y, int x) : Piece(color, y, x) {}
    // Implement Pawn-specific move logic here

    void generateRookLegalMoves(LegalMovesMap& legalMovesMap, const std::string chessboard[8][8], bool whiteTurn) {
        // Define legalMoves within the function
        std::vector<std::string> legalMoves;

        // Offset arrays for diagonal moves
        int offsetX[4] = {0, 1, 0, -1};
        int offsetY[4] = {-1, 0, 1, 0};

        // Iterate through all diagonal directions
        for (int i = 0; i < 4; i++) {
            int newY = getRowIndex();
            int newX = getColIndex();

            while (true) {
                newY += offsetY[i];
                newX += offsetX[i];

                // Check if the new position is out of bounds
                if (newY < 0 || newY >= 8 || newX < 0 || newX >= 8) {
                    break; // Out of bounds, stop in this direction
                }

                // If the square is empty, it's a legal move
                if (chessboard[newY][newX] == "-") {
                    legalMoves.push_back("R" + convertToAlgebraicNotationPos(newY, newX));
                }
                
                // If the square is occupied by an opponent's piece, it's a capture move
                else if (chessboard[newY][newX][0] != getColor()[0]) {
                    legalMoves.push_back("Rx" + convertToAlgebraicNotationPos(newY, newX));
                    break; // Capture move, stop in this direction after capturing
                }

                // If the square is occupied by any piece (including your own), stop in this direction
                else {
                    break;
                }
            }
        }

        // Store legal moves in the corresponding map
        legalMovesMap[getPosition()] = legalMoves;
    }

    // Override getType() to return "Rook"
    std::string getType() const override {
        return "Rook";
    }
};

class Queen : public Piece {
public:
    Queen(std::string color, int y, int x) : Piece(color, y, x) {}
    // Implement Pawn-specific move logic here

    void generateQueenLegalMoves(LegalMovesMap& legalMovesMap, const std::string chessboard[8][8], bool whiteTurn) {
        // Define legalMoves within the function
        std::vector<std::string> legalMoves;

        // Offset arrays for diagonal moves
        int offsetX[8] = {0, 1, 1, 1, 0, -1, -1, -1};
        int offsetY[8] = {-1, -1, 0, 1, 1, 1, 0, -1};

        // Iterate through all diagonal directions
        for (int i = 0; i < 8; i++) {
            int newY = getRowIndex();
            int newX = getColIndex();

            while (true) {
                newY += offsetY[i];
                newX += offsetX[i];

                // Check if the new position is out of bounds
                if (newY < 0 || newY >= 8 || newX < 0 || newX >= 8) {
                    break; // Out of bounds, stop in this direction
                }

                // If the square is empty, it's a legal move
                if (chessboard[newY][newX] == "-") {
                    legalMoves.push_back("Q" + convertToAlgebraicNotationPos(newY, newX));
                }
                
                // If the square is occupied by an opponent's piece, it's a capture move
                else if (chessboard[newY][newX][0] != getColor()[0]) {
                    legalMoves.push_back("Qx" + convertToAlgebraicNotationPos(newY, newX));
                    break; // Capture move, stop in this direction after capturing
                }

                // If the square is occupied by any piece (including your own), stop in this direction
                else {
                    break;
                }
            }
        }

        // Store legal moves in the corresponding map
        legalMovesMap[getPosition()] = legalMoves;
    }

    // Override getType() to return "Queen"
    std::string getType() const override {
        return "Queen";
    }
};

class King : public Piece {
public:
    King(std::string color, int y, int x) : Piece(color, y, x) {}
    // Implement Pawn-specific move logic here

    void generateKingLegalMoves(LegalMovesMap& legalMovesMap, const std::string chessboard[8][8], bool whiteTurn) {
        // Define legalMoves within the function
        std::vector<std::string> legalMoves;

        int offsetX[8] = {0, 1, 1, 1, 0, -1, -1, -1};
        int offsetY[8] = {-1, -1, 0, 1, 1, 1, 0, -1};

        int currentY = getRowIndex();
        int currentX = getColIndex();

        for (int i = 0; i < 8; i++) {
            int newY = currentY + offsetY[i];
            int newX = currentX + offsetX[i];

            if (newY >= 0 && newY < 8 && newX >= 0 && newX < 8) {
                // Check if the square is empty ("-") and not occupied by an opponent's piece
                if (chessboard[newY][newX] == "-" && ((whiteTurn && blackPieceOccupation[newY][newX] == 0) || (!whiteTurn && whitePieceOccupation[newY][newX] == 0))) {
                    // Add this move to the list of legal moves as a non-capture move
                    legalMoves.push_back("K" + convertToAlgebraicNotationPos(newY, newX));
                }
                // Check if the square is occupied by an opponent's piece
                else if (chessboard[newY][newX] != "-" && ((whiteTurn && chessboard[newY][newX][0] == 'B') || (!whiteTurn && chessboard[newY][newX][0] == 'W'))) {
                    // Add this move to the list of legal moves as a capture move
                    legalMoves.push_back("Kx" + convertToAlgebraicNotationPos(newY, newX));
                }
            }
        }

        // Store legal moves in the corresponding map
        legalMovesMap[getPosition()] = legalMoves;
    }

    // Override getType() to return "King"
    std::string getType() const override {
        return "King";
    }
};

class Chessboard {
public:
    // SETTING UP THE CHESSBOARD
    void initializeBoard() {
        // INITIALIZING 8x8 ARRAY WITH PIECES
        for (int i = 0; i < 8; i++) { // I DENOTES THE ROWS
            for (int j = 0; j < 8; j++) { // J DENOTES THE COLUMNS
                // BLACK PIECES: i = 0 and i = 1, WHITE PIECES: i = 6 and i = 7
                if (i == 0) {
                    if (j == 0 || j == 7) {
                        chessboard[i][j] = "BR";
                        blackPieces.push_back(std::make_shared<Rook>("Black", i, j));
                    } else if (j == 1 || j == 6) {
                        chessboard[i][j] = "BN";
                        blackPieces.push_back(std::make_shared<Knight>("Black", i, j));
                    } else if (j == 2 || j == 5) {
                        chessboard[i][j] = "BB";
                        blackPieces.push_back(std::make_shared<Bishop>("Black", i, j));
                    } else if (j == 3) {
                        chessboard[i][j] = "BQ";
                        blackPieces.push_back(std::make_shared<Queen>("Black", i, j));
                    } else if (j == 4) {
                        chessboard[i][j] = "BK";
                        blackPieces.push_back(std::make_shared<King>("Black", i, j));
                    }
                } else if (i == 1) {
                    chessboard[i][j] = "BP";
                    blackPieces.push_back(std::make_shared<Pawn>("Black", i, j));
                } else if (i == 6) {
                    chessboard[i][j] = "WP";
                    whitePieces.push_back(std::make_shared<Pawn>("White", i, j));
                } else if (i == 7) {
                    // ROOK
                    if (j == 0 || j == 7) {
                        chessboard[i][j] = "WR";
                        whitePieces.push_back(std::make_shared<Rook>("White", i, j));
                    } else if (j == 1 || j == 6) {
                        chessboard[i][j] = "WN";
                        whitePieces.push_back(std::make_shared<Knight>("White", i, j));
                    } else if (j == 2 || j == 5) {
                        chessboard[i][j] = "WB";
                        whitePieces.push_back(std::make_shared<Bishop>("White", i, j));
                    } else if (j == 3) {
                        chessboard[i][j] = "WQ";
                        whitePieces.push_back(std::make_shared<Queen>("White", i, j));
                    } else if (j == 4) {
                        chessboard[i][j] = "WK";
                        whitePieces.push_back(std::make_shared<King>("White", i, j));
                    }
                } else {
                    chessboard[i][j] = "-";
                }
            }
        }
    }

    void virtual_initializeBoard() {

        virtual_blackPieces.clear();
        virtual_whitePieces.clear();

        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                std::string pieceID = virtual_chessboard[i][j];
                if (pieceID == "WP") {
                    virtual_whitePieces.push_back(std::make_shared<Pawn>("White", i, j));
                } else if (pieceID == "WN") {
                    virtual_whitePieces.push_back(std::make_shared<Knight>("White", i, j));
                } else if (pieceID == "WB") {
                    virtual_whitePieces.push_back(std::make_shared<Bishop>("White", i, j));
                } else if (pieceID == "WR") {
                    virtual_whitePieces.push_back(std::make_shared<Rook>("White", i, j));
                } else if (pieceID == "WQ") {
                    virtual_whitePieces.push_back(std::make_shared<Queen>("White", i, j));
                } else if (pieceID == "WK") {
                    virtual_whitePieces.push_back(std::make_shared<King>("White", i, j));
                } else if (pieceID == "BP") {
                    virtual_blackPieces.push_back(std::make_shared<Pawn>("Black", i, j));
                } else if (pieceID == "BN") {
                    virtual_blackPieces.push_back(std::make_shared<Knight>("Black", i, j));
                } else if (pieceID == "BB") {
                    virtual_blackPieces.push_back(std::make_shared<Bishop>("Black", i, j));
                } else if (pieceID == "BR") {
                    virtual_blackPieces.push_back(std::make_shared<Rook>("Black", i, j));
                } else if (pieceID == "BQ") {
                    virtual_blackPieces.push_back(std::make_shared<Queen>("Black", i, j));
                } else if (pieceID == "BK") {
                    virtual_blackPieces.push_back(std::make_shared<King>("Black", i, j));
                }
            }
        }
    }

    // DISPLAY CHESSBOARD
    void displayBoard() {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                // FORMATS OUTPUT IN A TABULAR MANNER
                std::cout << std::setw(3) << chessboard[i][j];
            }
            std::cout << std::endl;
        }
    }

    // METHOD TO PRINT POSITIONS OF CHESS PIECES
    void printPiecePositions() {
        // OF WHITE COLOUR:
        std::cout << "White's Chess Piece Positions:" << std::endl;
        for (const auto& piece : whitePieces) {
            std::cout << piece->getType() << "(" << piece->getRowIndex() << ", " << piece->getColIndex() << "), ";
        }
        std::cout << std::endl;
        std::cout << std::endl;
        // OF BLACK COLOUR:
        std::cout << "Black's Chess Piece Positions:" << std::endl;
        for (const auto& piece : blackPieces) {
            std::cout << piece->getType() << "(" << piece->getRowIndex() << ", " << piece->getColIndex() << "), ";
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }

    void printLegalMoves(bool whiteTurn) {
        const LegalMovesMap& legalMovesMap = whiteTurn ? whiteLegalMovesMap : blackLegalMovesMap;

        if (whiteTurn) {
            std::cout << "Legal moves for White:" << std::endl;
        } else {
            std::cout << "Legal moves for Black:" << std::endl;
        }

        for (const auto& entry : legalMovesMap) {
            const std::string& position = entry.first;
            const std::vector<std::string>& legalMoves = entry.second;

            std::cout << position << ": ";
            for (const auto& move : legalMoves) {
                std::cout << move << " ";
            }
            std::cout << ", ";
        }
        std::cout << std::endl;
    }

    void virtual_printLegalMoves(bool whiteTurn) {
        const LegalMovesMap& legalMovesMap = whiteTurn ? virtual_whiteLegalMovesMap : virtual_blackLegalMovesMap;

        if (whiteTurn) {
            std::cout << "Legal moves for White:" << std::endl;
        } else {
            std::cout << "Legal moves for Black:" << std::endl;
        }

        for (const auto& entry : legalMovesMap) {
            const std::string& position = entry.first;
            const std::vector<std::string>& legalMoves = entry.second;

            std::cout << position << ": ";
            for (const auto& move : legalMoves) {
                std::cout << move << " ";
            }
            std::cout << ", ";
        }
        std::cout << std::endl;
    }

    // Modify the generateLegalMoves function to handle all piece types
    void generateLegalMoves(bool whiteTurn) {
        // Clear the legalMoves maps
        whiteLegalMovesMap.clear();
        blackLegalMovesMap.clear();

        // Iterate through the pieces of the current player
        const std::vector<std::shared_ptr<Piece>>& currentPlayerPieces = whiteTurn ? whitePieces : blackPieces;

        for (const auto& piece : currentPlayerPieces) {
            if (piece->getType() == "Pawn") {
                std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
                if (pawn) {
                    pawn->generatePawnLegalMoves(whiteTurn ? whiteLegalMovesMap : blackLegalMovesMap, chessboard, whiteTurn);
                }
            } else if (piece->getType() == "Knight") {
                std::shared_ptr<Knight> knight = std::dynamic_pointer_cast<Knight>(piece);
                if (knight) {
                    knight->generateKnightLegalMoves(whiteTurn ? whiteLegalMovesMap : blackLegalMovesMap, chessboard, whiteTurn);
                }
            } else if (piece->getType() == "Bishop") {
                std::shared_ptr<Bishop> bishop = std::dynamic_pointer_cast<Bishop>(piece);
                if (bishop) {
                    bishop->generateBishopLegalMoves(whiteTurn ? whiteLegalMovesMap : blackLegalMovesMap, chessboard, whiteTurn);
                }
            } else if (piece->getType() == "Rook") {
                std::shared_ptr<Rook> rook = std::dynamic_pointer_cast<Rook>(piece);
                if (rook) {
                    rook->generateRookLegalMoves(whiteTurn ? whiteLegalMovesMap : blackLegalMovesMap, chessboard, whiteTurn);
                }
            } else if (piece->getType() == "Queen") {
                std::shared_ptr<Queen> queen = std::dynamic_pointer_cast<Queen>(piece);
                if (queen) {
                    queen->generateQueenLegalMoves(whiteTurn ? whiteLegalMovesMap : blackLegalMovesMap, chessboard, whiteTurn);
                }
            } else if (piece->getType() == "King") {
                std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
                if (king) {
                    king->generateKingLegalMoves(whiteTurn ? whiteLegalMovesMap : blackLegalMovesMap, chessboard, whiteTurn);
                }
            }
        }

        // Remove keys with no associated legal moves
        auto& legalMovesMap = whiteTurn ? whiteLegalMovesMap : blackLegalMovesMap;
        auto it = legalMovesMap.begin();
        while (it != legalMovesMap.end()) {
            if (it->second.empty()) {
                it = legalMovesMap.erase(it);
            } else {
                ++it;
            }
        }
    }

    void virtual_generateLegalMoves(bool whiteTurn) {
        // Clear the legalMoves maps
        virtual_whiteLegalMovesMap.clear();
        virtual_blackLegalMovesMap.clear();

        // Iterate through the pieces of the current player
        const std::vector<std::shared_ptr<Piece>>& currentPlayerPieces = whiteTurn ? virtual_whitePieces : virtual_blackPieces;

        for (const auto& piece : currentPlayerPieces) {
            if (piece->getType() == "Pawn") {
                std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
                if (pawn) {
                    pawn->generatePawnLegalMoves(whiteTurn ? virtual_whiteLegalMovesMap : virtual_blackLegalMovesMap, virtual_chessboard, whiteTurn);
                }
            } else if (piece->getType() == "Knight") {
                std::shared_ptr<Knight> knight = std::dynamic_pointer_cast<Knight>(piece);
                if (knight) {
                    knight->generateKnightLegalMoves(whiteTurn ? virtual_whiteLegalMovesMap : virtual_blackLegalMovesMap, virtual_chessboard, whiteTurn);
                }
            } else if (piece->getType() == "Bishop") {
                std::shared_ptr<Bishop> bishop = std::dynamic_pointer_cast<Bishop>(piece);
                if (bishop) {
                    bishop->generateBishopLegalMoves(whiteTurn ? virtual_whiteLegalMovesMap : virtual_blackLegalMovesMap, virtual_chessboard, whiteTurn);
                }
            } else if (piece->getType() == "Rook") {
                std::shared_ptr<Rook> rook = std::dynamic_pointer_cast<Rook>(piece);
                if (rook) {
                    rook->generateRookLegalMoves(whiteTurn ? virtual_whiteLegalMovesMap : virtual_blackLegalMovesMap, virtual_chessboard, whiteTurn);
                }
            } else if (piece->getType() == "Queen") {
                std::shared_ptr<Queen> queen = std::dynamic_pointer_cast<Queen>(piece);
                if (queen) {
                    queen->generateQueenLegalMoves(whiteTurn ? virtual_whiteLegalMovesMap : virtual_blackLegalMovesMap, virtual_chessboard, whiteTurn);
                }
            } else if (piece->getType() == "King") {
                std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
                if (king) {
                    king->generateKingLegalMoves(whiteTurn ? virtual_whiteLegalMovesMap : virtual_blackLegalMovesMap, virtual_chessboard, whiteTurn);
                }
            }
        }
    }

    // Helper function to print moves
    void printMoves(const std::vector<std::string>& moves) {
        for (const auto& move : moves) {
            std::cout << move << " ";
        }
        std::cout << std::endl;
    }

    void movePieceToPosition(const std::string& fromPosition, const std::string& toPosition) {
        // Find the piece at the 'fromPosition'
        std::shared_ptr<Piece> pieceToMove = findPieceAtPosition(fromPosition);

        if (pieceToMove) {

            // Calculate the new position based on the algebraic notation
            int newY = 8 - (toPosition[1] - '0');
            int newX = toPosition[0] - 'a';

            // Update the piece's position
            pieceToMove->setRow(newY);
            pieceToMove->setCol(newX);

            // Check if there's a piece at the destination position
            std::shared_ptr<Piece> pieceAtDestination = findPieceAtPosition(toPosition);

            if (pieceAtDestination) {
                // Remove the captured piece from the game
                removePieceAtPosition(toPosition);
            }

            // Remove the piece from its current position
            removePieceAtPosition(fromPosition);

            // Add the piece to the new position
            addPieceToPosition(pieceToMove, toPosition);

            // Update the chessboard representation
            updateChessboard(fromPosition, toPosition);
        }
    }

    void resetPieceOccupationMaps() {
        // Reset both piece occupation maps to all zeros
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                whitePieceOccupation[i][j] = 0;
                blackPieceOccupation[i][j] = 0;
            }   
        }
    }

    void virtual_resetPieceOccupationMaps() {
        // Reset both piece occupation maps to all zeros
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                virtual_whitePieceOccupation[i][j] = 0;
                virtual_blackPieceOccupation[i][j] = 0;
            }
        }
    }

    void resetAmbiguityMap() {
        // Reset both piece occupation maps to all zeros
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                ambiguityMap[i][j] = 0;
            }
        }
    }

    void hyper_virtual_resetPieceOccupationMaps() {
        // Reset both piece occupation maps to all zeros
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                hyper_virtual_whitePieceOccupation[i][j] = 0;
                hyper_virtual_blackPieceOccupation[i][j] = 0;
            }
        }
    }

    void createPieceOccupationMaps() {

        resetPieceOccupationMaps();

        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                std::string pieceID = chessboard[y][x];
                char pieceColour = pieceID[0];
                char pieceType = pieceID[1];

                if (pieceType == 'P') {
                    if (pieceColour == 'W') {
                        if (y - 1 < 8 && x - 1 >= 0) {
                            whitePieceOccupation[y - 1][x - 1]++;
                        }
                        if (y - 1 < 8 && x + 1 < 8) {
                            whitePieceOccupation[y - 1][x + 1]++;
                        }
                    } else {
                        if (y + 1 < 8 && x - 1 >= 0) {
                            blackPieceOccupation[y + 1][x - 1]++;
                        }
                        if (y + 1 < 8 && x + 1 < 8) {
                            blackPieceOccupation[y + 1][x + 1]++;
                        }
                    }
                } else if (pieceType == 'N') {
                    int offsetX[8] = {1, 2, 2, 1, -1, -2, -2, -1};
                    int offsetY[8] = {-2, -1, 1, 2, 2, 1, -1, -2};

                    for (int i = 0; i < 8; i++) {
                        int newY = y + offsetY[i];
                        int newX = x + offsetX[i];

                        if (newY >= 0 && newY < 8 && newX >= 0 && newX < 8) {
                            if (pieceColour == 'W') {
                                whitePieceOccupation[newY][newX]++;
                            } else {
                                blackPieceOccupation[newY][newX]++;
                            }
                        }
                    }
                } else if (pieceType == 'B') {
                    // Define the possible offsets for bishop moves
                    int offsetX[4] = {1, 1, -1, -1};
                    int offsetY[4] = {-1, 1, 1, -1};

                    // Check each possible diagonal move for the bishop
                    for (int i = 0; i < 4; i++) {
                        int newY = y + offsetY[i];
                        int newX = x + offsetX[i];

                        while (newY >= 0 && newY < 8 && newX >= 0 && newX < 8) {
                            if (pieceColour == 'W') {
                                if (chessboard[newY][newX] == "-") {
                                    whitePieceOccupation[newY][newX]++;
                                } else {
                                    whitePieceOccupation[newY][newX]++;
                                    break;
                                }
                            } else {
                                if (chessboard[newY][newX] == "-") {
                                    blackPieceOccupation[newY][newX]++;
                                } else {
                                    blackPieceOccupation[newY][newX]++;
                                    break;
                                }
                            }

                            newY += offsetY[i];
                            newX += offsetX[i];
                        }
                    }
                } else if (pieceType == 'R') {
                    // Define the possible offsets for bishop moves
                    int offsetX[4] = {0, 1, 0, -1};
                    int offsetY[4] = {-1, 0, 1, 0};

                    // Check each possible diagonal move for the bishop
                    for (int i = 0; i < 4; i++) {
                        int newY = y + offsetY[i];
                        int newX = x + offsetX[i];

                        while (newY >= 0 && newY < 8 && newX >= 0 && newX < 8) {
                            if (pieceColour == 'W') {
                                if (chessboard[newY][newX] == "-") {
                                    whitePieceOccupation[newY][newX]++;
                                } else {
                                    whitePieceOccupation[newY][newX]++;
                                    break;
                                }
                            } else {
                                if (chessboard[newY][newX] == "-") {
                                    blackPieceOccupation[newY][newX]++;
                                } else {
                                    blackPieceOccupation[newY][newX]++;
                                    break;
                                }
                            }

                            newY += offsetY[i];
                            newX += offsetX[i];
                        }
                    }
                } else if (pieceType == 'Q') {
                    // Define the possible offsets for bishop moves
                    int offsetX[8] = {0, 1, 1, 1, 0, -1, -1, -1};
                    int offsetY[8] = {-1, -1, 0, 1, 1, 1, 0, -1};

                    // Check each possible diagonal move for the bishop
                    for (int i = 0; i < 8; i++) {
                        int newY = y + offsetY[i];
                        int newX = x + offsetX[i];

                        while (newY >= 0 && newY < 8 && newX >= 0 && newX < 8) {
                            if (pieceColour == 'W') {
                                if (chessboard[newY][newX] == "-") {
                                    whitePieceOccupation[newY][newX]++;
                                } else {
                                    whitePieceOccupation[newY][newX]++;
                                    break;
                                }
                            } else {
                                if (chessboard[newY][newX] == "-") {
                                    blackPieceOccupation[newY][newX]++;
                                } else {
                                    blackPieceOccupation[newY][newX]++;
                                    break;
                                }
                            }

                            newY += offsetY[i];
                            newX += offsetX[i];
                        }
                    }
                } else if (pieceType == 'K') {
                    int offsetX[8] = {0, 1, 1, 1, 0, -1, -1, -1};
                    int offsetY[8] = {-1, -1, 0, 1, 1, 1, 0, -1};

                    for (int i = 0; i < 8; i++) {
                        int newY = y + offsetY[i];
                        int newX = x + offsetX[i];

                        if (newY >= 0 && newY < 8 && newX >= 0 && newX < 8) {
                            if (pieceColour == 'W') {
                                whitePieceOccupation[newY][newX]++;
                            } else {
                                blackPieceOccupation[newY][newX]++;
                            }

                            newY += offsetY[i];
                            newX += offsetX[i];
                        }
                    }
                }
            }
        }

        // DISPLAY OCCUPATION MAPS
        /*
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                // FORMATS OUTPUT IN A TABULAR MANNER
                std::cout << std::setw(3) << whitePieceOccupation[i][j];
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                // FORMATS OUTPUT IN A TABULAR MANNER
                std::cout << std::setw(3) << blackPieceOccupation[i][j];
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        */
    }

    void virtual_createPieceOccupationMaps() {
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                std::string pieceID = virtual_chessboard[y][x];
                char pieceColour = pieceID[0];
                char pieceType = pieceID[1];

                if (pieceType == 'P') {
                    if (pieceColour == 'W') {
                        if (y - 1 < 8 && x - 1 >= 0) {
                            virtual_whitePieceOccupation[y - 1][x - 1]++;
                        }
                        if (y - 1 < 8 && x + 1 < 8) {
                            virtual_whitePieceOccupation[y - 1][x + 1]++;
                        }
                    } else {
                        if (y + 1 < 8 && x - 1 >= 0) {
                            virtual_blackPieceOccupation[y + 1][x - 1]++;
                        }
                        if (y + 1 < 8 && x + 1 < 8) {
                            virtual_blackPieceOccupation[y + 1][x + 1]++;
                        }
                    }
                } else if (pieceType == 'N') {
                    int offsetX[8] = {1, 2, 2, 1, -1, -2, -2, -1};
                    int offsetY[8] = {-2, -1, 1, 2, 2, 1, -1, -2};

                    for (int i = 0; i < 8; i++) {
                        int newY = y + offsetY[i];
                        int newX = x + offsetX[i];

                        if (newY >= 0 && newY < 8 && newX >= 0 && newX < 8) {
                            if (pieceColour == 'W') {
                                virtual_whitePieceOccupation[newY][newX]++;
                            } else {
                                virtual_blackPieceOccupation[newY][newX]++;
                            }
                        }
                    }
                } else if (pieceType == 'B') {
                    // Define the possible offsets for bishop moves
                    int offsetX[4] = {1, 1, -1, -1};
                    int offsetY[4] = {-1, 1, 1, -1};

                    // Check each possible diagonal move for the bishop
                    for (int i = 0; i < 4; i++) {
                        int newY = y + offsetY[i];
                        int newX = x + offsetX[i];

                        while (newY >= 0 && newY < 8 && newX >= 0 && newX < 8) {
                            if (pieceColour == 'W') {
                                if (virtual_chessboard[newY][newX] == "-") {
                                    virtual_whitePieceOccupation[newY][newX]++;
                                } else {
                                    virtual_whitePieceOccupation[newY][newX]++;
                                    break;
                                }
                            } else {
                                if (virtual_chessboard[newY][newX] == "-") {
                                    virtual_blackPieceOccupation[newY][newX]++;
                                } else {
                                    virtual_blackPieceOccupation[newY][newX]++;
                                    break;
                                }
                            }

                            newY += offsetY[i];
                            newX += offsetX[i];
                        }
                    }
                } else if (pieceType == 'R') {
                    // Define the possible offsets for bishop moves
                    int offsetX[4] = {0, 1, 0, -1};
                    int offsetY[4] = {-1, 0, 1, 0};

                    // Check each possible diagonal move for the bishop
                    for (int i = 0; i < 4; i++) {
                        int newY = y + offsetY[i];
                        int newX = x + offsetX[i];

                        while (newY >= 0 && newY < 8 && newX >= 0 && newX < 8) {
                            if (pieceColour == 'W') {
                                if (virtual_chessboard[newY][newX] == "-") {
                                    virtual_whitePieceOccupation[newY][newX]++;
                                } else {
                                    virtual_whitePieceOccupation[newY][newX]++;
                                    break;
                                }
                            } else {
                                if (virtual_chessboard[newY][newX] == "-") {
                                    virtual_blackPieceOccupation[newY][newX]++;
                                } else {
                                    virtual_blackPieceOccupation[newY][newX]++;
                                    break;
                                }
                            }

                            newY += offsetY[i];
                            newX += offsetX[i];
                        }
                    }
                } else if (pieceType == 'Q') {
                    // Define the possible offsets for bishop moves
                    int offsetX[8] = {0, 1, 1, 1, 0, -1, -1, -1};
                    int offsetY[8] = {-1, -1, 0, 1, 1, 1, 0, -1};

                    // Check each possible diagonal move for the bishop
                    for (int i = 0; i < 8; i++) {
                        int newY = y + offsetY[i];
                        int newX = x + offsetX[i];

                        while (newY >= 0 && newY < 8 && newX >= 0 && newX < 8) {
                            if (pieceColour == 'W') {
                                if (virtual_chessboard[newY][newX] == "-") {
                                    virtual_whitePieceOccupation[newY][newX]++;
                                } else {
                                    virtual_whitePieceOccupation[newY][newX]++;
                                    break;
                                }
                            } else {
                                if (virtual_chessboard[newY][newX] == "-") {
                                    virtual_blackPieceOccupation[newY][newX]++;
                                } else {
                                    virtual_blackPieceOccupation[newY][newX]++;
                                    break;
                                }
                            }

                            newY += offsetY[i];
                            newX += offsetX[i];
                        }
                    }
                } else if (pieceType == 'K') {
                    int offsetX[8] = {0, 1, 1, 1, 0, -1, -1, -1};
                    int offsetY[8] = {-1, -1, 0, 1, 1, 1, 0, -1};

                    for (int i = 0; i < 8; i++) {
                        int newY = y + offsetY[i];
                        int newX = x + offsetX[i];

                        if (newY >= 0 && newY < 8 && newX >= 0 && newX < 8) {
                            if (pieceColour == 'W') {
                                virtual_whitePieceOccupation[newY][newX]++;
                            } else {
                                virtual_blackPieceOccupation[newY][newX]++;
                            }

                            newY += offsetY[i];
                            newX += offsetX[i];
                        }
                    }
                }
            }
        }

        // DISPLAY OCCUPATION MAPS
        /*
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                // FORMATS OUTPUT IN A TABULAR MANNER
                std::cout << std::setw(3) << virtual_whitePieceOccupation[i][j];
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                // FORMATS OUTPUT IN A TABULAR MANNER
                std::cout << std::setw(3) << virtual_blackPieceOccupation[i][j];
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        */
    }

    void hyper_virtual_createPieceOccupationMaps() {
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                std::string pieceID = hyper_virtual_chessboard[y][x]; // PREFIX HYPER_
                char pieceColour = pieceID[0];
                char pieceType = pieceID[1];

                if (pieceType == 'P') {
                    if (pieceColour == 'W') {
                        if (y - 1 < 8 && x - 1 >= 0) {
                            hyper_virtual_whitePieceOccupation[y - 1][x - 1]++;
                        }
                        if (y - 1 < 8 && x + 1 < 8) {
                            hyper_virtual_whitePieceOccupation[y - 1][x + 1]++;
                        }
                    } else {
                        if (y + 1 < 8 && x - 1 >= 0) {
                            hyper_virtual_blackPieceOccupation[y + 1][x - 1]++;
                        }
                        if (y + 1 < 8 && x + 1 < 8) {
                            hyper_virtual_blackPieceOccupation[y + 1][x + 1]++;
                        }
                    }
                } else if (pieceType == 'N') {
                    int offsetX[8] = {1, 2, 2, 1, -1, -2, -2, -1};
                    int offsetY[8] = {-2, -1, 1, 2, 2, 1, -1, -2};

                    for (int i = 0; i < 8; i++) {
                        int newY = y + offsetY[i];
                        int newX = x + offsetX[i];

                        if (newY >= 0 && newY < 8 && newX >= 0 && newX < 8) {
                            if (pieceColour == 'W') {
                                hyper_virtual_whitePieceOccupation[newY][newX]++;
                            } else {
                                hyper_virtual_blackPieceOccupation[newY][newX]++;
                            }
                        }
                    }
                } else if (pieceType == 'B') {
                    // Define the possible offsets for bishop moves
                    int offsetX[4] = {1, 1, -1, -1};
                    int offsetY[4] = {-1, 1, 1, -1};

                    // Check each possible diagonal move for the bishop
                    for (int i = 0; i < 4; i++) {
                        int newY = y + offsetY[i];
                        int newX = x + offsetX[i];

                        while (newY >= 0 && newY < 8 && newX >= 0 && newX < 8) {
                            if (pieceColour == 'W') {
                                if (hyper_virtual_chessboard[newY][newX] == "-") {
                                    hyper_virtual_whitePieceOccupation[newY][newX]++;
                                } else {
                                    hyper_virtual_whitePieceOccupation[newY][newX]++;
                                    break;
                                }
                            } else {
                                if (hyper_virtual_chessboard[newY][newX] == "-") {
                                    hyper_virtual_blackPieceOccupation[newY][newX]++;
                                } else {
                                    hyper_virtual_blackPieceOccupation[newY][newX]++;
                                    break;
                                }
                            }

                            newY += offsetY[i];
                            newX += offsetX[i];
                        }
                    }
                } else if (pieceType == 'R') {
                    // Define the possible offsets for bishop moves
                    int offsetX[4] = {0, 1, 0, -1};
                    int offsetY[4] = {-1, 0, 1, 0};

                    // Check each possible diagonal move for the bishop
                    for (int i = 0; i < 4; i++) {
                        int newY = y + offsetY[i];
                        int newX = x + offsetX[i];

                        while (newY >= 0 && newY < 8 && newX >= 0 && newX < 8) {
                            if (pieceColour == 'W') {
                                if (hyper_virtual_chessboard[newY][newX] == "-") {
                                    hyper_virtual_whitePieceOccupation[newY][newX]++;
                                } else {
                                    hyper_virtual_whitePieceOccupation[newY][newX]++;
                                    break;
                                }
                            } else {
                                if (hyper_virtual_chessboard[newY][newX] == "-") {
                                    hyper_virtual_blackPieceOccupation[newY][newX]++;
                                } else {
                                    hyper_virtual_blackPieceOccupation[newY][newX]++;
                                    break;
                                }
                            }

                            newY += offsetY[i];
                            newX += offsetX[i];
                        }
                    }
                } else if (pieceType == 'Q') {
                    // Define the possible offsets for bishop moves
                    int offsetX[8] = {0, 1, 1, 1, 0, -1, -1, -1};
                    int offsetY[8] = {-1, -1, 0, 1, 1, 1, 0, -1};

                    // Check each possible diagonal move for the bishop
                    for (int i = 0; i < 8; i++) {
                        int newY = y + offsetY[i];
                        int newX = x + offsetX[i];

                        while (newY >= 0 && newY < 8 && newX >= 0 && newX < 8) {
                            if (pieceColour == 'W') {
                                if (hyper_virtual_chessboard[newY][newX] == "-") {
                                    hyper_virtual_whitePieceOccupation[newY][newX]++;
                                } else {
                                    hyper_virtual_whitePieceOccupation[newY][newX]++;
                                    break;
                                }
                            } else {
                                if (hyper_virtual_chessboard[newY][newX] == "-") {
                                    hyper_virtual_blackPieceOccupation[newY][newX]++;
                                } else {
                                    hyper_virtual_blackPieceOccupation[newY][newX]++;
                                    break;
                                }
                            }

                            newY += offsetY[i];
                            newX += offsetX[i];
                        }
                    }
                } else if (pieceType == 'K') {
                    int offsetX[8] = {0, 1, 1, 1, 0, -1, -1, -1};
                    int offsetY[8] = {-1, -1, 0, 1, 1, 1, 0, -1};

                    for (int i = 0; i < 8; i++) {
                        int newY = y + offsetY[i];
                        int newX = x + offsetX[i];

                        if (newY >= 0 && newY < 8 && newX >= 0 && newX < 8) {
                            if (pieceColour == 'W') {
                                hyper_virtual_whitePieceOccupation[newY][newX]++;
                            } else {
                                hyper_virtual_blackPieceOccupation[newY][newX]++;
                            }

                            newY += offsetY[i];
                            newX += offsetX[i];
                        }
                    }
                }
            }
        }

        // DISPLAY OCCUPATION MAPS
        /*
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                // FORMATS OUTPUT IN A TABULAR MANNER
                std::cout << std::setw(3) << hyper_virtual_whitePieceOccupation[i][j];
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                // FORMATS OUTPUT IN A TABULAR MANNER
                std::cout << std::setw(3) << hyper_virtual_blackPieceOccupation[i][j];
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        */
    }

    void printTryMoves(const std::map<std::string, std::vector<int>>& results) {
        std::cout << "Results Map:" << std::endl;
        for (const auto& entry : results) {
            const std::string& position = entry.first;
            const std::vector<int>& moveLegality = entry.second;
            std::cout << position << ": ";
            for (int legality : moveLegality) {
                std::cout << legality << " ";
            }
            std::cout << ", ";
        }
        std::cout << std::endl;
    }

    std::map<std::string, std::vector<int>> TryMoves(bool whiteTurn, const std::map<std::string, std::vector<std::string>>& legalMovesMap) {
        std::map<std::string, std::vector<int>> results;

        for (const auto& move : legalMovesMap) {
            const std::string& key = move.first;
            const std::vector<std::string>& values = move.second;

            std::vector<int> legality;

            // Loop through values and print each move
            for (const std::string& value : values) {

                std::string move = value;

                char character;
                
                if (value.length() >= 4 && value[value.length() - 2] == '=' && std::isalpha(value[value.length() - 1])) {
                    character = value.back();
                    std::cout << character << std::endl;
                    move.erase(move.end() - 2, move.end());
                }
                
                std::string moveTo = move.substr(move.length() - 2);

                // std::cout << "Trying move: " << key << " to " << moveTo << std::endl;

                virtual_resetPieceOccupationMaps();
                
                for (int y = 0; y < 8; y++) {
                    for (int x = 0; x < 8; x++) {
                        virtual_chessboard[y][x] = chessboard[y][x];
                    }
                }

                // Extract the source and destination positions from the move
                std::string moveFrom = key;

                int fromY = 8 - (moveFrom[1] - '0');
                int fromX = moveFrom[0] - 'a';
                int toY = 8 - (moveTo[1] - '0');
                int toX = moveTo[0] - 'a';

                std::string temp = virtual_chessboard[fromY][fromX];
                virtual_chessboard[fromY][fromX] = "-";
                virtual_chessboard[toY][toX] = temp;

                // FIX BUGS

                if (value.length() >= 4 && value[value.length() - 2] == '=' && std::isalpha(value[value.length() - 1])) {
                    virtual_chessboard[toY][toX] = std::string(1, temp[0]) + std::string(1, character);
                }

                /*
                for (int y = 0; y < 8; y++) {
                    for (int x = 0; x < 8; x++) {
                        std::cout << std::setw(3) << virtual_chessboard[y][x];
                    }
                    std::cout << std::endl;
                }
                std::cout << std::endl;
                */

                // CALL DUPLICATE PIECEOCCUPATIONMAP FUNCTION
                virtual_createPieceOccupationMaps();

                int whiteKingY, whiteKingX, blackKingY, blackKingX;

                for (int y = 0; y < 8; y++) {
                    for (int x = 0; x < 8; x++) {
                        if (virtual_chessboard[y][x] == "WK") {
                            whiteKingY = y;
                            whiteKingX = x;
                        } else if (virtual_chessboard[y][x] == "BK") {
                            blackKingY = y;
                            blackKingX = x;
                        }
                    }
                }

                std::map<std::string, std::vector<std::string>> map = TwoMoveScan(!whiteTurn, !whiteTurn ? virtual_whiteLegalMovesMap : virtual_blackLegalMovesMap);

                if (whiteTurn) {
                    if (virtual_blackPieceOccupation[whiteKingY][whiteKingX] != 0) {
                        legality.push_back(1);
                    } else {
                        if (virtual_whitePieceOccupation[blackKingY][blackKingX] != 0) {
                            if (map.empty()) {
                                legality.push_back(3);
                            } else {
                                legality.push_back(2);
                            }
                        } else {
                            legality.push_back(0);
                        }
                    }
                } else {
                    if (virtual_whitePieceOccupation[blackKingY][blackKingX] != 0) {
                        legality.push_back(1);
                    } else {
                        if (virtual_blackPieceOccupation[whiteKingY][whiteKingX] != 0) {
                            if (map.empty()) {
                                legality.push_back(3);
                            } else {
                                legality.push_back(2);
                            }
                        } else {
                            legality.push_back(0);
                        }
                    }
                }
            }

            results[key] = legality;
        }

        // printTryMoves(results);
        // Scans for checks
        scanForChecks(whiteTurn, whiteTurn ? whiteLegalMovesMap : blackLegalMovesMap, results);
        // Scans for checks
        scanForCheckmates(whiteTurn, whiteTurn ? whiteLegalMovesMap : blackLegalMovesMap, results);
        // Filter out illegal moves from legalMovesMap
        filterIllegalMoves(whiteTurn, whiteTurn ? whiteLegalMovesMap : blackLegalMovesMap, results);

        return results;
    }
    
    std::map<std::string, std::vector<std::string>> TwoMoveScan(bool whiteTurn, const std::map<std::string, std::vector<std::string>>& virtual_legalMovesMap) {
        virtual_initializeBoard();
        virtual_generateLegalMoves(whiteTurn);
        // virtual_printLegalMoves(whiteTurn);

        std::map<std::string, std::vector<std::string>> results;

        for (const auto& move : virtual_legalMovesMap) {
            const std::string& key = move.first;
            const std::vector<std::string>& values = move.second;

            // Loop through values and print each move
            for (const std::string& value : values) {

                std::string move = value;

                char character;
                
                if (value.length() >= 4 && value[value.length() - 2] == '=' && std::isalpha(value[value.length() - 1])) {
                    character = value.back();
                    std::cout << character << std::endl;
                    move.erase(move.end() - 2, move.end());
                }
                
                std::string moveTo = move.substr(move.length() - 2);

                hyper_virtual_resetPieceOccupationMaps();

                // GENERATING LEGAL MOVES FOR EACH PLAYER IN EACH VIRTUAL_CHESSBOARD POSITION
                for (int y = 0; y < 8; y++) {
                    for (int x = 0; x < 8; x++) {
                        hyper_virtual_chessboard[y][x] = virtual_chessboard[y][x];
                    }
                }

                // Extract the source and destination positions from the move
                std::string moveFrom = key;

                int fromY = 8 - (moveFrom[1] - '0');
                int fromX = moveFrom[0] - 'a';
                int toY = 8 - (moveTo[1] - '0');
                int toX = moveTo[0] - 'a';

                std::string temp = hyper_virtual_chessboard[fromY][fromX];
                hyper_virtual_chessboard[fromY][fromX] = "-";
                hyper_virtual_chessboard[toY][toX] = temp;

                if (value.length() >= 4 && value[value.length() - 2] == '=' && std::isalpha(value[value.length() - 1])) {
                    hyper_virtual_chessboard[toY][toX] = std::string(1, temp[0]) + std::string(1, character);
                }

                /*
                for (int y = 0; y < 8; y++) {
                    for (int x = 0; x < 8; x++) {
                        std::cout << std::setw(3) << hyper_virtual_chessboard[y][x];
                    }
                    std::cout << std::endl;
                }
                */

                // CALL DUPLICATE PIECEOCCUPATIONMAP FUNCTION
                hyper_virtual_createPieceOccupationMaps();

                int whiteKingY, whiteKingX, blackKingY, blackKingX;

                for (int y = 0; y < 8; y++) {
                    for (int x = 0; x < 8; x++) {
                        if (hyper_virtual_chessboard[y][x] == "WK") {
                            whiteKingY = y;
                            whiteKingX = x;
                        } else if (hyper_virtual_chessboard[y][x] == "BK") {
                            blackKingY = y;
                            blackKingX = x;
                        }
                    }
                }

                if (whiteTurn) {
                    if (hyper_virtual_blackPieceOccupation[whiteKingY][whiteKingX] == 0) {
                        // If the condition is met, add the key-value pair to the results map
                        results[key] = values;
                    }
                } else {
                    if (hyper_virtual_whitePieceOccupation[blackKingY][blackKingX] == 0) {
                        // If the condition is met, add the key-value pair to the results map
                        results[key] = values;
                    }
                }
            }
        }

        return results;
    }

    void filterIllegalMoves(bool whiteTurn, std::map<std::string, std::vector<std::string>>& legalMovesMap, std::map<std::string, std::vector<int>>& results) {
        for (auto it = legalMovesMap.begin(); it != legalMovesMap.end();) {
            const std::string& key = it->first;
            std::vector<std::string>& legalMoves = it->second;

            const std::vector<int>& moveResults = results[key];

            // Iterate in reverse to remove from highest index to lowest
            for (int i = legalMoves.size() - 1; i >= 0; --i) {
                if (moveResults[i] == 1) {
                    legalMoves.erase(legalMoves.begin() + i);
                }
            }

            // Remove keys with no associated legal moves
            if (legalMoves.empty()) {
                it = legalMovesMap.erase(it);
            } else {
                ++it;
            }
        }
    }

    void scanForChecks(bool whiteTurn, std::map<std::string, std::vector<std::string>>& legalMovesMap, const std::map<std::string, std::vector<int>>& results) {
        for (auto& entry : legalMovesMap) {
            const std::string& key = entry.first;
            std::vector<std::string>& legalMoves = entry.second;

            const std::vector<int>& moveResults = results.at(key);

            for (int i = 0; i < legalMoves.size(); ++i) {
                if (moveResults[i] == 2) {
                    // Add a "+" to the move to indicate a check
                    legalMoves[i] += "+";
                }
            }
        }
    }

    void scanForCheckmates(bool whiteTurn, std::map<std::string, std::vector<std::string>>& legalMovesMap, const std::map<std::string, std::vector<int>>& results) {
        for (auto& entry : legalMovesMap) {
            const std::string& key = entry.first;
            std::vector<std::string>& legalMoves = entry.second;

            const std::vector<int>& moveResults = results.at(key);

            for (int i = 0; i < legalMoves.size(); ++i) {
                if (moveResults[i] == 3) {
                    // Add a "+" to the move to indicate a check
                    legalMoves[i] += "#";
                }
            }
        }
    }

    void scanAmbiguity(bool whiteTurn, const std::map<std::string, std::vector<std::string>>& legalMovesMap) {

        char scanForColour = whiteTurn ? 'W' : 'B';
        char scanForPieceType[4] = {'N', 'B', 'R', 'Q'};

        for (int i = 0; i < 4; i++) {
            resetAmbiguityMap();
            matchingPieceKeys = {};

            std::string targetID = std::string(1, scanForColour) + scanForPieceType[i];
            // Iterate through all squares on the chessboard
            for (int y = 0; y < 8; y++) {
                for (int x = 0; x < 8; x++) {
                    // Check if the piece on the current square matches the target ID
                    if (chessboard[y][x] == targetID) {
                        // Convert the piece's location to algebraic notation
                        std::string pieceLocation = std::string(1, static_cast<char>('a' + x)) + std::to_string(8 - y);

                        // Add the pieceLocation to the vector of matching piece keys
                        matchingPieceKeys.push_back(pieceLocation);

                        // Iterate through all keys in legalMovesMap
                        for (const auto& keyValuePair : legalMovesMap) {
                            const std::string& key = keyValuePair.first;
                            
                            // Check if pieceLocation matches the key
                            if (pieceLocation == key) {
                                // pieceLocation matches a key in legalMovesMap
                                // Now you can process this piece's legal moves
                                const std::vector<std::string>& legalMoves = keyValuePair.second;
                                
                                // Iterate through legalMoves and handle ambiguity
                                for (const std::string& legalMove : legalMoves) {
                                    // Create a new string variable to store the modified legal move
                                    std::string cleanLegalMove = legalMove;
                                    
                                    // Remove '+' or '#' if they exist
                                    if (!cleanLegalMove.empty() && (cleanLegalMove.back() == '+' || cleanLegalMove.back() == '#')) {
                                        cleanLegalMove.pop_back();
                                    }
                                    
                                    // Initialize the string with just the last two characters (coordinates in algebraic notation)
                                    cleanLegalMove = cleanLegalMove.substr(cleanLegalMove.length() - 2);

                                    // Update ambiguityMap based on the last two characters of cleanLegalMove
                                    int toY = 8 - (cleanLegalMove[1] - '0');
                                    int toX = cleanLegalMove[0] - 'a';
                                    ambiguityMap[toY][toX]++;
                                }
                            }
                        }
                    }
                }
            }

            // DEBUGGING
            /*
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    std::cout << std::setw(3) << ambiguityMap[i][j];
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
            */

            // CHECKING AMBIGUITY
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    if (ambiguityMap[i][j] > 1) {
                        std::string square = std::string(1, static_cast<char>('a' + j)) + std::to_string(8 - i);
                        // Check if the square has ambiguity
                        // Find matching piece keys for this square
                        std::vector<std::string> ambiguousPieces;
                        for (const auto& keyValuePair : legalMovesMap) {
                            const std::string& key = keyValuePair.first;
                            bool keyExistsInMatchingPieces = false;

                            // Check if the key exists in matchingPieceKeys
                            for (const std::string& matchingKey : matchingPieceKeys) {
                                if (key == matchingKey) {
                                    keyExistsInMatchingPieces = true;
                                    break;
                                }
                            }

                            // If the key exists in matchingPieceKeys, check for square coordinates
                            if (keyExistsInMatchingPieces) {
                                // Convert the square coordinates to algebraic notation
                                if (legalMovesMap.find(key) != legalMovesMap.end()) {
                                    const std::vector<std::string>& legalMoves = legalMovesMap.find(key)->second;
                                    for (const std::string& legalMove : legalMoves) {
                                        if (legalMove.find(square) != std::string::npos) {
                                            ambiguousPieces.push_back(key);
                                            break; // No need to check further legal moves for this key
                                        }
                                    }
                                }
                            }
                        }
                        /*
                        // Print the ambiguous pieces
                        if (!ambiguousPieces.empty()) {
                            std::cout << "Ambiguous Pieces at Square " << std::string(1, static_cast<char>('a' + j)) << 8 - i << ": ";
                            for (const std::string& piece : ambiguousPieces) {
                                std::cout << piece << " ";
                            }
                            std::cout << std::endl;
                        }
                        */

                        std::string concatenatedPieces;
                        for (const std::string& piece : ambiguousPieces) {
                            concatenatedPieces += piece;
                        }

                        // COUNT OCCURANCES OF EACH CHARACTER IN PIECE
                        for (const std::string& piece : ambiguousPieces) {
                            int letterCount = 0;
                            int numCount = 0;
                            char searchLetter = piece[0];
                            char searchNum = piece[1];

                            // SCAN
                            for (char c : concatenatedPieces) {
                                if (c == searchLetter) {
                                    letterCount++;
                                } else if (c == searchNum) {
                                    numCount++;
                                }
                            }

                            // std::cout << "Letter Count: " << letterCount << std::endl;
                            // std::cout << "Number Count: " << numCount << std::endl;

                            // Search for the ambiguous piece in legalMovesMap
                            auto it = legalMovesMap.find(piece);
                            
                            if (it != legalMovesMap.end()) {
                                // Found the ambiguous piece, now let's scan its values
                                const std::vector<std::string>& values = it->second;
                                
                                for (auto itValue = values.begin(); itValue != values.end(); ++itValue) {
                                    // Check if the value contains the ambiguity square
                                    size_t pos = itValue->find(square);
                                    if (pos != std::string::npos) {
                                        // The substring square exists in value
                                        std::string value = *itValue;

                                        // Initialize pieceTypeIdentifier as the first character of value
                                        char pieceTypeIdentifier = value[0];

                                        // Initialize remainder as the rest of value (excluding the first character)
                                        std::string remainder = value.substr(1);

                                        std::string newValue;

                                        if (letterCount > 1 && numCount > 1) {
                                            newValue = std::string(1, pieceTypeIdentifier) + piece + remainder;
                                        } else if (letterCount > 1 && numCount == 1) {
                                            newValue = std::string(1, pieceTypeIdentifier) + searchNum + remainder;
                                        } else {
                                            newValue = std::string(1, pieceTypeIdentifier) + searchLetter + remainder;
                                        }
                                        
                                        std::cout << newValue << std::endl;

                                        modifyLegalMove(whiteTurn ? whiteLegalMovesMap : blackLegalMovesMap, piece, value, newValue);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void modifyLegalMove(std::map<std::string, std::vector<std::string>>& legalMovesMap, const std::string& key, std::string& value, const std::string& newValue) {
        auto it = legalMovesMap.find(key);
        if (it != legalMovesMap.end()) {
            std::vector<std::string>& values = it->second;
            for (std::string& val : values) {
                if (val == value) {
                    val = newValue; // Update the value with newValue
                    break; // No need to continue searching
                }
            }
        }
    }

    void checkCastlingRights(bool whiteTurn, const std::map<std::string, std::vector<std::string>>& legalMovesMap) {
        int x = whiteTurn ? 0 : 1;
        // THE PLAYER MUST NOT HAVE CASTLED ALREADY OR MOVED THEIR KING
        for (int i = x; i < movesLog.size(); i += 2) {

            whiteCanCastleKingside = false;
            whiteCanCastleQueenside = false;
            blackCanCastleKingside = false;
            blackCanCastleQueenside = false;
            
            if (movesLog[i] == "O-O" || movesLog[i] == "O-O-O" || movesLog[i].find('K') != std::string::npos) {
                if (whiteTurn) {
                    whiteCastlingRightsKingside = false;
                    whiteCastlingRightsQueenside = false;
                    whiteCanCastleKingside = false;
                    whiteCanCastleQueenside = false;
                } else {
                    blackCastlingRightsKingside = false;
                    blackCastlingRightsQueenside = false;
                    blackCanCastleKingside = false;
                    blackCanCastleQueenside = false;
                }
                return;
            }
        }

        // A PLAYER CANNOT CASTLE WHEN THEY'RE IN CHECK
        if (movesLog.back().find('+') != std::string::npos) {
            if (whiteTurn) {
                whiteCanCastleKingside = false;
                whiteCanCastleQueenside = false;
            } else {
                blackCanCastleKingside = false;
                blackCanCastleQueenside = false;
            }
            return;
        }

        std::cout << "whiteRookMoves: ";
        for (const std::string& move : whiteRookMoves) {
            std::cout << move << " ";
        }
        std::cout << std::endl;

        std::cout << "blackRookMoves: ";
        for (const std::string& move : blackRookMoves) {
            std::cout << move << " ";
        }
        std::cout << std::endl;
        
        const std::vector<std::string>& rookMoves = whiteTurn ? whiteRookMoves : blackRookMoves;
        for (const std::string& move : rookMoves) {
            if (whiteTurn) {
                if (move == "a1") {
                    whiteCanCastleQueenside = false;
                    whiteCastlingRightsQueenside = false;
                } 
                if (move == "h1"){
                    whiteCanCastleKingside = false;
                    whiteCastlingRightsKingside = false;
                }
            } else {
                if (move == "a8") {
                    blackCanCastleQueenside = false;
                    blackCastlingRightsQueenside = false;
                } 
                if (move == "h8"){
                    blackCanCastleKingside = false;
                    blackCastlingRightsKingside = false;
                }
            }
        }

        if (whiteCastlingRightsKingside) {
            if (chessboard[7][5] == "-" && chessboard[7][6] == "-" && blackPieceOccupation[7][5] == 0 && blackPieceOccupation[7][6] == 0) {
                whiteCanCastleKingside = true;
            }
        } 
        if (whiteCastlingRightsQueenside) {
            if (chessboard[7][1] == "-" && chessboard[7][2] == "-" && chessboard[7][3] == "-" && blackPieceOccupation[7][2] == 0 && blackPieceOccupation[7][3] == 0) {
                whiteCanCastleQueenside = true;
            }
        }
        if (blackCastlingRightsKingside) {
            if (chessboard[0][5] == "-" && chessboard[0][6] == "-" && whitePieceOccupation[0][5] == 0 && whitePieceOccupation[0][6] == 0) {
                blackCanCastleKingside = true;
            }
        }
        if (blackCastlingRightsQueenside) {
            if (chessboard[0][1] == "-" && chessboard[0][2] == "-" && chessboard[0][3] == "-" && whitePieceOccupation[0][2] == 0 && whitePieceOccupation[0][3] == 0) {
                blackCanCastleQueenside = true;
            }
        }

        std::string castlingMove;

        if (whiteTurn) {
            if (whiteCanCastleKingside && whiteCanCastleKingside) {
                castlingMove = "O-O";
                addCastlingMove(whiteTurn, castlingMove, whiteTurn ? whiteLegalMovesMap : blackLegalMovesMap);
            } 
            if (whiteCanCastleQueenside && whiteCanCastleQueenside) {
                castlingMove = "O-O-O";
                addCastlingMove(whiteTurn, castlingMove, whiteTurn ? whiteLegalMovesMap : blackLegalMovesMap);
            }
        } else {
            if (blackCanCastleKingside && blackCanCastleKingside) {
                castlingMove = "O-O";
                addCastlingMove(whiteTurn, castlingMove, whiteTurn ? whiteLegalMovesMap : blackLegalMovesMap);
            } 
            if (blackCanCastleQueenside && blackCanCastleQueenside) {
                castlingMove = "O-O-O";
                addCastlingMove(whiteTurn, castlingMove, whiteTurn ? whiteLegalMovesMap : blackLegalMovesMap);
            }
        }
    }

    void findKeyOfRook(bool whiteTurn, std::string input_copy, const std::map<std::string, std::vector<std::string>>& legalMovesMap) {
        for (const auto& keyValuePair : legalMovesMap) {
            const std::string& key = keyValuePair.first;
            const std::vector<std::string>& values = keyValuePair.second;

            // Check if input_copy is found within the values
            if (std::find(values.begin(), values.end(), input_copy) != values.end()) {
                // Push the key to the correct player's rook moves vector
                if (whiteTurn) {
                    whiteRookMoves.push_back(key);
                } else {
                    blackRookMoves.push_back(key);
                }
                break; // Exit the loop after finding the key
            }
        }
    }

    void addCastlingMove(bool whiteTurn, std::string castlingMove, std::map<std::string, std::vector<std::string>>& legalMovesMap) {
        std::string kingPosition = whiteTurn ? "e1" : "e8"; // Determine the king's position based on the player's turn
    
        // Check if the king's position exists in the legalMovesMap
        auto it = legalMovesMap.find(kingPosition);
        
        if (it != legalMovesMap.end()) {
            // Found the king's position in legalMovesMap
            // Add the castling move to the list of legal moves for the king's position
            it->second.push_back(castlingMove);
        }
    }

    void performCastling(bool whiteTurn, const std::string& castlingMove) {
        // Check if it's kingside or queenside castling
        if (castlingMove == "O-O") {
            // Kingside castling
            if (whiteTurn) {
                // Check if white can kingside castle
                if (whiteCanCastleKingside) {
                    // Move the king
                    movePieceToPosition("e1", "g1");
                    // Move the rook
                    movePieceToPosition("h1", "f1");
                    // Update castling rights
                    whiteCanCastleKingside = false;
                    whiteCanCastleQueenside = false;
                    whiteCastlingRightsKingside = false;
                    whiteCastlingRightsQueenside = false;
                }
            } else {
                // Check if black can kingside castle
                if (blackCanCastleKingside) {
                    // Move the king
                    movePieceToPosition("e8", "g8");
                    // Move the rook
                    movePieceToPosition("h8", "f8");
                    // Update castling rights
                    blackCanCastleKingside = false;
                    blackCanCastleQueenside = false;
                    blackCastlingRightsKingside = false;
                    blackCastlingRightsQueenside = false;
                }
            }
        } else if (castlingMove == "O-O-O") {
            // Queenside castling
            if (whiteTurn) {
                // Check if white can queenside castle
                if (whiteCanCastleQueenside) {
                    // Move the king
                    movePieceToPosition("e1", "c1");
                    // Move the rook
                    movePieceToPosition("a1", "d1");
                    // Update castling rights
                    whiteCanCastleKingside = false;
                    whiteCanCastleQueenside = false;
                    whiteCastlingRightsKingside = false;
                    whiteCastlingRightsQueenside = false;
                }
            } else {
                // Check if black can queenside castle
                if (blackCanCastleQueenside) {
                    // Move the king
                    movePieceToPosition("e8", "c8");
                    // Move the rook
                    movePieceToPosition("a8", "d8");
                    // Update castling rights
                    blackCanCastleKingside = false;
                    blackCanCastleQueenside = false;
                    blackCastlingRightsKingside = false;
                    blackCastlingRightsQueenside = false;
                }
            }
        }
    }

    // Function to check for pawn promotion moves and add them
    void checkPawnPromotions(bool whiteTurn, const std::map<std::string, std::vector<std::string>>& legalMovesMap) {
        
        for (auto& keyValuePair : legalMovesMap) {
            const std::string& key = keyValuePair.first;
            const std::vector<std::string>& values = keyValuePair.second;

            // Create a vector to store new promotion moves
            std::vector<std::string> newMoves;

            // Create a vector to store pawn promotion moves for this key
            std::vector<std::string> pawnPromotionMoves;

            for (const std::string& move : values) {
                if (whiteTurn) {
                    // Check if it's a white pawn promotion move
                    if (move.back() == '8' && std::all_of(move.begin(), move.end() - 1, ::islower)) {
                        pawnPromotionMoves.push_back(move);
                    }
                    else {
                        // Keep non-promotion moves for white
                        newMoves.push_back(move);
                    }
                }
                else {
                    // Check if it's a black pawn promotion move
                    if (move.back() == '1' && std::all_of(move.begin(), move.end() - 1, ::islower)) {
                        pawnPromotionMoves.push_back(move);
                    }
                    else {
                        // Keep non-promotion moves for black
                        newMoves.push_back(move);
                    }
                }
            }

            // Add the original moves without promotion
            newMoves.insert(newMoves.end(), pawnPromotionMoves.begin(), pawnPromotionMoves.end());

            // Call a function to add 4 new promotion moves for each promotion move
            for (const std::string& promotionMove : pawnPromotionMoves) {
                addPawnPromotionMoves(key, promotionMove, whiteTurn ? whiteLegalMovesMap : blackLegalMovesMap);
            }
        }
    }

    // Function to add promotion moves for a pawn promotion move
    void addPawnPromotionMoves(const std::string& key, const std::string& pawnPromotionMove, std::map<std::string, std::vector<std::string>>& legalMovesMap) {
        // Find the key that contains pawnPromotionMove
        auto it = legalMovesMap.find(key);

        if (it != legalMovesMap.end()) {
            std::vector<std::string>& moves = it->second;

            // Remove the original pawn promotion move
            moves.erase(std::remove(moves.begin(), moves.end(), pawnPromotionMove), moves.end());

            // Add 4 new promotion moves
            // Add promotion move with Queen (Q)
            moves.push_back(pawnPromotionMove + "=Q");
            // Add promotion move with Rook (R)
            moves.push_back(pawnPromotionMove + "=R");
            // Add promotion move with Knight (N)
            moves.push_back(pawnPromotionMove + "=N");
            // Add promotion move with Bishop (B)
            moves.push_back(pawnPromotionMove + "=B");
        }
    }

    void performPawnPromotion(bool whiteTurn, std::string input_copy, char promotionPiece) {
        input_copy = input_copy.substr(input_copy.length() - 2);
        int y = 8 - (input_copy[1] - '0');
        int x = input_copy[0] - 'a';
        char colour = whiteTurn ? 'W' : 'B';
        std::string pieceID = std::string(1, colour) + std::string(1, promotionPiece);
        chessboard[y][x] = pieceID;

        // Find the pawn that reached the promotion square based on its position
        std::shared_ptr<Piece> pawnToPromote = nullptr;

        // Dereference the shared pointers to obtain raw pointers
        std::vector<std::shared_ptr<Piece>>& pieces = whiteTurn ? whitePieces : blackPieces;

        for (const auto& piece_ptr : pieces) {
            if (piece_ptr->getRowIndex() == y && piece_ptr->getColIndex() == x && piece_ptr->getType() == "Pawn") {
                pawnToPromote = piece_ptr;
                break;
            }
        }

        if (pawnToPromote != nullptr) {
            // Create the new piece
            std::shared_ptr<Piece> newPiece = nullptr;
            if (promotionPiece == 'Q') {
                newPiece = std::make_shared<Queen>(whiteTurn ? "White" : "Black", y, x);
            } else if (promotionPiece == 'R') {
                newPiece = std::make_shared<Rook>(whiteTurn ? "White" : "Black", y, x);
            } else if (promotionPiece == 'B') {
                newPiece = std::make_shared<Bishop>(whiteTurn ? "White" : "Black", y, x);
            } else if (promotionPiece == 'N') {
                newPiece = std::make_shared<Knight>(whiteTurn ? "White" : "Black", y, x);
            }

            if (newPiece != nullptr) {
                // Find and erase the old pawn from the vector
                auto iter = std::find(pieces.begin(), pieces.end(), pawnToPromote);
                if (iter != pieces.end()) {
                    pieces.erase(iter);
                }

                // Optionally, reset the shared_ptr for pawnToPromote (no need to delete it)
                pawnToPromote.reset();

                // Add the new piece to the appropriate vector
                pieces.push_back(newPiece);
            }
        }
    }

    const LegalMovesMap& getWhiteLegalMovesMap() const {
        return whiteLegalMovesMap;
    }

    const LegalMovesMap& getBlackLegalMovesMap() const {
        return blackLegalMovesMap;
    }

    const LegalMovesMap& getVirtualWhiteLegalMovesMap() const {
        return virtual_whiteLegalMovesMap;
    }

    const LegalMovesMap& getVirtualBlackLegalMovesMap() const {
        return virtual_blackLegalMovesMap;
    }

private:

    // SETS SIZE OF CHESSBOARD (8x8)
    std::string chessboard[8][8];
    // STORES ALL OF WHITE'S PIECES AS A VECTOR
    std::vector<std::shared_ptr<Piece>> whitePieces;
    // STORES ALL OF BLACK'S PIECES AS A VECTOR
    std::vector<std::shared_ptr<Piece>> blackPieces;

    // SETS SIZE OF VIRTUAL CHESSBOARD (8x8)
    std::string virtual_chessboard[8][8];
    // STORES ALL OF WHITE'S PIECES IN A VIRTUAL POSITION
    std::vector<std::shared_ptr<Piece>> virtual_whitePieces;
    // STORES ALL OF BLACK'S PIECES IN A VIRTUAL POSITION
    std::vector<std::shared_ptr<Piece>> virtual_blackPieces;

    // Create a vector to store the keys of pieces matching the target ID
    std::vector<std::string> matchingPieceKeys;

    // SETS SIZE OF VIRTUAL CHESSBOARD (8x8)
    std::string hyper_virtual_chessboard[8][8];

    // Define LegalMovesMap for white and black pieces
    LegalMovesMap whiteLegalMovesMap;
    LegalMovesMap blackLegalMovesMap;

    // Define LegalMovesMap for virtua wwhite and black pieces
    LegalMovesMap virtual_whiteLegalMovesMap;
    LegalMovesMap virtual_blackLegalMovesMap;

    // Helper function to find a piece at a given position
    std::shared_ptr<Piece> findPieceAtPosition(const std::string& position) {
        for (auto& piece : whitePieces) {
            if (piece->getPosition() == position) {
                return piece;
            }
        }
        for (auto& piece : blackPieces) {
            if (piece->getPosition() == position) {
                return piece;
            }
        }
        return nullptr; // Piece not found
    }

    // Helper function to remove a piece at a given position
    void removePieceAtPosition(const std::string& position) {
        whitePieces.erase(std::remove_if(whitePieces.begin(), whitePieces.end(),
            [&position](const std::shared_ptr<Piece>& piece) {
                return piece->getPosition() == position;
            }), whitePieces.end());

        blackPieces.erase(std::remove_if(blackPieces.begin(), blackPieces.end(),
            [&position](const std::shared_ptr<Piece>& piece) {
                return piece->getPosition() == position;
            }), blackPieces.end());
    }

    // Helper function to add a piece to a given position
    void addPieceToPosition(const std::shared_ptr<Piece>& piece, const std::string& position) {
        if (piece->getColor() == "White") {
            whitePieces.push_back(piece);
        } else {
            blackPieces.push_back(piece);
        }
    }


    // Helper function to update the chessboard based on the pieces' positions
    void updateChessboard(const std::string& fromPosition, const std::string& toPosition) {
        // Calculate the coordinates for the 'from' and 'to' positions
        int fromY = 8 - (fromPosition[1] - '0');
        int fromX = fromPosition[0] - 'a';
        int toY = 8 - (toPosition[1] - '0');
        int toX = toPosition[0] - 'a';

        // Store the piece in 'temp'
        std::string temp = chessboard[fromY][fromX];

        // Set the 'from' position to empty ("-")
        chessboard[fromY][fromX] = "-";

        // Update the 'to' position with the stored piece
        chessboard[toY][toX] = temp;
    }
};

int main() {
    Chessboard chessboard;
    chessboard.initializeBoard();

    // WHITE ALWAYS STARTS FIRST
    bool whiteTurn = true;
    // CHECKMATE VARIABLE TO DETERMINE WHETHER GAME ENDS
    bool checkmate = false;
    // ALTERNATIVELY, DRAW IF MOVE REPETITION OR 50-MOVE RULE
    bool draw = false;
    // TEMPORARY TURN LIMIT
    int i = 0;

    while (!checkmate) {
        chessboard.displayBoard();
        chessboard.createPieceOccupationMaps();
        chessboard.printPiecePositions(); 
        std::string input;

        bool valid = false;
        while (valid == false) {
            chessboard.generateLegalMoves(whiteTurn); // Generate legal moves for the current player
            if (!movesLog.empty()) {
                chessboard.checkCastlingRights(whiteTurn, whiteTurn ? chessboard.getWhiteLegalMovesMap() : chessboard.getBlackLegalMovesMap());
            }
            chessboard.checkPawnPromotions(whiteTurn, whiteTurn ? chessboard.getWhiteLegalMovesMap() : chessboard.getBlackLegalMovesMap());
            chessboard.TryMoves(whiteTurn, whiteTurn ? chessboard.getWhiteLegalMovesMap() : chessboard.getBlackLegalMovesMap());
            chessboard.scanAmbiguity(whiteTurn, whiteTurn ? chessboard.getWhiteLegalMovesMap() : chessboard.getBlackLegalMovesMap());
            chessboard.printLegalMoves(whiteTurn);   // Print legal moves for the current player
            
            // Print the moves map for each player
            if (whiteTurn) {
                std::cout << "White's Legal Moves Map:" << std::endl;
                const LegalMovesMap& whiteMovesMap = chessboard.getWhiteLegalMovesMap();
                for (const auto& entry : whiteMovesMap) {
                    const std::string& position = entry.first;
                    const std::vector<std::string>& legalMoves = entry.second;
                    std::cout << position << ": ";
                    for (const auto& move : legalMoves) {
                        std::cout << move << " ";
                    }
                    std::cout << ", ";
                }
                std::cout << std::endl;
            } else {
                std::cout << "Black's Legal Moves Map:" << std::endl;
                const LegalMovesMap& blackMovesMap = chessboard.getBlackLegalMovesMap();
                for (const auto& entry : blackMovesMap) {
                    const std::string& position = entry.first;
                    const std::vector<std::string>& legalMoves = entry.second;
                    std::cout << position << ": ";
                    for (const auto& move : legalMoves) {
                        std::cout << move << " ";
                    }
                    std::cout << ", ";
                }
                std::cout << std::endl;
            }

            std::cout << std::endl;

            for (size_t moveNumber = 0; moveNumber < movesLog.size(); ++moveNumber) {
                // Print the move number before white's move
                if (moveNumber % 2 == 0) {
                    std::cout << moveNumber / 2 + 1 << ". ";
                }
                
                // Print the player's move
                std::cout << movesLog[moveNumber];

                // Add a space after every move
                std::cout << " ";
            }
            std::cout << std::endl;

            if (i % 2 == 0) {
                std::cout << "White to move. " << ((i-(i%2))/2)+1 << ". ";
            } else {
                std::cout << "Black to move. " << ((i-(i%2))/2)+1 << ". ";
            }

            std::cin >> input;
            std::cout << std::endl;

            if (isInputValid(input, whiteTurn ? chessboard.getWhiteLegalMovesMap() : chessboard.getBlackLegalMovesMap())) {
                movesLog.push_back(input);
                std::string input_copy = input;
                
                if (input_copy.find('R') != std::string::npos) {
                    std::cout << "Move starts with R." << std::endl;
                    chessboard.findKeyOfRook(whiteTurn, input_copy, whiteTurn ? chessboard.getWhiteLegalMovesMap() : chessboard.getBlackLegalMovesMap());
                }

                // Check if the input move ends with '+' or '#'
                if (input_copy.back() == '+' || input_copy.back() == '#') {
                    // Remove the last character if it's '+' or '#'
                    input_copy.pop_back();
                    if (input_copy.back() == '#') {
                        checkmate = true;
                        // Terminate the program
                        std::cout << "Checkmate! Game over." << std::endl;
                        std::abort; // 0 indicates successful program termination
                    }
                }

                std::cout << "reached2" << std::endl;

                if (input_copy == "O-O" || input_copy == "O-O-O") {
                    chessboard.performCastling(whiteTurn, input_copy);
                } else {
                    if (input_copy.length() >= 4 && input_copy[input_copy.length() - 2] == '=' && std::isalpha(input_copy[input_copy.length() - 1])) {
                        // Handle pawn promotion here
                        // Extract the piece to promote to (the last character)
                        char promotionPiece = input_copy.back();

                        // Remove the last two characters to get the destination square
                        std::string new_copy = input_copy;
                        input_copy.erase(input_copy.end() - 2, input_copy.end());

                        std::cout << input_copy << std::endl;

                        std::cout << "Pawn promotion move." << std::endl; 

                        const LegalMovesMap& movesMap = whiteTurn ? chessboard.getWhiteLegalMovesMap() : chessboard.getBlackLegalMovesMap();

                        // EXTRACT MOVE-TO POSITION OF PIECE FROM INPUT
                        std::string pieceMoveToPosition = input_copy.substr(input_copy.length() - 2);
                        // Initialize a variable to store the position of the piece moved
                        std::string pieceMovedPosition = "";

                        // Use std::find to search for the input move in the vector of legal moves
                        for (const auto& entry : movesMap) {
                            if (std::find(entry.second.begin(), entry.second.end(), input) != entry.second.end()) {
                                // This piece can make the move
                                pieceMovedPosition = entry.first;
                                break;
                            }
                        }

                        chessboard.movePieceToPosition(pieceMovedPosition, pieceMoveToPosition);
                        std::cout << "reached1" << std::endl;
                        chessboard.performPawnPromotion(whiteTurn, input_copy, promotionPiece);
                    } else {
                        // It's a regular move
                        // EXTRACT MOVE-TO POSITION OF PIECE FROM INPUT
                        std::string pieceMoveToPosition = input_copy.substr(input_copy.length() - 2);
                        // Initialize a variable to store the position of the piece moved
                        std::string pieceMovedPosition = "";

                        const LegalMovesMap& movesMap = whiteTurn ? chessboard.getWhiteLegalMovesMap() : chessboard.getBlackLegalMovesMap();

                        // Use std::find to search for the input move in the vector of legal moves
                        for (const auto& entry : movesMap) {
                            if (std::find(entry.second.begin(), entry.second.end(), input) != entry.second.end()) {
                                // This piece can make the move
                                pieceMovedPosition = entry.first;
                                break;
                            }
                        }

                        chessboard.movePieceToPosition(pieceMovedPosition, pieceMoveToPosition);

                        std::cout << "Piece moved: " << pieceMovedPosition << std::endl;
                    }
                }

                valid = true;
            } else {
                std::cout << "Invalid move. Try again." << std::endl;
            }
        }

        ++i;

        whiteTurn = !whiteTurn;
    }
    return 0;
}