# pragma once

# include <sig/sn_poly_editor.h>
# include <sig/sn_lines2.h>
# include <sig/sn_lines.h>
# include <sig/gs_light.h>

# include <sigogl/ui_button.h>
# include <sigogl/ui_check_button.h>
# include <sigogl/ui_slider.h>
# include <sigogl/ws_viewer.h>
# include "Plane.h"
# include "RepublicCarrier.h"
# include "Bucket.h"
# include "Cube.h"
# include "rocket.h"
# include "bomb.h"

// Viewer for this example:
class MyViewer : public WsViewer
{  protected :
	SnGroup* heli, * small_p, * big_p, * carrier, * land, * boats;
	//SnGroup* shadowHeli;
	SnTransform* body, * c, * p, * entire, * bprop, * baxis, * saxis, * sprop, * t, * sH, * h, * _land[2], * _boat,*_scene;
	SnLines* l;
	SnPolyEditor* _polyed;
	SnLines* _curve;
	SnLines* _curveCam;
	SnLines* _curvePlane;
	SnLines* _curveRepublic;
	SnLines* _curveCam1;
	SnLines* _curveRocket;

	UiSlider* _slider;
	GsArray<GsPnt> _points;
	GsArray<GsPnt> _camPathPoints;
	GsArray<GsPnt> _planePathPoints;
	GsArray<GsPnt> _republicPathPoints;
	GsArray<GsPnt> _camPath1;
	GsArray<GsPnt> _rocketPathPoints;


	SnMaterial* shMaterial;
	int altitude = 0;
	float speed_r = 0.0f, offset = 0.0f;
	bool engine_on = false, mode = false;
	int object = 0;//0 = Heli, 1 = Carrier, 2 = Both only if landed. 
	float angle = gspi / 10;
	int cMode = 0;

	enum MenuEv { EvNormals, EvAnimate, EvAnimate2, EvDeltaT, EvExit };
	UiCheckButton* _nbut;
	bool _animating;

	GsVec q = GsVec(0.0f,40.0f,0.0f);

	// Misc
	SnGroup* _pointPlacement;

	int i_heli = 0, i_plane = 0, i_rep, i_rock = 0;

	bool randomColor = false, _animate1 = false;
	// Jeff's Plane
	Plane* mahPlane;
	Bomb* dahBomb;

	// Alan's Rocket
	Rocket* dahRocket;

	RepublicCarrier* Venator;

	Bucket* bucket;
	Cube* cube;

   public :
	MyViewer ( int x, int y, int w, int h, const char* l );
	void build_ui();
	void options(int c);
	void import_models();
	void import_land();
	void align_models();
	void compute_mappings(char* file);
	void build_pad();
	void compute_curves();
	void run_propellors(bool on);
	void translateY(SnTransform* t, float y);
	void translateX(SnTransform* t, float x);
	void translateZ(SnTransform* t, float z);
	void translate(SnTransform* t, float x, float y, float z);
	void rotateX(SnTransform* t, float r);
	void rotateY(SnTransform* t, float r);
	void rotateZ(SnTransform* t, float r);
	void scaleBody(SnTransform* t, float s);
	void cameraMode(int mode);
	void computeShadow();

	// misc
	void showPoints(GsArray<GsPnt> P);
	void computeAnimate1();

	void run_animation();
	virtual int handle_keyboard ( const GsEvent &e ) override;
	virtual int uievent ( int e ) override;
	virtual void spin_animation_occured() override;
	virtual int handle(const GsEvent& e) override;

};

