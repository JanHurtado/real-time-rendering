#include "myRenderer.h"

///////////////////////COMMON///////////////////////// (begin)

const uint NUM_FLOATS_PER_VERTICE = 17;
const uint VERTEX_BYTE_SIZE = NUM_FLOATS_PER_VERTICE * sizeof(float);


bool checkStatus(GLuint objectID, PFNGLGETSHADERIVPROC objectPropertyGetterFunc, PFNGLGETSHADERINFOLOGPROC getInfoLogFunc, GLenum statusType)
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar * buffer = new GLchar[infoLogLength];
		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		cout << buffer << endl;
		delete[] buffer;
		return false;
	}
	return true;
}

bool checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID)
{
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

///////////////////////COMMON///////////////////////// (end)


myRenderer::myRenderer()
{
	//currentDrawFlag = e_draw_faces;
	light = glm::vec4(0.1f,0.1f,0.1f,1.0f);
	lightPosition = glm::vec3(800.f, 800.f, 800.f);
	sceneCentralPoint = glm::vec3(0.0f, 0.0f, 0.0f);
	m_width = 1920;
	m_height = 1080;
	m_fov = 45.0f;
	m_near = 1.0f;
	m_far = 1000.0f;
	modelToWorldMatrix = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
}

myRenderer::~myRenderer()
{
	glDeleteBuffers(1, &m_vertexBufferID);
	glDeleteBuffers(1, &m_elementBufferID);
	glUseProgram(0);
	glDeleteProgram(m_programID);
}

void myRenderer::addShape(ShapeData * _sd)
{
	m_shapes.push_back(_sd);
	m_vertexOffsets.push_back(0);
	m_elementOffsets.push_back(0);
	m_vertexArrayObjectIDs.push_back(0);
	m_draw_modes.push_back(e_draw_faces);
	m_albedoIDs.push_back(0);
	m_albedoSamplerIDs.push_back(0);
	m_bumpMapIDs.push_back(0);
	m_bumpMapSamplerIDs.push_back(0);
}

void myRenderer::addShape(ShapeData * _sd , myDrawFlags _draw_mode)
{
	m_shapes.push_back(_sd);
	m_vertexOffsets.push_back(0);
	m_elementOffsets.push_back(0);
	m_vertexArrayObjectIDs.push_back(0);
	m_draw_modes.push_back(_draw_mode);
	m_albedoIDs.push_back(0);
	m_albedoSamplerIDs.push_back(0);
	m_bumpMapIDs.push_back(0);
	m_bumpMapSamplerIDs.push_back(0);
}

void myRenderer::removeShape(size_t index)
{
	glDeleteVertexArrays(1, &m_vertexArrayObjectIDs[index]);
	m_shapes.erase(m_shapes.begin() + index);
	m_vertexOffsets.erase(m_vertexOffsets.begin() + index);
	m_elementOffsets.erase(m_elementOffsets.begin() + index);
	m_vertexArrayObjectIDs.erase(m_vertexArrayObjectIDs.begin() + index);
	m_draw_modes.erase(m_draw_modes.begin() + index);
	m_albedoIDs.erase(m_albedoIDs.begin()+index);
	m_albedoSamplerIDs.erase(m_albedoSamplerIDs.begin() + index);
	m_bumpMapIDs.erase(m_bumpMapIDs.begin() + index);
	m_bumpMapSamplerIDs.erase(m_bumpMapSamplerIDs.begin() + index);
}

void myRenderer::addShader(GLenum _shaderType, const string & _fileName)
{
	myShader * t_ptr_shader = new myShader(_shaderType);
	t_ptr_shader->readShaderCode(_fileName);
	m_shaders.push_back(t_ptr_shader);
}

void myRenderer::addShader(myShader * _shader)
{
	m_shaders.push_back(_shader);
}

void myRenderer::setShapeDrawMode(size_t _shape_index, myDrawFlags _mode)
{
	m_draw_modes[_shape_index] = _mode;
}

void myRenderer::clearShapes()
{
	for (size_t i = 0; i < m_vertexArrayObjectIDs.size(); i++)
	{
		glDeleteVertexArrays(1, &m_vertexArrayObjectIDs[i]);
	}
	m_shapes.clear();
	m_vertexOffsets.clear();
	m_elementOffsets.clear();
	m_vertexArrayObjectIDs.clear();
	m_draw_modes.clear();
	m_albedoIDs.clear();
	m_albedoSamplerIDs.clear();
	m_bumpMapIDs.clear();
	m_bumpMapSamplerIDs.clear();
}

void myRenderer::clearAndDeleteShapes()
{
	for (size_t i = 0; i < m_shapes.size(); i++)
	{
		glDeleteVertexArrays(1, &m_vertexArrayObjectIDs[i]);
		delete m_shapes[i];
	}
	m_shapes.clear();
	m_vertexOffsets.clear();
	m_elementOffsets.clear();
	m_vertexArrayObjectIDs.clear();
	m_draw_modes.clear();
	m_albedoIDs.clear();
	m_albedoSamplerIDs.clear();
	m_bumpMapIDs.clear();
	m_bumpMapSamplerIDs.clear();
}

void myRenderer::clearShaders()
{
	m_shaders.clear();
}

void myRenderer::clearAndDeleteShaders()
{
	for (size_t i = 0; i < m_shaders.size(); i++)
	{
		glDetachShader(m_programID,m_shaders[i]->getShaderID());
		delete m_shaders[i];
	}
	m_shaders.clear();
}

void myRenderer::createProgram()
{
	m_programID = glCreateProgram();
}

bool myRenderer::installShaders()
{
	for (size_t i = 0; i < m_shaders.size(); i++)
	{
		m_shaders[i]->createShader();
		m_shaders[i]->compileShader();
		if (!checkShaderStatus(m_shaders[i]->getShaderID())) return false;
		glAttachShader(m_programID, m_shaders[i]->getShaderID());
	}
	glLinkProgram(m_programID);
	if (!checkProgramStatus(m_programID)) return false;
	glUseProgram(m_programID);
	return true;
}

void myRenderer::sendDataSingleBuffer()
{
	GLsizeiptr totalVertexBufferSize = 0;
	GLsizeiptr totalElementBufferSize = 0;
	for (size_t i = 0; i < m_shapes.size(); i++)
	{
		totalVertexBufferSize += m_shapes[i]->vertexBufferSize();
		totalElementBufferSize += m_shapes[i]->indexBufferSize();
	}

	glGenBuffers(1, &m_vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, totalVertexBufferSize, 0, GL_DYNAMIC_DRAW);
	GLsizeiptr currentVertexOffset = 0;
	for (size_t i = 0; i < m_shapes.size(); i++)
	{
		m_vertexOffsets[i] = currentVertexOffset;
		glBufferSubData(GL_ARRAY_BUFFER, currentVertexOffset, m_shapes[i]->vertexBufferSize(), m_shapes[i]->vertices);
		currentVertexOffset += m_shapes[i]->vertexBufferSize();
	}

	glGenBuffers(1, &m_elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalElementBufferSize, 0, GL_STATIC_DRAW);
	GLsizeiptr currentElementOffset = 0;
	for (size_t i = 0; i < m_shapes.size(); i++)
	{
		m_elementOffsets[i] = currentElementOffset;
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, currentElementOffset, m_shapes[i]->indexBufferSize(), m_shapes[i]->indices);
		currentElementOffset += m_shapes[i]->indexBufferSize();
	}

	for (size_t i = 0; i < m_shapes.size(); i++)
	{
		//glDeleteVertexArrays(1, &m_vertexArrayObjectIDs[i]);
		glGenVertexArrays(1, &m_vertexArrayObjectIDs[i]);
		
		glBindVertexArray(m_vertexArrayObjectIDs[i]);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(m_vertexOffsets[i]));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(m_vertexOffsets[i] + sizeof(float)* 3));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(m_vertexOffsets[i] + sizeof(float)* 6));
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(m_vertexOffsets[i] + sizeof(float)* 9));
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(m_vertexOffsets[i] + sizeof(float)* 11));
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(m_vertexOffsets[i] + sizeof(float)* 14));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferID);
	}
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferID);

}

