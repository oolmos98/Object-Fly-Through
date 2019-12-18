
# include "my_viewer.h"
#include <iostream>

# include <sigogl/ui_button.h>
# include <sigogl/ui_radio_button.h>
# include <sig/sn_primitive.h>
# include <sig/sn_transform.h>
# include <sig/sn_manipulator.h>
# include <sig/sn_material.h>

# include <sigogl/ws_run.h>


/*

CAUTION!! This program uses the current SIG-11-13 because of the added SnMaterial

*/
//Speed of translations!
float speed = 0.8f;
//Global Models
SnModel* sa;
SnModel* ba;
SnModel* pad[8];
SnModel* model[4];
SnModel* harrier[7];
SnModel* land_models[4];
SnModel* boat_model[4];
SnModel* curve;
GsPnt position = GsPnt(0, 0, 0);
GsArray<GsPnt> calcPoints;
GsArray<GsPnt> camPath;		// To be used for camera pathing
GsArray<GsPnt> planePath;
GsArray<GsPnt> republicPath;
GsArray<GsPnt> rocketPath;

//Not used
//SnGroup* sun;
//SnModel* s;
MyViewer::MyViewer(int x, int y, int w, int h, const char* l) : WsViewer(x, y, w, h, l)
{
	_nbut = 0;
	_animating = false;

	rootg()->add(_scene = new SnTransform());
	build_ui();
	compute_curves();
	computeAnimate1();
	import_land();

	import_models();
	align_models();
	options(0);
	//num_lights(4);
	
	//gsout << num_lights() << gsnl;
	/*light(0).position = GsVec(0, -80, 60);
	light(1).position = GsVec(0, 80, 60);
	light(2).position = GsVec(0, 80, -60);
	light(3).position = GsVec(0, -80, -60);

	render();*/

}

static void my_polyed_callback(SnPolyEditor* pe, enum SnPolyEditor::Event e, int pid)
{
	MyViewer* v = (MyViewer*)pe->userdata();
	if (e == SnPolyEditor::PostMovement || e == SnPolyEditor::PostEdition || e == SnPolyEditor::PostInsertion)
	{
		//v->update_scene();
	}
}

void MyViewer::build_ui ()
{
	UiPanel* p;// *sp;
	UiManager* uim = WsWindow::uim();
	p = uim->add_panel ( "", UiPanel::HorizLeft );
	/*p->add ( new UiButton ( "View", sp=new UiPanel() ) );
	{	UiPanel* p=sp;
		p->add ( _nbut=new UiCheckButton ( "Normals", EvNormals ) ); 
	}*/
	p->add(new UiButton("First Outer Cube Animation", EvAnimate));
	p->add(new UiButton("Inner Cube Animation", EvAnimate2));
	p->add ( new UiButton ( "Exit", EvExit ) ); p->top()->separate();

	p->add(_slider = new UiSlider(" dt:", EvDeltaT, 0, 0, 150));

	_slider->separate();

	_slider->range(0.01f, 0.25f);

	light().position.z = 2.0f;
	light().position.y = 2.0f; 
	//light().diffuse = GsColor::darkgray;

	//light().position.x = 1.0f;
	light().ambient = GsColor::gray;
	light().specular = GsColor::brown;
	
}
void MyViewer::options(int c) {
	//switch (c) {

	//case 0: {
	//	output("Hello there human:) \nPressing Keys:\n\
	//		'q' = Rotate Main Propellor Counter-clockwise\n\
	//		'a' = Rotate Main Propellor Clockwise\n\
	//		'w' = Rotate Tail Propellor Counter-clockwise\n\
	//		's' = Rotate Tail Propellor Clockwise\n\
	//		'z' = Scale Up\n\
	//		'x' = Scale Down\n\
	//		'SPACE' = Change Camera View\n\n\
	//		'ENTER' = Turn On Helicopter, must be on to control\n\
	//		'm' = Translate Up/Down or Move forward/backwards (Helicopter)\n\
	//		'c' = Helicopter or Carrier\n\
	//		'Up' = Translate Up or Move Forward\n\
	//		'Down' = Translate Down or Move Back\n\
	//		'Left' = Change Direction\n\
	//		'Right' = Change Direction");
	//	//output_pos(20, 70);
	//	break;
	//}
	//case 1: {
	//	output("Turn On Helictopter!");
	//	break;
	//}
	//case 2: {
	//	if(engine_on)
	//	output("Helicopter is now on!");
	//	else output("Helicopter is now off!");

	//	break;
	//}
	//case 4: {
	//	output("Cant go any lower becasause realism!");
	//	break;
	//}
	//case 5: {
	//	output("Changed Camera!");
	//	break;
	//}
	//case 6: {
	//	output("Moving!");
	//	break;
	//}
	//case 7: {
	//	output("Rotating!");
	//	break;
	//}
	//case 8: {
	//	output("Scaling!");
	//	break;
	//}
	//case 9: {
	//	output("Changed Mode!");
	//	break;
	//}
	//case 10: {
	//	output("Changed Object!");
	//	break;
	//}
	//}
	//char* m,* o,* e;
	//if (engine_on) e = "ON";
	//else e = "OFF";

	//if (object == 0) o = "HELICOPTER";
	//else o = "CARRIER";
	//if (!mode) m = "UP or DOWN";
	//else m = "FORWARD or BACK";
	//message().setf("Current Values:\t\
	//				Mode: %s\t\
	//				Object: %s\t\
	//				Engine: %s",m,o,e);
}


