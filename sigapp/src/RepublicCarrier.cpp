#include "RepublicCarrier.h"
#include <sig/sn_material.h>
#include <sig/sn_transform.h>
#include <thread>

// Default Constructor
RepublicCarrier::RepublicCarrier() {
	planeClass = new SnGroup;
	currentScale = 1.0f;
	_animating = false;
	import_model();
}


// Overloaded constructor
RepublicCarrier::RepublicCarrier(float model_scale) {
	planeClass = new SnGroup;
	currentScale = model_scale;
	_animating = false;
	import_model();
}

RepublicCarrier::~RepublicCarrier() {
	planeClass->remove_all();
}

void RepublicCarrier::import_model() {

	carr = new SnModel;
	body = new SnGroup;
	sceneTransform = new SnTransform;
	//flapsGroup = new SnGroup;

	if (carr->model()->load_obj("../model/venator2.obj"))
		body->add(carr);

	else gsout << "error" << gsnl;
	//carr->model()->smooth(true);
	//body->separator(true);
	carr->color(GsColor(74,74,74));
	carr->model()->flat(true);
	carr->model()->rotate(GsQuat(GsVec::j, gspi));
	//carr->model()->scale(1.3f);
	GsMat rot, trans;
	rot.rot(GsVec::j, gs2pi);
	trans.translation(GsVec(0.0f, 5.0f, 0.0f));
	sceneTransform->get() = trans * rot * trans.inverse();

	
	///*
	//	Add the sceneTransform
	//*/
	planeClass->add(sceneTransform);
	planeClass->add_group(body, true);
	


	GsMat scaling, position;
	position.translation(currPos);
	scaling.scaling(currentScale);
	sceneTransform->get() = position * rotX * rotY * rotZ * position.inverse() * position * scaling;
}

void RepublicCarrier::run_animation(float time) {

		GsVec origin = GsVec(0.0f, 1.0f, 1.0f);
		GsMat translations;
		GsMat ptrans, prot;
		prot.rotz(gs2pi / 10.0f*time);
		ptrans.translation(GsVec(0.0f, 0.85f, -7.5f));
		
}


int RepublicCarrier::handle_keyboard(const GsEvent& e) {
	GsMat trans, scaling;
	scaling.scaling(currentScale);
	switch (e.key)
	{
	case GsEvent::KeyEsc: gs_exit(); return 1;
	default:
	{
		return 0;
	}
	/*
		Doing rotX * rotY * rotZ will save the model's x, y, z rotation.
	*/
	// Tilt the carr to left
	case GsEvent::KeyLeft:
	case 'a': {

		sceneTransform->get() = trans * rotX * rotY * rotZ * trans.inverse() * trans * scaling;

		break;
	}
			// Tilt the carr to right
	case GsEvent::KeyRight:
	case 'd': {

		sceneTransform->get() = trans * rotX * rotY * rotZ * trans.inverse() * trans * scaling;

		break;
	}
			// Tilt the carr backward
	case GsEvent::KeyDown: {

		sceneTransform->get() = trans * rotX * rotY * rotZ * trans.inverse() * trans * scaling;


		break;
	}
						 // Tilt the carr forward
	case  GsEvent::KeyUp: {

		sceneTransform->get() = trans * rotX * rotY * rotZ * trans.inverse() * trans * scaling;

		break;
	}
						// Rotate the carr towards the right
	case 'e': {

		sceneTransform->get() = trans * rotX * rotY * rotZ * trans.inverse() * trans * scaling;


		break;
	}
			// Rotate the carr towards the left
	case 'q': {

		sceneTransform->get() = trans * rotX * rotY * rotZ * trans.inverse() * trans * scaling;

		break;
	}
			// Move the carr forward
	}
	return 1;
}

//void RepublicCarrier::updateShadows(GsVec pos, GsMat trans, GsMat shadowMat, GsMat rot) {
//	GsMat oPos, scaling;
//	scaling.scaling(currentScale);
//	oPos.translation(pos);
//	/*
//		Idea,
//		translate, rotate, apply projection matrix, translate
//	*/
//	shadow_trans->get().set(oPos * shadowMat * trans * rot * trans.inverse() * oPos * scaling);
//}

void RepublicCarrier::set_position(GsVec pos) {
	GsMat position, scaling;
	currPos = pos;
	scaling.scaling(currentScale);
	position.translation(currPos);
	sceneTransform->get() = position * rotX * rotY * rotZ * position.inverse() * position * scaling;
}

void RepublicCarrier::setScaling(float sca) {
	GsMat scaling, position;
	position.translation(currPos);
	currentScale = sca;
	scaling.scaling(currentScale);
	sceneTransform->get() = position * rotX * rotY * rotZ * position.inverse() * position * scaling;
}

void RepublicCarrier::setrotX(float X) {
	GsMat scaling, position;
	position.translation(currPos);
	scaling.scaling(currentScale);
	rotX.rotx(X);
	sceneTransform->get() = position * rotX * rotY * rotZ * position.inverse() * position * scaling;
}

void RepublicCarrier::setrotY(float Y) {
	GsMat scaling, position;
	position.translation(currPos);
	scaling.scaling(currentScale);
	rotY.roty(Y);
	sceneTransform->get() = position * rotX * rotY * rotZ * position.inverse() * position * scaling;
}

void RepublicCarrier::setrotZ(float Z) {
	GsMat scaling, position;
	position.translation(currPos);
	scaling.scaling(currentScale);
	rotZ.rotz(Z);
	sceneTransform->get() = position * rotX * rotY * rotZ * position.inverse() * position * scaling;
}