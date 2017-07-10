
#include "myGLWindow.h"

#include <string>
#include <fstream>
#include <streambuf>


/** @defgroup mesh_processing Mesh Processing
*   Module containing mesh processing tools useful for the application
*/

/** @defgroup visualization Visualization based in OpenGL
*   Module containing OpenGL abstractions for visualization of triangular meshes
*/

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	ShapeData sd;
	sd.loadFromFile(argv[1]);
	myGLWindow gl_window;
	gl_window.show();
	gl_window.setShape(&sd);

	return app.exec();
}