void MyViewer::import_models ()
{
	//Allocating Memory first.
	sa = new SnModel();
	ba = new SnModel();
	heli = new SnGroup;
	small_p = new SnGroup;
	big_p = new SnGroup;
	carrier = new SnGroup;
	//shadowHeli = new SnGroup;

	for (int i = 0; i < 4; i++) {
		model[i] = new SnModel();
	}
	for (int i = 0; i < 8; i++) {
		pad[i] = new SnModel();
	}
	for (int i = 0; i < 7; i++) {
		harrier[i] = new SnModel();
	}

	// Jeff's Plane
	mahPlane = new Plane();
	mahPlane->setScaling(0.35f);
	mahPlane->set_position(_planePathPoints[2]);
	rootg()->add_group(mahPlane->model(), true);

	/*dahRocket = new Rocket();
	dahRocket->setScaling(0.05f);
	dahRocket->set_position(_rocketPathPoints[0]);
	rootg()->add_group(dahRocket->model(), true);*/

	
	Venator = new RepublicCarrier();
	Venator->setScaling(0.5f);
	//Venator->setrotY(gspi);
	Venator->set_position(_republicPathPoints[1]);
	rootg()->add_group(Venator->model(), true);


	bucket = new Bucket();
	bucket->set_position(GsVec(0, -175, 0));
	bucket->setScaling(0.55f);

	rootg()->add_group(bucket->model(), true);

	
	cube = new Cube();
	//Cube->set_position(GsVec(0, 100, -50));
	//Cube->setScaling(2.0f);

	rootg()->add_group(cube->model(), true);

	//Building HelicopterPads and making sure its positioned correctly before any transformations
	build_pad();
	float height = 5.6f;
	pad[0]->model()->translate(GsVec(0, height, 0));
	pad[1]->model()->translate(GsVec(10, height, 0));
	pad[2]->model()->translate(GsVec(-10, height, 0));
	pad[3]->model()->translate(GsVec(-20, height, 0));
	pad[4]->model()->translate(GsVec(-30, height, 0));
	pad[5]->model()->translate(GsVec(-40, height, 0));
	pad[6]->model()->translate(GsVec(20, height, 0));
	pad[7]->model()->translate(GsVec(30, height, 0));

	//Loading/creating the models from the .obj file included.
	model[0]->model()->load_obj("../model/heliBody.obj");
	model[1]->model()->load_obj("../model/heliProp.obj");
	model[2]->model()->load_obj("../model/heliProp2.obj");
	model[3]->model()->load_obj("../model/heliCarrier2.obj");

	//Loading Harrier Obj.
	harrier[0]->model()->load_obj("../model/Sea_Harrier.obj");
	harrier[1]->model()->load_obj("../model/Sea_Harrier.obj");
	harrier[2]->model()->load_obj("../model/Sea_Harrier.obj");
	harrier[3]->model()->load_obj("../model/Sea_Harrier.obj");
	harrier[4]->model()->load_obj("../model/Sea_Harrier.obj");
	harrier[5]->model()->load_obj("../model/Sea_Harrier.obj");
	harrier[6]->model()->load_obj("../model/Sea_Harrier.obj");

	//Making sure cylinders are perfectly at the origin, like every object.
	//This idea is so that the transformations are computed perfectly.
	sa->model()->make_cylinder(GsPnt(-0.5, 0.0, 0.0), GsPnt(0.5, 0.0, 0.0), 0.5f, 0.5f, 25, true);
	ba->model()->make_cylinder(GsPnt(-0.7, 0.0, 0.0), GsPnt(0.3, 0.0, 0.0), 0.4f, 0.3f, 25, true);
	sa->color(GsColor::red);
	ba->color(GsColor::red);

	model[0]->model()->translate(GsVec(0, 0, 0));
	model[1]->model()->translate(GsVec(2.5f, 0, -0.1f));
	model[2]->model()->translate(GsVec(0.6f, 0.0f, -0.3f));
	model[3]->model()->translate(GsVec(0.0f, -0.5f, 2.0f));


	harrier[0]->model()->translate(GsVec(12.0f, 6.35f, 0.0f));
	harrier[1]->model()->translate(GsVec(24.0f, 6.35f, 0.0f));
	harrier[2]->model()->translate(GsVec(36.0f, 6.35f, 0.0f));
	harrier[3]->model()->translate(GsVec(-12.0f, 6.35f, 0.0f));
	harrier[4]->model()->translate(GsVec(-24.0f, 6.35f, 0.0f));
	harrier[5]->model()->translate(GsVec(-36.0f, 6.35f, 0.0f));
	harrier[6]->model()->translate(GsVec(48.0f, 6.35f, 0.0f));

	//model[4]->model()->rotate(GsQuat::angle(180));
	//SunGroup
	//sun = new SnGroup;
	//s = new SnModel();
	//s->model()->load_obj("../model/sun.obj");
	//sun->separator(true);
	//sun->add(s);
	//s->model()->scale(0.01f);
	//s->model()->translate(GsVec(20, 40, -20));
	//compute_mappings(" ");
	//rootg()->add(sun);


	//HeliBody 
	heli->separator(true);
	heli->add(body = new SnTransform);
	heli->add(model[0]);
	model[0]->model()->flat(true);
	model[0]->color(GsColor(40, 252, 3));
	

	//Small Prop
	small_p->separator(true);
	small_p->add(saxis = new SnTransform);
	small_p->add(sa);
	small_p->add(sprop = new SnTransform);
	small_p->add(model[1]);
	model[1]->model()->flat(true);
	model[1]->color(GsColor::cyan);

	//Big Prop
	big_p->separator(true);
	big_p->add(baxis = new SnTransform);
	big_p->add(ba);
	big_p->add(bprop = new SnTransform);
	big_p->add(model[2]);
	model[2]->model()->flat(true);
	model[2]->color(GsColor(74, 3, 252));

	//HeliCarrier
	carrier->separator(true);
	carrier->add(c = new SnTransform);
	carrier->add(model[3]);
	
	carrier->add(p = new SnTransform);
	carrier->add(pad[0]);
	carrier->add(pad[1]);
	carrier->add(pad[2]);
	carrier->add(pad[3]);
	carrier->add(pad[4]);
	carrier->add(pad[5]);
	carrier->add(pad[6]);
	carrier->add(pad[7]);
	carrier->add(h = new SnTransform);
	carrier->add(harrier[0]);
	carrier->add(harrier[1]);
	carrier->add(harrier[2]);
	carrier->add(harrier[3]);
	carrier->add(harrier[4]);
	carrier->add(harrier[5]);
	carrier->add(harrier[6]);

	model[3]->model()->flat(true);
	model[3]->color(GsColor(98, 122, 89));

	//Given by the Professor
	//This part sets the material to be placed into the shadow models
	/*shMaterial = new SnMaterial;
	GsMaterial m;
	m.diffuse = GsColor::black;
	shMaterial->material(m, 4);
	shMaterial->restore(true);

	shadowHeli->separator(true);
	shadowHeli->add(sH = new SnTransform);
	shadowHeli->add(shMaterial);
	shadowHeli->add(big_p);
	shadowHeli->add(heli);
	shadowHeli->add(small_p);*/
	
	
	////Adding the heli group into root.
	
	//t transform moves the entire helicopter
	//entire moves every object
	rootg()->add(entire = new SnTransform);
	rootg()->add_group(carrier);
	
	rootg()->add(t = new SnTransform);
	rootg()->add_group(heli);
	rootg()->add_group(small_p);
	rootg()->add_group(big_p);
	//rootg()->add_group(shadowHeli);
	compute_mappings("");

	
}




