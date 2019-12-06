#include "Plane.h"
#include <sig/sn_material.h>
#include <sig/sn_transform.h>
#include <thread>

// Default Constructor
Plane::Plane() {
	planeClass = new SnGroup;
	currentScale = 1.0f;
	_animating = false;
	import_model();
}


// Overloaded constructor
Plane::Plane(float model_scale) {
	planeClass = new SnGroup;
	currentScale = model_scale;
	_animating = false;
	import_model();
}

Plane::~Plane() {
	planeClass->remove_all();
}

void Plane::import_model() {

	plane = new SnModel();
	planeBody = new SnGroup;
	sceneTransform = new SnTransform;
	flapsGroup = new SnGroup;
	engineRunning = false;

	plane->model()->load_obj("../model/jeff_models/plane.obj");
	plane->model()->smooth(true);
	planeBody->add(plane);
	plane->color(GsColor::darkblue);

	plane->model()->scale(0.25f);
	GsMat rot, trans;
	rot.rot(GsVec::j, gs2pi);
	trans.translation(GsVec(0.0f, 5.0f, 0.0f));
	sceneTransform->get() = trans * rot * trans.inverse();

	// Add a texture for the plane model
	GsModel& material = *plane->model();
	g2 = material.G.push();
	g2->fi = 0;
	g2->fn = material.F.size();
	g2->dmap = new GsModel::Texture;
	g2->dmap->fname.set("../textures/skintexture.png");

	material.M.push().init();
	material.M.top() = GsMaterial::DefaultDiffuse;
	material.set_mode(GsModel::Hybrid, GsModel::PerGroupMtl);
	material.textured = true;

	/*
		Plane Model Shadow Group
		- Create a copy of the 2 models that was textured which prevent the 'flickering'
		of both of the models when it is being updated in the scene
	*/
	planeShadowGroup[0] = new SnGroup;
	planeShadowGroup[0]->separator(true);
	planeModel[0] = new SnModel;
	planeModel[0]->model()->load_obj("../model/jeff_models/plane.obj");
	planeModel[0]->model()->scale(0.25f);
	planeModel[0]->model()->smooth(true);
	planeShadowGroup[0]->add(planeModel[0]);
	planeModel[0]->color(GsColor::black);

	GsMat ptrans, prot;
	prot.rotx(gspi / 2);
	ptrans.translation(GsVec(0.0f, 0.0f, 0.0f));
	planeShadowGroup[1] = new SnGroup;
	planeShadowGroup[1]->separator(true);
	propShadow = new SnTransform;
	planeModel[1] = new SnModel;
	planeModel[1]->model()->load_obj("../model/jeff_models/propeller.obj");
	planeModel[1]->model()->smooth(true);
	planeModel[1]->model()->scale(0.015f);
	planeShadowGroup[1]->add(propShadow);
	planeShadowGroup[1]->add(planeModel[1]);
	planeModel[1]->color(GsColor::black);

	planeModel[1]->model()->transform(ptrans * prot * ptrans.inverse());

	propShadow->get().translation(GsVec(0.0f, 0.85f, -7.5f));

	/*
		Propeller Model Group
	*/
	prot.rotx(gspi / 2);
	ptrans.translation(GsVec(0.0f, 0.0f, 0.0f));
	propGroup = new SnGroup;
	propeller = new SnModel;
	prop_trans = new SnTransform;
	propGroup->separator(true);
	propeller->model()->load_obj("../model/jeff_models/propeller.obj");
	propeller->model()->scale(0.015f);
	propeller->model()->smooth(true);

	propeller->model()->transform(ptrans * prot * ptrans.inverse());
	propGroup->add(prop_trans);
	propGroup->add(propeller);

	prop_trans->get().translation(GsVec(0.0f, 0.85f, -7.5f));

	GsModel& material1 = *propeller->model();
	g = material1.G.push();
	g->fi = 1;
	g->fn = material1.F.size();
	g->dmap = new GsModel::Texture;
	g->dmap->fname.set("../textures/metal.jpg");

	material1.M.push().init();
	material1.M.top() = GsMaterial::DefaultDiffuse;
	material1.set_mode(GsModel::Hybrid, GsModel::PerGroupMtl);
	material1.textured = true;

	/*
		Flaps Model Group
	*/
	flaps[0] = new SnModel;
	flapsTransform[0] = new SnTransform;
	flaps[0]->model()->make_box(GsBox());
	GsMat sca;
	sca.scaling(6.3f, -0.2f, 1.0f);
	trans.translation(GsVec(1.8f, 0.65f, 0.7f));
	rot.roty(gspi / 37);
	flaps[0]->model()->transform(trans * rot * trans.inverse() * sca);
	flaps[0]->color(GsColor::lightgray);
	flapsGroup->add(flapsTransform[0]);
	flapsGroup->add(flaps[0]);
	flapsTransform[0]->get().translation(GsVec(1.8f, 0.8f, 0.7f));


	flaps[1] = new SnModel;
	flapsTransform[1] = new SnTransform;
	flaps[1]->model()->make_box(GsBox());
	sca.scaling(-6.3f, 0.2f, 1.0f);
	trans.translation(GsVec(-1.8f, 0.65f, 0.7f));
	rot.roty(-gspi / 37);
	flaps[1]->model()->transform(trans * rot * trans.inverse() * sca);
	flaps[1]->color(GsColor::lightgray);
	flapsGroup->add(flapsTransform[1]);
	flapsGroup->add(flaps[1]);
	flapsTransform[1]->get().translation(GsVec(-3.5f, -0.15f, 0.0f));

	/*
		Elevators Model Group
	*/
	elvtorGroup = new SnGroup;
	e_transform[0] = new SnTransform;
	elvtorGroup->add(e_transform[0]);

	elevators[0] = new SnModel;
	elevators[0]->model()->make_box(GsBox());
	sca.scaling(1.6f, -0.17f, 0.5f);
	trans.translation(GsVec(0.0f, 0.0f, 3.95f));
	rot.roty(-gspi / 18);
	elevators[0]->model()->transform(trans * rot * trans.inverse() * sca);
	elevators[0]->color(GsColor::lightgray);
	elvtorGroup->add(elevators[0]);
	elvtorGroup->separator(true);


	e_transform[1] = new SnTransform;
	elvtorGroup->add(e_transform[1]);
	elevators[1] = new SnModel;
	elevators[1]->model()->make_box(GsBox());
	sca.scaling(1.6f, -0.17f, 0.5f);
	trans.translation(GsVec(0.0f, 0.0f, 0.0f));
	rot.roty(gspi / 18);
	elevators[1]->model()->transform(trans * rot * trans.inverse() * sca);
	elevators[1]->color(GsColor::lightgray);
	elvtorGroup->add(elevators[1]);
	elvtorGroup->separator(true);


	e_transform[0]->get().translation(GsVec(-2.7f, 1.6f, 4.75f));
	e_transform[1]->get().translation(GsVec(3.0f, -0.01f, 0.4f));


	/*
		Rudder Model Group
	*/
	rudderGroup = new SnGroup;
	rudder = new SnModel;
	r_trans = new SnTransform;
	rudder->model()->make_box(GsBox());
	sca.scaling(0.1f, -1.5f, 0.5f);
	rot.rotx(gspi / 90);
	trans.translation(GsVec(0.0f, 0.0f, 0.0f));
	rudder->color(GsColor::lightgray);
	rudder->model()->transform(trans * rot * trans.inverse() * sca);
	rudderGroup->add(r_trans);
	rudderGroup->add(rudder);

	r_trans->get().translation(GsVec(-0.05f, 3.2f, 5.22f));

	/*
		Shadow material (got it from PA2 solution)
	*/
	SnMaterial* sham = new SnMaterial;
	GsMaterial mtl;
	mtl.diffuse = GsColor::black;
	sham->material(mtl, 8);
	sham->restore(true);

	/*
		Initialize the Shadow Group and its transformation
	*/
	shadows = new SnGroup;
	shadows->separator(true);
	shadow_trans = new SnTransform;
	shadows->add(shadow_trans);
	shadows->add(sham);
	shadows->add_group(planeShadowGroup[0], true);
	shadows->add_group(planeShadowGroup[1], true);
	shadows->add_group(flapsGroup, true);
	shadows->add_group(elvtorGroup, true);
	shadows->add_group(rudderGroup, true);
	trans.translation(currPos);
	shadow_trans->get().set(trans * shadowMat);

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


	GsMat scaling, position;
	position.translation(currPos);
	scaling.scaling(currentScale);
	sceneTransform->get() = position * rotX * rotY * rotZ * position.inverse() * position * scaling;
}

