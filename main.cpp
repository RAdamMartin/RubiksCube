#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <glui.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>

#include "util.h"
#include "timer.h"
#include "RubiksCube.h"

// *************** GLOBAL VARIABLES *************************
const float PI = 3.14159;
const float SPINNER_SPEED = 0.1;

// --------------- USER INTERFACE VARIABLES -----------------
// Window settings
int windowID;				// Glut window ID (for display)
int Win[2];					// window (x,y) size

char msg[256];				// String used for status message
GLUI_StaticText* status;	// Status message ("Status: <msg>")
GLUI* glui_joints;


// ---------------- ANIMATION VARIABLES ---------------------
// Frame settings
Timer* frameRateTimer;
const float SEC_PER_FRAME = 1.0 / 60.0;

// Camera settings
bool updateCamZPos = false;
int  lastX = 0;
int  lastY = 0;
const float ZOOM_SCALE = 0.01;

GLdouble camXPos =  0.0;
GLdouble camYPos =  0.0;
GLdouble camZPos = -7.5;

const GLdouble CAMERA_FOVY = 60.0;
const GLdouble NEAR_CLIP   = 0.5;
const GLdouble FAR_CLIP    = 1000.0;

RubiksCube cube = RubiksCube();
float root_rotate_x;
float root_rotate_y;
float root_rotate_z;

float face_rotate;
int target_face;
int prev_target;
GLUI_Spinner *glui_rot_spinner;

bool random_animate = false;
RubiksCube::side random_face;
float random_rotate = 0;
int random_count = 0;

// ***********  FUNCTION HEADER DECLARATIONS ****************
// Initialization functions
void initDS();
void initGlut(int argc, char** argv);
void initGlui();
void initGl();

// Callbacks for handling events in glut
void reshape(int w, int h);
void animate();
void clamp();
void display(void);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);


// ******************** FUNCTIONS ************************
// main() function
// Initializes the user interface (and any user variables)
// then hands over control to the event handler, which calls 
// display() whenever the GL window needs to be redrawn.
int main(int argc, char** argv)
{
    // Process program arguments
    if(argc != 3) {
        printf("Usage: demo [width] [height]\n");
        printf("Using 640x480 window by default...\n");
        Win[0] = 640;
        Win[1] = 480;
    } else {
        Win[0] = atoi(argv[1]);
        Win[1] = atoi(argv[2]);
    }


    // Initialize data structs, glut, glui, and opengl
	initDS();
    initGlut(argc, argv);
    initGlui();
    initGl();

    // Invoke the standard GLUT main event loop
    glutMainLoop();

    return 0;         // never reached
}


// Create / initialize global data structures
void initDS()
{
    frameRateTimer = new Timer();
    target_face = prev_target = 0;
}


// Initialize glut and create a window with the specified caption 
void initGlut(int argc, char** argv)
{
	// Init GLUT
	glutInit(&argc, argv);

    // Set video mode: double-buffered, color, depth-buffered
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Create window
    glutInitWindowPosition (0, 0);
    glutInitWindowSize(Win[0],Win[1]);
    windowID = glutCreateWindow(argv[0]);

    // Setup callback functions to handle events
    glutReshapeFunc(reshape);	// Call reshape whenever window resized
    glutDisplayFunc(display);	// Call display whenever new frame needed
	glutMouseFunc(mouse);		// Call mouse whenever mouse button pressed
	glutMotionFunc(motion);		// Call motion whenever mouse moves while button pressed
}

// Quit button handler.  Called when the "quit" button is pressed.
void quitButton(int)
{
  exit(0);
}

void randomizeCube(int){
    std::srand(time(NULL));
    cube.rotateFace(static_cast<RubiksCube::side>(prev_target), 0);
    cube.rotateFace(static_cast<RubiksCube::side>(target_face), 0);
    prev_target = target_face;
    face_rotate = 0;
    glui_rot_spinner->set_float_val(0.0);
    random_face = static_cast<RubiksCube::side>(rand()%6);
    random_count=0;
    random_rotate=0;
    random_animate=true;
    // for (int i = 0; i < 100; i++){
    //     cube.rotateFace(static_cast<RubiksCube::side>(rand()%6), 90);
    //     cube.clamp();
    // }
}