void MyViewer::import_land() {
	//Allocate memory for the land group.
	land = new SnGroup;
	boats = new SnGroup;
	//Allocate memory for the models
	for (int i = 0; i < 4; i++) {
		land_models[i] = new SnModel();
	}

	boats->separator(true);
	for (int i = 0; i < 4; i++) {
		boat_model[i] = new SnModel();
		boat_model[i]->model()->load_obj("../model/boat.obj");
		boat_model[i]->model()->scale(0.35f);
		boat_model[i]->color(GsColor(132, 132, 130));
		boats->add(boat_model[i]);
	}
	boat_model[0]->model()->translate(GsVec(30, -2.5f, 30));
	boat_model[1]->model()->translate(GsVec(5, -2.5f, -30));
	boat_model[2]->model()->translate(GsVec(-30, -2.5f, 30));
	boat_model[3]->model()->translate(GsVec(-30, -2.5f, -60));



	//Loading the lands.
	land_models[0]->model()->load_obj("../model/water2.obj");

	for (int i = 1; i < 4; i++) {
		if (land_models[i]->model()->load("../model/low-poly-mill.obj")) {}
		else gsout << "no Load" << gsnl;

		land_models[i]->model()->scale(0.1f);
		land_models[i]->model()->flat(true);
	}
	land_models[1]->model()->translate(GsVec(30, 30, -25));
	land_models[2]->model()->translate(GsVec(10, 20, 30));
	land_models[3]->model()->translate(GsVec(-30, 20, -10));

	land_models[0]->model()->scale(0.03f);
	land_models[0]->model()->translate(GsVec(0, -5.5f, 0));
	//land_models[0]->model()->translate(GsVec(0, 20, 0));
	//land_models[0]->model()->flat(true);
	//land_models[0]->model()->smooth(true);
	land_models[0]->color(GsColor(0, 141, 199));
	//land_models[0]->color(GsColor(138, 140, 123));
	land->separator(true);
	land->add(_land[0] = new SnTransform);
	land->add(land_models[0]);
	land->add(_land[1] = new SnTransform);
	land->add(land_models[1]);
	land->add(land_models[2]);
	land->add(land_models[3]);

	rootg()->add_group(boats);
	rootg()->add_group(land);
}
void MyViewer::align_models() {

	GsMat sc, rot, trans;
	
	body->get().scaling(0.07f);
	bprop->get().scaling(1.5f);

	scaleBody(t, 0.1f);
	sc.scaling(0.5f);
	trans.translation(GsVec(0, 10.5f, 21.0f));
	saxis->get().mult(sprop->get(), sc * trans);

	trans.translation(GsVec(0.0f, 4.2f, -3.5f));
	rot.rotz(gspi / 2);
	
	baxis->get().mult(bprop->get(),trans*rot);

	sc.scaling(0.5f);
	rot.rotz(-0.005f);
	trans.translation(GsVec(0.0f, -5.7f, 0));
	c->get().mult(c->get(), rot*sc * trans);
	t->get().scaling(0.15f);
	trans.translation(GsVec(0, -offset, 0));
	//computeShadow();
	entire->get().roty(gspi);

	sc.scaling(0.84f);
	rot.roty(gspi);
	h->get().mult(h->get(), sc * rot);
	sc.scaling(0.03f);
	rot.rotx(gspi);

	sc.scaling(0.008f);
	trans.translation(GsVec(0, -5, 0));
	_land[0]->get().mult(_land[0]->get(), trans*rot);
	_land[1]->get().mult(_land[1]->get(), rot);


}

