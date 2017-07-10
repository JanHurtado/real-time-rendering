#ifndef MYRENDERER_H
#define MYRENDERER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>

#include "myShader.h"
#include "myShape.h"
#include "myCamera.h"
#include "myTexture.h"
#include "mySampler.h"
#include "../DemoUI/motion.h"
#include "../Pinocchio/skeleton.h"

#include "CImg.h"
using namespace cimg_library;

/** @addtogroup visualization
  * @brief Renderer.
  *
  * @{
  */

/**
 * @brief The myDrawFlags enum - Rendering mode
 */
enum myDrawFlags {
    e_draw_faces, /**< draw mesh faces */
    e_draw_wireframe, /**< draw mesh edges */
    e_draw_points, /**< draw mesh points (vertices) */
    e_draw_selection /**< draw as a selection (red points / customizable) */
};

/**
 * @brief The myRenderer class - Renderer for multiple shapes (triangular meshes) visualization
 */
class myRenderer
{
public:
    /**
     * @brief myRenderer - default constructor
     */
	myRenderer();
    /**
     * @brief ~myRenderer - destructor
     */
	virtual ~myRenderer();
    /**
     * @brief addShape - add a shape (shape data) for rendering
     * @param _sd shape data pointer to be added
     */
	void addShape(ShapeData * _sd);
    /**
     * @brief addShape - add a shape (shape data) for rendering specifying its rendering mode
     * @param _sd shape data pointer to be added
     * @param _draw_mode rendering mode
     */
	void addShape(ShapeData * _sd, myDrawFlags _draw_mode);
    /**
     * @brief setShapeDrawMode - set rendering mode for a specific shape
     * @param _shape_index index of the target shape
     * @param _mode rendering mode for the target shape
     */
    void setShapeDrawMode(size_t _shape_index, myDrawFlags _mode);
    /**
     * @brief removeShape - remove a shape
     * @param index index of the target shape
     */
	void removeShape(size_t index);
    /**
     * @brief addShader - create and add a shader
     * @param _shaderType shader type
     * @param _fileName shader code file name
     */
	void addShader(GLenum _shaderType, const string & _fileName);
    /**
     * @brief addShader - add a shader
     * @param _shader pointer to the shader
     */
	void addShader(myShader * _shader);
    /**
     * @brief clearShapes - erase shape containers without deleting pointers
     */


	void addTexture(const char * file_name);
	void addSampler();

	void createTextures();
	void createSamplers();

	void setUpTextures();
	void setUpSamplers();


	void setAlbedo(int shape_index, int texture_index, int sampler_index);

	void setBumpMap(int shape_index, int texture_index, int sampler_index);


	void clearShapes();
    /**
     * @brief clearAndDeleteShapes - erate shape containers and delete items
     */
	void clearAndDeleteShapes();
    /**
     * @brief clearShaders - erase shader containers without deleting pointers
     */
	void clearShaders();
    /**
     * @brief clearAndDeleteShaders - erase shader containers and delete items
     */
	void clearAndDeleteShaders();
    /**
     * @brief createProgram - create OpenGl program in the current context
     */
	void createProgram();
    /**
     * @brief installShaders - compile and attach shaders to the main program (m_programID) in the current context
     * @return success
     */
	bool installShaders();
    /**
     * @brief sendDataSingleBuffer - send shape data to the GPU using OpenGL
     */
	void sendDataSingleBuffer();
    /**
     * @brief updateVertexBuffer - update and send vertex data for a single shape
     * @param index target shape index
     */
	void updateVertexBuffer(size_t index);
    /**
     * @brief resendDataSingleBuffer - resend shape data to the GPU using OpenGL
     */
	void resendDataSingleBuffer();

    /**
     * @brief draw - draw al shapes considering their rendering modes
     */
	void draw();
    /**
     * @brief initialize - renderer initialization
     */
	void initialize();
    /**
     * @brief initializeInteractor - default initialization of renderer parameters based on shape data
     */
	void initializeInteractor();
    /**
     * @brief setDefaultValues - set custom renderer parameters
     */
	void setDefaultValues();
    /**
     * @brief computeCentralPoint - compute the centroid of all points of all shapes
     */
	void computeCentralPoint();
    /**
     * @brief computeBoundingSphereRadius - compute the bounding sphere radius centered on the centroid of all points of all shapes
     */
	void computeBoundingSphereRadius();
    /**
     * @brief rotateObjects - rotate all shapes (model to world matrix manipulation) regarding a mouse movement based interaction
     * @param newMousePosition current mouse position
     */
	void rotateObjects(const glm::vec2& newMousePosition);
    /**
     * @brief translateCamera - translate all shapes (model to world matrix manipulation) regarding a mouse movement interaction
     * @param newMousePosition current mouse position
     */
	void translateCamera(const glm::vec2& newMousePosition);
    /**
     * @brief zoom - translate camera position (forward/backward) regarding a step size
     * @param delta - step size
     */
	void zoom(float delta);

