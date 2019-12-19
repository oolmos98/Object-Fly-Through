
# include "my_viewer.h"

# include <sigogl/ui_button.h>
# include <sigogl/ui_radio_button.h>
# include <sig/sn_primitive.h>
# include <sig/sn_transform.h>
# include <sig/sn_manipulator.h>
# include <sig/sn_material.h>

# include <sigogl/ws_run.h>

MyViewer::MyViewer ( int x, int y, int w, int h, const char* l ) : WsViewer(x,y,w,h,l)
{
	moveCam = false; // sets the camera mode
	_nbut=0;
	_animating=false;
	build_ui ();
	build_scene ();

	output_pos(20, 20);
	output("r - reset\nspace bar - change camera mode\n'a','d', left arrow key, right arrow key - Tilt the plane sideways\nArrow up, Arrow down - tilt plane x-axis \n'w' - Move the plane forward\n's' - stop the plane\n'q' & 'e' - rotate the plane in Y-axis");
}

void MyViewer::build_ui ()
{
	UiPanel *p, *sp;
	UiManager* uim = WsWindow::uim();
	p = uim->add_panel ( "", UiPanel::HorizLeft );
	p->add ( new UiButton ( "View", sp=new UiPanel() ) );
	{	UiPanel* p=sp;
		p->add ( _nbut=new UiCheckButton ( "Normals", EvNormals ) ); 
	}
	p->add ( new UiButton ( "Animate", EvAnimate ) );
	p->add ( new UiButton ( "Exit", EvExit ) ); p->top()->separate();
}

void MyViewer::add_model ( SnShape* s, GsVec p )
{
	// This method demonstrates how to add some elements to our scene graph: lines,
	// and a shape, and all in a group under a SnManipulator.
	// Therefore we are also demonstrating the use of a manipulator to allow the user to
	// change the position of the object with the mouse. If you do not need mouse interaction,
	// you can just use a SnTransform to apply a transformation instead of a SnManipulator.
	// You would then add the transform as 1st element of the group, and set g->separator(true).
	// Your scene graph should always be carefully designed according to your application needs.

	SnManipulator* manip = new SnManipulator;
	GsMat m;
	m.translation ( p );
	manip->initial_mat ( m );

	SnGroup* g = new SnGroup;
	SnLines* l = new SnLines;
	l->color(GsColor::orange);
	g->add(s);
	g->add(l);
	manip->child(g);
	// manip->visible(false); // call this to turn off mouse interaction
	rootg()->add(manip);
}

void MyViewer::build_scene()
{
	/*
		Initialize all the required variables for the scene
	*/
	camMode = 0;
	forward = false;
	backward = false;
	left = false;
	right = false;
	followPlane = false;

	/*
		Create the plane model
	*/
	

	/*
		Point Getter
		- Used for getting the axis of rotation for the flaps, rudder and elevators
	*/

	//points = new SnGroup;
	//point = new SnModel;
	//point->model()->make_sphere(GsPnt(0.0f, 1.6f, 5.3f), 0.1f, 50, true);
	//point->color(GsColor::red);
	//points->separator(true);
	//points->add(point);
	//rootg()->add(points);

	/*
	GsVec A = GsPnt(0.0f, 3.1f, 5.3f) - GsPnt(0.0f, 1.6f, 5.3f);
	GsQuat rotR(A, gs2pi/120);
	*/

	/*
		Plane Model Group
	*/
	plane = new SnModel();
	planeBody = new SnGroup;
	sceneTransform = new SnTransform;
	flapsGroup = new SnGroup;
	engineRunning = false;

	plane->model()->load_obj("../model/plane.obj");
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
	planeModel[0]->model()->load_obj("../model/plane.obj");
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
	planeModel[1]->model()->load_obj("../model/propeller.obj");
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
	propeller->model()->load_obj("../model/propeller.obj");
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
	trans.translation(GsVec(0.0f,0.0f, 3.95f));
	rot.roty(-gspi/18);
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
		Floor Model Group
	*/
	gFloor = new SnGroup;
	floor = new SnModel;
	f_trans = new SnTransform;
	floor->model()->make_box(GsBox());
	floor->color(GsColor::green);
	rot.rotx(gspi / 2);
	sca.scaling(200.0f, 200.0f, -1.0f);
	floor->model()->transform( rot * sca);
	gFloor->add(f_trans);
	gFloor->add(floor);
	f_trans->get().translation(GsVec(-100.0f, -10.0f, -100.0f));


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
	rudder->model()->transform(trans* rot* trans.inverse()* sca);
	rudderGroup->add(r_trans);
	rudderGroup->add(rudder);

	r_trans->get().translation(GsVec(-0.05f, 3.2f, 5.22f));

	/*
		The sun model group (light)
	*/
	sunG = new SnGroup;
	sun = new SnModel;
	sunTran = new SnTransform;
	sun->model()->make_sphere(p, 1.0f, 100, true);
	sun->color(GsColor::yellow);
	sunG->add(sunTran);
	sunG->add(sun);

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

	/*
		I add the floor, sun and the shadow before the sceneTransform
		so that the transformation that is applied to sceneTransform does not affect
		these models. (separator is true but for some reason it still applying the transformation to 
		the other groups.
	*/
	rootg()->add_group(gFloor, true);
	rootg()->add_group(sunG, true);
	rootg()->add_group(shadows, true);

	/*
		Add the sceneTransform
	*/
	rootg()->add(sceneTransform);
	rootg()->add_group(planeBody, true);
	rootg()->add_group(propGroup, true);
	rootg()->add_group(flapsGroup, true);
	rootg()->add_group(elvtorGroup, true);
	rootg()->add_group(rudderGroup, true);
	
	
}
	