void MyViewer::compute_mappings(char* file) {

	for (int i = 0; i < 7; i++) {
		////Code Provided by the Professor in the PA5 Pdf.
		GsModel& m = *harrier[i]->model();
		//To reuse G to recompute texture.
		m.G.init();
		GsModel::Group& g = *m.G.push();
		g.fi = 0;
		g.fn = m.F.size();
		g.dmap = new GsModel::Texture;
		g.dmap->fname.set("../textures/Dirty.jpg");
		m.M.push().init();
		m.M.top() = GsMaterial::DefaultAmbient;

		int nv = m.V.size();
		m.T.size(nv);
		int k = 0;

		m.set_mode(GsModel::Hybrid, GsModel::PerGroupMtl);
		m.textured = true;
	}

}

void MyViewer::build_pad() {
	for (int i = 0; i < 8; i++) {
		GsModel& m = *pad[i]->model();
		float x = 4.0f;
		//Vertices
		m.V.push() = GsVec(-x, 0, -x);
		m.V.push() = GsVec(-x, 0, x);
		m.V.push() = GsVec(x, 0, -x);
		m.V.push() = GsVec(-x, 0, x);
		m.V.push() = GsVec(x, 0, x);
		m.V.push() = GsVec(x, 0, -x);
		//Faces
		m.F.push() = GsModel::Face(0, 1, 2);
		m.F.push() = GsModel::Face(2, 3, 4);

		//Normals
		m.N.push() = normalize(GsVec(0, 1, 0));
		m.N.push() = normalize(GsVec(0, 1, 0));
		m.N.push() = normalize(GsVec(0, 1, 0));
		m.N.push() = normalize(GsVec(0, 1, 0));
		m.N.push() = normalize(GsVec(0, 1, 0));
		m.N.push() = normalize(GsVec(0, 1, 0));

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
		g.dmap->fname.set("../textures/helipad.jpg");
		m.M.push().init();
		m.set_mode(GsModel::Smooth, GsModel::PerGroupMtl);
		m.textured = true;
	}
}

