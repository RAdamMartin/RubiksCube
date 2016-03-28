/***********************************************************
             CSC418, Winter 2016
 
                 penguin.cpp
                 author: Mike Pratscher
                 based on code by: Eron Steger, J. Radulovich

		Main source file for assignment 2
		Uses OpenGL, GLUT and GLUI libraries
  
    Instructions:
        Please read the assignment page to determine 
        exactly what needs to be implemented.  Then read 
        over this file and become acquainted with its 
        design. In particular, see lines marked 'README'.
		
		Be sure to also look over keyframe.h and vector.h.
		While no changes are necessary to these files, looking
		them over will allow you to better understand their
		functionality and capabilites.

        Add source code where it appears appropriate. In
        particular, see lines marked 'TODO'.

        You should not need to change the overall structure
        of the program. However it should be clear what
        your changes do, and you should use sufficient comments
        to explain your code.  While the point of the assignment
        is to draw and animate the character, you will
        also be marked based on your design.

***********************************************************/
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <glui.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "keyframe.h"
#include "timer.h"
#include "vector.h"
#include "shapes.h"
#include "image.h"
#include "penguin.h"

// *************** GLOBAL VARIABLES *************************


const float PI = 3.14159;

const float SPINNER_SPEED = 0.1;

// --------------- USER INTERFACE VARIABLES -----------------

// Window settings
int windowID;				// Glut window ID (for display)
int Win[2];					// window (x,y) size

GLUI* glui_joints;			// Glui window with joint controls
GLUI* glui_keyframe;		// Glui window with keyframe controls
GLUI* glui_render;			// Glui window for render style

char msg[256];				// String used for status message
GLUI_StaticText* status;	// Status message ("Status: <msg>")


// ---------------- ANIMATION VARIABLES ---------------------

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

// Render settings
enum { WIREFRAME, SOLID, OUTLINED };	// README: the different render styles
int renderStyle = WIREFRAME;			// README: the selected render style
enum { NONE, MATTE, METALLIZED };	// README: the different material styles
int materialStyle = NONE;

//Light position
int lightOn = 0;
float lightTheta = 0.0;
const float LIGHT_RADIUS = 30.0;

// Animation settings
int animate_mode = 0;			// 0 = no anim, 1 = animate

// Keyframe settings
const char filenameKF[] = "keyframes.txt";	// file for loading / saving keyframes

Keyframe* keyframes;			// list of keyframes

int maxValidKeyframe   = 0;		// index of max VALID keyframe (in keyframe list)
const int KEYFRAME_MIN = 0;
const int KEYFRAME_MAX = 32;	// README: specifies the max number of keyframes

// Frame settings
char filenameF[128];			// storage for frame filename

int frameNumber = 0;			// current frame being dumped
int frameToFile = 0;			// flag for dumping frames to file

const float DUMP_FRAME_PER_SEC = 24.0;		// frame rate for dumped frames
const float DUMP_SEC_PER_FRAME = 1.0 / DUMP_FRAME_PER_SEC;

// Time settings
Timer* animationTimer;
Timer* frameRateTimer;

const float TIME_MIN = 0.0;
const float TIME_MAX = 10.0;	// README: specifies the max time of the animation
const float SEC_PER_FRAME = 1.0 / 60.0;

// Joint settings

// README: This is the key data structure for
// updating keyframes in the keyframe list and
// for driving the animation.
//   i) When updating a keyframe, use the values
//      in this data structure to update the
//      appropriate keyframe in the keyframe list.
//  ii) When calculating the interpolated pose,
//      the resulting pose vector is placed into
//      this data structure. (This code is already
//      in place - see the animate() function)
// iii) When drawing the scene, use the values in
//      this data structure (which are set in the
//      animate() function as described above) to
//      specify the appropriate transformations.
Keyframe* joint_ui_data;

