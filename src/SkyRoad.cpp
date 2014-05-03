/*
 * MainLoop.cpp
 *
 *  Created on: Dec 16, 2013
 *      Author: azmy
 */
#include "include/Angel.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Model.h"
#include "include/texture.hpp"
#include "include/shader.hpp"
#include "include/objloader.hpp"
#include "render.c"
#include "windows.h"
#include "Mmsystem.h"
/*
 * Defines
 */
int numThrown = 0;
#define MAX_THROWN_OUT 5
#define NUM_POINTS 100
#define DISPLACEMENT_MAX -5
#define MODEL_POOL_SIZE 5000
#define Z_MIN -2.5
#define RANG_MIN 1
#define RANG_MAX 3
#define MAX_ZSCALE 7
#define MIN_ZSCALE 7
#define MIN_Z_DIFFRENCE 2 //min depth between rungs of two scenes
#define NUMBER_RENDERED_OBJECTS 2 //number of rendered scenes after DISPLACEMENT_MAX
/*
 * end defines
 */
using namespace std;

GLuint program;

/*
 * GLOBAL VARIABLES
 */

Model *current;

GLuint uniform_TextureID, uniform_mytexture, normal_faces, uniFormAngles;
GLuint p_Texture, c_Texture;
std::vector<glm::vec3> p_vertices, c_vertices;
bool res;
vector<glm::vec2> uvs;
vector<glm::vec3> normals; // no are using now ya osama ya abldelmgeed Ha Ha Haaa
GLuint p_vertexbuffer, c_vertexbuffer, ibo_cube_elements, vbo_cube_texcoords;
GLuint attribute_texcoord;
GLuint attribute_normal, attribute_pos, attribute_uv, attribute_normal_on_faces;
GLuint uvbuffer;
GLuint normalsbuffer;
float yCountDown = 0;
float yCountUp = 0;

int jumpCounter = 0;
bool jumpState = false;
bool isFalling = false;
bool rightJump = false;
bool leftJump = false;
int counter = 0;

float zConut = 0;
float farRenderZ = 2;
int timerWait = 4;

const GLfloat WindowWidth = 600;
const GLfloat WindowHeight = 400;
GLint currentColor;
glm::mat4 view;
glm::mat4 proj;
GLint uniView;
GLint scale;
GLint uniCubeTranslations;
GLint uniformDrawingPlane;
glm::vec3 zOffset;
GLint uniCubeScale;
glm::vec3 cubeScale;

vector<Model*> models;
int numOFmodels;

Model *ModelPool[MODEL_POOL_SIZE];
bool taken[MODEL_POOL_SIZE];
int ModelPoolPointer = 0;
vector<int> lastSceneModelsPos;
Model *jetPlane= new Model();

//cube vertices
static GLfloat g_vertex_buffer_data[] = {
		// front
		-0.25, -0.25, 0.25, 0.25, -0.25, 0.25, 0.25, 0.25, 0.25, -0.25, 0.25,
		0.25,
		// top
		-0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, -0.25, -0.25, 0.25,
		-0.25,
		// back
		0.25, -0.25, -0.25, -0.25, -0.25, -0.25, -0.25, 0.25, -0.25, 0.25, 0.25,
		-0.25,
		// bottom
		-0.25, -0.25, -0.25, 0.25, -0.25, -0.25, 0.25, -0.25, 0.25, -0.25,
		-0.25, 0.25,
		// left
		-0.25, -0.25, -0.25, -0.25, -0.25, 0.25, -0.25, 0.25, 0.25, -0.25, 0.25,
		-0.25,
		// right
		0.25, -0.25, 0.25, 0.25, -0.25, -0.25, 0.25, 0.25, -0.25, 0.25, 0.25,
		0.25, };
// for texture the floor
//hamozo

static GLushort cube_elements[] = {
// front
		0, 1, 2, 2, 3, 0,
		// top
		4, 5, 6, 6, 7, 4,
		// back
		8, 9, 10, 10, 11, 8,
		// bottom
		12, 13, 14, 14, 15, 12,
		// left
		16, 17, 18, 18, 19, 16,
		// right
		20, 21, 22, 22, 23, 20, };
//hamozo
GLfloat cube_texcoords[2*4*6] = {
// front
		0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0};
/*
 * END GLOBAL VARIABLES
 */

Model* getModel();
/**
 * Function Headers
 * */