void Plane::run_animation(float time) {
	//if (_animating) { // do nothing if animating is not enabled

		GsVec origin = GsVec(0.0f, 1.0f, 1.0f);
		GsMat translations;
		GsMat ptrans, prot;
		prot.rotz(gs2pi*time);
		ptrans.translation(GsVec(0.0f, 0.85f, -7.5f));
		prop_trans->get() = ptrans * prot;
		propShadow->get() = ptrans * prot;
	//}
}


int Plane::handle_keyboard(const GsEvent& e) {
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
		// Tilt the plane to left
		case GsEvent::KeyLeft:
		case 'a': {
			if (flapangle > -(MAX_ANGLE_ALLOWED)) {
				flapangle--;
				// Generate an axis of rotation.
				GsVec A = (GsPnt(1.9f, 0.7f, 0.85f) - GsPnt(8.2f, 0.7f, 0.40f));
				GsQuat rotQA(A, -gs2pi / 120);
				// Rotates the model around the provided axis
				flaps[0]->model()->rotate(rotQA);

				GsVec B = (GsPnt(-1.9f, 0.7f, 0.85f) - GsPnt(-8.2f, 0.7f, 0.40f));
				GsQuat rotQB(B, -gs2pi / 120);
				flaps[1]->model()->rotate(rotQB);
			}
			if (currentang < (MAX_ANGLE_ALLOWED)) {
				currentang++;
				rotZ.rotz(gs2pi / 270 * currentang);
				trans.translation(currPos);
				sceneTransform->get() = trans * rotX * rotY * rotZ * trans.inverse() * trans * scaling;
			}
			break;
		}
				// Tilt the plane to right
		case GsEvent::KeyRight:
		case 'd': {
			if (flapangle < MAX_ANGLE_ALLOWED) {
				flapangle++;
				GsVec A1 = (GsPnt(1.9f, 0.7f, 0.85f) - GsPnt(8.2f, 0.7f, 0.40f));
				GsQuat rotQA1(A1, gs2pi / 120);
				flaps[0]->model()->rotate(rotQA1);
				GsVec B1 = (GsPnt(-1.9f, 0.7f, 0.85f) - GsPnt(-8.2f, 0.7f, 0.40f));
				GsQuat rotQB1(B1, gs2pi / 120);
				flaps[1]->model()->rotate(rotQB1);
			}

			if (currentang > -(MAX_ANGLE_ALLOWED)) {
				currentang--;
				rotZ.rotz(gs2pi / 270 * currentang);
				trans.translation(currPos);
				sceneTransform->get() = trans * rotX * rotY * rotZ * trans.inverse() * trans * scaling;
			}
			break;
		}
				// Tilt the plane backward
		case GsEvent::KeyDown: {
			if (eleangle < MAX_ANGLE_ALLOWED) {
				eleangle++;
				vertAng++;
				GsVec A = GsPnt(-0.25f, 1.5f, 5.25f) - GsPnt(-2.0f, 1.5f, 4.95f);
				GsQuat rotE1(A, -gs2pi / 120);
				elevators[0]->model()->rotate(rotE1);

				GsVec B = GsPnt(0.25f, 1.5f, 5.25f) - GsPnt(2.0f, 1.5f, 4.95f);
				GsQuat rotE2(B, gs2pi / 120);
				elevators[1]->model()->rotate(rotE2);

				rotX.rotx(gs2pi / 270 * vertAng);
				trans.translation(currPos);
				sceneTransform->get() = trans * rotX * rotY * rotZ * trans.inverse() * trans * scaling;
			}

			break;
		}
							 // Tilt the plane forward
		case  GsEvent::KeyUp: {
			if (eleangle > -MAX_ANGLE_ALLOWED) {
				eleangle--;
				vertAng--;
				GsVec A = GsPnt(-0.25f, 1.5f, 5.25f) - GsPnt(-2.0f, 1.5f, 4.95f);
				GsQuat rotE1(A, gs2pi / 120);
				elevators[0]->model()->rotate(rotE1);

				GsVec B = GsPnt(0.25f, 1.5f, 5.25f) - GsPnt(2.0f, 1.5f, 4.95f);
				GsQuat rotE2(B, -gs2pi / 120);
				elevators[1]->model()->rotate(rotE2);

				rotX.rotx(gs2pi / 270 * vertAng);
				trans.translation(currPos);
				sceneTransform->get() = trans * rotX * rotY * rotZ * trans.inverse() * trans * scaling;
			}
			break;
		}
							// Rotate the plane towards the right
		case 'e': {
			if (r_angle > -MAX_ANGLE_ALLOWED) {
				currentang--;
				r_angle--;
				GsVec A = (GsPnt(0.0f, 3.1f, 5.3f) - GsPnt(0.0f, 1.6f, 5.3f));
				GsQuat rotR(A, gs2pi / 120);
				rudder->model()->rotate(rotR);

				rotY.roty(gs2pi / 270 * r_angle);
				trans.translation(currPos);
				sceneTransform->get() = trans * rotX * rotY * rotZ * trans.inverse() * trans * scaling;

			}
			break;
		}
				// Rotate the plane towards the left
		case 'q': {
			if (r_angle < MAX_ANGLE_ALLOWED) {
				r_angle++;
				currentang++;
				GsVec A = (GsPnt(0.0f, 3.1f, 5.3f) - GsPnt(0.0f, 1.6f, 5.3f));
				GsQuat rotR(A, -gs2pi / 120);
				rudder->model()->rotate(rotR);

				rotY.roty(gs2pi / 270 * r_angle);
				trans.translation(currPos);
				sceneTransform->get() = trans * rotX * rotY * rotZ * trans.inverse() * trans * scaling;
			}
			break;
		}
				// Move the plane forward
		case 'l': {
			engineRunning = true;
			/*backward = false;
			forward = true;*/
			_animating = true;
			break;
		}
				// Stop the plane
		case 'o': {
			engineRunning = false;
			/*forward = false;
			backward = true;*/
			_animating = false;
			break;
		}
	}
	return 1;
}

