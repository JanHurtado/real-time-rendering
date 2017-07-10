#include "myGLWindow.h"

myGLWindow::myGLWindow(QWidget *parent)
: QOpenGLWidget(parent)
{
	m_fps = 0.0;
}


myGLWindow::~myGLWindow()
{
	delete renderer;
}

void myGLWindow::sendDataToOpenGL()
{
	makeCurrent();
	renderer->sendDataSingleBuffer();
	doneCurrent();
}

void myGLWindow::initializeGL()
{
	renderer = new myRenderer;
	setMouseTracking(true);
	renderer->addShader(GL_VERTEX_SHADER, "VertexShaderCode.glsl");
	renderer->addShader(GL_FRAGMENT_SHADER, "FragmentShaderCode.glsl");
	renderer->addTexture("lambertian.jpg"); // 0
	renderer->addTexture("bump2n.png"); // 1
	renderer->addSampler(); // default  0
	renderer->setWidth(width());
	renderer->setHeight(height());
	renderer->initialize();
}


void myGLWindow::paintGL()
{
	const qreal t = 0.05;
	qreal iFps = 1E9 / m_timer.nsecsElapsed();
	m_fps = (1.0 - t)*m_fps + t*iFps;
	this->setWindowTitle(QString::number(m_fps));

    renderer->setWidth(width());
    renderer->setHeight(height());
	//renderer->draw();
	renderer->deferred_draw();
	m_timer.restart();
}

bool myGLWindow::event(QEvent *event)
{
	if (event->type() == QEvent::MouseMove) {
		QMouseEvent * e = static_cast<QMouseEvent *>(event);
		if (e->buttons() == Qt::LeftButton)
			renderer->rotateObjects(glm::vec2(e->x(), e->y()));
		if (e->buttons() == Qt::RightButton)
			renderer->translateCamera(glm::vec2(e->x(), e->y()));
		repaint();
		return true;
	}
	else if (event->type() == QEvent::Wheel)
	{
		QWheelEvent * e = static_cast<QWheelEvent *>(event);
		renderer->zoom(static_cast<float>(e->delta()));
		repaint();
		return true;
	}
	return QOpenGLWidget::event(event);
}

void myGLWindow::screenShot()
{
	int n = 3 * renderer->getWidth() * renderer->getHeight();
	BYTE* pixels = new BYTE[n];

	glReadPixels(0, 0, renderer->getWidth(), renderer->getHeight(), GL_RGB, GL_UNSIGNED_BYTE, pixels);

	cout << n << endl;
	CImg<unsigned char> image(renderer->getWidth(), renderer->getHeight(), 1 , 3);

	int pos = 0;
	for (int r = renderer->getHeight() - 1; r >= 0; r--)
	{
		for (int c = 0; c < renderer->getWidth(); c++)
		{

			image(c, r, 0, 0) = pixels[pos];
			pos++;
			image(c, r, 0, 1) = pixels[pos];
			pos++;
			image(c, r, 0, 2) = pixels[pos];
			pos++;
		}
	}

	image.save("ss.png");
}

void myGLWindow::keyPressEvent(QKeyEvent* e)
{
	switch (e->key())
	{
	case Qt::Key::Key_A:
		setVisualizationMode(0);
		break;
	case Qt::Key::Key_S:
		setVisualizationMode(1);
		break;
	case Qt::Key::Key_D:
		setVisualizationMode(2);
		break;
	case Qt::Key::Key_C:
		screenShot();
		break;
	}
}

void myGLWindow::setVisualizationMode(int mode)
{ 
	if (renderer->getNumberOfShapes() > 0)
	{
		renderer->setShapeDrawMode(0, (myDrawFlags)mode);
		repaint();
	}
}

void myGLWindow::setShape(ShapeData * _shape)
{
	renderer->clearShapes();
	renderer->addShape(_shape);
	renderer->setAlbedo(0, 0, 0);
	renderer->setBumpMap(0, 1, 0);
	makeCurrent();
	renderer->resendDataSingleBuffer();
	doneCurrent();
	renderer->setDefaultValues();
	repaint();
}

void myGLWindow::removeSelection()
{
	if (renderer->getNumberOfShapes() > 1)
	{
		renderer->removeShape(1);
		makeCurrent();
		renderer->resendDataSingleBuffer();
		doneCurrent();
		repaint();
	}
	else
	{
		repaint();
	}
}

void myGLWindow::setSelection(ShapeData * _selection)
{
	if (renderer->getNumberOfShapes() <= 1)
	{
		renderer->addShape(_selection, e_draw_selection);
	}
	else
	{
		renderer->removeShape(1);
		renderer->addShape(_selection,e_draw_selection);
	}
	makeCurrent();
	renderer->resendDataSingleBuffer();
	doneCurrent();
	repaint();
}

void myGLWindow::updateMesh()
{
	if (renderer->getNumberOfShapes() > 0)
	{
		makeCurrent();
		renderer->updateVertexBuffer(0);
		doneCurrent();
		repaint();
	}
}

void myGLWindow::addShader(GLenum _shaderType, const string & _fileName)
{
	makeCurrent();
	renderer->addShader(_shaderType,_fileName);
	doneCurrent();
}

void myGLWindow::clearAndDeleteShaders()
{
	makeCurrent();
	renderer->clearAndDeleteShaders();
	doneCurrent();
}

void myGLWindow::installShaders()
{
	makeCurrent();
	renderer->installShaders();
	doneCurrent();
}

glm::vec2 myGLWindow::getCurrentMousePosition()
{ 
	QPoint p = mapFromGlobal(QCursor::pos()); return glm::vec2(p.x(), p.y());
}

glm::vec3 myGLWindow::getRayDirection(glm::vec2 & pos)
{ 
	return renderer->getRayDirection(pos);
}

myCamera * myGLWindow::getCamera()
{ 
    return renderer->getCamera();
}

glm::mat4 myGLWindow::getModelToWorldMatrix()
{ 
    return renderer->getModelToWorldMatrix();
}
