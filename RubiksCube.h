#ifndef RUBIKS_PIECE_H_INCLUDED
#define RUBIKS_PIECE_H_INCLUDED

#include "util.h"

class Piece{
    public: 
        Piece();
        void clamp();
        void draw();
        void rotate(float theta, double x, double y, double z);
        void setColours(Colour * cols);
        void translate(double x, double y, double z);
    protected:
        Colour colours[6];
        float theta;
        double axis[3];
        Matrix4x4 rotation;
};

class Face{
    public:
        Face();
        void addPieces(Face *src, Piece *p0, Piece *p1, Piece *p2);
        void clamp();
        void rotate(float theta);
        void setAxis(double x, double y, double z);
        void setLinks(Face *top, Face *right, Face *bot, Face *left);
        void setPieces(Piece ** pieces);
    protected:
        Face * links[4];
        Piece * pieces[9];
        double axis[3];
        float theta;
};

class RubiksCube
{
public:
    enum side{ _white=0,
               _yellow=1,
               _green=2,
               _blue=3,
               _red=4,
               _orange=5};
    enum axis{ _x=0,
               _y=1,
               _z=0};
           
    RubiksCube();
    void clamp();
    void draw();
    void rotateFace(side face, float theta);
protected:
    Face faces[6];
    Piece pieces[27];
    Colour colours[6];
};

#endif