static float N(int i, int K, float t)
{
	float ui = (float)i;
	if (K == 1)
	{
		return (ui <= t) && (t < (ui + 1)) ? 1.0f : 0.0f;
	}
	else
	{
		float x = ((t - ui) / (K - 1)) * N(i, K - 1, t) +
			((ui + K - t) / (K - 1)) * N(i + 1, K - 1, t);
		return x;
	}
}
static GsPnt eval_bspline(float t, int K, const GsArray<GsPnt>& P)
{
	GsPnt point;
	for (int i = 0; i < P.size(); i++)
	{
		point += (P[i] * N(i, K, t));
	}
	return point;
}
void MyViewer::compute_curves() {
	rootg()->add(_curve = new SnLines);
	rootg()->add(_curveCam = new SnLines);
	rootg()->add(_curvePlane = new SnLines);
	rootg()->add(_curveRepublic = new SnLines);
	rootg()->add(_curveRocket = new SnLines);

	_curve->color(GsColor(20, 200, 25));
	_curve->line_width(3.0f);

	_curveCam->line_width(3.0f);
	_curveCam->color(GsColor(20, 200, 25));

	_curvePlane->line_width(3.0f);
	_curvePlane->color(GsColor::lightblue);

	_curveRepublic->line_width(3.0f);
	_curveRepublic->color(GsColor::red);

	_curveRocket->line_width(3.0f);
	_curveRocket->color(GsColor::orange);
	
	_curve->swap_visibility();
	_curveCam->swap_visibility();
	_curvePlane->swap_visibility();
	_curveRepublic->swap_visibility();
	_curveRocket->swap_visibility();

	float height = 15.5f;
	_points.push() = position;
	_points.push() = position;
	_points.push() = GsPnt(0,height,0);
	_points.push() = GsPnt(0, height, 40);
	_points.push() = GsPnt(40, height, 35);
	_points.push() = GsPnt(40, height, 20);
	_points.push() = GsPnt(20, height, -20);
	_points.push() = GsPnt(0, height-5, -20);
	_points.push() = GsPnt(-10, height-5, -20);
	_points.push() = GsPnt(-30, height-5, -50);
	_points.push() = GsPnt(-10, height-5, -10);
	_points.push() = GsPnt(0, height-5, 40);
	_points.push() = GsPnt(-10, height-5, 30);
	_points.push() = GsPnt(0, height-5, -5);
	_points.push() = GsPnt(0, height - 5, 0);
	_points.push() = GsPnt(0, 0, 0);
	_points.push() = GsPnt(0, 0, 0);



	_curve->init();

	float deltat = _slider->value();
	_curve->begin_polyline();
	
	for (float t = 2; t < _points.size(); t += deltat) // note: the t range may change according to the curve
	{
		GsPnt x = eval_bspline(t, 3, _points);
		_curve->push(x);
		calcPoints.push(x);
	}
	/*for (int i = 0; i < calcPoints.size();i++)
	gsout << calcPoints[i] << gsnl;*/

	_curve->end_polyline();
	

	/*
		Rocket Path
	*/
	//float height = 15.5f;
	/*_rocketPathPoints.push() = GsPnt(30, -2.5f, 30);
	_rocketPathPoints.push() = GsPnt(30, -2.5f, 30);
	_rocketPathPoints.push() = GsPnt(30, -2.5f, 30);
	_rocketPathPoints.push() = GsPnt(60, height, 50);
	_rocketPathPoints.push() = GsPnt(70, height, 80);
	_rocketPathPoints.push() = GsPnt(40, height, 35);
	_rocketPathPoints.push() = GsPnt(40, height, 20);
	_rocketPathPoints.push() = GsPnt(20, height, -20);
	_rocketPathPoints.push() = GsPnt(0, height - 5, -20);
	_rocketPathPoints.push() = GsPnt(-10, height - 5, -20);
	_rocketPathPoints.push() = GsPnt(-30, height - 5, -50);
	_rocketPathPoints.push() = GsPnt(-10, height - 5, -10);
	_rocketPathPoints.push() = GsPnt(0, height - 5, 40);
	_rocketPathPoints.push() = GsPnt(-10, height - 5, 30);*/
	
	//_curveRocket->init();

	////float deltat = _slider->value();
	//_curveRocket->begin_polyline();

	//for (float t = 2; t < _rocketPathPoints.size(); t += deltat) // note: the t range may change according to the curve
	//{
	//	GsPnt x = eval_bspline(t, 3, _rocketPathPoints);
	//	_curveRocket->push(x);
	//	rocketPath.push(x);
	//}

	//_curveRocket->end_polyline();


	/*
		Camera Pathing
	*/
	_camPathPoints.push() = GsPnt(20.0f, 40.0f, 60.0f);
	_camPathPoints.push() = GsPnt(50.0f, 35.0f, 30.0f);
	_camPathPoints.push() = GsPnt(65.0f, 30.0f, 0.0f);
	_camPathPoints.push() = GsPnt(50.0f, 28.0f, -30.0f);
	_camPathPoints.push() = GsPnt(20.0f, 25.0f, -55.0f);
	_camPathPoints.push() = GsPnt(-15.0f, 22.0f, -50.0f);
	_camPathPoints.push() = GsPnt(-40.0f, 20.0f, -25.0f);
	_camPathPoints.push() = GsPnt(-30.0f, 18.0f, 18.0f);
	_camPathPoints.push() = GsPnt(0.0f, 18.0f, 35.0f);
	_camPathPoints.push() = GsPnt(20.0f, 18.0f, 20.0f);

	_curveCam->init();
	_curveCam->begin_polyline();

	for (float t = 2; t < _camPathPoints.size(); t += deltat) // note: the t range may change according to the curve
	{
		GsPnt x = eval_bspline(t, 3, _camPathPoints);
		_curveCam->push(x);
		camPath.push(x);
	}

	_curveCam->end_polyline();


	/*
		Plane Path
	*/
	float radius = 50.0f;
	float h = 20.0f;
	for (float theta = gs2pi + (2 * (gs2pi / 10)); theta >= 0; theta -= gs2pi / 10) {
		_planePathPoints.push() = GsPnt(radius * cosf(theta), h, radius * sinf(theta));
		h += h > 20.0f ? -5.0f : 5.0f;

	}
	radius = 65.0f;
	for (float theta = gs2pi + (2 * (gs2pi / 60)); theta >= 0; theta -= gs2pi / 60) {
	_republicPathPoints.push() = GsPnt(radius * cosf(theta), 40.0f, radius * sinf(theta));

	}


	//showPoints(_planePathPoints);

	_curvePlane->init();
	_curvePlane->begin_polyline();
	for (float t = 2; t < _planePathPoints.size(); t += deltat) // note: the t range may change according to the curve
	{
		GsPnt x = eval_bspline(t, 3, _planePathPoints);
		_curvePlane->push(x);
		planePath.push(x);
	
	}
	_curvePlane->end_polyline();

	_curveRepublic->init();
	_curveRepublic->begin_polyline();

	for (float t = 2; t < _republicPathPoints.size(); t += deltat) // note: the t range may change according to the curve
	{
		GsPnt x = eval_bspline(t, 3, _republicPathPoints);
		_curveRepublic->push(x);
		republicPath.push(x);
	}
	_curveRepublic->end_polyline();

}

void MyViewer::showPoints(GsArray<GsPnt> P) {
	_pointPlacement = new SnGroup;
	for (int i = 0; i < P.size(); i++)
	{
		SnModel* point = new SnModel;
		point->model()->make_sphere(P[i], 1.0f, 50, true);
		point->color(GsColor::red);
		_pointPlacement->add(point);
	}
	rootg()->add(_pointPlacement);
}


