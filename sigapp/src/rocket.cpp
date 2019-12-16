#include "rocket.h"
#include <sig/sn_material.h>
#include <sig/sn_transform.h>
#include <thread>

Rocket::Rocket() {
	rocketClass = new SnGroup;
	//currentScale = 1.0f;
	_animating = false;
	import_model();
}

Rocket::Rocket(float model_scale) {
	rocketClass = new SnGroup;
	currentScale = model_scale;
	_animating = false;
	import_model();
}

Rocket::~Rocket() {
	rocketClass->remove_all();
}

void Rocket::import_model() {

	rocket = new SnModel;
	rocketClass = new SnGroup;
	trocket = new SnTransform;

	rocket->model()->load_obj("../model/rocket.obj");
	rocket->model()->smooth(true);
	rocketClass->add(trocket = new SnTransform);

	rocket->model()->scale(0.05f);
	GsMat rot, trans;
	rot.rot(GsVec::i, gs2pi/4);
	trans.translation(GsVec(0.0f, 15.0f, -15.0f));
	trocket->get() = trans * rot * trans.inverse();

	GsModel& material = *rocket->model();
	g = material.G.push();
	g->fi = 0;
	g->fn = material.F.size();
	g->dmap = new GsModel::Texture;
	g->dmap->fname.set("../textures/rocketTex.png");

	material.M.push().init();
	material.M.top() = GsMaterial::DefaultDiffuse;
	material.set_mode(GsModel::Hybrid, GsModel::PerGroupMtl);
	material.textured = true;

	rocketClass->add(trocket);
	rocketClass->add(rocket, true);

}