void myRenderer::initialize()
{
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	createProgram();
	sendDataSingleBuffer();
	installShaders();
	initializeInteractor();
	createTextures();
	createSamplers();
	setUpTextures();
	setUpSamplers();
	//setTexturesAndSamplers();
	////////deferred
	deferred_createPrograms();
	deferred_installShaders();
	deferred_setUpGBuffer();
	deferred_generateLights();


	printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));
}

void myRenderer::initializeInteractor()
{
	computeCentralPoint();
	computeBoundingSphereRadius();
	camera.setPosition(sceneCentralPoint+glm::vec3(0.0f,0.0f,1.0f*boundingSphereRadius));
	camera.setMovementSpeed(boundingSphereRadius/50.0f);
}

void myRenderer::setDefaultValues()
{
	//currentDrawFlag = e_draw_faces;
	sceneCentralPoint = glm::vec3(0.0f, 0.0f, 0.0f);
	m_width = 1920;
	m_height = 1080;
	m_fov = 45.0f;
	m_near = 0.01f;
	m_far = 1000.0f;
	modelToWorldMatrix = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
	initializeInteractor();
}

void myRenderer::resendDataSingleBuffer()
{
	GLsizeiptr totalVertexBufferSize = 0;
	GLsizeiptr totalElementBufferSize = 0;
	for (size_t i = 0; i < m_shapes.size(); i++)
	{
		totalVertexBufferSize += m_shapes[i]->vertexBufferSize();
		totalElementBufferSize += m_shapes[i]->indexBufferSize();
	}

	//glGenBuffers(1, &m_vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, totalVertexBufferSize, 0, GL_DYNAMIC_DRAW);
	GLsizeiptr currentVertexOffset = 0;
	for (size_t i = 0; i < m_shapes.size(); i++)
	{
		m_vertexOffsets[i] = currentVertexOffset;
		glBufferSubData(GL_ARRAY_BUFFER, currentVertexOffset, m_shapes[i]->vertexBufferSize(), m_shapes[i]->vertices);
		currentVertexOffset += m_shapes[i]->vertexBufferSize();
	}

	//glGenBuffers(1, &m_elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalElementBufferSize, 0, GL_STATIC_DRAW);
	GLsizeiptr currentElementOffset = 0;
	for (size_t i = 0; i < m_shapes.size(); i++)
	{
		m_elementOffsets[i] = currentElementOffset;
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, currentElementOffset, m_shapes[i]->indexBufferSize(), m_shapes[i]->indices);
		currentElementOffset += m_shapes[i]->indexBufferSize();
	}
	//cout << "vertex buffer : "<< m_vertexBufferID << endl;
	for (size_t i = 0; i < m_shapes.size(); i++)
	{
		glGenVertexArrays(1, &m_vertexArrayObjectIDs[i]);
		
		glBindVertexArray(m_vertexArrayObjectIDs[i]);

		//cout << m_vertexArrayObjectIDs[i] << endl;
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(m_vertexOffsets[i]));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(m_vertexOffsets[i] + sizeof(float)* 3));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(m_vertexOffsets[i] + sizeof(float)* 6));
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(m_vertexOffsets[i] + sizeof(float)* 9));
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(m_vertexOffsets[i] + sizeof(float)* 11));
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(m_vertexOffsets[i] + sizeof(float)* 14));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferID);
	}
}