// README: To change the range of a particular DOF,
// simply change the appropriate min/max values below
const float ROOT_TRANSLATE_X_MIN = -5.0;
const float ROOT_TRANSLATE_X_MAX =  5.0;
const float ROOT_TRANSLATE_Y_MIN = -5.0;
const float ROOT_TRANSLATE_Y_MAX =  5.0;
const float ROOT_TRANSLATE_Z_MIN = -5.0;
const float ROOT_TRANSLATE_Z_MAX =  5.0;
const float ROOT_ROTATE_X_MIN    = -180.0;
const float ROOT_ROTATE_X_MAX    =  180.0;
const float ROOT_ROTATE_Y_MIN    = -180.0;
const float ROOT_ROTATE_Y_MAX    =  180.0;
const float ROOT_ROTATE_Z_MIN    = -180.0;
const float ROOT_ROTATE_Z_MAX    =  180.0;
const float HEAD_MIN             = -180.0;
const float HEAD_MAX             =  180.0;
const float ARM_SCALE_MIN        =  0.5;
const float ARM_SCALE_MAX        =  2.0;
const float SHOULDER_PITCH_MIN   = -45.0;
const float SHOULDER_PITCH_MAX   =  45.0;
const float SHOULDER_YAW_MIN     = -45.0;
const float SHOULDER_YAW_MAX     =  45.0;
const float SHOULDER_ROLL_MIN    = -45.0;
const float SHOULDER_ROLL_MAX    =  45.0;
const float HIP_PITCH_MIN        = -45.0;
const float HIP_PITCH_MAX        =  45.0;
const float HIP_YAW_MIN          = -45.0;
const float HIP_YAW_MAX          =  45.0;
const float HIP_ROLL_MIN         = -45.0;
const float HIP_ROLL_MAX         =  45.0;
const float BEAK_MIN             =  0.0;
const float BEAK_MAX             =  1.0;
const float ELBOW_MIN            =  0.0;
const float ELBOW_MAX            = 75.0;
const float KNEE_MIN             =  0.0;
const float KNEE_MAX             = 75.0;


// ***********  FUNCTION HEADER DECLARATIONS ****************
// Initialization functions
void initDS();
void initGlut(int argc, char** argv);
void initGlui();
void initGl();

// Callbacks for handling events in glut
void reshape(int w, int h);
void animate();
void display(void);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);

// Functions to help draw the object
Vector getInterpolatedJointDOFS(float time);
void drawWing(int x_shift, int ds);

// Image functions
void writeFrame(char* filename, bool pgm, bool frontBuffer);


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
	keyframes = new Keyframe[KEYFRAME_MAX];
	for( int i = 0; i < KEYFRAME_MAX; i++ )
		keyframes[i].setID(i);

	animationTimer = new Timer();
	frameRateTimer = new Timer();
	joint_ui_data  = new Keyframe();
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


// Load Keyframe button handler. Called when the "load keyframe" button is pressed
void loadKeyframeButton(int)
{
	// Get the keyframe ID from the UI
	int keyframeID = joint_ui_data->getID();

	// Update the 'joint_ui_data' variable with the appropriate
	// entry from the 'keyframes' array (the list of keyframes)
	*joint_ui_data = keyframes[keyframeID];

	// Sync the UI with the 'joint_ui_data' values
	glui_joints->sync_live();
	glui_keyframe->sync_live();

	// Let the user know the values have been loaded
	sprintf(msg, "Status: Keyframe %d loaded successfully", keyframeID);
	status->set_text(msg);
}

// Update Keyframe button handler. Called when the "update keyframe" button is pressed
void updateKeyframeButton(int)
{
	// Get the keyframe ID from the UI
	int keyframeID = joint_ui_data->getID();

	// Update the 'maxValidKeyframe' index variable
	// (it will be needed when doing the interpolation)
    if (keyframeID > maxValidKeyframe){
        maxValidKeyframe = keyframeID;
    }

	// Update the appropriate entry in the 'keyframes' array
	// with the 'joint_ui_data' data
    keyframes[keyframeID] = *joint_ui_data; 

	// Let the user know the values have been updated
	sprintf(msg, "Status: Keyframe %d updated successfully", keyframeID);
	status->set_text(msg);
}

// Load Keyframes From File button handler. Called when the "load keyframes from file" button is pressed
//
// ASSUMES THAT THE FILE FORMAT IS CORRECT, ie, there is no error checking!
//
void loadKeyframesFromFileButton(int)
{
	// Open file for reading
	FILE* file = fopen(filenameKF, "r");
	if( file == NULL )
	{
		sprintf(msg, "Status: Failed to open file %s", filenameKF);
		status->set_text(msg);
		return;
	}

	// Read in maxValidKeyframe first
	fscanf(file, "%d", &maxValidKeyframe);

	// Now read in all keyframes in the format:
	//    id
	//    time
	//    DOFs
	//
	for( int i = 0; i <= maxValidKeyframe; i++ )
	{
		fscanf(file, "%d", keyframes[i].getIDPtr());
		fscanf(file, "%f", keyframes[i].getTimePtr());

		for( int j = 0; j < Keyframe::NUM_JOINT_ENUM; j++ )
			fscanf(file, "%f", keyframes[i].getDOFPtr(j));
	}

	// Close file
	fclose(file);

	// Let the user know the keyframes have been loaded
	sprintf(msg, "Status: Keyframes loaded successfully");
	status->set_text(msg);
}