    /**
     * @brief getProgramID - get OpenGL program ID of renderer
     * @return OpenGL porgram ID
     */
	GLuint getProgramID(){ return m_programID; }
    /**
     * @brief getIndexOffsetAt - get indices offset for a shape
     * @param pos index of the target shape
     * @return offset
     */
	GLsizeiptr getIndexOffsetAt(size_t pos){ return m_elementOffsets[pos]; }
    /**
     * @brief getVertexArrayObjectIDAt - get vertex array object ID (OpenGL) for a shape
     * @param pos index of the target shape
     * @return vertex array object ID (OpenGL)
     */
	GLuint getVertexArrayObjectIDAt(size_t pos){ return m_vertexArrayObjectIDs[pos]; }
    /**
     * @brief getNumberOfShapes - get the number of shapes contained in the renderer
     * @return number of shapes
     */
	size_t getNumberOfShapes(){ return m_shapes.size(); }
    /**
     * @brief getRayDirection - computation of the direction of a ray starting in camera position and ending in a point of the corresponding window contained in the near plane
     * @param pos position in the window (contained in the near plane)
     * @return ray direction
     */
	glm::vec3 getRayDirection(glm::vec2 & pos);

    /////////////////////////////////////////////////
    /// getters
    /////////////////////////////////////////////////
    GLuint getWidth(){return m_width;}
    GLuint getHeight(){return m_height;}
    GLfloat getFOV(){return m_fov;}
    GLfloat getNear(){return m_near;}
    GLfloat getFar(){return m_far;}
    glm::mat4 getModelToWorldMatrix(){return modelToWorldMatrix;}
    glm::vec3 getSceneCentralPoint(){return sceneCentralPoint;}
    float getBoundingSphereRadius(){return boundingSphereRadius;}
    myCamera * getCamera(){return &camera;}

    /////////////////////////////////////////////////
    /// setters
    /////////////////////////////////////////////////
    void setWidth(GLuint _width){m_width = _width;}
    void setHeight(GLuint _height){m_height = _height;}
    void setFOV(GLfloat _fov){m_fov = _fov;}
    void setNear(GLfloat _near){m_near = _near;}
    void setFar(GLfloat _far){m_far = _far;}
    void setModelToWorldMatrix(glm::mat4 & _modelToWorldMatrix){modelToWorldMatrix = _modelToWorldMatrix;}
    void setSceneCentralPoint(glm::vec3 & _point){sceneCentralPoint = _point;}
    void setBoundingSphereRadius(float _radius){boundingSphereRadius = _radius;}

private:

    /////////////////////////////////////////////////
    /// Rendering
    /////////////////////////////////////////////////
    /**
     * @brief m_width - window width
     */
    GLuint m_width;
    /**
     * @brief m_height - window height
     */
    GLuint m_height;
    /**
     * @brief m_fov - field of view (FOV)
     */
    GLfloat m_fov;
    /**
     * @brief m_near - near plane distance
     */
    GLfloat m_near;
    /**
     * @brief m_far - far plane distance
     */
    GLfloat m_far;
    /**
     * @brief camera - main camera for renderer
     */
    myCamera camera;
    /**
     * @brief light - main light "color"
     */
    glm::vec4 light;
    /**
     * @brief lightPosition - main light position
     */
    glm::vec3 lightPosition;

    /////////////////////////////////////////////////
    /// Interaction
    /////////////////////////////////////////////////
    /**
     * @brief modelToWorldMatrix - 4x4 model to world matrix (for all shapes)
     */
    glm::mat4 modelToWorldMatrix;
    /**
     * @brief oldMousePosition - old mouse position for mouse move interactions
     */
    glm::vec2 oldMousePosition;