vector<glm::vec3> normalOnFaces;
void initialize_normal_faces() {
	for (int i = 0; i < 4; i++) {
		normalOnFaces.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	}
	for (int i = 0; i < 4; i++) {
		normalOnFaces.push_back(glm::vec3(0.0f, 1.0f, 0.0f));		////
	}
	for (int i = 0; i < 4; i++) {
		normalOnFaces.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
	}
	for (int i = 0; i < 4; i++) {
		normalOnFaces.push_back(glm::vec3(0.0f, -1.0f, 0.0f));		////
	}
	for (int i = 0; i < 4; i++) {
		normalOnFaces.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
	}
	for (int i = 0; i < 4; i++) {
		normalOnFaces.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	}
}

void init_cube_vertices() {
	for (int i = 0; i < (6 * 6 * 3); i += 3) {
		glm::vec3 vertex = glm::vec3(g_vertex_buffer_data[i],
				g_vertex_buffer_data[i + 1], g_vertex_buffer_data[i + 2]);
		c_vertices.push_back(vertex);
	}
	initialize_normal_faces();
}

void buildSpecial3() {

	for (int i = 0; i < lastSceneModelsPos.size(); ++i) {
		lastSceneModelsPos.pop_back();
		--i;
	}

	Model *next1 = getModel();
	next1->scale.x = 7;
	next1->scale.z = 5;
	next1->pos.x = 0;
	next1->pos.z = farRenderZ + 0.25 * next1->scale.z;
	models.push_back(next1);
	farRenderZ = next1->pos.z + next1->scale.z * 0.25;

	Model *next3 = getModel();
	next3->scale.x = 1;
	next3->scale.z = 5;
	next3->pos.x = -1 + 0.25 * next3->scale.x;
	next3->pos.z = farRenderZ + 0.25 * next3->scale.z;
	models.push_back(next3);
	farRenderZ = next3->pos.z + next3->scale.z * 0.25;

	Model *next2 = getModel();
	next2->scale.x = 7;
	next2->scale.z = 5;
	next2->pos.x = 0;
	next2->pos.z = farRenderZ + 0.25 * next2->scale.z;
	models.push_back(next2);
	farRenderZ = next2->pos.z + next2->scale.z * 0.25;

	Model *next4 = getModel();
	next4->scale.x = 1;
	next4->scale.z = 5;
	next4->pos.x = 1 - 0.25 * next4->scale.x;

	next4->pos.z = farRenderZ + 0.25 * next4->scale.z;
	models.push_back(next4);
	farRenderZ = next4->pos.z + next4->scale.z * 0.25;

	Model *next5 = getModel();
	next5->scale.x = 7;
	next5->scale.z = 5;
	next5->pos.x = 0;
	next5->pos.z = farRenderZ + 0.25 * next4->scale.z;
	models.push_back(next5);
	farRenderZ = next5->pos.z + next5->scale.z * 0.25;

	current = next5;
	lastSceneModelsPos.push_back(0);

}
void buildSpecial2() {
	/*
	 *
	 *                  ----
	 *                  |  |5
	 *                  ----
	 *           ------------------
	 *           |                |4
	 *           |----------------|
	 *           | |            | |
	 *           | |            | |
	 *           | |            | |
	 *           | | 3          | |2
	 *           |-|            |-|
	 *                   | |
	 *                   | |
	 *                   | |
	 *                   | |1
	 *                   | |
	 *                   | |
	 */

	for (int i = 0; i < lastSceneModelsPos.size(); ++i) {
		lastSceneModelsPos.pop_back();
		--i;
	}

	Model *next1 = getModel();
	next1->scale.x = 1;
	next1->scale.z = 7;
	next1->pos.x = 0;
	next1->pos.z = farRenderZ + next1->scale.z * 0.25;
	models.push_back(next1);
	farRenderZ = next1->pos.z + next1->scale.z * 0.25;

	Model *next2 = getModel();
	next2->scale.x = 1;
	next2->scale.z = 10;
	next2->pos.x = 1 - 0.25 * next2->scale.x;
	;
	next2->pos.z = farRenderZ + 0.25 * next2->scale.z;
	models.push_back(next2);

	Model *next3 = getModel();
	next3->scale.x = 1;
	next3->scale.z = 10;
	next3->pos.x = -1 + 0.25 * next3->scale.x;
	next3->pos.z = farRenderZ + 0.25 * next3->scale.z;
	models.push_back(next3);

	farRenderZ = next3->pos.z + next3->scale.z * 0.25;

	Model *next4 = getModel();
	next4->scale.x = 8;
	next4->scale.z = 7;
	next4->pos.x = 0;
	next4->pos.z = farRenderZ + 0.25 * next4->scale.z;
	farRenderZ = next4->pos.z + next4->scale.z * 0.25;
	models.push_back(next4);

	Model *next5 = getModel();
	next5->scale.x = 1;
	next5->scale.z = 4;
	next5->pos.x = 0;
	next5->pos.z = farRenderZ + 0.25 * next5->scale.z;
	farRenderZ = next5->pos.z + next5->scale.z * 0.25;
	models.push_back(next5);

	current = next5;

	lastSceneModelsPos.push_back(0);

}
//special shape 1
//for rendering
void buildSpecialOne() {

	/*
	 * ----------------------------
	 * ----------------------------  4
	 *     pppp          pppp
	 *     pppp3         pppp  2
	 * ----------------------------
	 * ----------------------------  1
	 */

	for (int i = 0; i < lastSceneModelsPos.size(); ++i) {
		lastSceneModelsPos.pop_back();
		--i;
	}

	Model *next1 = getModel();

	next1->scale.x = 7;
	next1->scale.z = MAX_ZSCALE;
	next1->pos.x = 0;
	next1->pos.z = farRenderZ + next1->scale.z * 0.25;
	models.push_back(next1);
	farRenderZ = next1->pos.z + next1->scale.z * 0.25;

	Model *next2 = getModel();

	next2->scale.x = 1;
	next2->scale.z = MAX_ZSCALE;
	next2->pos.x = 1 - 0.25 * next2->scale.x;
	next2->pos.z = farRenderZ + next2->scale.z * 0.25;
	models.push_back(next2);

	Model *next3 = getModel();
	next3->scale.x = 1;
	next3->scale.z = MAX_ZSCALE;
	next3->pos.x = -1 + 0.25 * next3->scale.x;
	next3->pos.z = farRenderZ + next3->scale.z * 0.25;
	models.push_back(next3);

	farRenderZ = next3->pos.z + next3->scale.z * 0.25;

	Model *next4 = getModel();

	next4->scale.x = 7;
	next4->scale.z = MAX_ZSCALE;
	next4->pos.x = 0;
	next4->pos.z = farRenderZ + next4->scale.z * 0.25;
	models.push_back(next4);

	farRenderZ = next4->pos.z + next4->scale.z * 0.25;
	current = next4;

	lastSceneModelsPos.push_back(0);
	/*if (currentLoc == 0) {
	 //left
	 next->pos.x = 1 - 0.25 * next->scale.x;
	 next->pos.z = farRenderZ;
	 next->scale.z = zScale;

	 } else if (currentLoc == 1) {
	 //middle
	 next->pos.x = 0;
	 next->pos.z = farRenderZ;
	 next->scale.z = zScale;
	 } else {
	 //right
	 next->pos.x = -1 `+ 0.25 * next->scale.x;
	 next->pos.z = farRenderZ;
	 next->scale.z = zScale;

	 }

	 */

}