void myRenderer::draw()
{
	glUseProgram(m_programID);

	glViewport(0, 0, m_width, m_height);
	//glClearColor(0, 0, 0, 1);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glm::mat4 modelToProjectionMatrix;
	glm::mat4 viewToProjectionMatrix = glm::perspective(m_fov, ((float)m_width) / ((float)(m_height)), m_near, m_far);
	glm::mat4 worldToViewMatrix = camera.getWorldToViewMatrix();
	glm::mat4 worldToProjectionMatrix = viewToProjectionMatrix * worldToViewMatrix;

	// 0: pointSize            1: not defined             2: not defined
	glm::vec3 aditionalProperties = glm::vec3(1.0f,0.0f,0.0f);

	GLuint fullTransformationUniformLocation;
	GLuint modelToWorldMatrixUniformLocation;
	GLuint lightUniformLocation;
	GLuint lightPositionUniformLocation;
	GLuint cameraPositionUniformLocation;
	GLuint aditionalPropertiesUniformLocation;
	GLuint textureSamplerLocation;
	GLuint bumpMapSamplerLocation;

	fullTransformationUniformLocation = glGetUniformLocation(m_programID, "modelToProjectionMatrix");
	modelToWorldMatrixUniformLocation = glGetUniformLocation(m_programID, "modelToWorldMatrix");
	lightUniformLocation = glGetUniformLocation(m_programID, "lightVector");
	lightPositionUniformLocation = glGetUniformLocation(m_programID, "lightPositionVector");
	cameraPositionUniformLocation = glGetUniformLocation(m_programID, "cameraPositionVector");
	aditionalPropertiesUniformLocation = glGetUniformLocation(m_programID, "aditionalProperties");
	textureSamplerLocation = glGetUniformLocation(m_programID, "mainTexture");
	bumpMapSamplerLocation = glGetUniformLocation(m_programID, "mainBumpMap");

	modelToProjectionMatrix = worldToProjectionMatrix * modelToWorldMatrix;

	glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &modelToWorldMatrix[0][0]);
	glUniform4fv(lightUniformLocation, 1, &light[0]);
	glUniform3fv(lightPositionUniformLocation, 1, &lightPosition[0]);
	glUniform3fv(cameraPositionUniformLocation, 1, &(camera.getPosition())[0]);
	glUniform3fv(aditionalPropertiesUniformLocation, 1, &aditionalProperties[0]);

	glUniform1i(textureSamplerLocation, 0);
	glUniform1i(bumpMapSamplerLocation, 2);

	/*glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, _texture);
	glBindSampler(0, _textureSampler);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, _bumpMap);
	glBindSampler(2, _bumpMapSampler);*/

	/*glBindSampler(0, _textureSampler);
	glBindTextureUnit(0, _texture);

	glBindSampler(1, _bumpMapSampler);
	glBindTextureUnit(1, _bumpMap);*/

	for (size_t i = 0; i < m_shapes.size(); i++)
	{
		glUseProgram(m_programID);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, m_albedoIDs[i]);
		glBindSampler(0, m_albedoSamplerIDs[i]);
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, m_bumpMapIDs[i]);
		glBindSampler(2, m_bumpMapSamplerIDs[i]);

		glBindVertexArray(m_vertexArrayObjectIDs[i]);
		int currentDrawFlag = m_draw_modes[i];
		switch (currentDrawFlag)
		{
		case e_draw_faces:
			glDrawElements(GL_TRIANGLES, m_shapes[i]->numIndices, GL_UNSIGNED_INT, (void*)(m_elementOffsets[i])); 
			break;
		case e_draw_wireframe: 
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawElements(GL_TRIANGLES, m_shapes[i]->numIndices, GL_UNSIGNED_INT, (void*)(m_elementOffsets[i]));
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case e_draw_points:
			glDrawArrays(GL_POINTS, 0, m_shapes[i]->numVertices);
			break;
		case e_draw_selection:
			aditionalProperties[0] = 3.0f;
			glUniform3fv(aditionalPropertiesUniformLocation, 1, &aditionalProperties[0]);
			glDrawArrays(GL_POINTS, 0, m_shapes[i]->numVertices);
			aditionalProperties[0] = 1.0f;
			break;
		}
		glBindVertexArray(0);
	}
}

