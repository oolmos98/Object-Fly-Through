#pragma once
#include <sig/gs.h>
# include <sig/sn_model.h>
#include <sig/sn_group.h>
#include <sig/sn_transform.h>
# include <sigogl/ws_viewer.h>
#define MAX_ANGLE_ALLOWED			10

class Plane
{
protected:
	SnGroup* planeClass;

	/*
		Plane Model
	*/
	SnModel* plane;
	SnGroup* planeBody, * flapsGroup;
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
		1.0f, -p.x / p.y, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, -8.9f,
		0.0f, -p.z / p.y, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
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

	float currentScale;

	bool _animating;

	float lastY = 0.0f;

public:
	Plane();
	Plane(float model_scale);
	~Plane();

	SnGroup* model() { return planeClass; }
	void import_model();
	void set_position(GsVec pos);
	void setrotX(float X);
	void setrotY(float Y);
	void setrotZ(float Z);

	void setScaling(float sca);
	void run_animation(float time);
	void updateShadows(GsVec shadowPos, GsMat trans, GsMat shadowMat, GsMat rot);
	int handle_keyboard(const GsEvent& e);
};

