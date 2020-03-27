#include "game_mechanics.h"

extern sharedVars shared;

// checks if any of the player's/bot's pieces must capture
void checkMustCapture(bool turn, bool *capture) {
  using c::num_pieces;
  // starts the index at start
  // depending on which side the current player is
  int8_t start = turn ? num_pieces : 0;
  for (int i = start; i < start + num_pieces; i++) {
    Piece *piece = &shared.gamePieces[i];
    moveSt moves = {NOT, NOT, NOT, NOT};
    // check which pieces can capture
    check::capture(*piece, moves);
    check::backwards(*piece, moves);
    // fill array with bool values
    capture[i % num_pieces] = has::captures(moves);
  }
}

// show/hide which pieces can capture
void showCap(bool *capture, bool turn, bool show) {
  // find starting index for piece array
  // turn = true : player's pieces
  // turn = false : bot's pieces
  int8_t start = turn ? c::num_pieces : 0;
  for (int i = 0; i < c::num_pieces; i++) {
    if (capture[i]) {
      if (show) {
        // show that the piece can capture
        draw::highlight(shared.gamePieces[i + start], true);
      } else {
        // redraw the piece to remove highlight
        draw::piece(shared.gamePieces[i + start]);
      }
    }
  }
}

void newSel(int8_t piecePos, moveSt& moves) {
  // do nothing if same piece was selected
  if (piecePos == shared.selected) {return;}
  // unhighlights old piece and its moves
  draw::unhighlight(*nspiece::find(shared.selected));
  shared.selected = piecePos;
  nsmove::show(piecePos, moves);
}

void attemptMove(selected& pieceSel, int8_t piecePos, moveSt& moves) {
  // do nothing if no piece was selected
  if (pieceSel == PIECE) {
    // check if the moves are legal for this piece
    Piece* piece = nspiece::find(shared.selected);
    move legal = nsmove::legal(*piece, piecePos, moves); 
    if (legal == MOVE) {
      draw::unhighlight(*piece); // remove move marks        
      nsmove::piece(piece->pos, piecePos); // move piece
      shared.selected = -1; // now no piece is selected
      pieceSel = DONE; // done moving
    }
  }
}

// implements must capture rule
// returns true if the player had to capture
bool mustCapture(bool turn) {
  bool capture[c::num_pieces];
  checkMustCapture(turn, capture);
  bool check = false;
  for (int i = 0; i < c::num_pieces; i++) {
    if (capture[i]) {
      check = true;
      break;
    }
  }
  // if there are no captures, move on to just moves
  if (!check) {return false;}
  // else, make the player capture
  selected pieceSel = NO_PIECE;
  moveSt moves;
  showCap(capture, turn);
  while (pieceSel != DONE) {
    chooseMove(pieceSel, turn, moves, capture, CAPTURE);
  }
  return true;
}

// lets player choose a piece to move
void chooseMove(selected& pieceSel, bool turn, 
                moveSt& moves, bool* capture, move type) {
  int8_t piecePos = nspiece::touch();
  // loop again if nothing was touched
  if (piecePos < 0) {return;} 

  // selecting a new piece
  tile currentPlayer = turn ? PLAYER : BOT;

  if (board(piecePos) == currentPlayer) {
    if (nsmove::canMove(piecePos, moves, currentPlayer, type)) {
      // do nothing if same piece was selected
      shared.tft->println("im here");
      if (piecePos == shared.selected) {return;}
      // unhighlights old piece and its moves
      draw::unhighlight(*nspiece::find(shared.selected));
      shared.selected = piecePos;
      nsmove::show(piecePos, moves);
      pieceSel = PIECE; // now a piece is selected
    }
  } else if (board(piecePos) == EMPTY) {
    attemptMove(pieceSel,piecePos, moves);
  }
}