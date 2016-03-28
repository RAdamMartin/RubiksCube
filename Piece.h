#ifndef RUBIKS_PIECE_H_INCLUDED
#define RUBIKS_PIECE_H_INCLUDED

#include "Colour.h"

class Face{
    public:
        Face();
        
}

class Piece{
    public: 
        Piece(std::vector<Face> faces);
        Piece(std::vector<Colour> cols);
        draw();
}

#endif