//allocate memory to the
//model pool object
void initModelPool() {
	for (int i = 0; i < MODEL_POOL_SIZE; ++i) {
		Model *current = new Model();
		ModelPool[i] = current;
	}

}
//get next free model
//returns null when the pool is full
Model* getModel() {

	if (ModelPoolPointer == MODEL_POOL_SIZE) {
		ModelPoolPointer = 0;
	}
	Model* next = ModelPool[ModelPoolPointer];
	taken[ModelPoolPointer] = true;
	++ModelPoolPointer;
	return next;
}

void display(void);

///

//free a model from the pool
//simply retract the pool pointer
//as we gurantee that freeing order
//is preserved
//also remove the model from
//being drawed models
void freeModel(int drawingIndex) {
	cout << models.size() << endl;
	cout << "freeing index" << drawingIndex << endl;
	models.erase(models.begin() + drawingIndex);
	cout << models.size() << endl;
	taken[ModelPoolPointer] = false;
	--ModelPoolPointer;
}

//when scene is translated
//a certain displacement
//DISPLACEMENT_MAX
//this method is called
int cuurentDirection = 1;
bool risk = false;
void renderNext() {

//	buildSpecialOne();
//	return;

	/*
	 * using the "Simple" algorithm
	 * choose a random number (range from min to max) of
	 * 1- rangs
	 * 2- obstacles(y scale) but maintain feasibility
	 * 3- random positions of rangs within fixed regions
	 */

	cout << "---------------render-------------------" << endl;
	Model *next;

	//get random number of
	//ranges ranges from
	//RANG_MIN to RANG_MAX
	int rangs = rand() % (RANG_MAX + 1);
	if (rangs < RANG_MIN) {
		rangs = RANG_MIN;
	}

	cout << "number of rangs = " << rangs << endl;
	int obstacles = rand() % (rangs);
	cout << "number of obstacles =" << obstacles << endl;
	bool taken[RANG_MAX];
	for (int i = 0; i < RANG_MAX; ++i) {
		taken[i] = false;
	}

	/////////////enforce a feasible solution////////////////
	//risk is only in three cases |-- and |-| and --|
	// |-| is not completely left as a game challenge
	next = getModel();
	int choosenPos;
	int lastPos = lastSceneModelsPos.at(rand() % lastSceneModelsPos.size());
	int sum = 0;
	if (lastSceneModelsPos.size() == 2) {
		int pos1 = lastSceneModelsPos.at(0);
		int pos2 = lastSceneModelsPos.at(1);
		sum = pos1 + pos2;
		if (sum == 2) {
			//put in middle
			next->pos.x = 0;
			next->pos.z = farRenderZ;
			next->scale.z = MAX_ZSCALE;
			choosenPos = 1;
		}
	}
	if (sum != 2) {
		if (lastPos == 0) {
			//left
			//choose 0(left) or 1(middle)
			int choosen = rand() % 2;
			if (choosen == 0) {
				next->pos.x = 0;
				next->pos.z = farRenderZ;
				next->scale.z = MAX_ZSCALE;
				choosenPos = 1;
			} else {
				next->pos.x = 1 - 0.25 * next->scale.x;
				next->pos.z = farRenderZ;
				next->scale.z = MAX_ZSCALE;
				choosenPos = 0;
			}

		} else if (lastPos == 2) {
			//right
			//choose 1 or 2
			int choosen = rand() % 2;
			if (choosen == 0) {
				next->pos.x = 0;
				next->pos.z = farRenderZ;
				next->scale.z = MAX_ZSCALE;
				choosenPos = 1;
			} else {
				next->pos.x = 0;
				next->pos.z = farRenderZ;
				next->scale.z = MAX_ZSCALE;
				choosenPos = 2;

			}

		} else {
			choosenPos = -100;
			//middle
			//DO NOTHING
		}
	}
	//empty the vector
	for (int i = 0; i < lastSceneModelsPos.size(); ++i) {
		lastSceneModelsPos.pop_back();
		--i;
	}
	for (int i = 0; i < lastSceneModelsPos.size(); ++i) {
		cout << "jhibthieunhign not empty " << lastSceneModelsPos.size()
				<< endl;
	}
	if (choosenPos != -100) {
		models.push_back(next);
		current = next;
		next->pos.z += next->scale.z * 0.25;
		lastSceneModelsPos.push_back(choosenPos);
		--rangs;
		taken[choosenPos] = true;
	}

	//////////////////////////////////////////////////////

	//Arrange the ranges randomly
	for (int i = 0; i < rangs; ++i) {
//		//get random z-scale
//		int zScale = rand() % (MAX_ZSCALE + 1);
//		if (zScale < MIN_ZSCALE)
//			zScale = MIN_ZSCALE;

		/////////////////////
		next = getModel();
		int currentLoc = rand() % (RANG_MAX);
		if (taken[currentLoc] == true) {
			for (int i = 0; i < RANG_MAX; ++i) {
				if (taken[i] == false)
					currentLoc = i;
			}
		}
		taken[currentLoc] = true;
		//TODO make this generic

		//Map the cube coordinates
		//according to the chosen
		//position
		current = next;
		if (currentLoc == 0) {
			//left
			next->pos.x = 1 - 0.25 * next->scale.x;
			next->pos.z = farRenderZ;
			next->scale.z = MAX_ZSCALE;

		} else if (currentLoc == 1) {
			//middle
			next->pos.x = 0;
			next->pos.z = farRenderZ;
			next->scale.z = MAX_ZSCALE;
		} else {
			//right
			next->pos.x = -1 + 0.25 * next->scale.x;
			next->pos.z = farRenderZ;
			next->scale.z = MAX_ZSCALE;

		}
		next->pos.z += next->scale.z * 0.25;

		current = next;

		//add the obstacle
		if (obstacles > 0) {
			//next->scale.y = 2;
			--obstacles;
		} else {
			lastSceneModelsPos.push_back(currentLoc);
		}
		//add the cube to
		//the list being drawn
		models.push_back(next);
	}

}