// Save Keyframes To File button handler. Called when the "save keyframes to file" button is pressed
void saveKeyframesToFileButton(int)
{
	// Open file for writing
	FILE* file = fopen(filenameKF, "w");
	if( file == NULL )
	{
		sprintf(msg, "Status: Failed to open file %s", filenameKF);
		status->set_text(msg);
		return;
	}

	// Write out maxValidKeyframe first
	fprintf(file, "%d\n", maxValidKeyframe);
	fprintf(file, "\n");

	// Now write out all keyframes in the format:
	//    id
	//    time
	//    DOFs
	//
	for( int i = 0; i <= maxValidKeyframe; i++ )
	{
		fprintf(file, "%d\n", keyframes[i].getID());
		fprintf(file, "%f\n", keyframes[i].getTime());

		for( int j = 0; j < Keyframe::NUM_JOINT_ENUM; j++ )
			fprintf(file, "%f\n", keyframes[i].getDOF(j));

		fprintf(file, "\n");
	}

	// Close file
	fclose(file);

	// Let the user know the keyframes have been saved
	sprintf(msg, "Status: Keyframes saved successfully");
	status->set_text(msg);
}

// Animate button handler.  Called when the "animate" button is pressed.
void animateButton(int)
{
  // synchronize variables that GLUT uses
  glui_keyframe->sync_live();

  // toggle animation mode and set idle function appropriately
  if( animate_mode == 0 )
  {
	// start animation
	frameRateTimer->reset();
	animationTimer->reset();

	animate_mode = 1;
	GLUI_Master.set_glutIdleFunc(animate);

	// Let the user know the animation is running
	sprintf(msg, "Status: Animating...");
	status->set_text(msg);
  }
  else
  {
	// stop animation
	animate_mode = 0;
	GLUI_Master.set_glutIdleFunc(NULL);

	// Let the user know the animation has stopped
	sprintf(msg, "Status: Animation stopped");
	status->set_text(msg);
  }
}

// Render Frames To File button handler. Called when the "Render Frames To File" button is pressed.
void renderFramesToFileButton(int)
{
	// Calculate number of frames to generate based on dump frame rate
	int numFrames = int(keyframes[maxValidKeyframe].getTime() * DUMP_FRAME_PER_SEC) + 1;

	// Generate frames and save to file
	frameToFile = 1;
	for( frameNumber = 0; frameNumber < numFrames; frameNumber++ )
	{
		// Get the interpolated joint DOFs
		joint_ui_data->setDOFVector( getInterpolatedJointDOFS(frameNumber * DUMP_SEC_PER_FRAME) );

		// Let the user know which frame is being rendered
		sprintf(msg, "Status: Rendering frame %d...", frameNumber);
		status->set_text(msg);

		// Render the frame
		display();
	}
	frameToFile = 0;

	// Let the user know how many frames were generated
	sprintf(msg, "Status: %d frame(s) rendered to file", numFrames);
	status->set_text(msg);
}

// Quit button handler.  Called when the "quit" button is pressed.
void quitButton(int)
{
  exit(0);
}

