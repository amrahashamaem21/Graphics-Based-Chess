This project is a graphical Chess game developed in C++ using the SFML (Simple and Fast Multimedia Library) framework.
It provides a full two-player chess experience with drag-and-drop interaction, visual board rendering, and rule-based move validation.
The game follows standard chess rules including legal movement, capturing, check detection, checkmate, and stalemate conditions.

Board Representation
The game uses an 8×8 board represented internally as a 2D character array.
Each piece is represented using characters: uppercase letters represent White pieces and lowercase letters represent Black pieces.
The mapping is as follows:
P/p for Pawn
R/r for Rook
N/n for Knight
B/b for Bishop
Q/q for Queen
K/k for King

Graphical Interface
The graphical interface is built using SFML.
Each square is 100×100 pixels.
Chess pieces are rendered using textures (image files).
Pieces can be selected and moved using mouse drag-and-drop functionality.
A side panel displays game status and captured pieces.

Game Flow
The game starts with the standard chess initial setup.
White always moves first.
Players take turns moving pieces.
A player can only move their own pieces.
Moves are performed by clicking on a piece and dragging it to a destination square.

Piece Movement Rules
Movement rules are implemented separately for each piece:

Pawns move forward one square or two squares from their starting position.
Pawns capture diagonally.
Knights move in L-shape (2+1 pattern).
Bishops move diagonally.
Rooks move horizontally and vertically.
Queens combine rook and bishop movement.
Kings move one square in any direction.

Path Checking
Path checking is implemented for pieces like rooks, bishops, and queens.
These pieces cannot jump over other pieces.
Knights are the only pieces that can jump over others.

Move Validation System
The game prevents illegal moves using a two-step validation system:
First, it checks if the move follows the basic movement rules of the piece.
Second, it ensures the move does not leave the player's own king in check.

King Safety Logic
King safety is handled by simulating moves:
The game temporarily performs the move.
It checks if the king becomes attacked.
If the king is in danger, the move is rejected.

Check Detection
Check detection is implemented by scanning all opponent pieces.
If any opponent piece can attack the king’s position, the king is in check.
When a king is in check, the board highlights the king’s square in red.

Checkmate Detection
Checkmate detection works as follows:
The game checks if the current player is in check.
It then tries all possible moves for that player.
If no legal move exists to escape check, it is declared checkmate.
The opponent is declared the winner.

Stalemate Detection
Stalemate detection is also implemented:
If the player is not in check but has no legal moves, the game ends in a draw.

Pawn Promotion
Pawn promotion is supported:
When a pawn reaches the last rank, it is automatically promoted to a Queen.
The sprite is updated visually to reflect the promotion.

Unsupported Features
En Passant is NOT implemented in this version.
Castling is also NOT implemented.

Capturing System
Capturing pieces:
When a piece is captured, it is removed from the board.
The corresponding sprite is moved off-screen.
Captured piece counts are tracked separately for White and Black.

Debugging Output
The game includes debugging output in the console:
It shows selected pieces and their positions.
It logs successful moves.
It reports illegal moves and explains whether the issue is movement or king safety.
It can display all legal moves for debugging checkmate logic.

Board Coordinates
The interface includes coordinate labels:
Columns are labeled a to h (lowercase).
Rows are labeled 1 to 8.

Visual Highlights
The currently selected square is highlighted in green.
The king’s square is highlighted in red when in check.

Side Panel Information
The right-side panel displays:
Current player turn
Check status
Game result (checkmate or draw)
Number of captured pieces

Game Ending Conditions
The game ends when:
A checkmate occurs (winner is declared).
A stalemate occurs (draw).

Compilation Instructions
To compile this project:
You need a C++ compiler (such as g++ or Visual Studio).
You must have SFML installed and properly linked.

Required Resources
Required resources:
SFML library
Font file (arial.ttf or equivalent)
Piece images (pawn, rook, knight, bishop, queen, king for both colors)