void MyViewer::computeAnimate1() {

		rootg()->add(_curveCam1 = new SnLines);
		_curveCam1->line_width(2.0f);
		_curveCam1->color(GsColor::cyan);

		_camPath1.push(GsPnt(600, 600, 600));
		_camPath1.push(GsPnt(600, 600, 600));
		_camPath1.push(GsPnt(550, 550, 550));
		_camPath1.push(GsPnt(500, 500, 500));
		_camPath1.push(GsPnt(450, 450, 450));
		_camPath1.push(GsPnt(400, 400, 250));

		_camPath1.push(GsPnt(250, 250, 250));
		_camPath1.push(GsPnt(250, 200, 200));
		_camPath1.push(GsPnt(250, 150, 150));
		_camPath1.push(GsPnt(250, 140, 120));
		_camPath1.push(GsPnt(200, 130, 100));
		_camPath1.push(GsPnt(150,100,100));
		_camPath1.push(GsPnt(100, 40, 100));

		float radius = 100;
		for (float theta = gs2pi + (2 * (gs2pi / 60)); theta >= 0; theta -= gs2pi / 60) {
			_camPath1.push() = GsPnt(radius * cosf(theta), 40.0f, radius * sinf(theta));
		}
		float deltat = _slider->value();
		_curveCam1->begin_polyline();

		for (float t = 2; t < _camPath1.size(); t += deltat) // note: the t range may change according to the curve
		{
			GsPnt x = eval_bspline(t, 3, _camPath1);
			_curveCam1->push(x);
		}

		_curveCam1->end_polyline();
		_curveCam1->swap_visibility();
	
}

void MyViewer::run_propellors(bool on) {

	//Code provided by Professor, using this for a consistent computation on any computers
	double t = 0, lt = 0, t0 = gs_time();
	double frdt = 1.0 / 60.0; //frames
	do {
		while (t - lt < frdt) { ws_check(); t = gs_time() - t0; }

		lt = gs_time() - t0;
		if (engine_on == false)break;
		//computeShadow();
		rotateX(baxis, -angle);
		rotateX(saxis, -angle);
		render();
		ws_check();
	} while (on == true);
}
void MyViewer::translateY(SnTransform* t,float y){
	GsMat& mat = t->get(), tr;
	tr.translation(GsVec(0, y, 0));
	mat.mult(mat, tr);
	position = GsPnt(position.x, position.y + y, position.z);
	
}
void MyViewer::translateX(SnTransform* t, float x) {
	GsMat& mat = t->get(), tr;
	tr.translation(GsVec(x, 0, 0));
	mat.mult(mat, tr);
	position = GsPnt(position.x + x, position.y, position.z);

}
void MyViewer::translateZ(SnTransform* t, float z) {
	GsMat& mat = t->get(), tr;
	tr.translation(GsVec(0, 0, z));
	mat.mult(mat, tr);

}
void MyViewer::translate(SnTransform* t,float x, float y, float z) {
	GsMat& mat = t->get(), tr;
	tr.translation(GsVec(x, y, z));
	mat.mult(mat, tr);

}
void MyViewer::rotateX(SnTransform* t, float r){
	GsMat& mat = t->get(), rot;
	rot.rotx(r);
	mat.mult(mat, rot);
}
void MyViewer::rotateY(SnTransform* t, float r) {
	GsMat& mat = t->get(), rot;
	rot.roty(r);
	mat.mult(mat, rot);
}
void MyViewer::rotateZ(SnTransform* t, float r) {
	GsMat& mat = t->get(), rot;
	rot.rotz(r);
	mat.mult(mat, rot);
}
void MyViewer::scaleBody(SnTransform* t, float s){
	GsMat& mat = t->get(), sc;
	sc.scaling(s);
	mat.mult(mat, sc);

}

void MyViewer::cameraMode(int mode) {
	GsMat tr, rot,rot1, rot2;
	rot.rotx(gspi / 227);
	rot1.roty(gspi / 365);
	rot2.rotz(gspi / 2);
	
	switch (mode) {
		
	case 0: {
	//	camera().init();
		camera().eye.z = 40.0f;
		camera().eye.y = 40.0f;
		//camera().up.z = -2.0f;
		camera().up;
		camera().center;
		camera().fovy = GS_TORAD(60);
		render();
		break;
	}
	case 1: {
			//Code for time part provided by Professor
			double t = 0, lt = 0, t0 = gs_time(); 
			double frdt = 1.0 / 30.0; //frames
			int index = 0;
			camera().init();
			if (cMode == 1) {

				do
				{
					while (t - lt < frdt) { ws_check(); t = gs_time() - t0; }

					lt = gs_time() - t0;

					if (engine_on) {
						rotateX(baxis, angle);
						rotateX(saxis, angle);
					}
					if (index < camPath.size()) {
						camera().eye = camPath[index];
						camera().center;
						camera().up;
						camera().fovy = GS_TORAD(84);
					}
					index++;
					message().setf("local time = % f", lt);
					render();
					ws_check();
				} while (cMode == 1);
			}
	
		break;
	}
	case 2: {
		if (i_heli < calcPoints.size()) {
			camera().eye = calcPoints[i_heli] + GsPnt(5, 15, -20);
			camera().center = calcPoints[i_heli];
			camera().fovy = GS_TORAD(60);
			camera().up;
		}
		break;
	}
	case 3: {
		if (i_plane < planePath.size()) {
			camera().eye = planePath[i_plane] + GsPnt(10, 15, 10);
			camera().center = planePath[i_plane];
			camera().fovy = GS_TORAD(60);
			camera().up;
		}
		break;
	}
	case 4: {
		if (i_rep < republicPath.size()) {
			camera().eye = republicPath[i_rep] + GsPnt(15, 25, 15);
			camera().center = republicPath[i_rep];
			camera().fovy = GS_TORAD(60);
			camera().up;
		}
		break;
	
	}
	}

	
}

