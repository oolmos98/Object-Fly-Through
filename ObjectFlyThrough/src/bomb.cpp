#include "bomb.h"
# include <sig/sn_model.h>
#include <sig/sn_group.h>
#include <sig/sn_transform.h>
Bomb::Bomb() {
	import_model();
}

Bomb::~Bomb() {
	bombGroup->remove_all();
}

void Bomb::import_model() {
	//gsout << "Loading Bomb" << gsnl;
	GsMat translation, scale, rotate;
	bombGroup = new SnGroup;
	bombs = new SnGroup;
	explosions = new SnGroup;
	bombModel = new SnModel;
	explosion = new SnModel;
	bombT = new SnTransform;
	explosionT = new SnTransform;
	bombScale = 1.0f;
	explosionScale = 1.0f;
	bombDropped = false;
	bombExplode = false;
	if (bombModel->model()->load_obj("../model/jeff_models/bomb/Fat_Man_Bomb.obj")) {}
	else gsout << "Error Loading the Fat Man Bomb!" << gsnl;
	bombModel->model()->scale(bombScale);
	bombModel->model()->smooth(true);
	rotate.rotz(gspi / 2);
	bombModel->model()->rotate(GsQuat(rotate));
	bombModel->color(GsColor::darkgreen);
	bombs->add(bombT);
	bombs->add(bombModel);

	explosion->model()->make_sphere(position, 2.0f, 100, true);
	explosion->color(GsColor::yellow);
	explosions->add(explosionT);
	explosions->add(explosion);


	bombGroup->add_group(bombs, true);
	bombGroup->add_group(explosions, true);

	position = GsVec(0.0f, -60.0f, 0.0f);
	translation.translation(position);
	scale.scaling(bombScale);
	bombT->get() = translation * scale;
	scale.scaling(explosionScale);
	explosionT->get() = translation * scale;
}


void Bomb::dropBomb() {
	if (bombDropped) {
		GsMat pos, scale;
		bombDroppedLoc -= GsVec(0.0f, 0.5f, 0.0f);
		pos.translation(bombDroppedLoc);
		scale.scaling(bombScale);
		bombT->get() = pos * scale;
		if (bombDroppedLoc.y <= -6.0f) {
			bombExplode = true;
			Explode();
		}
	}

	if (bombExplode) {
		Explode();
	}
}

void Bomb::setPosition(GsVec pos) {
	position = pos;
	GsMat translation, scale;
	translation.translation(position);
	scale.scaling(bombScale);
	bombT->get() = translation * scale;

}

void Bomb::Explode() {
	
	GsMat pos, scale;
	if (explosionScale < 15.0f) {
		pos.translation(GsVec(bombDroppedLoc.x, -6.0f, bombDroppedLoc.z));
		explosionScale += 0.1f;
		scale.scaling(explosionScale);
		explosionT->get() = pos * scale;
	}
	else {
		explosionScale = 1.0f;
		bombExplode = false;
		bombDropped = false;
		position = GsVec(0.0f, -20.0f, 0.0f);
		pos.translation(position); 
		scale.scaling(bombScale);
		bombT->get() = pos * scale;
		scale.scaling(explosionScale);
		explosionT->get() = pos * scale;
	}
}

void Bomb::setScale(float scale) {
	GsMat translation, scaling;
	translation.translation(position);
	bombScale = scale;
	scaling.scaling(bombScale);
	bombT->get() = translation * scaling;
}

int Bomb::handle_keyboard(const GsEvent& e) {
	return 1;
}