void myRenderer::updateVertexBuffer(size_t index)
{
	glBindBuffer(55, m_vertexBufferID);
	glBufferSubData(55, m_vertexOffsets[index], m_shapes[index]->vertexBufferSize(), m_shapes[index]->vertices);
}

void myRenderer::computeCentralPoint()
{
	float num = 0.0f;
	glm::vec3 t_sceneCentralPoint = glm::vec3(0.0f, 0.0f, 0.0f);
	for (size_t i = 0; i < m_shapes.size(); i++)
	{
		for (size_t j = 0; j < m_shapes[i]->numVertices; j++)
		{
			t_sceneCentralPoint += glm::vec3(m_shapes[i]->vertices[j].position.x,m_shapes[i]->vertices[j].position.y,m_shapes[i]->vertices[j].position.z);
			num += 1.0f;
		}
	}
	sceneCentralPoint = t_sceneCentralPoint/num;
}

void myRenderer::computeBoundingSphereRadius()
{
	float max = 0.0f;
	for (size_t i = 0; i < m_shapes.size(); i++)
	{
		for (size_t j = 0; j < m_shapes[i]->numVertices; j++)
		{
			glm::vec3 current_point = glm::vec3(m_shapes[i]->vertices[j].position.x, m_shapes[i]->vertices[j].position.y, m_shapes[i]->vertices[j].position.z);
			float dist = glm::distance(current_point,sceneCentralPoint);
			if (dist>max)
				max = dist;
		}
	}
	boundingSphereRadius = max;

}

