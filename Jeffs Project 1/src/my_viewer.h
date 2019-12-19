# pragma once
//#pragma comment(lib, "winmm.lib")

# include <sig/sn_poly_editor.h>
# include <sig/sn_lines2.h>

/*
	This is for playing a sound that I plan to add but decided to no go with it
	because it will only works on windows platform.
*/
//#include <windows.h>
//#include <iostream>


# include <sigogl/ui_button.h>
# include <sigogl/ws_viewer.h>
# include <sig/sn_model.h>


/*
	Autodesk: NETFABB
	https://sourcegraphics.com/3d/software/netfabb/autodesk-netfabb?gclid=Cj0KCQjwjOrtBRCcARIsAEq4rW4vixK-avIY4a0EGICN4iJsI5ntr8srkRp8QZhAQ85HDNm3v3UrTAEaApWaEALw_wcB

	Plane
	- Will drop bombs
	- Old Style WWII Plane
*/

// Define a max allowed angle
#define MAX_ANGLE_ALLOWED			10
// Viewer for this example:
class MyViewer : public WsViewer
{  protected :
	enum MenuEv { EvNormals, EvAnimate, EvExit };
	UiCheckButton* _nbut;
	bool _animating;

	/*
		Plane Model
	*/
	SnModel* plane;
	SnGroup* planeBody,* flapsGroup;
	SnGroup* propGroup;

	/*
		Propeller model
	*/
	SnModel* propeller;
	SnTransform* prop_trans;

	// Whole Scene Transform
	SnTransform* sceneTransform;

	/*
		Flaps Model
	*/
	SnModel* flaps[2];
	SnTransform* flapsTransform[2];

	/*
		Elevators Model
	*/
	SnGroup* elvtorGroup;
	SnModel* elevators[2];
	SnTransform* e_transform[2];

	/*
		Plane Rudders
	*/
	SnGroup* rudderGroup;
	SnModel* rudder;
	SnTransform* r_trans;

	/*
		The Sun
	*/
	SnGroup* sunG;
	SnModel* sun;
	SnTransform* sunTran;
	
	/*
		Temporary models/group for getting points for my GsQuat
	*/
	SnGroup* points;
	SnModel* point;

	/*
		Material models
	*/
	GsModel::Group* g; // Propeller texture
	GsModel::Group* g2; // Body texture

	// Limiters for the transforms
	int currentang = 0;
	int vertAng = 0;
	int flapangle = 0;
	int eleangle = 0;
	int r_angle = 0;
	bool engineRunning;

	/*
		Floor model group variables
	*/
	SnGroup* gFloor;
	SnModel* floor;
	SnTransform* f_trans;
	/*
		Shadow model group
			- 2 models are copied
			- Rest of the models are shared
	*/
	SnModel* planeModel[2];
	SnGroup* planeShadowGroup[2];
	SnTransform* propShadow;
	SnGroup* shadows;
	SnTransform* shadow_trans;

	// Light source for the shadow.
	GsVec p = GsVec(0.0f, 15.5f, 5.0f);

	// Shadow position no longer used
	//GsVec shadowPos = GsVec(0.0f, -8.9f, 0.0f);

	// Projection shadow matrix, position is set to -8.9f to make it touch the floor
	GsMat shadowMat = GsMat(
		1.0f,	-p.x/p.y,	0.0f,	0.0f,
		0.0f,		0.0f,	0.0f,	-8.9f,
		0.0f,	-p.z/p.y,	1.0f,	0.0f,
		0.0f,		0.0f,	0.0f,	1.0f
	);

	/*
		Camera 
	*/
	bool moveCam; // Variable that will tell the program to move the cam
	bool followPlane;	// Variable for following the camera
	int camMode;		// Camera modes
	/*
		Movements
	*/
	bool forward;
	// The following variables will be added but decided not to due to
	// complexity and lack of time. (Will be added in the future)
	bool backward;
	bool left;
	bool right;
	float vertical;
	// Matrices that will save the current rotation of the plane.
	GsMat rotX, rotY, rotZ;

	// This is current position of the plane, useful when I am 
	// updating my shadow and making the camera follow the plane.
	GsVec currPos = GsVec(0.0f, 0.0f, 0.0f);

   public :
	MyViewer ( int x, int y, int w, int h, const char* l );
	void build_ui ();
	// This function updates the shadow_transformation matrix;
	void updateShadows(GsVec shadowPos, GsMat trans, GsMat shadowMat, GsMat rot);
	void add_model ( SnShape* s, GsVec p );
	void build_scene ();
	void show_normals ( bool view );
	void run_animation ();
	virtual int handle_keyboard ( const GsEvent &e ) override;
	virtual int uievent ( int e ) override;
};