// Below is an example of how to control the main loop of an animation:
void MyViewer::run_animation()
{
	double frdt = 1.0 / 120.0; // delta time to reach given number of frames per second
	double t = 0, lt = 0, t0 = gs_time();
	float v = 0.1f;
	float i = 0.0f;
	float r = 1.0f;
	GsVec origin = GsVec(0.0f, 1.0f, 1.0f);
	GsMat translations;
	do // run for a while:
	{	
		while ( t-lt<frdt ) { ws_check(); t=gs_time()-t0; } // wait until it is time for next frame
		if (!_animating) {
			break;
		}
		double yinc = (t - lt) * v;
		lt = t;
		i++;
		// This part runs the propeller of the plane.
		if (engineRunning) {
			GsMat ptrans, prot;
			prot.rotz(gs2pi / 10.0f * i);
			ptrans.translation(GsVec(0.0f, 0.85f, -7.5f));
			prop_trans->get() = ptrans * prot;
			propShadow->get() = ptrans * prot;
		}
		// This makes the plane go forward.
		if (forward) {
			currPos = GsVec(currPos) + GsVec(0.0f, 0.0f, -(0.01f + v));
			translations.translation(currPos);
			sceneTransform->get() = translations * rotX * rotY * rotZ * translations.inverse() * translations;
			updateShadows(currPos, translations, shadowMat, rotX.inverse() * rotY * rotZ.inverse());
		}

		// This makes the camera rotate
		if (moveCam) {
			camera().rotate(GsQuat(origin, gs2pi / 60.0f / 120.0f));
			camera().center;
		}

		// This makes the camera follow the plane
		if (followPlane) {
			camera().eye.x = currPos.x;
			camera().eye.z = 20.0f + currPos.z;
			camera().eye.y = 20.0f + currPos.y;
			camera().center = currPos;
		}

		message().setf("local time=%f", lt);
		ws_check();
		render();
	}	while (_animating);
}


void MyViewer::show_normals ( bool view )
{
	// Note that primitives are only converted to meshes in GsModel
	// at the first draw call.
	GsArray<GsVec> fn;
	SnGroup* r = (SnGroup*)root();
	for ( int k=0; k<r->size(); k++ )
	{	SnManipulator* manip = r->get<SnManipulator>(k);
		SnShape* s = manip->child<SnGroup>()->get<SnShape>(0);
		SnLines* l = manip->child<SnGroup>()->get<SnLines>(1);
		if ( !view ) { l->visible(false); continue; }
		l->visible ( true );
		if ( !l->empty() ) continue; // build only once
		l->init();
		if ( s->instance_name()==SnPrimitive::class_name )
		{	GsModel& m = *((SnModel*)s)->model();
			m.get_normals_per_face ( fn );
			const GsVec* n = fn.pt();
			float f = 0.33f;
			for ( int i=0; i<m.F.size(); i++ )
			{	const GsVec& a=m.V[m.F[i].a]; l->push ( a, a+(*n++)*f );
				const GsVec& b=m.V[m.F[i].b]; l->push ( b, b+(*n++)*f );
				const GsVec& c=m.V[m.F[i].c]; l->push ( c, c+(*n++)*f );
			}
		}  
	}
}

