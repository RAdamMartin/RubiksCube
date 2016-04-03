#include "RubiksCube.h"
#include <iostream>

Face::Face(){
    dir = 0;
};

void Face::addPieces(Face * src, Piece *p0, Piece *p1, Piece *p2){
    if (src == links[0]){
        pieces[0] = p0;
        pieces[1] = p1;
        pieces[2] = p2;
    } else if (src == links[1]){
        pieces[2] = p0;
        pieces[5] = p1;
        pieces[8] = p2;
    } else if (src == links[2]){
        pieces[6] = p0;
        pieces[7] = p1;
        pieces[8] = p2;
    } else if (src == links[3]){
        pieces[0] = p0;
        pieces[3] = p1;
        pieces[6] = p2;
    } else {
        std::cerr << "Pieces passed from unlinked face\n";
    }
};

void Face::clamp(){
    if (dir != 0){
        if(dir == 1){
            links[0]->addPieces(this, pieces[0], pieces[3], pieces[6]);
            links[1]->addPieces(this, pieces[2], pieces[1], pieces[0]);
            links[2]->addPieces(this, pieces[8], pieces[5], pieces[2]);
            links[3]->addPieces(this, pieces[6], pieces[7], pieces[8]);
        } else { //dir == -1
            links[0]->addPieces(this, pieces[2], pieces[5], pieces[8]);
            links[1]->addPieces(this, pieces[8], pieces[7], pieces[6]);
            links[2]->addPieces(this, pieces[6], pieces[3], pieces[0]);
            links[3]->addPieces(this, pieces[0], pieces[1], pieces[2]);
        }
        
        for(int i = 0; i < 9; i++){
            pieces[i]->clamp();
        }
    }
}

void Face::rotate(double theta){
    dir = 0;
    if (theta >= 45){
        dir = 1;
    } else if (theta <= -45){
        dir = -1;
    }
    for(int i = 0; i < 9; i++){
        pieces[i]->rotate(theta, axis[0], axis[1], axis[2]);
    }
}

void Face::setAxis(double x, double y, double z){
    axis[0] = x;
    axis[1] = y;
    axis[2] = z;
}

void Face::setLinks(Face *top, Face *right, Face *bot, Face *left){
    links[0] = top;
    links[1] = right;
    links[2] = bot;
    links[3] = left;
};

void Face::setPieces(Piece **src){
    for(int i=0; i < 9; i ++){
        pieces[i] = src[i]; 
    }
};


RubiksCube::RubiksCube(){
    colours[_white]  = Colour(255,255,255);
    colours[_yellow] = Colour(100, 84,  0);
    colours[_green]  = Colour(  0, 62, 38);
    colours[_blue]   = Colour(  0, 32, 73);
    colours[_red]    = Colour( 77, 12, 23);
    colours[_orange] = Colour(100, 35,  0);
    Colour black     = Colour(  0,  0,  0);
    
    /*Initialize translations*/
    for (int i = 0; i < 9; i++){
        int x = 0;
        if (i == 0 || i == 3 || i == 6){
            x = -1;
        } else if (i == 2 || i == 5 || i == 8){
            x = 1;
        }
        int y = 0;
        if (i > 5){
            y = -1;
        } else if (i < 3){
            y = 1;
        }
        
        pieces[i].translate(x, y, 1); //White Face
        pieces[i+9].translate(x, y, 0); //Middle Slice
        pieces[i+18].translate(x, y, -1); //Yellow Face
    }
    
    Colour temp_colours[6] = {black, black, black, black, black, black};
    
    /*Initialize White face piece colours*/
    temp_colours[0] = colours[_white];

    temp_colours[1] = colours[_orange];    /*Top slice*/
    pieces[1].setColours(temp_colours);
    temp_colours[2] = colours[_blue];
    pieces[2].setColours(temp_colours);
    temp_colours[2] = black;
    temp_colours[4] = colours[_green];
    pieces[0].setColours(temp_colours);
    
    temp_colours[1] = black;     /*Middle slice*/
    pieces[3].setColours(temp_colours);
    temp_colours[4] = black;
    pieces[4].setColours(temp_colours);
    temp_colours[2] = colours[_blue];
    pieces[5].setColours(temp_colours);
    
    temp_colours[3] = colours[_red];     /*Bottom slice*/
    pieces[8].setColours(temp_colours);
    temp_colours[2] = black;
    pieces[7].setColours(temp_colours);
    temp_colours[4] = colours[_green];
    pieces[6].setColours(temp_colours);
    
    /*Initialize Yellow face piece colours*/
    temp_colours[0] = temp_colours[1] = temp_colours[2] = temp_colours[3] = temp_colours[4] = black;
    temp_colours[5] = colours[_yellow];

    temp_colours[1] = colours[_orange];    /*Top slice*/
    pieces[19].setColours(temp_colours);
    temp_colours[2] = colours[_blue];
    pieces[20].setColours(temp_colours);
    temp_colours[2] = black;
    temp_colours[4] = colours[_green];
    pieces[18].setColours(temp_colours);
    
    temp_colours[1] = black;     /*Middle slice*/
    pieces[21].setColours(temp_colours);
    temp_colours[4] = black;
    pieces[22].setColours(temp_colours);
    temp_colours[2] = colours[_blue];
    pieces[23].setColours(temp_colours);
    
    temp_colours[3] = colours[_red];     /*Bottom slice*/
    pieces[26].setColours(temp_colours);
    temp_colours[2] = black;
    pieces[25].setColours(temp_colours);
    temp_colours[4] = colours[_green];
    pieces[24].setColours(temp_colours);
    
    Piece * temp_pieces[9];
    
    faces[_white].setLinks(faces+_orange,
                           faces+_blue,
                           faces+_red,
                           faces+_green);
                               
    faces[_yellow].setLinks(faces+_orange,
                            faces+_green,
                            faces+_red,
                            faces+_blue);
                                
    faces[_green].setLinks(faces+_orange,
                           faces+_white,
                           faces+_red,
                           faces+_yellow);
                               
    faces[_blue].setLinks(faces+_orange,
                          faces+_yellow,
                          faces+_red,
                          faces+_white);
                              
    faces[_red].setLinks(faces+_white,
                         faces+_blue,
                         faces+_yellow,
                         faces+_green);
                               
    faces[_orange].setLinks(faces+_yellow,
                            faces+_blue,
                            faces+_white,
                            faces+_green);
                                
    for (int i = 0; i <27; i++){
        
    }
};

void RubiksCube::clamp(){
    for (int i = 0; i < 6; i++){
        faces[i].clamp();
    }
}

void RubiksCube::draw(){
    for (int i = 0; i < 27; i ++){
        pieces[i].draw();
    }
};

void RubiksCube::rotateFace(side face, double theta){
    faces[face].rotate(theta);
};
