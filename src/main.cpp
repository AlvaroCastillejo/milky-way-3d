//include some standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

//AFEGIT
#include "imageloader.h"

//include OpenGL libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//CARREGAR ESFERA
#include <string>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
std::string basepath = "assets/";
std::string inputfile = basepath + "sphere.obj";
std::vector< tinyobj::shape_t > shapes;
std::vector< tinyobj::material_t > materials;
std::string err;
bool ret = tinyobj::LoadObj(shapes, materials, err, inputfile.c_str(), basepath.c_str());

//include some custom code files
#include "glfunctions.h" //include all OpenGL stuff
#include "Shader.h" // class to compile shaders

using namespace std;
using namespace glm;

//global variables to help us do things
int g_ViewportWidth = 1080; int g_ViewportHeight = 1080;
double x_mouse, y_mouse;
const vec3 g_backgroundColor(0.0f, 0.0f, 0.0f);
int addition = 2;
int scene = 0;

//Variables para trabajar con Esfera
double x = 4, y = 4, z = 0;
GLuint g_simpleShader = 0;
GLuint g_Vao = 0;
GLuint g_NumTriangles = 0;

//Camera variables
double x2 = 0, y2 = 0, z2 = 3;
double angle_x = 0, angle_z = 0;

double angle_y = 0;

GLuint texture_id = 0;
GLuint texture_id2 = 0;

//parallel light source
vec3 g_light_dir(100, 100, 100);


// ------------------------------------------------------------------------------------------
// This function manually creates a square geometry (defined in the array vertices[])
// ------------------------------------------------------------------------------------------
void load()
{
	//test it loaded correctly
	if (!err.empty()) { // `err` may contain warning message.
		std::cerr << err << std::endl;
	}	

	//**********************
	// CODE TO LOAD EVERYTHING INTO MEMORY
	//**********************

	//load the shader
	Shader simpleShader("src/shader.vert", "src/shader.frag");
	g_simpleShader = simpleShader.program;

	//UV coord buffer
	GLfloat* uvs = &(shapes[0].mesh.texcoords[0]);
	GLuint uvs_size = shapes[0].mesh.texcoords.size() * sizeof(GLfloat);

	//create the VAO where we store all geometry (stored in g_Vao)
	g_Vao = gl_createAndBindVAO();

	//create vertex buffer for positions, colors, and indices, and bind them to shader
	gl_createAndBindAttribute(&(shapes[0].mesh.positions[0]), shapes[0].mesh.positions.size() * sizeof(float), g_simpleShader, "a_vertex", 3);

	gl_createIndexBuffer(&(shapes[0].mesh.indices[0]), shapes[0].mesh.indices.size() * sizeof(unsigned int));
	gl_createAndBindAttribute(uvs, uvs_size, g_simpleShader, "a_uv", 2);
	
	gl_createAndBindAttribute(&(shapes[0].mesh.normals[0]), shapes[0].mesh.normals.size() * sizeof(float), g_simpleShader, "a_normal", 3);
	
	//store number of triangles (use in draw())
	g_NumTriangles = shapes[0].mesh.indices.size() / 3;

	//Load earth texture
	Image* image = loadBMP("assets/earthmap1k.bmp"); 
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,	//target
		0,	//level, =0 base, no mipmap
		GL_RGB,	//the way opengl stores the data
		image->width,	//width
		image->height,	//height
		0,	//border, must be 0!
		GL_RGB,	//the format of the original data
		GL_UNSIGNED_BYTE,	//type of data
		image->pixels	//pointer to the start of data
	);

	//Load universe texture.
	Image* image2 = loadBMP("assets/milkyway.bmp"); //CUIDADO POSAR LA DESTINACIÓ DE L'ARXIU CORRECTAMENT
	glGenTextures(1, &texture_id2);
	glBindTexture(GL_TEXTURE_2D, texture_id2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,	//target
		0,	//level, =0 base, no mipmap
		GL_RGB,	//the way opengl stores the data
		image2->width,	//width
		image2->height,	//height
		0,	//border, must be 0!
		GL_RGB,	//the format of the original data
		GL_UNSIGNED_BYTE,	//type of data
		image2->pixels	//pointer to the start of data
	);

}

