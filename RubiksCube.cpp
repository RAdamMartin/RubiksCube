#include "RubiksCube.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>

Matrix4x4 getRotationMatrix(int rots, double * axis){
    Matrix4x4 M;
      
    double s = rots/std::abs(rots);
    double c = 0;
    if (rots == 2 || rots == -2){
        s = 0;
        c = rots/std::abs(rots);
    }
    if (axis[0]+axis[1]+axis[2] == -1){
        s = -s;
    }
    if (axis[0]){
        M.setVal( 5, c);
        M.setVal(10, c);
        M.setVal( 6,-s);
        M.setVal( 9, s);
    } else if (axis[1]){
        M.setVal( 0, c);
        M.setVal(10, c);
        M.setVal( 2, s);
        M.setVal( 8,-s);
    } else {
        M.setVal( 0, c);
        M.setVal( 5, c);
        M.setVal( 1,-s);
        M.setVal( 4, s);
    }
    std::cout <<"Times:\n";
    std::cout << M;
    return M;
}

void setGlColour(int ind, Colour * colours){
    /*Material*/
    float shine = 0.4; 
    float r = (float)(colours[ind][0]/255.0);
    float g = (float)(colours[ind][1]/255.0);
    float b = (float)(colours[ind][2]/255.0);
    GLfloat mat_diffuse[] = { r, g, b, 1 };
    GLfloat mat_ambient[] = { r, g, b, 1};
    GLfloat mat_specular[]= { 0.5, 0.5, 0.5, 1};

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT,GL_SHININESS,shine*128.0);

    glColor3d(colours[ind][0]/255, colours[ind][1]/255, colours[ind][2]/255);
}

void drawCube(Colour * cols){    
    //Assign normal vectors for faces and calculate normals for vertices
    Vector3D top   = Vector3D( 0.0,  1.0,  0.0);
    Vector3D bot   = Vector3D( 0.0, -1.0,  0.0);
    Vector3D left  = Vector3D( 1.0,  0.0,  0.0);
    Vector3D right = Vector3D(-1.0,  0.0,  0.0);
    Vector3D front = Vector3D( 0.0,  0.0,  1.0);
    Vector3D back  = Vector3D( 0.0,  0.0, -1.0);
        
    Vector3D A = (bot + front + left );
    Vector3D B = (bot + front + right);
    Vector3D C = (top + front + right);
    Vector3D D = (top + front + left );
    Vector3D E = (bot + back  + left );
    Vector3D F = (bot + back  + right);
    Vector3D G = (top + back  + right);
    Vector3D H = (top + back  + left );
    
    A.normalize();
    B.normalize();
    C.normalize();
    D.normalize();
    E.normalize();
    F.normalize();
    G.normalize();
    H.normalize();
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
	glEnd();
};

Piece::Piece(){
    theta = 0;
    axis[0] = 0;
    axis[1] = 0;
    axis[2] = 0;
};

void Piece::clamp(){
    int rots = round(theta/90.0);
    std::cout <<rots <<"\n"; 
    if (rots != 0){
        std::cout << rotation;        
        rotation = getRotationMatrix(rots, axis)*rotation;
        std::cout << rotation;
    }
    theta = 0;
};

void Piece::draw(){
    glPushMatrix();
        glRotatef(theta, axis[0], axis[1], axis[2]);
        Vector4D col;
        double matrix[16];
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                matrix[i*4 + j] = rotation[j][i];
            }
        }
        glMultMatrixd(matrix);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        drawCube(colours);
        Colour c = Colour(0,0,0);
        Colour cols[6] = {c,c,c,c,c,c};
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        drawCube(cols);
    glPopMatrix(); 
}

void Piece::rotate(float ang, double x, double y, double z){
    theta = ang;
    axis[0] = x;
    axis[1] = y;
    axis[2] = z;
};

void Piece::setColours(Colour * cols){
    for (int i = 0; i < 6; i++){
        colours[i] = cols[i];
    }
};
void Piece::translate(double x, double y, double z){
    rotation.setVal(3,x);
    rotation.setVal(7,y);
    rotation.setVal(11,z);
};


Face::Face(){
    theta = 0;
};