void myRenderer::rotateObjects(const glm::vec2& newMousePosition)
{
	glm::vec2 mouseDelta = newMousePosition - oldMousePosition;
	if (glm::length(mouseDelta) > 20.0f || glm::length(mouseDelta) < 1.0f)
	{
		oldMousePosition = newMousePosition;
		return;
	}
	const float ROTATIONAL_SPEED = 0.1f;

	float valx = mouseDelta.x * ROTATIONAL_SPEED;
	float valy = mouseDelta.y * ROTATIONAL_SPEED;

	glm::mat4 transform = glm::translate(glm::vec3(-sceneCentralPoint.x, -sceneCentralPoint.y, -sceneCentralPoint.z));
	glm::mat4 rotator = glm::rotate(valx, glm::vec3(0.0f,1.0f,0.0f)) * glm::rotate(valy, glm::vec3(1.0f,0.0f,0.0f));
	transform = rotator*transform;
	glm::mat4 temp = translate(glm::vec3(sceneCentralPoint.x, sceneCentralPoint.y, sceneCentralPoint.z));
	transform = temp * transform;
	
	modelToWorldMatrix = transform * modelToWorldMatrix;

	oldMousePosition = newMousePosition;
	return;
}

void myRenderer::translateCamera(const glm::vec2& newMousePosition)
{
	glm::vec2 mouseDelta = newMousePosition - oldMousePosition;
	if (glm::length(mouseDelta) > 20.0f)
	{
		oldMousePosition = newMousePosition;
		return;
	}
	float TRANSLATION_SPEED = camera.getMovementSpeed();

	float valx = mouseDelta.x * TRANSLATION_SPEED;
	float valy = mouseDelta.y * TRANSLATION_SPEED;

	camera.setPosition(camera.getPosition()-camera.getUP()*valy+camera.getStrafeDirection()*valx);

	oldMousePosition = newMousePosition;
	return;
}

void myRenderer::zoom(float delta)
{
	if (delta >= 0.0f)
		camera.moveForward();
	else
		camera.moveBackward();
}

glm::vec3 myRenderer::getRayDirection(glm::vec2 & pos)
{
	float x = (2.0f * pos.x) / m_width - 1.0f;
	float y = 1.0f - (2.0f * pos.y) / m_height;
	float z = 1.0f;
	// normalised device space
	glm::vec3 ray_nds = glm::vec3(x, y, z);
	// clip space
	glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
	// eye space
	glm::mat4 proj_mat = glm::perspective(m_fov, ((float)m_width) / ((float)(m_height)), m_near, m_far);
	glm::mat4 view_mat = glm::lookAt(camera.getPosition(), camera.getPosition() + camera.getViewDirection(), camera.getUP());

	glm::vec4 ray_eye = inverse(proj_mat) * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
	// world space
	glm::vec3 ray_wor = glm::vec3(inverse(view_mat) * ray_eye);
	// don't forget to normalise the vector at some point
	ray_wor = glm::normalize(ray_wor);
	return ray_wor;
}



void myRenderer::addTexture(const char * file_name)
{
	m_textures.push_back(new myTexture(file_name));
}

void myRenderer::addSampler()
{
	m_samplers.push_back(new mySampler);
}

void myRenderer::createTextures()
{
	for (int i = 0; i < m_textures.size(); i++)
		m_textures[i]->createTexture();
}

void myRenderer::createSamplers()
{
	for (int i = 0; i < m_samplers.size(); i++)
		m_samplers[i]->createSampler();
}

void myRenderer::setUpTextures()
{
	for (int i = 0; i < m_textures.size(); i++)
		m_textures[i]->setUpTexture();
}

void myRenderer::setUpSamplers()
{
	for (int i = 0; i < m_samplers.size(); i++)
		m_samplers[i]->setUpSampler();
}

void myRenderer::setAlbedo(int shape_index, int texture_index, int sampler_index)
{
	m_albedoIDs[shape_index] = m_textures[texture_index]->m_textureID;
	m_albedoSamplerIDs[shape_index] = m_samplers[sampler_index]->m_textureSamplerID;
}

void myRenderer::setBumpMap(int shape_index, int texture_index, int sampler_index)
{
	m_bumpMapIDs[shape_index] = m_textures[texture_index]->m_textureID;
	m_bumpMapSamplerIDs[shape_index] = m_samplers[sampler_index]->m_textureSamplerID;
}

void myRenderer::deferred_createPrograms()
{
	cout << "create programs  deferred" << endl;
	deferred1_programID = glCreateProgram();
	deferred2_programID = glCreateProgram();

	quadVAO = 0;
	quadVBO = 0;
}