void update(int _ptr) {

	//cout << "farZframe: --------------" << zConut << endl;
	zConut = zConut - 0.01;
		if (zConut <= DISPLACEMENT_MAX && models.size()<200 ) {

			//prepare new model
			zConut = 0;
			//starting z-index
			// |---------|end-------------|start <--- at time t0
			// |--------------|end(new start)-------------|start   <<--- after DISPLACEMENT_MAX
			// |---------|diff| new start = end + DISPLACEMENT_MAX

			farRenderZ = farRenderZ + DISPLACEMENT_MAX;

			int random = rand() % 3;

			if (numThrown >= MAX_THROWN_OUT) {
				numThrown = 0;
				if (random == 0) {
					buildSpecialOne();
				} else if (random == 1) {
					buildSpecial2();
				} else {
					buildSpecial3();
				}
			}

//
//			if (numThrown >= MAX_THROWN_OUT * 3) {
//
//				//farRenderZ += 5;
//				buildSpecial3();
//				farRenderZ = farRenderZ + zConut;
//				numThrown = 0;
//			} else if (numThrown >= MAX_THROWN_OUT * 2) {
//				buildSpecial2();
//				//farRenderZ += 5;
//			} else if (numThrown >= MAX_THROWN_OUT * 1) {
//				buildSpecialOne();
//				//farRenderZ += 5;
//			}

			for (int i = 0; i < NUMBER_RENDERED_OBJECTS; ++i) {
				renderNext();
				farRenderZ = (current->pos.z + current->scale.z * 0.25);
			}

			cout
					<< "---------------------------passed rendering--------------------------------------"
					<< endl;
			//buildSpecialOne();
		}
	//int size =  models.size();
	for (int i = 0; i < models.size(); i++) {

		models.at(i)->pos.z -= 0.01; //move models toward screen

		//after displacement max
		//render multiple scenes

		if (models.at(i)->pos.z < Z_MIN) {
		cout<<"SIZE BEFOR"<<models.size()<<endl;
			int j = 0;
			while (models.size() != 0 && j < models.size()) {
				//cout << "models.at(j), j = " << j << endl;
				if(models.at(j)->pos.z < Z_MIN){
				models.erase(models.begin() + j);
				++numThrown;
				}
				j++;
			}
			//i fucking need to remove
			//this fuckin object
			//from the models vector
			//freeModel(i);

			cout<<"SIZE AFTER"<<models.size()<<endl;
		}

	}
	glutPostRedisplay();
	glutTimerFunc(timerWait, update, 0);
}
void engine(int _ptr) {

	//glUniform3f(uniCubeTranslations, 0, 0, zOffset.z);

	glutTimerFunc(timerWait, update, 0);
	display();
}
void STARTEngine() {
	cout << "Coming here" << endl;
	engine(0);
}

