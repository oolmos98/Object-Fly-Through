#include "Cube.h"
#include <sig/sn_material.h>
#include <sig/sn_transform.h>
#include <thread>

// Default Constructor
Cube::Cube() {
	cube = new SnGroup;
	currentScale = 1.0f;
	import_model();
}


// Overloaded constructor
Cube::Cube(float model_scale) {
	cube = new SnGroup;
	currentScale = model_scale;
	_animating = false;
	import_model();
}

Cube::~Cube() {
	cube->remove_all();
}

void Cube::import_model() {

	carr[0] = new SnModel();
	carr[1] = new SnModel();
	carr[2] = new SnModel();
	carr[3] = new SnModel();
	carr[4] = new SnModel();
	carr[5] = new SnModel();

	carr2[0] = new SnModel();
	carr2[1] = new SnModel();
	carr2[2] = new SnModel();
	carr2[3] = new SnModel();
	carr2[4] = new SnModel();
	carr2[5] = new SnModel();
	face = new SnGroup;
	sceneTransform = new SnTransform;
	//flapsGroup = new SnGroup;

	for (int i = 0; i < 6; i++) {
		GsModel& m = *carr[i]->model();
		float x = 200.0f;
		float h = 200.0f;
		//Vertices
		m.V.push() = GsVec(-x, -h, -x);
		m.V.push() = GsVec(-x, -h, x);
		m.V.push() = GsVec(x, -h, -x);
		m.V.push() = GsVec(-x, -h, x);
		m.V.push() = GsVec(x, -h, x);
		m.V.push() = GsVec(x, -h, -x);
		//Faces
		m.F.push() = GsModel::Face(0, 1, 2);
		m.F.push() = GsModel::Face(2, 3, 4);

		if (i == 0) {
			m.N.push() = (GsVec(0, 60, 80));
			m.N.push() = (GsVec(0, 60, 80));
			m.N.push() = (GsVec(0, 60, 80));
			m.N.push() = (GsVec(0, 60, 80));
			m.N.push() = (GsVec(0, 60, 80));
			m.N.push() = (GsVec(0, 60, 80));
		}
		else if (i == 1) {
			m.N.push() = (GsVec(60, 0, 80));
			m.N.push() = (GsVec(60, 0, 80));
			m.N.push() = (GsVec(60, 0, 80));
			m.N.push() = (GsVec(60, 0, 80));
			m.N.push() = (GsVec(60, 0, 80));
			m.N.push() = (GsVec(60, 0, 80));
		}
		else if (i == 2) {
			m.N.push() = (GsVec(0, -60, 80));
			m.N.push() = (GsVec(0, -60, 80));
			m.N.push() = (GsVec(0, -60, 80));
			m.N.push() = (GsVec(0, -60, 80));
			m.N.push() = (GsVec(0, -60, 80));
			m.N.push() = (GsVec(0, -60, 80));
		}
		else if (i == 3 || i == 4||i ==5) {
			m.N.push() = (GsVec(-60, 0, 80));
			m.N.push() = (GsVec(-60, 0, 80));
			m.N.push() = (GsVec(-60, 0, 80));
			m.N.push() = (GsVec(-60, 0, 80));
			m.N.push() = (GsVec(-60, 0, 80));
			m.N.push() = (GsVec(-60, 0, 80));
		}
		else {
				m.N.push() = (GsVec(0, 60, 80));
				m.N.push() = (GsVec(0, 60, 80));
				m.N.push() = (GsVec(0, 60, 80));
				m.N.push() = (GsVec(0, 60, 80));
				m.N.push() = (GsVec(0, 60, 80));
				m.N.push() = (GsVec(0, 60, 80));
		}
		//Textures, code given by Professor
		m.T.push() = GsVec2(0.0f, 1.0f);
		m.T.push() = GsVec2(0.0f, 0.0f);
		m.T.push() = GsVec2(1.0f, 1.0f);
		m.T.push() = GsVec2(0.0f, 0.0f);
		m.T.push() = GsVec2(1.0f, 0.0f);
		m.T.push() = GsVec2(1.0f, 1.0f);

		GsModel::Group& g = *m.G.push();

		g.fi = 0;
		g.fn = m.F.size();
		g.dmap = new GsModel::Texture;
		g.dmap->fname.set("../textures/faceBottom.jpg");
		m.M.push().init();
		m.M.top() = GsMaterial::DefaultEmission;
		m.set_mode(GsModel::Smooth, GsModel::PerGroupMtl);
		m.textured = true;
	}
	face->add(carr[0]);
	face->add(carr[1]);
	face->add(carr[2]);
	face->add(carr[3]);
	face->add(carr[4]);
	face->add(carr[5]);

	carr[1]->model()->rotate(GsQuat(GsVec::k, gspi / 2));
	carr[2]->model()->rotate(GsQuat(GsVec::k, gspi));
	carr[3]->model()->rotate(GsQuat(GsVec::k, -gspi / 2));
	carr[4]->model()->rotate(GsQuat(GsVec::k, -gspi / 2));
	carr[4]->model()->rotate(GsQuat(GsVec::j, -gspi / 2));
	carr[5]->model()->rotate(GsQuat(GsVec::k, -gspi / 2));
	carr[5]->model()->rotate(GsQuat(GsVec::j, gspi / 2));

	for (int i = 0; i < 6; i++) {
		GsModel& m = *carr2[i]->model();
		float x = 200.0f;
		float h = 200.0f;
		//Vertices
		m.V.push() = GsVec(-x, -h, -x);
		m.V.push() = GsVec(-x, -h, x);
		m.V.push() = GsVec(x, -h, -x);
		m.V.push() = GsVec(-x, -h, x);
		m.V.push() = GsVec(x, -h, x);
		m.V.push() = GsVec(x, -h, -x);
		//Faces
		m.F.push() = GsModel::Face(2, 1, 0);
		m.F.push() = GsModel::Face(4, 3, 2);

		m.N.push() = (GsVec(0, -10, 0));
		m.N.push() = (GsVec(0, -10, 0));
		m.N.push() = (GsVec(0, -10, 0));
		m.N.push() = (GsVec(0, -10, 0));
		m.N.push() = (GsVec(0, -10, 0));
		m.N.push() = (GsVec(0, -10, 0));
		//Textures, code given by Professor
		m.T.push() = GsVec2(0.0f, 1.0f);
		m.T.push() = GsVec2(0.0f, 0.0f);
		m.T.push() = GsVec2(1.0f, 1.0f);
		m.T.push() = GsVec2(0.0f, 0.0f);
		m.T.push() = GsVec2(1.0f, 0.0f);
		m.T.push() = GsVec2(1.0f, 1.0f);

		GsModel::Group& g = *m.G.push();

		g.fi = 0;
		g.fn = m.F.size();
		g.dmap = new GsModel::Texture;
		g.dmap->fname.set("../textures/faceBottom.jpg");
		m.M.push().init();
		m.M.top() = GsMaterial::DefaultEmission;
		m.set_mode(GsModel::Smooth, GsModel::PerGroupMtl);
		m.textured = true;
	}
	face->add(carr2[0]);
	face->add(carr2[1]);
	face->add(carr2[2]);
	face->add(carr2[3]);
	face->add(carr2[4]);
	face->add(carr2[5]);

	carr2[1]->model()->rotate(GsQuat(GsVec::k, gspi / 2));
	carr2[2]->model()->rotate(GsQuat(GsVec::k, gspi));
	carr2[3]->model()->rotate(GsQuat(GsVec::k, -gspi / 2));
	carr2[4]->model()->rotate(GsQuat(GsVec::k, -gspi / 2));
	carr2[4]->model()->rotate(GsQuat(GsVec::j, -gspi / 2));
	carr2[5]->model()->rotate(GsQuat(GsVec::k, -gspi / 2));
	carr2[5]->model()->rotate(GsQuat(GsVec::j, gspi / 2));

	


	///*
	//	Add the sceneTransform
	//*/
	cube->add(sceneTransform);
	cube->add_group(face, true);
	
}