void rotateFace(int){
    cube.rotateFace(static_cast<RubiksCube::side>(prev_target), 0);
    prev_target = target_face;
    face_rotate = 0;
    glui_rot_spinner->set_float_val(0.0);
    
    cube.rotateFace(static_cast<RubiksCube::side>(target_face), 90);
    cube.clamp();
}

void clamp(){
    
}

// Callback idle function for animating the scene
void animate(){
    if (random_animate){
        if (frameRateTimer->elapsed() > SEC_PER_FRAME ){
            random_rotate += 2;
            cube.rotateFace(random_face, random_rotate);
            if (random_rotate >= 90){
                cube.clamp();
                random_face = static_cast<RubiksCube::side>(rand()%6);
                random_rotate=0;
                random_count+=1;
            }
            if(random_count > 20){
                random_animate = false;
            }
        }
    }
    else if (target_face != prev_target){
        if (face_rotate >= 45 || face_rotate <= -45){
            cube.clamp();
        } else {
            cube.rotateFace(static_cast<RubiksCube::side>(prev_target), 0);
        }
        prev_target = target_face;
        face_rotate = 0;
        glui_rot_spinner->set_float_val(0.0);
    } else {    
        cube.rotateFace(static_cast<RubiksCube::side>(target_face), face_rotate);
    }
    
	if (frameRateTimer->elapsed() > SEC_PER_FRAME )
	{
		// Tell glut window to update itself. This will cause the display()
		// callback to be called, which renders the object (once you've written
		// the callback).
		glutSetWindow(windowID);
		glutPostRedisplay();

		// Restart the frame rate timer
		// for the next frame
		frameRateTimer->reset();
	}
}


// Initialize GLUI and the user interface
void initGlui()
{
	GLUI_Panel* glui_panel;
	GLUI_Spinner* glui_spinner;
    GLUI_Checkbox* glui_checkbox;
	GLUI_RadioGroup* glui_radio_group;

    GLUI_Master.set_glutIdleFunc(animate);
    
    glui_joints = GLUI_Master.create_glui("Joint Control", 0, Win[0]+12, 0);
    glui_panel = glui_joints->add_panel("Main Rotation");
    
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, 
                                                     "rotate x:",
                                                     GLUI_SPINNER_FLOAT, 
                                                     &root_rotate_x);
	glui_spinner->set_float_limits(-180, 180, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel,
                                                     "rotate y:",
                                                     GLUI_SPINNER_FLOAT,
                                                     &root_rotate_y);
	glui_spinner->set_float_limits(-180, 180, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel,
                                                     "rotate z:",
                                                     GLUI_SPINNER_FLOAT,
                                                     &root_rotate_z);
	glui_spinner->set_float_limits(-180, 180, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);
    
    glui_panel = glui_joints->add_panel("Face Rotation");
    glui_radio_group = glui_joints->add_radiogroup_to_panel(glui_panel, &target_face);
    glui_joints->add_radiobutton_to_group(glui_radio_group, "White");
    glui_joints->add_radiobutton_to_group(glui_radio_group, "Yellow");
    glui_joints->add_radiobutton_to_group(glui_radio_group, "Green");
    glui_joints->add_radiobutton_to_group(glui_radio_group, "Blue");
    glui_joints->add_radiobutton_to_group(glui_radio_group, "Red");
    glui_joints->add_radiobutton_to_group(glui_radio_group, "Orange");
    
	glui_rot_spinner = glui_joints->add_spinner_to_panel(glui_panel, 
                                                     "rotation :",
                                                     GLUI_SPINNER_FLOAT, 
                                                     &face_rotate);
	glui_rot_spinner->set_float_limits(-180, 180, GLUI_LIMIT_WRAP);
	glui_rot_spinner->set_speed(SPINNER_SPEED);

	// Add button to randomize cube
	// glui_panel = glui_joints->add_panel("", GLUI_PANEL_NONE);
	glui_joints->add_button_to_panel(glui_panel, "Rotate", 0, rotateFace);
	glui_joints->add_button_to_panel(glui_panel, "Randomize", 0, randomizeCube);


	// Add button to quit
	glui_panel = glui_joints->add_panel("", GLUI_PANEL_NONE);
	glui_joints->add_button_to_panel(glui_panel, "Quit", 0, quitButton);
    
}


