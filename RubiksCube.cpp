#include "RubiksCube.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include "vector.h"

void setGlColour(int ind, Colour * colours){
    /*Material*/
    float r = (float)(colours[ind][0]/255.0);
    float g = (float)(colours[ind][1]/255.0);
    float b = (float)(colours[ind][2]/255.0);
    GLfloat mat_ambient[] = { r, g, b, 1};
    GLfloat mat_diffuse[] = { r, g, b, 1 };
    GLfloat mat_specular[]= { r, g, b, 1};
    float shine = 0.2; 
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT,GL_SHININESS,shine*128.0);

    glColor3d(colours[ind][0]/255, colours[ind][1]/255, colours[ind][2]/255);
}

void drawCube(Colour * cols){    
    //Assign normal vectors for faces and calculate normals for vertices
    Vector top   = Vector(3);
    Vector bot   = Vector(3);
    Vector left  = Vector(3);
    Vector right = Vector(3);
    Vector front = Vector(3);
    Vector back  = Vector(3);
    
    top.set3d  ( 0.0,  1.0,  0.0);
    bot.set3d  ( 0.0, -1.0,  0.0);
    left.set3d ( 1.0,  0.0,  0.0);
    right.set3d(-1.0,  0.0,  0.0);
    front.set3d( 0.0,  0.0,  1.0);
    back.set3d ( 0.0,  0.0, -1.0);
    
    Vector A = (bot + front + left )/3;
    Vector B = (bot + front + right)/3;
    Vector C = (top + front + right)/3;
    Vector D = (top + front + left )/3;
    Vector E = (bot + back  + left )/3;
    Vector F = (bot + back  + right)/3;
    Vector G = (top + back  + right)/3;
    Vector H = (top + back  + left)/3;
	glBegin(GL_QUADS);
		// draw front face
        setGlColour(0, cols);
        glNormal3f(A[0], A[1], A[2]);
		glVertex3f(-1.00, -1.00, 1.00); //A
        glNormal3f(B[0], B[1], B[2]);
		glVertex3f( 1.00, -1.00, 1.00); //B
        glNormal3f(C[0], C[1], C[2]);
		glVertex3f( 1.00,  1.00, 1.00); //C
        glNormal3f(D[0], D[1], D[2]); 
		glVertex3f(-1.00,  1.00, 1.00); //D

		// draw back face
        setGlColour(5, cols);
        glNormal3f(F[0], F[1], F[2]);
		glVertex3f( 1.00, -1.00, -1.00); //F
        glNormal3f(E[0], E[1], E[2]);
		glVertex3f(-1.00, -1.00, -1.00); //E
        glNormal3f(H[0], H[1], H[2]);
		glVertex3f(-1.00,  1.00, -1.00); //H
        glNormal3f(G[0], G[1], G[2]);
		glVertex3f( 1.00,  1.00, -1.00); //G

		// draw left face (stage right)
        setGlColour(4, cols);
		glNormal3f(E[0], E[1], E[2]);
		glVertex3f(-1.00, -1.00, -1.00); //E
        glNormal3f(A[0], A[1], A[2]);
		glVertex3f(-1.00, -1.00, 1.00); //A
        glNormal3f(D[0], D[1], D[2]); 
		glVertex3f(-1.00,  1.00, 1.00); //D
        glNormal3f(H[0], H[1], H[2]);
		glVertex3f(-1.00,  1.00, -1.00); //H

		// draw right face (stage left)
        setGlColour(2, cols);
        glNormal3f(B[0], B[1], B[2]);
		glVertex3f( 1.00, -1.00, 1.00); //B
        glNormal3f(F[0], F[1], F[2]);
		glVertex3f( 1.00, -1.00, -1.00); //F
        glNormal3f(G[0], G[1], G[2]);
		glVertex3f( 1.00,  1.00, -1.00); //G
        glNormal3f(C[0], C[1], C[2]);
		glVertex3f( 1.00,  1.00, 1.00); //C

		// draw top
        setGlColour(1, cols);
        glNormal3f(D[0], D[1], D[2]); 
		glVertex3f(-1.00,  1.00, 1.00); //D
        glNormal3f(C[0], C[1], C[2]);
		glVertex3f( 1.00,  1.00, 1.00); //C
        glNormal3f(G[0], G[1], G[2]);
		glVertex3f( 1.00,  1.00, -1.00); //G
        glNormal3f(H[0], H[1], H[2]);
		glVertex3f(-1.00,  1.00, -1.00); //H

		// draw bottom
        setGlColour(3, cols);
        glNormal3f(E[0], E[1], E[2]);
		glVertex3f(-1.00, -1.00, -1.00); //E
        glNormal3f(F[0], F[1], F[2]);
		glVertex3f( 1.00, -1.00, -1.00); //F
        glNormal3f(B[0], B[1], B[2]);
		glVertex3f( 1.00, -1.00, 1.00); //B
        glNormal3f(A[0], A[1], A[2]);
		glVertex3f(-1.00, -1.00, 1.00); //A
        // glColor3f(1.00, 0.0, 1.00);
	glEnd();
};

