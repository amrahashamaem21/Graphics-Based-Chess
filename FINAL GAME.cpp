#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include <cctype>
#include <cstdlib>

using namespace std;
using namespace sf;

const int size = 100;
int originalRow = -1;
int originalCol = -1;
bool isMoving = false;
char selectedPieceChar = ' ';
sf::Vector2f offset;
int selectedX = -1;
int selectedY = -1;
int lastMoveFromRow = -1, lastMoveFromCol = -1;
int lastMoveToRow = -1, lastMoveToCol = -1;
bool lastMoveWasPawnTwoStep = false;
bool inCheck = false;
int moveCounter = 0;
int enPassantValidUntilMove = -1;
char currentPlayer = 'W';
char winner = ' ';
int selectedSpriteID = -1;
char board[8][8] = {
    {'r','n','b','q','k','b','n','r'},
    {'p','p','p','p','p','p','p','p'},
    {' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' '},
    {'P','P','P','P','P','P','P','P'},
    {'R','N','B','Q','K','B','N','R'}
};
int whiteCapturedPieces = 0;
int blackCapturedPieces = 0;
Sprite wpawns[8];
Sprite bpawns[8];
Sprite wRook1, wRook2, wKnight1, wKnight2, wBishop1, wBishop2, wQueen, wKing;
Sprite bRook1, bRook2, bKnight1, bKnight2, bBishop1, bBishop2, bQueen, bKing;
bool isWhite(char p) {
    if (p == ' ') return false;
    return isupper(p);
}
bool isCurrentPlayerPiece(char p) {
    if (p == ' ') return false;
    bool isPieceWhite = isWhite(p);
    return (currentPlayer == 'W' && isPieceWhite) || (currentPlayer == 'B' && !isPieceWhite);
}
bool isFriendlyTarget(char movingPiece, char targetPiece) {
    if (movingPiece == ' ' || targetPiece == ' ') return false;
    return isWhite(movingPiece) == isWhite(targetPiece);
}
Sprite& getSpriteByID(int id) {
    if (id >= 0 && id <= 7) return wpawns[id];
    if (id >= 8 && id <= 15) return bpawns[id - 8];
    switch (id) {
    case 16: return wRook1; case 17: return wRook2;
    case 18: return wKnight1; case 19: return wKnight2;
    case 20: return wBishop1; case 21: return wBishop2;
    case 22: return wQueen; case 23: return wKing;
    case 24: return bRook1; case 25: return bRook2;
    case 26: return bKnight1; case 27: return bKnight2;
    case 28: return bBishop1; case 29: return bBishop2;
    case 30: return bQueen; case 31: return bKing;
    }
    return wRook1;
}
void removeSprite(int r, int c) {
    char piece = board[r][c];
    if (piece == ' ') return;

    float currentX = (float)c * 100.f;
    float currentY = (float)r * 100.f;
    float offScreenX = 800.f;
    float offScreenY = 800.f;

    auto applyActionToSprite = [&](Sprite& sprite) {
        const float tolerance = 1.0f;
        if (std::abs(sprite.getPosition().x - currentX) < tolerance &&
            std::abs(sprite.getPosition().y - currentY) < tolerance) {
            sprite.setPosition(offScreenX, offScreenY);
            return true;
        }
        return false;
        };

    if (tolower(piece) == 'p') {
        Sprite* pawnArray = isWhite(piece) ? wpawns : bpawns;
        for (int i = 0; i < 8; ++i) {
            if (applyActionToSprite(pawnArray[i])) return;
        }
        return;
    }
    char type = tolower(piece);
    if (type == 'r') {
        if (applyActionToSprite(wRook1)) return;
        if (applyActionToSprite(wRook2)) return;
        if (applyActionToSprite(bRook1)) return;
        if (applyActionToSprite(bRook2)) return;
    }
    else if (type == 'n') {
        if (applyActionToSprite(wKnight1)) return;
        if (applyActionToSprite(wKnight2)) return;
        if (applyActionToSprite(bKnight1)) return;
        if (applyActionToSprite(bKnight2)) return;
    }
    else if (type == 'b') {
        if (applyActionToSprite(wBishop1)) return;
        if (applyActionToSprite(wBishop2)) return;
        if (applyActionToSprite(bBishop1)) return;
        if (applyActionToSprite(bBishop2)) return;
    }
    else if (type == 'q') {
        if (applyActionToSprite(wQueen)) return;
        if (applyActionToSprite(bQueen)) return;
    }
    else if (type == 'k') {
        if (applyActionToSprite(wKing)) return;
        if (applyActionToSprite(bKing)) return;
    }
}

int getSpriteID(int r, int c, char piece) {
    float targetX = (float)c * 100.f;
    float targetY = (float)r * 100.f;
    const float tolerance = 1.0f;

    auto isAt = [&](const Sprite& sprite) {
        return std::abs(sprite.getPosition().x - targetX) < tolerance &&
            std::abs(sprite.getPosition().y - targetY) < tolerance;
        };

    if (tolower(piece) == 'p') {
        Sprite* pawnArray = isWhite(piece) ? wpawns : bpawns;
        int baseID = isWhite(piece) ? 0 : 8;
        for (int i = 0; i < 8; ++i) {
            if (isAt(pawnArray[i])) return baseID + i;
        }
        return -1;
    }

    if (isAt(wRook1)) return 16; if (isAt(wRook2)) return 17;
    if (isAt(wKnight1)) return 18; if (isAt(wKnight2)) return 19;
    if (isAt(wBishop1)) return 20; if (isAt(wBishop2)) return 21;
    if (isAt(wQueen)) return 22; if (isAt(wKing)) return 23;
    if (isAt(bRook1)) return 24; if (isAt(bRook2)) return 25;
    if (isAt(bKnight1)) return 26; if (isAt(bKnight2)) return 27;
    if (isAt(bBishop1)) return 28; if (isAt(bBishop2)) return 29;
    if (isAt(bQueen)) return 30; if (isAt(bKing)) return 31;

    return -1;
}
bool isPathClear(int r1, int c1, int r2, int c2) {
    int dr = r2 - r1;
    int dc = c2 - c1;

    if (dr == 0 || dc == 0) {
        int rDir = (dr == 0) ? 0 : (dr > 0 ? 1 : -1);
        int cDir = (dc == 0) ? 0 : (dc > 0 ? 1 : -1);
        int r = r1 + rDir;
        int c = c1 + cDir;
        while (r != r2 || c != c2) {
            if (board[r][c] != ' ') { return false; }
            r += rDir;
            c += cDir;
        }
    }
    else if (std::abs(dr) == std::abs(dc)) {
        int rDir = (dr > 0 ? 1 : -1);
        int cDir = (dc > 0 ? 1 : -1);
        int r = r1 + rDir;
        int c = c1 + cDir;
        while (r != r2) {
            if (board[r][c] != ' ') { return false; }
            r += rDir;
            c += cDir;
        }
    }
    return true;
}
bool checkPawnMove(int r1, int c1, int r2, int c2, char p) {
    int dir = isWhite(p) ? -1 : 1;
    int rd = r2 - r1, cd = c2 - c1;
    if (cd == 0 && rd == dir) return board[r2][c2] == ' ';
    if (cd == 0 && rd == 2 * dir) {
        int sr = isWhite(p) ? 6 : 1;
        if (r1 == sr && board[r1 + dir][c1] == ' ' && board[r2][c2] == ' ') return true;
    }
    if (std::abs(cd) == 1 && rd == dir) {
        if (board[r2][c2] != ' ' && isWhite(board[r2][c2]) != isWhite(p)) return true;
    }

    return false;
}

bool checkKnightMove(int r1, int c1, int r2, int c2) {
    int dr = std::abs(r2 - r1);
    int dc = std::abs(c2 - c1);
    return (dr == 2 && dc == 1) || (dr == 1 && dc == 2);
}

bool checkRookMove(int r1, int c1, int r2, int c2) {
    if (r1 == r2 || c1 == c2) { return isPathClear(r1, c1, r2, c2); }
    return false;
}

bool checkBishopMove(int r1, int c1, int r2, int c2) {
    if (std::abs(r2 - r1) == std::abs(c2 - c1)) { return isPathClear(r1, c1, r2, c2); }
    return false;
}

bool checkQueenMove(int r1, int c1, int r2, int c2) {
    return checkRookMove(r1, c1, r2, c2) || checkBishopMove(r1, c1, r2, c2);
}

bool checkKingMove(int r1, int c1, int r2, int c2) {
    int dr = std::abs(r2 - r1);
    int dc = std::abs(c2 - c1);
    return dr <= 1 && dc <= 1 && (dr != 0 || dc != 0);
}

bool isMoveValidIgnoringKingSafety(int r1, int c1, int r2, int c2, char piece) {
    if (r2 < 0 || r2 >= 8 || c2 < 0 || c2 >= 8) return false;
    if (r1 == r2 && c1 == c2) return false;
    if (board[r1][c1] != piece) return false;

    char target = board[r2][c2];
    if (target != ' ' && isFriendlyTarget(piece, target)) return false;

    char type = tolower(piece);
    if (type == 'p') return checkPawnMove(r1, c1, r2, c2, piece);
    if (type == 'n') return checkKnightMove(r1, c1, r2, c2);
    if (type == 'r') return checkRookMove(r1, c1, r2, c2);
    if (type == 'b') return checkBishopMove(r1, c1, r2, c2);
    if (type == 'q') return checkQueenMove(r1, c1, r2, c2);
    if (type == 'k') return checkKingMove(r1, c1, r2, c2);

    return false;
}
bool isCheck(char player) {
    int kr = -1, kc = -1;
    for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++)
        if ((player == 'W' && board[r][c] == 'K') || (player == 'B' && board[r][c] == 'k')) { kr = r; kc = c; }

    if (kr == -1) return true; 

    for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++) {
        char piece = board[r][c];
        if (piece != ' ' && isWhite(piece) != (player == 'W')) { 
            if (tolower(piece) == 'p') {
                int dir = isWhite(piece) ? -1 : 1;
                if (r + dir == kr && std::abs(c - kc) == 1) return true;
            }
            else if (isMoveValidIgnoringKingSafety(r, c, kr, kc, piece)) {
                return true;
            }
        }
    }
    return false;
}
bool isMoveLegalForKingSafety(int r1, int c1, int r2, int c2, char player) {
    char pieceToMove = board[r1][c1];
    char capturedPiece = board[r2][c2];
    board[r2][c2] = pieceToMove;
    board[r1][c1] = ' ';
    bool leavesInCheck = isCheck(player);
    board[r1][c1] = pieceToMove;
    board[r2][c2] = capturedPiece;
    return !leavesInCheck;
}
bool isValidMove(int r1, int c1, int r2, int c2) {
    char piece = board[r1][c1];
    if (piece == ' ') return false;
    if (!isMoveValidIgnoringKingSafety(r1, c1, r2, c2, piece)) return false;
    return isMoveLegalForKingSafety(r1, c1, r2, c2, isWhite(piece) ? 'W' : 'B');
}
bool hasLegalMoves(char player) {

    bool debugOutput = true;

    if (winner != ' ') debugOutput = false; 

    if (debugOutput) {
        cout << "\n--- DEBUGGING LEGAL MOVES FOR " << player << " ---" << endl;
    }

    for (int r1 = 0; r1 < 8; r1++) for (int c1 = 0; c1 < 8; c1++) {
        char piece = board[r1][c1];
        if ((player == 'W' && isWhite(piece)) || (player == 'B' && !isWhite(piece))) {
            for (int r2 = 0; r2 < 8; r2++) for (int c2 = 0; c2 < 8; c2++) {
                if (isValidMove(r1, c1, r2, c2)) {
                    if (debugOutput) {
                        cout << "LEGAL MOVE FOUND: " << piece << " at "
                            << char('a' + c1) << (8 - r1) << " to " 
                            << char('a' + c2) << (8 - r2) << endl;  
                    }
                    return true;
                }
            }
        }
    }
    if (debugOutput) {
        cout << "--- NO LEGAL MOVES FOUND for " << player << " ---" << endl;
    }
    return false;
}