    /**
     * @brief sceneCentralPoint - scene central point
     */
    glm::vec3 sceneCentralPoint;
    /**
     * @brief boundingSphereRadius - bounding sphere radius (regarding the central point)
     */
    float boundingSphereRadius;

    /////////////////////////////////////////////////
    /// Data
    /////////////////////////////////////////////////


    /**
     * @brief m_programID - main program ID
     */
    GLuint m_programID;
    /**
     * @brief m_vertexBufferID - main vertex buffer ID
     */
    GLuint m_vertexBufferID;
    /**
     * @brief m_elementBufferID - main element buffer ID
     */
    GLuint m_elementBufferID;
    /**
     * @brief m_shaders - vector containing all shaders
     */
	vector<myShader*> m_shaders;
    /**
     * @brief m_shapes - vector containing all shapes
     */
	vector<ShapeData*> m_shapes;
    /**
     * @brief m_vertexOffsets - vector containing all shape vertices offsets
     */
	vector<GLsizeiptr> m_vertexOffsets;
    /**
     * @brief m_elementOffsets - vector containing all shape indices offsets
     */
    vector<GLsizeiptr> m_elementOffsets;
    /**
     * @brief m_vertexArrayObjectIDs - vector containing all vertex array objects IDs
     */
	vector<GLuint> m_vertexArrayObjectIDs;
    /**
     * @brief m_draw_modes - vector containing all shape rendering modes
     */
    vector<myDrawFlags> m_draw_modes;

	vector<myTexture*> m_textures;
	vector<mySampler*> m_samplers;


	vector<GLuint> m_albedoIDs;
	vector<GLuint> m_albedoSamplerIDs;

	vector<GLuint> m_bumpMapIDs;
	vector<GLuint> m_bumpMapSamplerIDs;


	//////////////////////////////////////////////////////
	//////////////////// DEFERRED ////////////////////////
	//////////////////////////////////////////////////////
	/**
	What coordinates do you do your lighting in? To illustrate, I'm going to assume world-space below.

	Normal maps are usually stored in tangent-space. Your vertex's T, B, and N values form a 3x3 rotation matrix, which rotates from model-space into tangent-space.
	The transpose of a rotation matrix is the same as it's inverse, so transpose(TBN) rotates tangent-space values back into model-space.

	By using your world matrix (model-space to world-space transform) and your TBN, you can rotate your normal-map values into world-space, and then write them into your g-buffer as usual.


	To contrast, the method you're reading about performs it's lighting calculations in tangent-space. This means that they don't have to perform per-pixel rotations on their normal maps, 
	but they do have to perform per-pixel rotation of the light and camera instead. There's no "right space" to do your calculations in -- it's just each will require different parameters 
	to be transformed in different ways.

	For certain kinds of forward-renderers, it turns out that tangent-space lighting can be the optimal choice (least transforms), but deferred renderers usually use world or view-space 
	(tangent space would require writing the TBN into your G-Buffer!).
	*/
public:

	GLuint deferred1_programID;
	GLuint deferred2_programID;

	GLuint deferred_GBuffer;

	GLuint deferred_positionTex;
	GLuint deferred_normalTex;
	GLuint deferred_albedoAndSpecTex;

	GLuint deferred_depthRBO;

	myShader * deferred1_vertexShader;
	myShader * deferred1_fragmentShader;

	myShader * deferred2_vertexShader;
	myShader * deferred2_fragmentShader;

	const GLuint NR_LIGHTS = 32;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;

	GLuint quadVAO;
	GLuint quadVBO;

	void deferred_createPrograms();

	void deferred_installShaders();

	void deferred_setUpGBuffer();

	void deferred_generateLights();

	void deferred_draw();


public:
	bool animate;

	Motion * motion;
	FileSkeleton * skeleton;
	vector<vector<float> > weights;
	vector<glm::vec3> originalPositions;
	vector<glm::vec3> originalNormals;
	vector<glm::vec3> originalTangents;
	vector<glm::vec3> originalBitangents;

	void setUpAnimation();
	void updateAnimation();

	vector<Transform<> > computeTransforms(Motion * motion, Skeleton & origSkel);
	vector<vector<float> > loadBoneWeights(int number_of_bones, int number_of_vertices, const char * filename);
	void updateShapeData(ShapeData & sd, Motion * motion, Skeleton & skeleton, vector<vector<float> > & weights);
	void sendMotionData(GLuint _programID);
};

/** @} */

#endif // MYRENDERER_H