// Initialize GLUI and the user interface
void initGlui()
{
	GLUI_Panel* glui_panel;
	GLUI_Spinner* glui_spinner;
    GLUI_Checkbox* glui_checkbox;
	GLUI_RadioGroup* glui_radio_group;

    GLUI_Master.set_glutIdleFunc(NULL);
    
	// Create GLUI window (joint controls) ***************
	glui_joints = GLUI_Master.create_glui("Joint Control", 0, Win[0]+12, 0);

    // Create controls to specify root position and orientation
	glui_panel = glui_joints->add_panel("Root");

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_X));
	glui_spinner->set_float_limits(ROOT_TRANSLATE_X_MIN, ROOT_TRANSLATE_X_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_Y));
	glui_spinner->set_float_limits(ROOT_TRANSLATE_Y_MIN, ROOT_TRANSLATE_Y_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_Z));
	glui_spinner->set_float_limits(ROOT_TRANSLATE_Z_MIN, ROOT_TRANSLATE_Z_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::ROOT_ROTATE_X));
	glui_spinner->set_float_limits(ROOT_ROTATE_X_MIN, ROOT_ROTATE_X_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::ROOT_ROTATE_Y));
	glui_spinner->set_float_limits(ROOT_ROTATE_Y_MIN, ROOT_ROTATE_Y_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::ROOT_ROTATE_Z));
	glui_spinner->set_float_limits(ROOT_ROTATE_Z_MIN, ROOT_ROTATE_Z_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	// Create controls to specify head rotation
	glui_panel = glui_joints->add_panel("Head");

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "head:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::HEAD));
	glui_spinner->set_float_limits(HEAD_MIN, HEAD_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	// Create controls to specify beak
	glui_panel = glui_joints->add_panel("Beak");

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "beak:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::BEAK));
	glui_spinner->set_float_limits(BEAK_MIN, BEAK_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);


	glui_joints->add_column(false);


	// Create controls to specify right arm
	glui_panel = glui_joints->add_panel("Right arm");
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "arm scale:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_ARM_SCALE));
	glui_spinner->set_float_limits(ARM_SCALE_MIN, ARM_SCALE_MAX, GLUI_LIMIT_CLAMP);
    glui_spinner->set_float_val(1.0);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "shoulder pitch:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_SHOULDER_PITCH));
	glui_spinner->set_float_limits(SHOULDER_PITCH_MIN, SHOULDER_PITCH_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "shoulder yaw:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_SHOULDER_YAW));
	glui_spinner->set_float_limits(SHOULDER_YAW_MIN, SHOULDER_YAW_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "shoulder roll:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_SHOULDER_ROLL));
	glui_spinner->set_float_limits(SHOULDER_ROLL_MIN, SHOULDER_ROLL_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "elbow:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_ELBOW));
	glui_spinner->set_float_limits(ELBOW_MIN, ELBOW_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	// Create controls to specify left arm
	glui_panel = glui_joints->add_panel("Left arm");
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "arm scale:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_ARM_SCALE));
	glui_spinner->set_float_limits(ARM_SCALE_MIN, ARM_SCALE_MAX, GLUI_LIMIT_CLAMP);
    glui_spinner->set_float_val(1.0);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "shoulder pitch:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_SHOULDER_PITCH));
	glui_spinner->set_float_limits(SHOULDER_PITCH_MIN, SHOULDER_PITCH_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "shoulder yaw:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_SHOULDER_YAW));
	glui_spinner->set_float_limits(SHOULDER_YAW_MIN, SHOULDER_YAW_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "shoulder roll:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_SHOULDER_ROLL));
	glui_spinner->set_float_limits(SHOULDER_ROLL_MIN, SHOULDER_ROLL_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "elbow:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_ELBOW));
	glui_spinner->set_float_limits(ELBOW_MIN, ELBOW_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);


	glui_joints->add_column(false);


	// Create controls to specify right leg
	glui_panel = glui_joints->add_panel("Right leg");

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "hip pitch:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_HIP_PITCH));
	glui_spinner->set_float_limits(HIP_PITCH_MIN, HIP_PITCH_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "hip yaw:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_HIP_YAW));
	glui_spinner->set_float_limits(HIP_YAW_MIN, HIP_YAW_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "hip roll:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_HIP_ROLL));
	glui_spinner->set_float_limits(HIP_ROLL_MIN, HIP_ROLL_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "knee:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_KNEE));
	glui_spinner->set_float_limits(KNEE_MIN, KNEE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	// Create controls to specify left leg
	glui_panel = glui_joints->add_panel("Left leg");

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "hip pitch:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_HIP_PITCH));
	glui_spinner->set_float_limits(HIP_PITCH_MIN, HIP_PITCH_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "hip yaw:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_HIP_YAW));
	glui_spinner->set_float_limits(HIP_YAW_MIN, HIP_YAW_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "hip roll:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_HIP_ROLL));
	glui_spinner->set_float_limits(HIP_ROLL_MIN, HIP_ROLL_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "knee:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_KNEE));
	glui_spinner->set_float_limits(KNEE_MIN, KNEE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

    // Create controls to specify lighting
	glui_panel = glui_joints->add_panel("Light Controls");
    
    glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "Pos:", GLUI_SPINNER_FLOAT, &lightTheta);
    glui_spinner->set_float_limits(-180, 180, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);
    glui_joints->add_column_to_panel(glui_panel, false);
    glui_checkbox = glui_joints->add_checkbox_to_panel(glui_panel, "Light", &lightOn);

	// Create GLUI window (keyframe controls) ************
	//
	glui_keyframe = GLUI_Master.create_glui("Keyframe Control", 0, 0, Win[1]+64);

	// Create a control to specify the time (for setting a keyframe)
	glui_panel = glui_keyframe->add_panel("", GLUI_PANEL_NONE);
	glui_spinner = glui_keyframe->add_spinner_to_panel(glui_panel, "Time:", GLUI_SPINNER_FLOAT, joint_ui_data->getTimePtr());
	glui_spinner->set_float_limits(TIME_MIN, TIME_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	// Create a control to specify a keyframe (for updating / loading a keyframe)
	glui_keyframe->add_column_to_panel(glui_panel, false);
	glui_spinner = glui_keyframe->add_spinner_to_panel(glui_panel, "Keyframe ID:", GLUI_SPINNER_INT, joint_ui_data->getIDPtr());
	glui_spinner->set_int_limits(KEYFRAME_MIN, KEYFRAME_MAX-1, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_keyframe->add_separator();

	// Add buttons to load and update keyframes
	// Add buttons to load and save keyframes from a file
	// Add buttons to start / stop animation and to render frames to file
	glui_panel = glui_keyframe->add_panel("", GLUI_PANEL_NONE);
	glui_keyframe->add_button_to_panel(glui_panel, "Load Keyframe", 0, loadKeyframeButton);
	glui_keyframe->add_button_to_panel(glui_panel, "Load Keyframes From File", 0, loadKeyframesFromFileButton);
	glui_keyframe->add_button_to_panel(glui_panel, "Start / Stop Animation", 0, animateButton);
	glui_keyframe->add_column_to_panel(glui_panel, false);
	glui_keyframe->add_button_to_panel(glui_panel, "Update Keyframe", 0, updateKeyframeButton);
	glui_keyframe->add_button_to_panel(glui_panel, "Save Keyframes To File", 0, saveKeyframesToFileButton);
	glui_keyframe->add_button_to_panel(glui_panel, "Render Frames To File", 0, renderFramesToFileButton);

	glui_keyframe->add_separator();

	// Add status line
	glui_panel = glui_keyframe->add_panel("");
	status = glui_keyframe->add_statictext_to_panel(glui_panel, "Status: Ready");

	// Add button to quit
	glui_panel = glui_keyframe->add_panel("", GLUI_PANEL_NONE);
	glui_keyframe->add_button_to_panel(glui_panel, "Quit", 0, quitButton);
	//
	// ***************************************************


	// Create GLUI window (render controls) ************
	//
	glui_render = GLUI_Master.create_glui("Render Control", 0, 367, Win[1]+64);
    
    //Create Material Control
    glui_panel = glui_render->add_panel("Material Style");
	glui_radio_group = glui_render->add_radiogroup_to_panel(glui_panel, &materialStyle);
   	glui_render->add_radiobutton_to_group(glui_radio_group, "None");
    glui_render->add_radiobutton_to_group(glui_radio_group, "Matte");
	glui_render->add_radiobutton_to_group(glui_radio_group, "Mettallic");
    
	// Create control to specify the render style
	glui_panel = glui_render->add_panel("Render Style");
	glui_radio_group = glui_render->add_radiogroup_to_panel(glui_panel, &renderStyle);
	glui_render->add_radiobutton_to_group(glui_radio_group, "Wireframe");
	glui_render->add_radiobutton_to_group(glui_radio_group, "Solid");
	glui_render->add_radiobutton_to_group(glui_radio_group, "Solid w/ outlines");
	//
	// ***************************************************


	// Tell GLUI windows which window is main graphics window
	glui_joints->set_main_gfx_window(windowID);
	glui_keyframe->set_main_gfx_window(windowID);
	glui_render->set_main_gfx_window(windowID);
}


// Performs most of the OpenGL intialization
void initGl(void)
{
    // glClearColor (red, green, blue, alpha)
    // Ignore the meaning of the 'alpha' value for now
    glClearColor(0.7f,0.7f,0.9f,1.0f);
}


// Calculates the interpolated joint DOF vector
// using Catmull-Rom interpolation of the keyframes
Vector getInterpolatedJointDOFS(float time)
{
	// Need to find the keyframes bewteen which
	// the supplied time lies.
	// At the end of the loop we have:
	//    keyframes[i-1].getTime() < time <= keyframes[i].getTime()
	//
	int i = 0;
	while( i <= maxValidKeyframe && keyframes[i].getTime() < time )
		i++;

	// If time is before or at first defined keyframe, then
	// just use first keyframe pose
	if( i == 0 )
		return keyframes[0].getDOFVector();

	// If time is beyond last defined keyframe, then just
	// use last keyframe pose
	if( i > maxValidKeyframe )
		return keyframes[maxValidKeyframe].getDOFVector();

	// Need to normalize time to (0, 1]
	time = (time - keyframes[i-1].getTime()) / (keyframes[i].getTime() - keyframes[i-1].getTime());

	// Get appropriate data points and tangent vectors
	// for computing the interpolation
	Vector p0 = keyframes[i-1].getDOFVector();
	Vector p1 = keyframes[i].getDOFVector();
	
	// Linear interpolation.
	return p0 + (p1-p0)*time;
}


// Callback idle function for animating the scene
void animate()
{
	// Only update if enough time has passed
	// (This locks the display to a certain frame rate rather
	//  than updating as fast as possible. The effect is that
	//  the animation should run at about the same rate
	//  whether being run on a fast machine or slow machine)
	if( frameRateTimer->elapsed() > SEC_PER_FRAME )
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
//
// README: This gets called by the event handler
// to draw the scene, so this is where you need
// to build your scene -- make your changes and
// additions here. All rendering happens in this
// function. For Assignment 2, updates to the
// joint DOFs (joint_ui_data) happen in the
// animate() function.
void display(void)
{
    // Clear the screen with the background colour
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    // Setup the model-view transformation matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	// Specify camera transformation
	glTranslatef(camXPos, camYPos, camZPos);


	// Get the time for the current animation step, if necessary
	if( animate_mode )
	{
		float curTime = animationTimer->elapsed();

		if( curTime >= keyframes[maxValidKeyframe].getTime() )
		{
			// Restart the animation
			animationTimer->reset();
			curTime = animationTimer->elapsed();
		}
		// Get the interpolated joint DOFs
		joint_ui_data->setDOFVector( getInterpolatedJointDOFS(curTime) );

		// Update user interface
		joint_ui_data->setTime(curTime);
		glui_keyframe->sync_live();
	}

    //Set render style
    glDepthFunc(GL_LESS);
    if (renderStyle == WIREFRAME){
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
    } else if (renderStyle == SOLID){
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
    } else if (renderStyle == OUTLINED) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glPolygonOffset(1.0,1.0);
    }
    
    //If the light is on, set its location based on camera and enable lighting,
    //depth tests and vector normalization.  Otherwise disable lighting and normalization
    if(lightOn && materialStyle != NONE){
        glClearColor (0.2, 0.2, 0.2, 1.0);
        glShadeModel (GL_SMOOTH);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_NORMALIZE);
        glEnable(GL_LIGHTING);
        
        GLfloat diffuseRGBA[] = {2.8f, 2.8f, 2.8f, 2.8f};
        GLfloat specularRGBA[] = {0.5f, 0.5f, 0.5f, 1.0f};
        GLfloat ambiant[] = {1.0f, 1.0f, 1.0f, 1.0f};
        
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
    } else {
        glClearColor (0.8, 0.8, 0.8, 1.0);
        glEnable(GL_NORMALIZE);
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
    }

    //Set material style.  matte material is less shiny and reflects less
    //light of all types
    
    //Default settings for none style
    GLfloat mat_ambient[] = { 0.1, 0.1, 0.1, 1};
    GLfloat mat_diffuse[] = { 0.1, 0.1, 0.1, 1 };
    GLfloat mat_specular[] = { 0.1, 0.1, 0.1, 1};
    float shine = 0.2; 
    if (materialStyle == MATTE){
        mat_ambient[0] = 0.05;
        mat_ambient[1] = 0.05;
        mat_ambient[2] = 0.05;
        
        mat_diffuse[0] = 0.1;
        mat_diffuse[1] = 0.1;
        mat_diffuse[2] = 0.1;
        
        mat_specular[0] = 0.04;
        mat_specular[1] = 0.04;
        mat_specular[2] = 0.04;
        
        shine = 0.1;
    } else if (materialStyle == METALLIZED){
        mat_ambient[0] = 0.5;
        mat_ambient[1] = 0.5;
        mat_ambient[2] = 0.5;
        
        mat_diffuse[0] = 0.3;
        mat_diffuse[1] = 0.3;
        mat_diffuse[2] = 0.3;
        
        mat_specular[0] = 0.9;
        mat_specular[1] = 0.9;
        mat_specular[2] = 0.9;
        
        shine = 0.7;
    }
    
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT,GL_SHININESS,shine*128.0);
    
	glColor3f(0.5, 1.0, 1.0);
    int ds = (renderStyle == OUTLINED);
	glPushMatrix();
        // Start Body
		glTranslatef(joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_X),
					 joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Y),
					 joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Z));
		glRotatef(joint_ui_data->getDOF(Keyframe::ROOT_ROTATE_X), 1.0, 0.0, 0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::ROOT_ROTATE_Y), 0.0, 1.0, 0.0);
        glRotatef(joint_ui_data->getDOF(Keyframe::ROOT_ROTATE_Z), 0.0, 0.0, 1.0);
        
        glPushMatrix();
            // Start Head
            glTranslatef(0.0, Penguin::BODY_HEIGHT, 0.0);
            glRotatef(joint_ui_data->getDOF(Keyframe::HEAD), 0.0, 1.0, 0.0);
            glPushMatrix();
                //Start Upper Beak
                glTranslatef(0.0,
                             Penguin::HEAD_HEIGHT,
                             (Penguin::HEAD_LOWER_DEPTH+Penguin::HEAD_UPPER_DEPTH)/2);
                glTranslatef(0.0,0.0,Penguin::LIP_LENGTH/2);
                glScalef(1,0.5,1);
                glPushMatrix();
                    //Start Lower Beak
                    glTranslatef(0.0,
                                -Penguin::LIP_RADIUS-joint_ui_data->getDOF(Keyframe::BEAK),
                                0.0);
                    glRotatef(180, 0.0, 0.0, 1.0);
                    draw(Penguin::drawLip, ds); 
                    //End Lower Beak
                glPopMatrix();
                draw(Penguin::drawLip, ds);
                //End Upper Beak
            glPopMatrix();
            glTranslatef(0.0, Penguin::HEAD_HEIGHT, 0.0);
            draw(Penguin::drawHead, ds);
            //End Head
        glPopMatrix();
 
        glPushMatrix();
            //Start Right Arm (Stage Left)
            glTranslatef(Penguin::BODY_WIDTH + Penguin::ARM_WIDTH, 0.0, 0.0);
            glTranslatef(0.0, Penguin::ARM_HEIGHT-0.2, 0.0);
            glRotatef(joint_ui_data->getDOF(Keyframe::R_SHOULDER_YAW), 1.0, 0.0, 0.0);
            glRotatef(joint_ui_data->getDOF(Keyframe::R_SHOULDER_ROLL), 0.0, 1.0, 0.0);
            glRotatef(joint_ui_data->getDOF(Keyframe::R_SHOULDER_PITCH), 0.0, 0.0, 1.0);
            glScalef(joint_ui_data->getDOF(Keyframe::R_ARM_SCALE),
                     joint_ui_data->getDOF(Keyframe::R_ARM_SCALE),
                     joint_ui_data->getDOF(Keyframe::R_ARM_SCALE));
            glPushMatrix();
                //Start Right Hand
                glTranslatef(0.0, -Penguin::ARM_HEIGHT-2*Penguin::HAND_Y_SCALE-0.2, 0.0);
                glRotatef(joint_ui_data->getDOF(Keyframe::R_ELBOW), 0.0, 0.0, 1.0);
                glTranslatef(0.0, -Penguin::HAND_Y_SCALE+0.1, 0.0);
                glScalef(Penguin::HAND_X_SCALE,
                         Penguin::HAND_Y_SCALE, 
                         Penguin::HAND_Z_SCALE); 
                draw(Penguin::drawHand, ds);
                //End Right Hand
            glPopMatrix();
            glTranslatef(0.0, -Penguin::ARM_HEIGHT+0.2, 0.0);
            draw(Penguin::drawArm, ds);
            //End Right Arm
        glPopMatrix();
        
        glPushMatrix();
            //Start Left Arm (Stage Right)
            glTranslatef(-(Penguin::BODY_WIDTH + Penguin::ARM_WIDTH), 0.0, 0.0);
            glTranslatef(0.0, Penguin::ARM_HEIGHT-0.2, 0.0);
            glRotatef(joint_ui_data->getDOF(Keyframe::L_SHOULDER_YAW), 1.0, 0.0, 0.0);
            glRotatef(joint_ui_data->getDOF(Keyframe::L_SHOULDER_ROLL), 0.0, 1.0, 0.0);
            glRotatef(joint_ui_data->getDOF(Keyframe::L_SHOULDER_PITCH), 0.0, 0.0, 1.0);
            glScalef(joint_ui_data->getDOF(Keyframe::L_ARM_SCALE),
                     joint_ui_data->getDOF(Keyframe::L_ARM_SCALE),
                     joint_ui_data->getDOF(Keyframe::L_ARM_SCALE));
            glPushMatrix();
                //Start Left Hand
                glTranslatef(0.0, -Penguin::ARM_HEIGHT-2*Penguin::HAND_Y_SCALE-0.2, 0.0);
                glRotatef(-joint_ui_data->getDOF(Keyframe::L_ELBOW), 0.0, 0.0, 1.0);
                glTranslatef(0.0, -Penguin::HAND_Y_SCALE+0.1, 0.0);
                glScalef(Penguin::HAND_X_SCALE,
                         Penguin::HAND_Y_SCALE, 
                         Penguin::HAND_Z_SCALE); 
                draw(Penguin::drawHand, ds);
                //End Left Hand
            glPopMatrix();
            glTranslatef(0.0, -Penguin::ARM_HEIGHT+0.2, 0.0);
            draw(Penguin::drawArm, ds);
            //End Left Arm
        glPopMatrix();
        
        glPushMatrix();
            //Start Right Leg (Stage Left)
            glTranslatef(Penguin::LEG_X_SHIFT,
                         -Penguin::BODY_HEIGHT-0.1,
                         0.0);
            glRotatef(joint_ui_data->getDOF(Keyframe::R_HIP_YAW), 1.0, 0.0, 0.0);
            glRotatef(joint_ui_data->getDOF(Keyframe::R_HIP_ROLL), 0.0, 1.0, 0.0);
            glRotatef(joint_ui_data->getDOF(Keyframe::R_HIP_PITCH), 0.0, 0.0, 1.0);
            
            glPushMatrix();
                //Start Right Foot
                glTranslatef(0.0, -2*Penguin::LEG_Y_SCALE+Penguin::FOOT_Y_SCALE, 0.0);
                glRotatef(joint_ui_data->getDOF(Keyframe::R_KNEE), 1.0, 0.0, 0.0);
                glTranslatef(0.0, 0.0, Penguin::FOOT_Z_SCALE);
                glScalef(Penguin::FOOT_X_SCALE,
                         Penguin::FOOT_Y_SCALE, 
                         Penguin::FOOT_Z_SCALE); 
                draw(Penguin::drawFoot, ds);
                //End Right Foot
            glPopMatrix();
            
            glTranslatef(0.0, -Penguin::LEG_Y_SCALE+0.1, 0.0);
            glScalef(Penguin::LEG_X_SCALE,
                     Penguin::LEG_Y_SCALE, 
                     Penguin::LEG_Z_SCALE); 
            draw(Penguin::drawLeg, ds);
            //End Right Leg
        glPopMatrix();
        
        glPushMatrix();
            //Start Left Leg (Stage Right)
            glTranslatef(-Penguin::LEG_X_SHIFT,
                         -Penguin::BODY_HEIGHT-0.1,
                         0.0);
            glRotatef(joint_ui_data->getDOF(Keyframe::L_HIP_YAW), 1.0, 0.0, 0.0);
            glRotatef(joint_ui_data->getDOF(Keyframe::L_HIP_ROLL), 0.0, 1.0, 0.0);
            glRotatef(joint_ui_data->getDOF(Keyframe::L_HIP_PITCH), 0.0, 0.0, 1.0);
            
            glPushMatrix();
                //Start Left Foot
                glTranslatef(0.0, -2*Penguin::LEG_Y_SCALE+Penguin::FOOT_Y_SCALE, 0.0);
                glRotatef(joint_ui_data->getDOF(Keyframe::L_KNEE), 1.0, 0.0, 0.0);
                glTranslatef(0.0, 0.0, Penguin::FOOT_Z_SCALE);
                glScalef(Penguin::FOOT_X_SCALE,
                         Penguin::FOOT_Y_SCALE, 
                         Penguin::FOOT_Z_SCALE); 
                draw(Penguin::drawFoot, ds);
                //End Left Foot
            glPopMatrix();
            
            glTranslatef(0.0, -Penguin::LEG_Y_SCALE+0.1, 0.0);
            glScalef(Penguin::LEG_X_SCALE,
                     Penguin::LEG_Y_SCALE, 
                     Penguin::LEG_Z_SCALE); 
            draw(Penguin::drawLeg, ds);
            //End Left Leg
        glPopMatrix();
        
        draw(Penguin::drawBody, ds);
        //End Body
	glPopMatrix();

    // Execute any GL functions that are in the queue just to be safe
    glFlush();

	// Dump frame to file, if requested
	if( frameToFile )
	{
		sprintf(filenameF, "frame%03d.ppm", frameNumber);
		writeFrame(filenameF, false, false);
	}

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

void writeFrame(char* filename, bool pgm, bool frontBuffer)
{
	static GLubyte* frameData = NULL;
	static int currentSize = -1;

	int size = (pgm ? 1 : 4);

	if( frameData == NULL || currentSize != size*Win[0]*Win[1] )
	{
		if (frameData != NULL)
			delete [] frameData;

		currentSize = size*Win[0]*Win[1];

		frameData = new GLubyte[currentSize];
	}

	glReadBuffer(frontBuffer ? GL_FRONT : GL_BACK);

	if( pgm )
	{
		glReadPixels(0, 0, Win[0], Win[1],
					 GL_LUMINANCE, GL_UNSIGNED_BYTE, frameData);
		writePGM(filename, frameData, Win[0], Win[1]);
	}
	else
	{
		glReadPixels(0, 0, Win[0], Win[1],
					 GL_RGBA, GL_UNSIGNED_BYTE, frameData);
		writePPM(filename, frameData, Win[0], Win[1]);
	}
}