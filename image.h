#ifndef __IMAGE_H_
#define __IMAGE_H_

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void writePGM(char* filename, GLubyte* buffer, int width, int height);
void writePPM(char* filename, GLubyte* buffer, int width, int height);

#endif