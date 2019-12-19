#pragma once
#include <sig/gs.h>
# include <sig/sn_model.h>
#include <sig/sn_group.h>
#include <sig/sn_transform.h>
# include <sigogl/ws_viewer.h>
#define MAX_ANGLE_ALLOWED 10

class Cube
{
protected:
	SnGroup* cube;
	/*
		Plane Model
	*/
	SnGroup* face;
	SnModel* carr[6];
	SnModel* carr2[6];

	// Whole Scene Transform
	SnTransform* sceneTransform;

	

	// Light source for the shadow.
	GsVec p = GsVec(0.0f, 15.5f, 5.0f);


	//// Projection shadow matrix, position is set to -8.9f to make it touch the floor
	//GsMat shadowMat = GsMat(
	//	1.0f, -p.x / p.y, 0.0f, 0.0f,
	//	0.0f, 0.0f, 0.0f, -8.9f,
	//	0.0f, -p.z / p.y, 1.0f, 0.0f,
	//	0.0f, 0.0f, 0.0f, 1.0f
	//);

	/*
		Camera
	*/
	
	/*
		Movements
	*/
	// The following variables will be added but decided not to due to
	// complexity and lack of time. (Will be added in the future)
	
	// Matrices that will save the current rotation of the plane.
	GsMat rotX, rotY, rotZ;

	// This is current position of the plane, useful when I am 
	// updating my shadow and making the camera follow the plane.
	GsVec currPos = GsVec(0.0f, 0.0f, 0.0f);

	float currentScale;

	bool _animating;
public:

	Cube();
	Cube(float model_scale);
	~Cube();

	SnGroup* model() { return cube; }
	void import_model();
	void set_position(GsVec pos);
	void setrotX(float X);
	void setrotY(float Y);
	void setrotZ(float Z);

	void setScaling(float sca);
	void run_animation(float time);
	int handle_keyboard(const GsEvent& e);
};