void Plane::updateShadows(GsVec pos, GsMat trans, GsMat shadowMat, GsMat rot) {
	GsMat oPos, scaling;
	scaling.scaling(currentScale);
	oPos.translation(pos);
	/*
		Idea,
		translate, rotate, apply projection matrix, translate
	*/
	shadow_trans->get().set(oPos * shadowMat * trans * rot * trans.inverse() * oPos * scaling);
}

void Plane::set_position(GsVec pos) {
	GsMat position, scaling;
	currPos = pos;
	scaling.scaling(currentScale);
	position.translation(currPos);
	sceneTransform->get() = position * rotX * rotY * rotZ * position.inverse() * position * scaling;
}

void Plane::setScaling(float sca) {
	GsMat scaling, position;
	position.translation(currPos);
	currentScale = sca;
	scaling.scaling(currentScale);
	sceneTransform->get() = position * rotX * rotY * rotZ * position.inverse() * position * scaling;
}

void Plane::setrotX(float X) {
	GsMat scaling, position;
	position.translation(currPos);
	scaling.scaling(currentScale);
	rotX.rotx(X);
	sceneTransform->get() = position * rotX * rotY * rotZ * position.inverse() * position * scaling;
}

void Plane::setrotY(float Y) {
	GsMat scaling, position;
	position.translation(currPos);
	scaling.scaling(currentScale);
	rotY.roty(Y);
	sceneTransform->get() = position * rotX * rotY * rotZ * position.inverse() * position * scaling;
}

void Plane::setrotZ(float Z) {
	GsMat scaling, position;
	position.translation(currPos);
	scaling.scaling(currentScale);
	rotZ.rotz(Z);
	sceneTransform->get() = position * rotX * rotY * rotZ * position.inverse() * position * scaling;
}