void intializeModels() {

	initModelPool();
	//buildSpecialOne();
	//buildSpecial2();
	//buildSpecial3();
	//return;

//	for (int i = 0; i < 2; i++) {
////
//	Model *model = getModel();
//	current = model;
//	model->scale = vec3(5, 1, 4);
//	model->pos = vec3(0, 0, farRenderZ);
//	farRenderZ += 5;
//	models.push_back(model);
//////		//AZMY?????????????///////////////
//	lastSceneModelsPos.push_back(0);
////		//////////////////////////////////
////	}

	buildSpecialOne();
	//farRenderZ = current->pos.z + current->scale.z * 0.25;
	for (int i = 0; i < 20; ++i) {
		if (i % 5 == 0) {
			buildSpecialOne();
			//} else if (i % 6 == 0) {
			//buildSpecial2();
			//} else if (i % 7 == 0) {
			//buildSpecial3();
		} else {
			renderNext();
			farRenderZ = current->pos.z + current->scale.z * 0.25;

		}
	}

}

void init(void) {

	//code relating to texture
	for(int i=1;i<6;i++)
		memcpy(&cube_texcoords[i*4*2],&cube_texcoords[0],2*4*sizeof(GLfloat));



	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Load shaders and use the resulting shader program
	program = InitShader("vshader.glsl", "fshader.glsl");

	/////////////////////////////////////////////////////////////////////
	//texturing
	// Load the texture
	p_Texture = loadDDS("FA-22_Raptor_P01.DDS");
	// Get a handle for our "myTextureSampler" uniform
	uniform_TextureID = glGetUniformLocation(program, "myTextureSampler");
	//	// Read our .obj file;

	cout << " vertices normals " << normals.size();
	res = loadOBJ("FA-22_Raptor.obj", p_vertices, uvs, normals);
	init_cube_vertices();
	cout << " vertices normals " << normals.size();

	// Load it into a VBO OF plane
	glGenBuffers(1, &p_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, p_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, p_vertices.size() * sizeof(glm::vec3),
			&p_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0],
			GL_STATIC_DRAW);

	glGenBuffers(1, &normalsbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3),
			&normals[0], GL_STATIC_DRAW);

	//hamozo part
	glGenBuffers(1, &ibo_cube_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements,
			GL_STATIC_DRAW);

	glGenBuffers(1, &vbo_cube_texcoords);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texcoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_texcoords), cube_texcoords,
			GL_STATIC_DRAW);

	glGenBuffers(1, &normal_faces);
	glBindBuffer(GL_ARRAY_BUFFER, normal_faces);
	glBufferData(GL_ARRAY_BUFFER, normalOnFaces.size() * sizeof(glm::vec3),
			&normalOnFaces[0], GL_STATIC_DRAW);

	// Load it into a VBO of cubes
	glGenBuffers(1, &c_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, c_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, c_vertices.size() * sizeof(glm::vec3),
			&c_vertices[0], GL_STATIC_DRAW);

	//for plane==============================
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, p_Texture);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(uniform_TextureID, 0);
	//==================================
	//for cubes
