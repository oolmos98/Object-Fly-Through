#pragma once
#include <sig/gs.h>
# include <sig/sn_model.h>
#include <sig/sn_group.h>
#include <sig/sn_transform.h>
# include <sigogl/ws_viewer.h>
#define MAX_ANGLE_ALLOWED			10

class Rocket
{
protected:
	SnGroup* rocketClass;
	
	// Rocket Model
	SnModel* rocket;
	SnTransform* trocket;

	// Material
	GsModel::Group* g; //rocket texture

	// Camera
	bool moveCam; // Variable that will tell the program to move the cam
	bool followPlane;	// Variable for following the camera
	int camMode;		// Camera modes

	// Movement
	GsMat rotX, rotY, rotZ; //mat save current rot of rocket

	GsVec currPos = GsVec(0.0f, 0.0f, 0.0f); //cam follow rocket

	float currentScale;

	bool _animating;

public:
	Rocket();
	Rocket(float model_scale);
	~Rocket();

	SnGroup* model() { return rocketClass; }
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