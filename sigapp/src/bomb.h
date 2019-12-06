#pragma once
#include <sig/gs.h>
# include <sig/sn_model.h>
#include <sig/sn_group.h>
#include <sig/sn_transform.h>
# include <sigogl/ws_viewer.h>
#include <thread>

class Bomb
{
private:
	SnGroup* bombGroup;
	SnModel* bombModel;
	SnTransform* bombT;
	float bombScale;
	GsVec position;

public:
	Bomb();
	~Bomb();

	void Explode();
	void import_model();
	void dropBomb();
	SnGroup* model() { return bombGroup; };
	int handle_keyboard(const GsEvent& e);
};

