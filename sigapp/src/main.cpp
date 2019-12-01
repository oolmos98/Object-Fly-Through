
# include "my_viewer.h"
# include "sig/gs_box.h"

# include <sigogl/ws_run.h>

int main ( int argc, char** argv )
{
	MyViewer* v = new MyViewer ( -1, -1, 900, 600, "Helicopter Project" );
	v->cmd ( WsViewer::VCmdAxis);
	v->background(GsColor(167, 171, 99));

	v->view_all();
	v->show ();
	v->update_axis(0);
	ws_run();

	return 1;
}