Piece::Piece(){
    
};

void Piece::clamp(){
    if (theta >= 45){
        theta = 90;
    } else if (theta <= -45){
        theta = -90;
    } else {
        theta = 0;
    }
    glPushMatrix();
        glLoadMatrixd(matrix);
        glPushMatrix();
            glRotated(theta, axis[0], axis[1], axis[2]);
            glGetDoublev(GL_MODELVIEW_MATRIX, matrix);
        glPopMatrix();
    glPopMatrix();
    theta = 0;
};

void Piece::draw(){
    glPushMatrix();
        glTranslated(t_x, t_y, t_z);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        drawCube(colours);
        Colour c = Colour(0,0,0);
        Colour cols[6] = {c,c,c,c,c,c};
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        drawCube(cols);
    glPopMatrix(); 
}

void Piece::rotate(double theta, double x, double y, double z){};

void Piece::setColours(Colour * cols){
    for (int i = 0; i < 6; i++){
        colours[i] = cols[i];
    }
};
void Piece::translate(double x, double y, double z){
    t_x = x;
    t_y = y;
    t_z = z;
};


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
    colours[_yellow] = Colour(255,255,  0);
    colours[_green]  = Colour(  0, 62, 38);
    colours[_blue]   = Colour(  0, 32, 73);
    colours[_red]    = Colour( 77, 12, 23);
    colours[_orange] = Colour(255,165,  0);
    Colour black     = Colour(  0,  0,  0);
    
    /*Initialize translations*/
    for (int i = 0; i < 9; i++){
        int x = 0;
        if (i == 0 || i == 3 || i == 6){
            x = -2;
        } else if (i == 2 || i == 5 || i == 8){
            x = 2;
        }
        int y = 0;
        if (i > 5){
            y = -2;
        } else if (i < 3){
            y = 2;
        }
        
        pieces[i].translate(x, y, 2); //White Face
        pieces[i+9].translate(x, y, 0); //Middle Slice
        pieces[i+18].translate(x, y, -2); //Yellow Face
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
    
    /*Initialize Middle slice piece colours*/
    temp_colours[0] = temp_colours[2] = temp_colours[3] = temp_colours[4] = temp_colours[5] = black;
    temp_colours[1] = colours[_orange];

    pieces[10].setColours(temp_colours);    /*Top slice*/
    temp_colours[2] = colours[_blue];
    pieces[11].setColours(temp_colours);
    temp_colours[2] = black;
    temp_colours[4] = colours[_green];
    pieces[9].setColours(temp_colours);
    
    temp_colours[1] = black;             /*Middle slice*/
    pieces[12].setColours(temp_colours);
    temp_colours[4] = black;
    pieces[13].setColours(temp_colours);
    temp_colours[2] = colours[_blue];
    pieces[14].setColours(temp_colours);
    
    temp_colours[3] = colours[_red];     /*Bottom slice*/
    pieces[17].setColours(temp_colours);
    temp_colours[2] = black;
    pieces[16].setColours(temp_colours);
    temp_colours[4] = colours[_green];
    pieces[15].setColours(temp_colours);
    
    Piece * temp_pieces[9];
    
    faces[_white].setLinks(faces+_orange*sizeof(Face),
                           faces+_blue*sizeof(Face),
                           faces+_red*sizeof(Face),
                           faces+_green*sizeof(Face));
                               
    faces[_yellow].setLinks(faces+_orange*sizeof(Face),
                            faces+_green*sizeof(Face),
                            faces+_red*sizeof(Face),
                            faces+_blue*sizeof(Face));
                                
    faces[_green].setLinks(faces+_orange*sizeof(Face),
                           faces+_white*sizeof(Face),
                           faces+_red*sizeof(Face),
                           faces+_yellow*sizeof(Face));
                               
    faces[_blue].setLinks(faces+_orange*sizeof(Face),
                          faces+_yellow*sizeof(Face),
                          faces+_red*sizeof(Face),
                          faces+_white*sizeof(Face));
                              
    faces[_red].setLinks(faces+_white*sizeof(Face),
                         faces+_blue*sizeof(Face),
                         faces+_yellow*sizeof(Face),
                         faces+_green*sizeof(Face));
                               
    faces[_orange].setLinks(faces+_yellow*sizeof(Face),
                            faces+_blue*sizeof(Face),
                            faces+_white*sizeof(Face),
                            faces+_green*sizeof(Face));
};

void RubiksCube::clamp(){
    for (int i = 0; i < 6; i++){
        faces[i].clamp();
    }
}

void RubiksCube::draw(){
    for (int i = 0; i < 27; i ++){
        // glPushMatrix();
        pieces[i].draw();
        // glPopMatrix();
    }
};

void RubiksCube::rotateFace(side face, double theta){
    faces[face].rotate(theta);
};
