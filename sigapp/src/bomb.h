#pragma once
#include <sig/gs.h>
# include <sig/sn_model.h>
#include <sig/sn_group.h>
#include <sig/sn_transform.h>
# include <sigogl/ws_viewer.h>
//#include <thread>

class Bomb 
{
private:
	SnGroup* bombGroup;
	SnGroup* bombs;
	SnGroup* explosions;
	SnModel* bombModel;
	SnTransform* bombT;
	float bombScale;
	float explosionScale;
	GsVec position;
	bool bombExplode;
	SnModel* explosion;
	SnTransform* explosionT;


public:
	Bomb();
	~Bomb();
	bool bombDropped;
	GsVec bombDroppedLoc = GsVec(0.0f, 0.0f, 0.0f);
	void Explode();
	void import_model();
	void dropBomb();
	void setScale(float scale);
	void setPosition(GsVec pos);
	SnGroup* model() { return bombGroup; };
	int handle_keyboard(const GsEvent& e);
};