//	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &c_Texture);
	glBindTexture(GL_TEXTURE_2D, c_Texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, // target
			0,  // level, 0 = base, no minimap,
			GL_RGB, // internalformat
			gimp_image.width,  // width
			gimp_image.height,  // height
			0,  // border, always 0 in OpenGL ES
			GL_RGB,  // format
			GL_UNSIGNED_BYTE, // type
			gimp_image.pixel_data);

	//============================

	// 1rst attribute buffer : vertices
	attribute_pos = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(attribute_pos);
	glBindBuffer(GL_ARRAY_BUFFER, p_vertexbuffer);
	glVertexAttribPointer(attribute_pos, // attribute
			3, // size
			GL_FLOAT, // type
			GL_FALSE, // normalized?
			0, // stride
			(void*) 0 // array buffer offset
			);
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(attribute_pos);
	glBindBuffer(GL_ARRAY_BUFFER, c_vertexbuffer);
	glVertexAttribPointer(attribute_pos, // attribute
			3, // size
			GL_FLOAT, // type
			GL_FALSE, // normalized?
			0, // stride
			(void*) 0 // array buffer offset
			);

	// 2nd attribute buffer :UVs
	attribute_uv = glGetAttribLocation(program, "vertexUV");
	glEnableVertexAttribArray(attribute_uv);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(attribute_uv, // attribute
			2, // size
			GL_FLOAT, // type
			GL_FALSE, // normalized?
			0, // stride
			(void*) 0 // array buffer offset
			);

	// 3rd attribute buffer : normals
	attribute_normal = glGetAttribLocation(program, "normal");
	glEnableVertexAttribArray(attribute_normal);
	glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer);
	glVertexAttribPointer(attribute_normal, // attribute
			3, // size
			GL_FLOAT, // type
			GL_FALSE, // normalized?
			0, // stride
			(void*) 0 // array buffer offset
			);
// hamozo

	//  attribute buffer : normals
	attribute_normal_on_faces = glGetAttribLocation(program, "normalFaces");
	glEnableVertexAttribArray(attribute_normal_on_faces);
	glBindBuffer(GL_ARRAY_BUFFER, normal_faces);
	glVertexAttribPointer(attribute_normal_on_faces, // attribute
			3, // size
			GL_FLOAT, // type
			GL_FALSE, // normalized?
			0, // stride
			(void*) 0 // array buffer offset
			);

	attribute_texcoord = glGetAttribLocation(program, "texcoord");
	uniform_mytexture = glGetUniformLocation(program, "mytexture");
	glUniform1i(uniform_mytexture, /*GL_TEXTURE*/1);
	glEnableVertexAttribArray(attribute_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texcoords); /////////////////////////////////////////////////////////////
	glVertexAttribPointer(attribute_texcoord, // attribute
			2,                  // number of elements per vertex, here (x,y)
			GL_FLOAT,           // the type of each element
			GL_FALSE,           // take our values as-is
			0,                  // no extra data between each position
			0                   // offset of first element
			);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, c_Texture);