int MyViewer::handle_keyboard ( const GsEvent &e )
{
	int ret = WsViewer::handle_keyboard ( e ); // 1st let system check events
	if ( ret ) return ret;
	GsMat trans;

	switch ( e.key )
	{	case GsEvent::KeyEsc : gs_exit(); return 1;
		case 'n' : { bool b=!_nbut->value(); _nbut->value(b); show_normals(b); return 1; }
		default:
		{
			gsout << "Key pressed: " << e.key << gsnl;
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
				sceneTransform->get() = trans * rotX * rotY * rotZ * trans.inverse() * trans;
			}
			break;
		}
		// Tilt the plane to right
		case GsEvent::KeyRight:
		case 'd': {
			if (flapangle < MAX_ANGLE_ALLOWED){
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
				sceneTransform->get() = trans * rotX * rotY * rotZ * trans.inverse() * trans;
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
				sceneTransform->get() = trans * rotX * rotY * rotZ * trans.inverse() * trans;
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
				sceneTransform->get() = trans * rotX * rotY * rotZ * trans.inverse() * trans;
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
				sceneTransform->get() = trans * rotX * rotY * rotZ * trans.inverse() * trans;
				
			}
			render();
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
				sceneTransform->get() = trans * rotX * rotY * rotZ * trans.inverse() * trans;
			}
			break;
		}
		// Move the plane forward
		case 'w': {
			_animating = true;
			engineRunning = true;
			backward = false;
			forward = true;
			//PlaySound("D:\\CSE170\\Project1\\sigapp\\model\\planeidle.wav", NULL, SND_ASYNC | SND_FILENAME);
			run_animation();
			break;
		}
		// Stop the plane
		case 's': {
			engineRunning = false;
			forward = false;
			backward = true;
			//PlaySound("D:\\CSE170\\Project1\\sigapp\\model\\aircraft011.wav", NULL, SND_ASYNC | SND_FILENAME);
			break;
		}

		// Reset everything
		case 'r': {
			currPos = GsVec(0.0f, 0.0f, 0.0f);
			trans.translation(currPos);
			rotX.rotx(0);
			rotY.roty(0);
			rotZ.rotz(0);
			_animating = false;
			forward = false;
			moveCam = false;
			followPlane = false;
			currentang = vertAng = flapangle = eleangle = r_angle = 0;
			engineRunning = 0;
			camera().init();
			camera().rotate(GsQuat(GsVec(0.0f, 0.0f, 0.0f), 0));
			camera().eye.z = 40.0f;
			camera().eye.y = 40.0f;
			camera().up.z = -2.0f;
			camera().up.y = 2.0f;
			sceneTransform->get() = trans * rotX * rotY * rotZ * trans.inverse() * trans;
			updateShadows(currPos, trans, shadowMat, rotX* rotY* rotZ);
			break;
		}
		// Change the camera modes
		case GsEvent::KeySpace: {
			camMode++;
			if (camMode == 1) { // Fixed point Camera mode
				moveCam = false;
				camera().init();
				camera().rotate(GsQuat(GsVec(0.0f, 0.0f, 0.0f), 0));
				camera().eye.z = 40.0f;
				camera().eye.y = 40.0f;
				camera().up.z = -2.0f;
				camera().up.y = 2.0f;
			}
			else if (camMode == 2) { // Moving camera mode (rotating around an axis)
				moveCam = true;
				_animating = true;
				camera().init();
				camera().rotate(GsQuat(GsVec(0.0f, 1.0f, 1.0f), 0));
				camera().eye.z = 40.0f;
				camera().eye.y = 40.0f;
				camera().up.z = -2.0f;
				camera().up.y = 2.0f;
				run_animation();
			}
			else if (camMode == 3) { // Moving camera mode (following plane)
				moveCam = false;
				camera().init();
				camera().rotate(GsQuat(GsVec(0.0f, 0.0f, 0.0f), 0));
				camera().eye.z = 20.0f;
				camera().eye.y = 20.0f;
				camera().center;
				camera().up.z = -1.0f;
				camera().up.y = 1.0f;
				followPlane = !followPlane;
				run_animation();
			}
			else if(camMode > 3){	// Reser camera mode
				camMode = 0;
				moveCam = false;
				followPlane = false;
				camera().init();
				camera().rotate(GsQuat(GsVec(0.0f, 0.0f, 0.0f), 0));
				camera().eye.z = 20.0f;
				camera().eye.y = 20.0f;
				camera().up.z = -1.0f;
				camera().up.y = 1.0f;
			}
			//char test[32];
			//sprintf(test, "camMode: %d", camMode);
			//output(test);
			break;
		}
	}

	// Don't forget to update the shadow!
	// Shadow must have an opposite angle of rotation from the original
	updateShadows(currPos, trans, shadowMat, rotX.inverse() * rotY* rotZ.inverse());
	ws_check();
	render();
	return 1;
}

// Calculation of shadow (updating the shadow)
void MyViewer::updateShadows(GsVec pos, GsMat trans, GsMat shadowMat, GsMat rot) {
	GsMat oPos;
	oPos.translation(pos);
	/*
		Idea,
		translate, rotate, apply projection matrix, translate
	*/
	shadow_trans->get().set(oPos * shadowMat * trans * rot * trans.inverse() * oPos);
}

int MyViewer::uievent ( int e )
{
	switch ( e )
	{	case EvNormals: show_normals(_nbut->value()); return 1;
		case EvAnimate: run_animation(); return 1;
		case EvExit: gs_exit();
	}
	return WsViewer::uievent(e);
}