void checkEndGame() {
    char nextPlayer = (currentPlayer == 'W') ? 'B' : 'W';
    bool nextPlayerInCheck = isCheck(nextPlayer);
    bool nextPlayerHasLegalMoves = hasLegalMoves(nextPlayer);

    if (nextPlayerInCheck && !nextPlayerHasLegalMoves) {
        winner = currentPlayer;
    }
    else if (!nextPlayerInCheck && !nextPlayerHasLegalMoves) {
        winner = 'D';
    }
}

int main() {
    int sidePanelWidth = 350;
    RenderWindow window(VideoMode(8 * 100 + sidePanelWidth, 8 * 100), "SFML Chess");

    sf::Font font;
    if (!font.loadFromFile("C:/Users/Amraha/source/repos/ChessGame/ChessGame/x64/Debug/Fonts/arial.ttf")) {
        if (!font.loadFromFile("C:/Users/Amraha/source/repos/ChessGame/ChessGame/x64/Debug/Fonts/arial.ttf")) {
            std::cerr << "Error loading font! Ensure 'arial.ttf' or the hardcoded path is correct.\n";
            return 1;
        }
    }

    sf::Text statusText;
    statusText.setFont(font);
    statusText.setCharacterSize(24);
    statusText.setPosition(810, 50);

    sf::Text capturedText;
    capturedText.setFont(font);
    capturedText.setCharacterSize(18);
    capturedText.setPosition(810, 150);

    const float scale = 0.8f;
    const float pawnScale = 0.25f;

    Texture wpawnTexture, bpawnTexture;
    Texture wRook_tex, bRook_tex, wKnight_tex, bKnight_tex, wBishop_tex, bBishop_tex;
    Texture wQueen_tex, bQueen_tex, wKing_tex, bKing_tex;

    if (!wpawnTexture.loadFromFile("C:/Users/Amraha/source/repos/ChessGame/Images/pawn.w.png")) return 1;
    if (!bpawnTexture.loadFromFile("C:/Users/Amraha/source/repos/ChessGame/Images/pawn.b.png")) return 1;
    if (!wRook_tex.loadFromFile("C:/Users/Amraha/source/repos/ChessGame/Images/rook.w.png")) return 1;
    if (!bRook_tex.loadFromFile("C:/Users/Amraha/source/repos/ChessGame/Images/rook.b.png")) return 1;
    if (!wKnight_tex.loadFromFile("C:/Users/Amraha/source/repos/ChessGame/Images/knight.w.png")) return 1;
    if (!bKnight_tex.loadFromFile("C:/Users/Amraha/source/repos/ChessGame/Images/knight.b.png")) return 1;
    if (!wBishop_tex.loadFromFile("C:/Users/Amraha/source/repos/ChessGame/Images/bishop.w.png")) return 1;
    if (!bBishop_tex.loadFromFile("C:/Users/Amraha/source/repos/ChessGame/Images/bishop.b.png")) return 1;
    if (!wQueen_tex.loadFromFile("C:/Users/Amraha/source/repos/ChessGame/Images/queen.w.png")) { return 1; }
    if (!bQueen_tex.loadFromFile("C:/Users/Amraha/source/repos/ChessGame/Images/queen.b.png")) { return 1; }
    if (!wKing_tex.loadFromFile("C:/Users/Amraha/source/repos/ChessGame/Images/king.w.png")) return 1;
    if (!bKing_tex.loadFromFile("C:/Users/Amraha/source/repos/ChessGame/Images/king.b.png")) return 1;

    for (int i = 0; i < 8; i++) {
        wpawns[i].setTexture(wpawnTexture);
        wpawns[i].setPosition((float)i * 100.f, 6.f * 100.f);
        wpawns[i].setScale(pawnScale, pawnScale);
    }
    for (int i = 0; i < 8; i++) {
        bpawns[i].setTexture(bpawnTexture);
        bpawns[i].setPosition((float)i * 100.f, 1.f * 100.f);
        bpawns[i].setScale(pawnScale, pawnScale);
    }

    wRook1.setTexture(wRook_tex); wRook1.setPosition(0.f * 100.f, 7.f * 100.f);
    wRook2.setTexture(wRook_tex); wRook2.setPosition(7.f * 100.f, 7.f * 100.f);
    wKnight1.setTexture(wKnight_tex); wKnight1.setPosition(1.f * 100.f, 7.f * 100.f);
    wKnight2.setTexture(wKnight_tex); wKnight2.setPosition(6.f * 100.f, 7.f * 100.f);
    wBishop1.setTexture(wBishop_tex); wBishop1.setPosition(2.f * 100.f, 7.f * 100.f);
    wBishop2.setTexture(wBishop_tex); wBishop2.setPosition(5.f * 100.f, 7.f * 100.f);
    wQueen.setTexture(wQueen_tex); wQueen.setPosition(3.f * 100.f, 7.f * 100.f);
    wKing.setTexture(wKing_tex); wKing.setPosition(4.f * 100.f, 7.f * 100.f);

    bRook1.setTexture(bRook_tex); bRook1.setPosition(0.f * 100.f, 0.f);
    bRook2.setTexture(bRook_tex); bRook2.setPosition(7.f * 100.f, 0.f);
    bKnight1.setTexture(bKnight_tex); bKnight1.setPosition(1.f * 100.f, 0.f);
    bKnight2.setTexture(bKnight_tex); bKnight2.setPosition(6.f * 100.f, 0.f);
    bBishop1.setTexture(bBishop_tex); bBishop1.setPosition(2.f * 100.f, 0.f);
    bBishop2.setTexture(bBishop_tex); bBishop2.setPosition(5.f * 100.f, 0.f);
    bQueen.setTexture(bQueen_tex); bQueen.setPosition(3.f * 100.f, 0.f);
    bKing.setTexture(bKing_tex); bKing.setPosition(4.f * 100.f, 0.f);

    wRook1.setScale(scale, scale); wRook2.setScale(scale, scale); wKnight1.setScale(scale, scale); wKnight2.setScale(scale, scale);
    wBishop1.setScale(scale, scale); wBishop2.setScale(scale, scale); wQueen.setScale(scale, scale); wKing.setScale(scale, scale);
    bRook1.setScale(scale, scale); bRook2.setScale(scale, scale); bKnight1.setScale(scale, scale); bKnight2.setScale(scale, scale);
    bBishop1.setScale(scale, scale); bBishop2.setScale(scale, scale); bQueen.setScale(scale, scale); bKing.setScale(scale, scale);

    inCheck = isCheck(currentPlayer);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {

            if (event.type == Event::Closed)
                window.close();

            if (winner != ' ' && event.type == sf::Event::MouseButtonPressed)
                continue;

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

                int clickCol = (int)(worldPos.x / 100.f);
                int clickRow = (int)(worldPos.y / 100.f);

                selectedSpriteID = -1;

                if (clickCol < 0 || clickCol >= 8 || clickRow < 0 || clickRow >= 8)
                    continue;

                char clickedPiece = board[clickRow][clickCol];

                if (clickedPiece != ' ' && isCurrentPlayerPiece(clickedPiece)) {

                    int id = getSpriteID(clickRow, clickCol, clickedPiece);

                    if (id != -1) {
                        selectedSpriteID = id;
                        originalCol = clickCol;
                        originalRow = clickRow;
                        selectedPieceChar = clickedPiece;
                        isMoving = true;

                        Sprite& selectedSprite = getSpriteByID(selectedSpriteID);
                        offset = worldPos - selectedSprite.getPosition();

                        selectedX = clickCol;
                        selectedY = clickRow;

                        string pieceName;
                        char pieceType = tolower(selectedPieceChar);
                        if (pieceType == 'p') pieceName = "Pawn";
                        else if (pieceType == 'r') pieceName = "Rook";
                        else if (pieceType == 'n') pieceName = "Knight";
                        else if (pieceType == 'b') pieceName = "Bishop";
                        else if (pieceType == 'q') pieceName = "Queen";
                        else if (pieceType == 'k') pieceName = "King";

                        string colorName = isWhite(selectedPieceChar) ? "White" : "Black";

                        cout << currentPlayer << " selecting: " << colorName << " " << pieceName << " at "
                            << char('a' + originalCol) << (8 - originalRow) << endl; // Changed 'A' to 'a'
                    }
                    else {
                        isMoving = false;
                        selectedX = -1;
                        selectedY = -1;
                    }
                }
                else {
                    selectedX = -1;
                    selectedY = -1;
                    isMoving = false;
                }
            }

            if (event.type == sf::Event::MouseMoved && isMoving && selectedSpriteID != -1) {
                sf::Vector2f worldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                sf::Vector2f newPos = worldPos - offset;

                Sprite& selectedSprite = getSpriteByID(selectedSpriteID);
                selectedSprite.setPosition(newPos);
            }

            if (event.type == sf::Event::MouseButtonReleased &&
                event.mouseButton.button == sf::Mouse::Left &&
                isMoving && selectedSpriteID != -1) {

                isMoving = false;

                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

                int newCol = (int)(worldPos.x / 100.f);
                int newRow = (int)(worldPos.y / 100.f);

                float snapX = (float)originalCol * 100.f;
                float snapY = (float)originalRow * 100.f;

                bool moveSuccessful = false;
                char pieceToMove = selectedPieceChar;
                char targetPiece = board[newRow][newCol];

                if (newCol >= 0 && newCol < 8 && newRow >= 0 && newRow < 8) {

                    char piece = board[originalRow][originalCol];
                    bool basicMoveValid = isMoveValidIgnoringKingSafety(originalRow, originalCol, newRow, newCol, piece);

                    if (isValidMove(originalRow, originalCol, newRow, newCol)) {

                        if (targetPiece != ' ') {
                            removeSprite(newRow, newCol);
                            if (isWhite(targetPiece)) {
                                blackCapturedPieces++;
                            }
                            else {
                                whiteCapturedPieces++;
                            }
                        }

                        Sprite& selectedSprite = getSpriteByID(selectedSpriteID);

                        if (tolower(selectedPieceChar) == 'p' && (newRow == 0 || newRow == 7)) {
                            pieceToMove = isWhite(selectedPieceChar) ? 'Q' : 'q';
                            selectedSprite.setTexture(isWhite(selectedPieceChar) ? wQueen_tex : bQueen_tex, true);
                            selectedSprite.setScale(scale, scale);
                            cout << "--- PROMOTION! Pawn to Queen. ---" << endl;
                        }

                        board[newRow][newCol] = pieceToMove;
                        board[originalRow][originalCol] = ' ';

                        snapX = (float)newCol * 100.f;
                        snapY = (float)newRow * 100.f;
                        moveSuccessful = true;

                        string pieceName;
                        char pieceType = tolower(selectedPieceChar);
                        if (pieceType == 'p') pieceName = "Pawn";
                        else if (pieceType == 'r') pieceName = "Rook";
                        else if (pieceType == 'n') pieceName = "Knight";
                        else if (pieceType == 'b') pieceName = "Bishop";
                        else if (pieceType == 'q') pieceName = "Queen";
                        else if (pieceType == 'k') pieceName = "King";

                        string colorName = isWhite(selectedPieceChar) ? "White" : "Black";

                        cout << currentPlayer << " moved " << colorName << " " << pieceName << " to "
                            << char('a' + newCol) << (8 - newRow) << " (Success)." << endl; // Changed 'A' to 'a'
                    }
                    else {
                        if (basicMoveValid) {
                            string pieceName;
                            char pieceType = tolower(selectedPieceChar);
                            if (pieceType == 'p') pieceName = "Pawn";
                            else if (pieceType == 'r') pieceName = "Rook";
                            else if (pieceType == 'n') pieceName = "Knight";
                            else if (pieceType == 'b') pieceName = "Bishop";
                            else if (pieceType == 'q') pieceName = "Queen";
                            else if (pieceType == 'k') pieceName = "King";

                            string colorName = isWhite(selectedPieceChar) ? "White" : "Black";

                            cout << "*** ILLEGAL MOVE (CONSTRAINT) *** Move must resolve check or prevent self-check. "
                                << colorName << " " << pieceName << " from " << char('a' + originalCol) << (8 - originalRow) // Changed 'A' to 'a'
                                << " to " << char('a' + newCol) << (8 - newRow) << " is ILLEGAL." << endl; // Changed 'A' to 'a'
                        }
                        else {
                            string pieceName;
                            char pieceType = tolower(selectedPieceChar);
                            if (pieceType == 'p') pieceName = "Pawn";
                            else if (pieceType == 'r') pieceName = "Rook";
                            else if (pieceType == 'n') pieceName = "Knight";
                            else if (pieceType == 'b') pieceName = "Bishop";
                            else if (pieceType == 'q') pieceName = "Queen";
                            else if (pieceType == 'k') pieceName = "King";

                            string colorName = isWhite(selectedPieceChar) ? "White" : "Black";

                            cout << "*** ILLEGAL MOVE (MOVEMENT RULE) *** " << colorName << " " << pieceName << " cannot move that way. Piece snapped back." << endl;
                        }
                    }
                }
                else {
                    cout << "Dropped off-board. Piece snapped back." << endl;
                }

                Sprite& selectedSprite = getSpriteByID(selectedSpriteID);
                selectedSprite.setPosition(snapX, snapY);

                if (moveSuccessful) {
                    lastMoveWasPawnTwoStep = false;
                    lastMoveFromRow = originalRow;
                    lastMoveFromCol = originalCol;
                    lastMoveToRow = newRow;
                    lastMoveToCol = newCol;

                    moveCounter++;
                    enPassantValidUntilMove = -1;
                    checkEndGame();
                    if (winner != ' ') {
                        cout << ">>> GAME OVER: ";
                        if (winner == 'D') cout << "STALEMATE! DRAW." << endl;
                        else cout << "CHECKMATE! " << winner << " WINS!" << endl;
                    }

                    if (winner == ' ') {
                        currentPlayer = (currentPlayer == 'W') ? 'B' : 'W';
                        inCheck = isCheck(currentPlayer);
                        if (inCheck) {
                            cout << ">>> " << currentPlayer << " is IN CHECK!" << endl;
                        }
                    }

                    selectedX = newCol;
                    selectedY = newRow;
                }
                else {
                    selectedX = -1;
                    selectedY = -1;
                }

                originalRow = -1;
                originalCol = -1;
                selectedPieceChar = ' ';
                selectedSpriteID = -1;
            }
        }

        window.clear();

        sf::Color lightColor(210, 230, 250);
        sf::Color darkColor(90, 130, 180);
        sf::Color checkColor(204, 0, 0);

        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                sf::RectangleShape square(sf::Vector2f(100.f, 100.f));
                square.setPosition((float)col * 100.f, (float)row * 100.f);

                if ((col + row) % 2 == 0)
                    square.setFillColor(lightColor);
                else
                    square.setFillColor(darkColor);

                if (inCheck) {
                    char piece = board[row][col];
                    if ((currentPlayer == 'W' && piece == 'K') || (currentPlayer == 'B' && piece == 'k')) {
                        square.setFillColor(checkColor);
                    }
                }

                window.draw(square);
            }
        }
        sf::Text fileLabel;
        fileLabel.setFont(font);
        fileLabel.setCharacterSize(16);
        fileLabel.setFillColor(sf::Color::Black);
        for (int col = 0; col < 8; col++) {
            
            fileLabel.setString(std::string(1, char('a' + col)));
            
            fileLabel.setPosition((float)col * 100.f + 85.f, 780.f);
            window.draw(fileLabel);
        }

        sf::Text rankLabel;
        rankLabel.setFont(font);
        rankLabel.setCharacterSize(16);
        rankLabel.setFillColor(sf::Color::Black);
        for (int row = 0; row < 8; row++) {
            rankLabel.setString(std::to_string(8 - row));
            
            rankLabel.setPosition(5.f, (float)row * 100.f + 5.f);
            window.draw(rankLabel);
        }

        if (selectedX != -1 && selectedY != -1) {
            RectangleShape highlight(Vector2f(100.f, 100.f));
            highlight.setPosition((float)selectedX * 100.f, (float)selectedY * 100.f);
            highlight.setFillColor(Color::Transparent);
            highlight.setOutlineThickness(5.f);
            highlight.setOutlineColor(Color::Green);
            window.draw(highlight);
        }

        for (int i = 0; i < 8; i++) {
            window.draw(wpawns[i]);
            window.draw(bpawns[i]);
        }

        window.draw(wRook1); window.draw(wRook2); window.draw(wKnight1); window.draw(wKnight2);
        window.draw(wBishop1); window.draw(wBishop2); window.draw(wQueen); window.draw(wKing);

        window.draw(bRook1); window.draw(bRook2); window.draw(bKnight1); window.draw(bKnight2);
        window.draw(bBishop1); window.draw(bBishop2); window.draw(bQueen); window.draw(bKing);

        if (winner != ' ') {
            if (winner == 'D') {
                statusText.setString("GAME OVER!\nDRAW by Stalemate!");
                statusText.setFillColor(sf::Color::Yellow);
            }
            else {
                statusText.setString("GAME OVER!\nCHECKMATE! " + std::string(1, winner) + " wins!");
                statusText.setFillColor(sf::Color::Red);
            }
        }
        else {
            std::string statusMsg = std::string(1, currentPlayer) + "'s Turn";
            if (isCheck(currentPlayer)) {
                statusMsg += "\n(CHECK!)";
                statusText.setFillColor(sf::Color::Red);
            }
            else {
                
                statusText.setFillColor((currentPlayer == 'W') ? sf::Color(255, 255, 255) : sf::Color::White); // Changed (50, 50, 50) to White
            }
            statusText.setString(statusMsg);
            statusText.setOutlineColor(sf::Color::Black);
            statusText.setOutlineThickness(1.f);
        }

        window.draw(statusText);

        capturedText.setString(
            "Captured Pieces:\n"
            "White: " + std::to_string(whiteCapturedPieces) +
            "\nBlack: " + std::to_string(blackCapturedPieces)
        );

        capturedText.setFillColor(sf::Color::White);
        window.draw(capturedText);

        window.display();
    }

    return 0;
}