//end hmozo

	glUseProgram(program);

	// Compute the MVP matrix from keyboard and mouse input

	/////////////////////////////////////////////////////////////////////

	//--------------------------------------------------------
	//calculating view matrix
	view = glm::lookAt(glm::vec3(0.0f, 1.2f, -0.4f),
			glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.5f, 0.0f));
	uniView = glGetUniformLocation(program, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	//----------------------------------
	//calculating projection matrix
	proj = glm::perspective(45.0f, 800.0f / 600.0f, 1.0f, 100.0f);
	GLint uniProj = glGetUniformLocation(program, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	///////////////////////////////////////////////////////////////////////
	scale = glGetUniformLocation(program, "Scale");

	currentColor = glGetUniformLocation(program, "faceColor");

	uniCubeTranslations = glGetUniformLocation(program, "offset_Z");
	zOffset.z = -1;

	uniformDrawingPlane = glGetUniformLocation(program, "isPlane");

	uniCubeScale = glGetUniformLocation(program, "scale");

	uniFormAngles= glGetUniformLocation(program, "angles");


	//jetPlane init vectors
	jetPlane->scale = vec3(0.5, 0.5, 0.5);
	jetPlane->pos = vec3(0, 0.375, farRenderZ);
	yCountDown = jetPlane->pos.y;
	yCountUp = jetPlane->pos.y;
	jetPlane->angles = vec3(0.0,0.0,0.0);
	intializeModels();
	glClearColor(0.0, 0.0, 0.0, 1.0);		// white background

}

bool checkOnSurface() {		// check that jetPlaneis on surface of the map
	float tayraXLeft = jetPlane->pos.x - 0.125;
	float tayraXRight = jetPlane->pos.x + 0.125;
	float tayraZBack = jetPlane->pos.z - 0.125;
	float tayraZFront = jetPlane->pos.z + 0.125;
	bool flag = false;
	if (isFalling == false) {
		for (int i = 0; i < models.size(); i++) {
			float modelZBack = models.at(i)->pos.z
					- models.at(i)->scale.z * 0.4;
			float modelZFront = models.at(i)->pos.z
					+ models.at(i)->scale.z * 0.4;
			float modelXRight = models.at(i)->pos.x
					+ models.at(i)->scale.x * 0.25;
			float modelXLeft = models.at(i)->pos.x
					- models.at(i)->scale.x * 0.25;
			if (((modelZBack < tayraZBack && modelZFront > tayraZBack)
					|| (modelZFront < tayraZBack && modelZFront > tayraZFront))
					&& ((modelXLeft < tayraXLeft && modelXRight > tayraXRight)
							|| (modelXRight > tayraXLeft
									&& modelXRight < tayraXRight)
							|| (modelXLeft > tayraXLeft
									&& modelXLeft < tayraXRight))) {
				flag = true;
				break;
			}
		}

	}
	return flag;
}

void special_keys(int key, int x, int y) {
	cout << isFalling << endl;
	if (isFalling == false && jumpState == false && rightJump == false
			&& leftJump == false) {
		switch (key) {
		case GLUT_KEY_UP:
			jumpState = true;
			break;
		case GLUT_KEY_LEFT:
			leftJump = true;
			break;
		case GLUT_KEY_RIGHT:
			rightJump = true;
			break;
		}
	}
	if(isFalling && key == 	GLUT_KEY_DOWN){
					//jetPlane->pos.y = 0.375;
					//isFalling = false;

	}
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniform2f(uniformDrawingPlane, 200.0, 200.0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(attribute_pos);
	glBindBuffer(GL_ARRAY_BUFFER, c_vertexbuffer);
	glVertexAttribPointer(attribute_pos, // attribute
			3, // size
			GL_FLOAT, // type
			GL_FALSE, // normalized?
			0, // stride
			(void*) 0 // array buffer offset
			);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, c_Texture);

		zConut -= 00.01;
	for (int i = 0; i < models.size(); i++) {
		models.at(i)->pos.z -= 0.01;
		glUniform3f(uniCubeScale, models.at(i)->scale.x, models.at(i)->scale.y,
				models.at(i)->scale.z);
		glUniform3f(uniCubeTranslations, models.at(i)->pos.x,
				models.at(i)->pos.y, models.at(i)->pos.z);

		glUniform3f(currentColor, 0.0f, 0.0f, 1.0f);

		glm::mat4 scalee = glm::scale(glm::mat4(1.0f),
				glm::vec3(1.0f, 1.0f, 4.0f));

		glUniformMatrix4fv(scale, 1, GL_FALSE, glm::value_ptr(scalee));

		int size;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT,
				0);

//		glDrawArrays(GL_TRIANGLES, 0, 4);			// first face
//
//		glUniform3f(currentColor, 1.0f, 1.0f, 0.0f);
//		glDrawArrays(GL_TRIANGLES, 4, 4);
//
//		glUniform3f(currentColor, 0.0f, 1.0f, 0.25f);
//		glDrawArrays(GL_TRIANGLES, 12, 4);
//
//		glUniform3f(currentColor, 1.0f, 1.0f, 1.0f);
//		glDrawArrays(GL_TRIANGLES, 18, 4);
//
//		glUniform3f(currentColor, 1.00f, 0.5f, 0.0f);
//		glDrawArrays(GL_TRIANGLES, 24, 4);
//
//		glUniform3f(currentColor, 1.0f, 0.0f, 0.0f);
//		glDrawArrays(GL_TRIANGLES, 30, 4);

	}

	glUniform3f(uniCubeScale, jetPlane->scale.x, jetPlane->scale.y, jetPlane->scale.z);
	bool flag = checkOnSurface();
	if (jumpState) {			//animation of jump
		jumpCounter++;
		if (jumpCounter < 80) {
			yCountUp = yCountUp + 0.01;
		} else if (jumpCounter > 80 && jumpCounter <= 160) {
			yCountUp = yCountUp - 0.01;
		} else if (jumpCounter > 80) {
			jumpCounter = 0;
			yCountUp = jetPlane->pos.y;
			jumpState = false;
		}
	}
	if (rightJump || leftJump) {			//animation of jump
		jumpCounter++;
		if (jumpCounter < 70) {
			yCountUp = yCountUp + 0.008;
			if (rightJump) {
				jetPlane->angles.z -= 0.75;
				jetPlane->pos.x = jetPlane->pos.x - 0.01;
			} else if (leftJump) {
				jetPlane->pos.x = jetPlane->pos.x + 0.01;
				jetPlane->angles.z += 0.75;
			}

		} else if (jumpCounter > 70 && jumpCounter <= 140) {
			if (rightJump) {
					jetPlane->angles.z += 0.75;
			} else if (leftJump) {
				jetPlane->angles.z -= 0.75;
			}
			yCountUp = yCountUp - 0.008;
		} else if (jumpCounter > 140) {
			jumpCounter = 0;
			yCountUp = jetPlane->pos.y;
			rightJump = false;
			leftJump = false;
		}
	}
	if (!flag && !jumpState && !rightJump) {	//check if the jetPlane on surface
		yCountDown = yCountDown - 0.005;
		glUniform3f(uniCubeTranslations, jetPlane->pos.x, yCountDown,
				jetPlane->pos.z);
		//if (counter > 20) {	//isFalling is a bool to stop checking the surface if the jetPlane is already fall we used here a counter because in the beginning the flag is=0 before initialize the map
		isFalling = true;
		//}
	} else {
		glUniform3f(uniCubeTranslations, jetPlane->pos.x, yCountUp, jetPlane->pos.z);
	}
	glUniform2f(uniformDrawingPlane, 100.0, 100.0);
	////////////////////////////////////////
	// Draw the triangle !
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(attribute_pos);
	glBindBuffer(GL_ARRAY_BUFFER, p_vertexbuffer);
	glVertexAttribPointer(attribute_pos, // attribute
			3, // size
			GL_FLOAT, // type
			GL_FALSE, // normalized?
			0, // stride
			(void*) 0 // array buffer offset
			);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, p_Texture);

	glUniform3f(uniFormAngles, jetPlane->angles.x, jetPlane->angles.y, jetPlane->angles.z);
	glDrawArrays(GL_TRIANGLES, 0, p_vertices.size());

	//////////////////////////////////////////