void Cube::run_animation(float time) {

		GsVec origin = GsVec(0.0f, 1.0f, 1.0f);
		GsMat translations;
		GsMat ptrans, prot;
		prot.rotz(gs2pi / 10.0f*time);
		ptrans.translation(GsVec(0.0f, 0.85f, -7.5f));
		
}


int Cube::handle_keyboard(const GsEvent& e) {
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

//void Cube::updateShadows(GsVec pos, GsMat trans, GsMat shadowMat, GsMat rot) {
//	GsMat oPos, scaling;
//	scaling.scaling(currentScale);
//	oPos.translation(pos);
//	/*
//		Idea,
//		translate, rotate, apply projection matrix, translate
//	*/
//	shadow_trans->get().set(oPos * shadowMat * trans * rot * trans.inverse() * oPos * scaling);
//}

void Cube::set_position(GsVec pos) {
	GsMat position, scaling;
	currPos = pos;
	scaling.scaling(currentScale);
	position.translation(currPos);
	sceneTransform->get() = position * rotX * rotY * rotZ * position.inverse() * position * scaling;
}

void Cube::setScaling(float sca) {
	GsMat scaling, position;
	position.translation(currPos);
	currentScale = sca;
	scaling.scaling(currentScale);
	sceneTransform->get() = position * rotX * rotY * rotZ * position.inverse() * position * scaling;
}

void Cube::setrotX(float X) {
	GsMat scaling, position;
	position.translation(currPos);
	scaling.scaling(currentScale);
	rotX.rotx(X);
	sceneTransform->get() = position * rotX * rotY * rotZ * position.inverse() * position * scaling;
}

void Cube::setrotY(float Y) {
	GsMat scaling, position;
	position.translation(currPos);
	scaling.scaling(currentScale);
	rotY.roty(Y);
	sceneTransform->get() = position * rotX * rotY * rotZ * position.inverse() * position * scaling;
}

void Cube::setrotZ(float Z) {
	GsMat scaling, position;
	position.translation(currPos);
	scaling.scaling(currentScale);
	rotZ.rotz(Z);
	sceneTransform->get() = position * rotX * rotY * rotZ * position.inverse() * position * scaling;
}