void MyViewer::computeShadow() {

	GsMat s = GsMat(1.0f, (-q.x / q.y), 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, (-q.z / q.y), 1.0f, 0.0f,

		0.0f, 0.0f, 0.0f, 1.0f);

	GsMat tr;
	tr.translation(GsVec(0, -offset-5.0f, 0));
	sH->get().mult(tr, s);
}

void MyViewer::run_animation()
{
	output("ANIMATING!");
	//cameraMode(mode);
	if (_animate1) {
		double time = 0, lt = 0, t0 = gs_time();
		double frdt = 1.0 / 30; //frames
		int i = 0;
		while (i < _curveCam1->V.size()) {
			while (time - lt < frdt) { ws_check(); time = gs_time() - t0; }

			camera().eye = _curveCam1->V[i];
			camera().center = GsPnt(0,0,0);
			camera().fovy = GS_TORAD(60);
			camera().up.y = 40.0f;
			render();
			ws_check();
			i++;
		}
	}


	if (_animating) {
		GsMat tr, rot, sca;
		double time = 0, lt = 0, t0 = gs_time();
		double frdt = 1.0 / 30.0; //frames
		int i = i_heli;
		int ii = i_plane;
		int iii = i_rep;
		int iiii = i_rock;
		cMode = 2;
		//rot.roty(gspi/30);
		do
		{
			while (time - lt < frdt) { ws_check(); time = gs_time() - t0; }

			lt = gs_time() - t0;

			rotateX(baxis, -angle);
			rotateX(saxis, -angle);

			rotateY(_land[1], -gspi / 700);

			i = i_heli;
			ii = i_plane;
			iii = i_rep;
			iiii = i_rock;

			if (cMode < 5) {
				cameraMode(cMode);
			}
			if (i < calcPoints.size() - 1) {
				float x, y, z;

				x = calcPoints[i + 1].x;
				y = calcPoints[i + 1].y;
				z = calcPoints[i + 1].z;

				float angle = atan2(x - calcPoints[i].x, z - calcPoints[i].z);
				tr.translation(-calcPoints[i].x, calcPoints[i].y, -calcPoints[i].z);
				sca.scaling(0.15f);

				rot.roty(angle);
				t->get() = tr * rot * tr.inverse() * tr * sca;

				offset += calcPoints[i + 1].y - calcPoints[i].y;
				i_heli++;
			}
			else i_heli = 0;

			mahPlane->run_animation(float(lt));

			if (ii < planePath.size() - 1) {

				float angley = atan2(planePath[ii].x - planePath[ii + 1].x, planePath[ii].z - planePath[ii + 1].z);
				//float anglez = atan2(planePath[ii+1].y - planePath[ii].y, planePath[ii+1].x - planePath[ii].x);
				mahPlane->set_position(planePath[ii]);
				mahPlane->setrotY(angley);
				//mahPlane->setrotX(anglez);

				i_plane++;
			}
			else i_plane = 0;

			if (iii < republicPath.size() - 1) {

				float angle = atan2(republicPath[iii].x - republicPath[iii + 1].x, republicPath[iii].z - republicPath[iii + 1].z);

				Venator->set_position(republicPath[iii]);
				Venator->setrotY(angle);
				i_rep++;
			}
			else i_rep = 0;

			/*if (iiii < rocketPath.size() - 1) {

				float angley = atan2(planePath[iiii].x - planePath[iiii + 1].x, planePath[iiii].z - planePath[iiii + 1].z);

				dahRocket->set_position(rocketPath[iiii]);
				dahRocket->setrotZ(angle);
				i_rock++;
			}
			else i_rock = 0;*/

			if (randomColor) {
				boat_model[0]->color(GsColor(i * 2, ii, iii * 2));
				boat_model[1]->color(GsColor(i, ii * 3, iii * 5));
				boat_model[2]->color(GsColor(i * 5, ii * 3, iii * 2));
				boat_model[3]->color(GsColor(i * 2, ii * 6, iii * 2));
				land_models[0]->color(GsColor(i * 2, ii * 2, iii * 3));
			}
			else {
				boat_model[0]->color(GsColor(132, 132, 130));
				boat_model[1]->color(GsColor(132, 132, 130));
				boat_model[2]->color(GsColor(132, 132, 130));
				boat_model[3]->color(GsColor(132, 132, 130));
				land_models[0]->color(GsColor(0, 141, 199));
			}
			//computeShadow();
			render();

		} while (_animating);


		boat_model[0]->color(GsColor(132, 132, 130));
		boat_model[1]->color(GsColor(132, 132, 130));
		boat_model[2]->color(GsColor(132, 132, 130));
		boat_model[3]->color(GsColor(132, 132, 130));
		land_models[0]->color(GsColor(0, 141, 199));

		i_heli = i_plane = i_rep = 0;
		_animating = false;
		options(-1);

		mode = mode ? false : true;

	}
}