// Performs most of the OpenGL intialization
void initGl(void)
{
    // glClearColor (red, green, blue, alpha)
    // Ignore the meaning of the 'alpha' value for now
    glClearColor(0.7f,0.7f,0.9f,1.0f);
}

// Handles the window being resized by updating the viewport
// and projection matrices
void reshape(int w, int h)
{
	// Update internal variables and OpenGL viewport
	Win[0] = w;
	Win[1] = h;
	glViewport(0, 0, (GLsizei)Win[0], (GLsizei)Win[1]);

    // Setup projection matrix for new window
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective(CAMERA_FOVY, (GLdouble)Win[0]/(GLdouble)Win[1], NEAR_CLIP, FAR_CLIP);
}

// display callback
void display(void)
{
    // Clear the screen with the background colour
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    // Setup the model-view transformation matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	// Specify camera transformation
	glTranslatef(2*camXPos, 2*camYPos, 2*camZPos);

    //Set render style
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0,1.0);
    
    glEnable(GL_LIGHTING);
    glShadeModel (GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    
    /*Lights */
    GLfloat diffuseRGBA[] = {2.8f, 2.8f, 2.8f, 2.8f};
    GLfloat specularRGBA[] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat ambiant[] = {1.0f, 1.0f, 1.0f, 1.0f};
    
    const float LIGHT_RADIUS = 30.0;
    float lightTheta = 0;
    glEnable(GL_LIGHT0);
    GLfloat light0_position[] = { camXPos+ std::sin(lightTheta/360*M_PI)*LIGHT_RADIUS,
                                    camYPos,
                                    5+camZPos+std::cos(lightTheta/360*M_PI)*LIGHT_RADIUS,
                                    0.0 };
    GLfloat spotDirection[] = { -std::sin(lightTheta/360*M_PI),
                                    camYPos,
                                    -std::cos(lightTheta/360*M_PI),
                                    0.0 };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseRGBA);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularRGBA);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambiant);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDirection);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 35.0f);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 160.0f);
    
	glPushMatrix();
        // Start Body
		glRotatef(root_rotate_x, 1.0, 0.0, 0.0);
		glRotatef(root_rotate_y, 0.0, 1.0, 0.0);
        glRotatef(root_rotate_z, 0.0, 0.0, 1.0);
        cube.draw();
        //End Body
	glPopMatrix();

    // Execute any GL functions that are in the queue just to be safe
    glFlush();

    // Now, show the frame buffer that we just drew into.
    // (this prevents flickering).
    glutSwapBuffers();
}

// Handles mouse button pressed / released events
void mouse(int button, int state, int x, int y)
{
	// If the RMB is pressed and dragged then zoom in / out
	if( button == GLUT_RIGHT_BUTTON )
	{
		if( state == GLUT_DOWN )
		{
			lastX = x;
			lastY = y;
			updateCamZPos = true;
		}
		else
		{
			updateCamZPos = false;
		}
	}
}


// Handles mouse motion events while a button is pressed
void motion(int x, int y)
{
	// If the RMB is pressed and dragged then zoom in / out
	if( updateCamZPos )
	{
		// Update camera z position
		camZPos += (x - lastX) * ZOOM_SCALE;
		lastX = x;

		// Redraw the scene from updated camera position
		glutSetWindow(windowID);
		glutPostRedisplay();
	}
}