void myRenderer::deferred_installShaders()
{
	cout << "install shaders deferred" << endl;
	deferred1_vertexShader = new myShader(GL_VERTEX_SHADER);
	deferred1_vertexShader->readShaderCode("deferred1_vertexShader.glsl");
	deferred1_vertexShader->createShader();
	deferred1_vertexShader->compileShader();
	if (!checkShaderStatus(deferred1_vertexShader->getShaderID())) return;
	glAttachShader(deferred1_programID, deferred1_vertexShader->getShaderID());

	deferred1_fragmentShader = new myShader(GL_FRAGMENT_SHADER);
	deferred1_fragmentShader->readShaderCode("deferred1_fragmentShader.glsl");
	deferred1_fragmentShader->createShader();
	deferred1_fragmentShader->compileShader();
	if (!checkShaderStatus(deferred1_fragmentShader->getShaderID())) return;
	glAttachShader(deferred1_programID, deferred1_fragmentShader->getShaderID());

	glLinkProgram(deferred1_programID);
	if (!checkProgramStatus(deferred1_programID)) return;

	deferred2_vertexShader = new myShader(GL_VERTEX_SHADER);
	deferred2_vertexShader->readShaderCode("deferred2_vertexShader.glsl");
	deferred2_vertexShader->createShader();
	deferred2_vertexShader->compileShader();
	if (!checkShaderStatus(deferred2_vertexShader->getShaderID())) return;
	glAttachShader(deferred2_programID, deferred2_vertexShader->getShaderID());

	deferred2_fragmentShader = new myShader(GL_FRAGMENT_SHADER);
	deferred2_fragmentShader->readShaderCode("deferred2_fragmentShader.glsl");
	deferred2_fragmentShader->createShader();
	deferred2_fragmentShader->compileShader();
	if (!checkShaderStatus(deferred2_fragmentShader->getShaderID())) return;
	glAttachShader(deferred2_programID, deferred2_fragmentShader->getShaderID());

	glLinkProgram(deferred2_programID);
	if (!checkProgramStatus(deferred2_programID)) return;
}