int MyViewer::handle_keyboard(const GsEvent& e)
{
	int ret = WsViewer::handle_keyboard(e); // 1st let system check events
	if (ret) return ret;
	GsMat rot;
	int menu = 0;
	//will determine how fast each interaction is. Lower is bigger.
	mahPlane->handle_keyboard(e);
	switch (e.key)
	{
	case GsEvent::KeyEsc: gs_exit(); break;

	case GsEvent::KeyEnter:{
		if (object == 0) {
			if (engine_on && altitude == 0) { options(3); engine_on = false; }
			else engine_on = true;
			options(2);
			run_propellors(engine_on);
		}
		break;
	}
	case GsEvent::KeyUp: 
	{
		if (object == 0) {
			if (engine_on) {
				if (!mode) {
					if (altitude >= 0) {
						translateY(t, speed);
						offset += speed;
						altitude += 1;
						options(6);
					}
				}
				else {
					options(6);
					translateZ(t, -speed);
				}
			}
			else options(1);
		}
		if (object == 1 && altitude > 0) {
			options(6);
			translateX(c, speed);
		}
		else if (object == 1 && altitude == 0) {
			options(6);
			translateX(entire, speed);
		}
		break;
	}

	case GsEvent::KeyDown: {
		if (object == 0) {
			if (engine_on) {
				if (!mode) {
					if (altitude >= 1) {
						translateY(t, -speed);
						offset -= speed;
						altitude -= 1;
						options(6);
					}
					else options(4);
				}
				else {
					options(6);
					translateZ(t, speed);
				}

			}
			else options(1);
		}
		if (object == 1 && altitude > 0) {
			options(6);
			translateX(c, -speed);
		}
		else if (object == 1 && altitude == 0) {
			options(6);
			translateX(entire, -speed);

		}
		break;
	}

	case GsEvent::KeyLeft: case 'd': {
		if (object == 0) {
			if (engine_on) {
				options(7);
				rotateY(t, gspi / 60);
			}
			else options(1);

		}
		if (object == 1 && altitude > 0) {
			options(7);
			rotateY(c, gspi / 60);
		}
		else if (object == 1 && altitude == 0) {
			options(7);
			rotateY(entire, gspi / 60);
		}
		break;
	}

	case GsEvent::KeyRight: case 'e': {
		if (object == 0) {
			if (engine_on) {
				options(7);
				rotateY(t, -gspi / 60);
			}
			else options(1);

		}
		if (object == 1 && altitude > 0) {
			options(7);
			rotateY(c, -gspi / 60);
		}
		else if (object == 1 && altitude == 0) {
			options(7);
			rotateY(entire, -gspi / 60);
		}
		break;
	}

	case 'q': {
		options(7);
		rotateX(baxis, gspi / 60);
		break;
	}

	case 'a': {			
		options(7);
		rotateX(baxis, -gspi / 60);
		break;
	}

	case 'w': {
		options(7);
		rotateX(saxis, gspi / 60);
		break;
	}

	case 's': {
		rotateX(saxis, -gspi / 60);
		break;
	}
	case 'z': {
		scaleBody(entire, 1.1f);
		options(8);
		break;
	}

	case 'x': {
		scaleBody(entire, 0.9f);
		options(8);
		break;
	}

	case 'm':
		options(9);
		mode = !mode;
		break;

	case ' ': {
		//gsout << cMode << gsnl;
		if (_animating) {
			if (cMode == 4)
				cMode = 2;
			else if (cMode == 2) cMode = 3;
			else cMode = 4;
		}
		else {
			if (cMode == 1) cMode = 0;
			else cMode = 1;
		}
		cameraMode(cMode);

		options(5);
		break;
	}
	case 'c': {
		if (object < 1) {
			object++;
		}
		else object = 0; //reset
		options(10);
		break;
	}
	case 'v': {
		_curve->swap_visibility();
		_curveCam->swap_visibility();
		_curvePlane->swap_visibility();
		_curveRepublic->swap_visibility();
		//_curveRocket->swap_visibility();
		_curveCam1->swap_visibility();

		break;
	}
	case 'r': {
		randomColor = !randomColor;
		break;
	}
	case '1': {
		_animate1 = true;
		_animating = false;
		run_animation(); options(0); return 1;
	}
	case '2': {
		_animate1 = false;
		_animating = !_animating;
		run_animation(); options(0); return 1;
	}

	default:
	{
		options(0);
	}

	}
	options(-1);
	//computeShadow();
	render();
	ws_check();
	return 0;
}

int MyViewer::uievent ( int e )
{
	switch ( e )
	{
	case EvAnimate: _animate1 = true;
		_animating = false;
		run_animation(); options(0); return 1;

	case EvAnimate2: 
		_animate1 = false;
		_animating = !_animating;
		run_animation(); options(0); return 1;

	case EvExit: gs_exit();

	default: {
		options(0);
	}
	
	}
	return WsViewer::uievent(e);
}
void MyViewer::spin_animation_occured() {
	message().setf("%f", fps());
}
int MyViewer::handle(const GsEvent& e)
{
	return WsViewer::handle(e);
}