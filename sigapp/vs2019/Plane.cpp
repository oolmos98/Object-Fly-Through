#include "Plane.h"
#include <sig/sn_material.h>
#include <sig/sn_transform.h>

Plane::Plane() {
	planeClass = new SnGroup;
	import_model();
}


Plane::~Plane() {

}

void Plane::import_model() {

	planeClass->add_group(gFloor, true);
	planeClass->add_group(shadows, true);

	/*
		Add the sceneTransform
	*/
	planeClass->add(sceneTransform);
	planeClass->add_group(planeBody, true);
	planeClass->add_group(propGroup, true);
	planeClass->add_group(flapsGroup, true);
	planeClass->add_group(elvtorGroup, true);
	planeClass->add_group(rudderGroup, true);
}

void Plane::run_animation() {

}

void Plane::updateShadows(GsVec shadowPos, GsMat trans, GsMat shadowMat, GsMat rot) {

}