void myRenderer::deferred_setUpGBuffer()
{
	cout << "set up deferred" << endl;
	glGenFramebuffers(1, &deferred_GBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, deferred_GBuffer);
	// - Position color buffer
	glGenTextures(1, &deferred_positionTex);
	glBindTexture(GL_TEXTURE_2D, deferred_positionTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_width, m_height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, deferred_positionTex, 0);
	// - Normal color buffer
	glGenTextures(1, &deferred_normalTex);
	glBindTexture(GL_TEXTURE_2D, deferred_normalTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_width, m_height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, deferred_normalTex, 0);
	// - Color + Specular color buffer
	glGenTextures(1, &deferred_albedoAndSpecTex);
	glBindTexture(GL_TEXTURE_2D, deferred_albedoAndSpecTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, deferred_albedoAndSpecTex, 0);
	// - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
	// - Create and attach depth buffer (renderbuffer)
	glGenRenderbuffers(1, &deferred_depthRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, deferred_depthRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, deferred_depthRBO);

	// - Finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void myRenderer::deferred_generateLights()
{
	srand(13);
	for (GLuint i = 0; i < NR_LIGHTS; i++)
	{
		// Calculate slightly random offsets
		GLfloat xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		GLfloat yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
		GLfloat zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		//lightPositions.push_back(glm::vec3(8.0f, 8.0f, 8.0f));
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// Also calculate random color
		GLfloat rColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		GLfloat gColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		GLfloat bColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}
}

void myRenderer::deferred_draw()
{

	GLint oldFBO;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);

	// 1. Geometry Pass: render scene's geometry/color data into gbuffer
	glBindFramebuffer(GL_FRAMEBUFFER, deferred_GBuffer);
	glUseProgram(deferred1_programID);

	glViewport(0, 0, m_width, m_height);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glm::mat4 modelToProjectionMatrix;
	glm::mat4 viewToProjectionMatrix = glm::perspective(m_fov, ((float)m_width) / ((float)(m_height)), m_near, m_far);
	glm::mat4 worldToViewMatrix = camera.getWorldToViewMatrix();
	glm::mat4 worldToProjectionMatrix = viewToProjectionMatrix * worldToViewMatrix;

	GLuint fullTransformationUniformLocation;
	GLuint modelToWorldMatrixUniformLocation;
	GLuint cameraPositionUniformLocation;
	GLuint textureSamplerLocation;
	GLuint bumpMapSamplerLocation;

	fullTransformationUniformLocation = glGetUniformLocation(deferred1_programID, "modelToProjectionMatrix");
	modelToWorldMatrixUniformLocation = glGetUniformLocation(deferred1_programID, "modelToWorldMatrix");
	cameraPositionUniformLocation = glGetUniformLocation(deferred1_programID, "cameraPositionVector");
	textureSamplerLocation = glGetUniformLocation(deferred1_programID, "mainTexture");
	bumpMapSamplerLocation = glGetUniformLocation(deferred1_programID, "mainBumpMap");

	modelToProjectionMatrix = worldToProjectionMatrix * modelToWorldMatrix;

	glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &modelToWorldMatrix[0][0]);
	glUniform3fv(cameraPositionUniformLocation, 1, &(camera.getPosition())[0]);

	glUniform1i(textureSamplerLocation, 3);
	glUniform1i(bumpMapSamplerLocation, 4);

	for (size_t i = 0; i < m_shapes.size(); i++)
	{
		glUseProgram(deferred1_programID);
		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_2D, m_albedoIDs[i]);
		glBindSampler(3, m_albedoSamplerIDs[i]);
		glActiveTexture(GL_TEXTURE0 + 4);
		glBindTexture(GL_TEXTURE_2D, m_bumpMapIDs[i]);
		glBindSampler(4, m_bumpMapSamplerIDs[i]);

		glBindVertexArray(m_vertexArrayObjectIDs[i]);
		int currentDrawFlag = m_draw_modes[i];
		switch (currentDrawFlag)
		{
		case e_draw_faces:
			glDrawElements(GL_TRIANGLES, m_shapes[i]->numIndices, GL_UNSIGNED_INT, (void*)(m_elementOffsets[i]));
			break;
		case e_draw_wireframe:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawElements(GL_TRIANGLES, m_shapes[i]->numIndices, GL_UNSIGNED_INT, (void*)(m_elementOffsets[i]));
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case e_draw_points:
			glDrawArrays(GL_POINTS, 0, m_shapes[i]->numVertices);
			break;
		case e_draw_selection:
			glDrawArrays(GL_POINTS, 0, m_shapes[i]->numVertices);
			break;
		}
		glBindVertexArray(0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	glUseProgram(deferred2_programID);

	glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, m_width, m_height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, deferred_positionTex);
	//glBindSampler(0, deferred_GBufferSampler);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, deferred_normalTex);
	/*if (m_albedoIDs.size()>0)
		glBindTexture(GL_TEXTURE_2D, 	m_albedoIDs[0]);*/
	//glBindSampler(1, deferred_GBufferSampler);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, deferred_albedoAndSpecTex);
	//glBindSampler(2, deferred_GBufferSampler);

	// set sampler ????????????

	glUniform1i(glGetUniformLocation(deferred2_programID, "gPosition"), 0);
	glUniform1i(glGetUniformLocation(deferred2_programID, "gNormal"), 1);
	glUniform1i(glGetUniformLocation(deferred2_programID, "gAlbedoSpec"), 2);

	for (GLuint i = 0; i < lightPositions.size(); i++)
	{
		glUniform3fv(glGetUniformLocation(deferred2_programID, ("lights[" + std::to_string(i) + "].Position").c_str()), 1, &lightPositions[i][0]);
		glUniform3fv(glGetUniformLocation(deferred2_programID, ("lights[" + std::to_string(i) + "].Color").c_str()), 1, &lightColors[i][0]);
		// Update attenuation parameters and calculate radius
		const GLfloat constant = 1.0; // Note that we don't send this to the shader, we assume it is always 1.0 (in our case)
		const GLfloat linear = 0.7;
		const GLfloat quadratic = 1.8;
		glUniform1f(glGetUniformLocation(deferred2_programID, ("lights[" + std::to_string(i) + "].Linear").c_str()), linear);
		glUniform1f(glGetUniformLocation(deferred2_programID, ("lights[" + std::to_string(i) + "].Quadratic").c_str()), quadratic);
	}

	GLuint cameraPositionUniformLocation2;
	cameraPositionUniformLocation2 = glGetUniformLocation(deferred2_programID, "cameraPositionVector");
	glUniform3fv(cameraPositionUniformLocation2, 1, &(camera.getPosition())[0]);

	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(0);
}