// ------------------------------------------------------------------------------------------
// This function actually draws to screen and called non-stop, in a loop
// ------------------------------------------------------------------------------------------
void draw()
{

	//Cálculo para rotar la cámara
	angle_x = cos(glfwGetTime())*addition;		//by 2 for better looking of the perspective.
	angle_z = sin(glfwGetTime())*addition;
	
	x2 = angle_z;
	z2 = angle_x;

	angle_y = cos(glfwGetTime()) * 2;

	//clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//UNIVERSO
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear the screen
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	
	GLuint u_light_dir = glGetUniformLocation(g_simpleShader, "u_light_dir");
	glUniform3f(u_light_dir, g_light_dir.x, g_light_dir.y, g_light_dir.z);

	//bind the geometry
	glBindVertexArray(g_Vao);
	mat4 escalada = scale(mat4(1.0f), vec3(2.0f, 2.0f, 2.0f));
	mat4 model2 = translate(escalada, vec3(0, 0, 0)); //translate identity in -0.5x -0.6y
	gl_bindVAO(g_Vao); //bind the geometry
	
	//Projection matrix and shader.
	mat4 projection_matrix = perspective(
		60.0f, // Field of view
		1.0f, // Aspect ratio
		0.1f, // near plane (distance from camera)
		50.0f // Far plane (distance from camera)
	);
	GLuint projection_loc = glGetUniformLocation(g_simpleShader, "u_projection");
	glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
	
	mat4 view_matrix = glm::lookAt(
		glm::vec3(x2, y_mouse / 30, z2), //(EYE) the position of your camera, in world space
		glm::vec3(0, 0, 0), //(CENTER)  where you want to look at, in world space
		glm::vec3(0, 1, 0) //(UP) probably glm::vec3(0,1,0)
	);

	GLuint model_loc3 = glGetUniformLocation(g_simpleShader, "u_view"); //find "u_view" in shader
	glUniformMatrix4fv(model_loc3, 1, GL_FALSE, glm::value_ptr(view_matrix)); //upload the data to the shader

	if (scene == 0){
		x = 4;
		y = 4;
		z = 0;
	}
	else if (scene == 1) {
		x = 0;
		y = 0;
		z = 0;
	}


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id2);

	//Draw unievrse. Not working.
	glDrawElements(GL_TRIANGLES, 3 * g_NumTriangles, GL_UNSIGNED_INT, 0);

	//TIERRA
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glUseProgram(g_simpleShader);//activate shader
	glUniform3f(u_light_dir, g_light_dir.x, g_light_dir.y, g_light_dir.z);
	mat4 escalada2 = scale(mat4(1.0f), vec3(0.5f, 0.5f, 0.5f)); 
	mat4 model = translate(escalada2, vec3(x, y, z)); //translate identity in -0.5x -0.6y
	GLuint model_loc = glGetUniformLocation(g_simpleShader, "u_model");  //find “u_model” in shader
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model)); //upload the data to the shader
	glUniformMatrix4fv(model_loc3, 1, GL_FALSE, glm::value_ptr(view_matrix)); 
	glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix)); 

	//Phong
	GLuint u1 = glGetUniformLocation(g_simpleShader, "u_light_color");
	glUniform3f(u1, 1.f, 1.f, 1.f);
	GLuint u2 = glGetUniformLocation(g_simpleShader, "u_eye");
	glUniform3f(u2, 0.f, 0.f, 0.f);
	GLuint u3 = glGetUniformLocation(g_simpleShader, "u_glossiness");
	glUniform1f(u3, 50.f);
	GLuint u4 = glGetUniformLocation(g_simpleShader, "u_ambient");
	glUniform3f(u4, 0.f, 0.f, 0.f);
	

	gl_bindVAO(g_Vao); //bind the geometry
	glBindTexture(GL_TEXTURE_2D, texture_id);

	//Draw earth.
	glDrawElements(GL_TRIANGLES, 3 * g_NumTriangles, GL_UNSIGNED_INT, 0);
}



// ------------------------------------------------------------------------------------------
// This function is called every time you press a screen
// ------------------------------------------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//quit
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, 1);
	//reload
	if (key == GLFW_KEY_R && action == GLFW_PRESS) load();

	//Teapot movement
	if (key == GLFW_KEY_D) x = x - 0.1;
	if (key == GLFW_KEY_A) x = x + 0.1;
	if (key == GLFW_KEY_W) y = y - 0.1;
	if (key == GLFW_KEY_S) y = y + 0.1;
	//Teapot approach
	if (key == GLFW_KEY_Z) z = z + 0.1;
	//Teapot ward off
	if (key == GLFW_KEY_X) z = z - 0.1;

	//Rotate
	if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		//View matrix
		if (scene == 1) {
			scene = 0;
		}
		else {
			scene = 1;
		}
	}
	
}

// ------------------------------------------------------------------------------------------
// This function is called every time you click the mouse
// ------------------------------------------------------------------------------------------
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		cout << "Left mouse down at" << x_mouse << ", " << y_mouse << endl;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cout << "Left mouse down at" << yoffset << endl;
	if (yoffset < 0) {
		addition -= 1;
	}
	else {
		addition += 1;
	}
}

int main(void)
{	
	//setup window and boring stuff, defined in glfunctions.cpp
	GLFWwindow* window;
	if (!glfwInit())return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(g_ViewportWidth, g_ViewportHeight, "Hello OpenGL!", NULL, NULL);
	if (!window) { glfwTerminate();	return -1; }
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();

	//input callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
	glfwSetScrollCallback(window, scroll_callback);


	//load all the resources
	load();

	//loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		draw();

		//swap front and back buffers
		glfwSwapBuffers(window);

		//poll for and process events
		glfwPollEvents();

		//mouse position must be tracked constantly (callbacks do not give accurate delta)
		glfwGetCursorPos(window, &x_mouse, &y_mouse);
	}

	//terminate glfw and exit
	glfwTerminate();
	return 0;
}



