#include "bomb.h"

Bomb::Bomb() {
	bombGroup = new SnGroup;
	bombModel = new SnModel;
	bombT = new SnTransform;
	import_model();
}

Bomb::~Bomb() {
	bombGroup->remove_all();
}

void Bomb::import_model() {
	
	bombModel->model()->load_3ds("../jeff_models/bomb/AN602.3ds");
	bombModel->model()->scale(0.25f);
	bombModel ->model()->smooth(true);
	bombGroup->add(bombT);
	bombGroup->add(bombModel);
}


void Bomb::dropBomb() {

}

void Bomb::Explode() {

}

int Bomb::handle_keyboard(const GsEvent& e) {
	return 1;
}