//	glUniform3f(currentColor, 0.0f, 0.0f, 1.0f);
//	glDrawArrays(GL_TRIANGLES, 0+plane_vertices_size, 6);			// first face
//
//	glUniform3f(currentColor, 1.0f, 1.0f, 0.0f);
//	glDrawArrays(GL_TRIANGLES, 6+plane_vertices_size, 6);
//
//	glUniform3f(currentColor, 0.0f, 1.0f, 0.25f);
//	glDrawArrays(GL_TRIANGLES, 12+plane_vertices_size, 6);
//	glUniform3f(currentColor, 1.0f, 1.0f, 1.0f);
//	glDrawArrays(GL_TRIANGLES, 18+plane_vertices_size, 6);
//
//	glUniform3f(currentColor, 1.00f, 0.5f, 0.0f);
//	glDrawArrays(GL_TRIANGLES, 24+plane_vertices_size, 6);
//
//	glUniform3f(currentColor, 1.0f, 0.0f, 0.0f);
//	glDrawArrays(GL_TRIANGLES, 30+plane_vertices_size, 6);

	glUniform2f(uniformDrawingPlane, 200.0, 200.0);
	glutSwapBuffers();
	//cout << "Display" << endl;

}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutCreateWindow("hello world");

	glewInit();
	glutSpecialFunc(special_keys);
	init();
	glutDisplayFunc(display);
	STARTEngine();
	PlaySound(TEXT("k.wav"),NULL,SND_ASYNC|SND_LOOP|SND_NOSTOP);
	glutMainLoop();
	display();
	return 0;
}

