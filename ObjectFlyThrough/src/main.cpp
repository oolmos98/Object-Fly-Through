
# include "my_viewer.h"
# include "sig/gs_box.h"

# include <sigogl/ws_run.h>

int main ( int argc, char** argv )
{
	MyViewer* v = new MyViewer ( -1, -1, 900, 600, "Object Fly-Through Project" );
	v->cmd ( WsViewer::VCmdDefault);
	//v->background(GsColor(167, 171, 99));
	v->background(GsColor::black);
	v->view_all();
	v->show ();
	//v->num_lights(2);
	//v->update_axis(0);
	
	ws_run();

	return 1;
}