void Face::addPieces(Face * src, Piece *p0, Piece *p1, Piece *p2){
    if (src == links[0]){
        pieces[0] = p0;
        pieces[1] = p1;
        pieces[2] = p2;
    } else if (src == links[1]){
        pieces[8] = p0;
        pieces[5] = p1;
        pieces[2] = p2;
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
    if (theta < 0){
        theta += 360;
    }
    if (theta > 0){
        while (theta > 45){
            links[0]->addPieces(this, pieces[2], pieces[5], pieces[8]);
            links[1]->addPieces(this, pieces[8], pieces[7], pieces[6]);
            links[2]->addPieces(this, pieces[6], pieces[3], pieces[0]);
            links[3]->addPieces(this, pieces[0], pieces[1], pieces[2]);
            Piece *temp_pieces[9];
            for(int i = 0; i < 9; i++){
                temp_pieces[i] = pieces[i];
            }
            for(int i = 0; i < 3; i++){
                pieces[i]   = temp_pieces[(i*3)+2]; 
                pieces[i+3] = temp_pieces[(i*3)+1]; 
                pieces[i+6] = temp_pieces[(i*3)]; 
            }
            theta -= 90;
        }
        for(int i = 0; i < 9; i++){
            pieces[i]->clamp();
        }
    }
}

void Face::rotate(float angle){
    theta = angle;
    for(int i = 0; i < 9; i++){
        pieces[i]->rotate(angle, axis[0], axis[1], axis[2]);
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
    colours[_orange] = Colour(255, 55,  0);
    Colour black     = Colour(  2,  2,  2);
    
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
    
    /* Initialize faces */
    //Set Axes
    faces[ _white].setAxis(0, 0, 1);
    faces[_yellow].setAxis(0, 0, -1);
    faces[ _green].setAxis(-1, 0, 0);
    faces[  _blue].setAxis(1, 0, 0);
    faces[   _red].setAxis(0, -1, 0);
    faces[_orange].setAxis(0, 1, 0);
    
    //Set Links    
    faces[_white].setLinks(&(faces[_orange]),
                           &(faces[  _blue]),
                           &(faces[   _red]),
                           &(faces[ _green]));
                               
    faces[_yellow].setLinks(&(faces[_orange]),
                            &(faces[ _green]),
                            &(faces[   _red]),
                            &(faces[  _blue]));
                                
    faces[_green].setLinks(&(faces[_orange]),
                           &(faces[ _white]),
                           &(faces[   _red]),
                           &(faces[_yellow]));
                               
    faces[_blue].setLinks(&(faces[_orange]),
                          &(faces[_yellow]),
                          &(faces[   _red]),
                          &(faces[ _white]));
                              
    faces[_red].setLinks(&(faces[_yellow]),
                         &(faces[  _blue]),
                         &(faces[ _white]),
                         &(faces[ _green]));
                               
    faces[_orange].setLinks(&(faces[_yellow]),
                            &(faces[  _blue]),
                            &(faces[ _white]),
                            &(faces[ _green]));
                            
    //Set pieces
    Piece *temp_pieces[9];
    for(int i=0; i < 9; i++){
        temp_pieces[i] = &(pieces[i]);//*sizeof(Piece);
    }
    faces[_white].setPieces(temp_pieces);
    
    for(int i=0; i < 9; i++){
        temp_pieces[i] = &(pieces[i+18]);
    }
    faces[_yellow].setPieces(temp_pieces);
    
    for(int i=0; i < 3; i++){
        temp_pieces[i]   = &(pieces[18 - 9*i]);
        temp_pieces[i+3] = &(pieces[21 - 9*i]);
        temp_pieces[i+6] = &(pieces[24 - 9*i]);
    }
    faces[_green].setPieces(temp_pieces);
    
    for(int i=0; i < 3; i++){
        temp_pieces[i]   = &(pieces[2 + 9*i]);
        temp_pieces[i+3] = &(pieces[5 + 9*i]);
        temp_pieces[i+6] = &(pieces[8 + 9*i]);
    }
    faces[_blue].setPieces(temp_pieces);
    
    for(int i=0; i < 3; i++){
        temp_pieces[i]   = &(pieces[26 - i]);
        temp_pieces[i+3] = &(pieces[17 - i]);
        temp_pieces[i+6] = &(pieces[8 - i]);
    }
    faces[_red].setPieces(temp_pieces);
    
    for(int i=0; i < 3; i++){
        temp_pieces[i]   = &(pieces[18 + i]);
        temp_pieces[i+3] = &(pieces[9 + i]);
        temp_pieces[i+6] = &(pieces[i]);
    }
    faces[_orange].setPieces(temp_pieces);
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

void RubiksCube::rotateFace(side face, float theta){
    faces[face].rotate(theta);
};
