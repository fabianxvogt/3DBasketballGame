//
//  main.cpp
//  OpenGL Shadows
//
//  Created by CGIS on 05/12/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC

#include <iostream>
#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include "Shader.hpp"
#include "Camera.hpp"

#define TINYOBJLOADER_IMPLEMENTATION

#include "Model3D.hpp"
#include "Mesh.hpp"
#include "SkyBox.hpp"
#include "Obj3D.hpp"
#include <ctime>
#include "Quaternion.h"

int glWindowWidth = 3000;
int glWindowHeight = 2000;

/*
BEGIN: Basketball-Spiel: Variablen
Author: Fabian Vogt (s0570800)
*/
// Spielfeld
float boundsFB = 18.5;
float boundsL = 8;
float boundsR = 10;
float planeY = 0.1f;
float basket1Pos[3]{ 0.0f,3.6f, -13.62f };
float basket2Pos[3]{ 0.0f,3.6f, 13.62f };

// Ball
float ballPos[3] = { 0.0f, 0.0f, 0.0f };
float ballVelocity[3] = { 0.0f, 0.0f, 0.0f };
float ballSpin[3] = { 0.0f, 0.0f, 0.0f };
float ballAngle[3] = { 0.0f, 0.0f, 0.0f };
float ballPickUpDistance = 1.8;
float ballHitDistance = 2.5;
float ballRadius = 0.35f;
float ballscale = 1.2f;
float ballBasketJumpVelocity = 1.5f;
float winningBallBasketDistance = 0.4f;
bool ballIsFallingInBasket = false;
bool ballNotPickedUpYet = true;

glm::mat4 ballModel = glm::scale(glm::mat4(1.f), glm::vec3(ballscale, ballscale, ballscale));


// Player (Camera)
bool playerHasBall = false;
bool isJumping = false;
double initialHeight = 2;
double height = initialHeight;
double maxJumpHeight = 4;
/*
END: Basketball-Spiel: Variablen
*/

GLFWwindow* glWindow = NULL;

const GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

int retina_width, retina_height;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat3 lightDirMatrix;
GLuint lightDirMatrixLoc;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

gps::Camera myCamera(glm::vec3(0.0f, 2.5f, -5.0f), glm::vec3(0.0f, 2.5f, 1.0f), boundsL, boundsR, boundsFB);
GLfloat cameraSpeed = 0.1f;

bool pressedKeys[1024];
GLfloat angle1; // Diese Winkel stammen von der Ursprünglichen Kamerasteuerung und sollten nicht mehr verändert werden!!
GLfloat angle2;
GLfloat lightAngle;

gps::Model3D myModel;
gps::Model3D ground;
gps::Model3D lightCube;
gps::Model3D ball;
gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader depthMapShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

//textures
std::vector<const GLchar*> faces;
//skybox
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;
gps::Shader fontShader;



GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	myCustomShader.useShaderProgram();

	//set projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	//send matrix data to shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	lightShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//set Viewport transform
	glViewport(0, 0, retina_width, retina_height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

//void print(int x, int y, int z, char* string)
//{
//	//set the position of the text in the window using the x and y coordinates
//	glRasterPos2f(x, y);
//	//get the length of the string to display
//	int len = (int)strlen(string);
//
//	//loop to display character by character
//	for (int i = 0; i < len; i++)
//	{
//		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
//	}
//};
/*
BEGIN: Basketball-Spiel: Maussteuerung
Author: Fabian Vogt (s0570800)
*/
void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	myCamera.mouseRotate(window, xpos, ypos);
}
/*
END: Basketball-Spiel: Maussteuerung
*/

/*
BEGIN: Basketball-Spiel: Distanzen zwischen 2 Punkten
Author: Fabian Vogt (s0570800)
*/
double playerBallDistance() {
	return sqrt(pow(myCamera.getPosition().x - ballPos[0], 2) +
		pow(myCamera.getPosition().y - ballPos[1], 2) +
		pow(myCamera.getPosition().z - ballPos[2], 2));
}
double basket1BallDistance() {
	return sqrt(pow(basket1Pos[0] - ballPos[0], 2) +
		pow(basket1Pos[1] - ballPos[1], 2) +
		pow(basket1Pos[2] - ballPos[2], 2));
}
double basket2BallDistance() {
	return sqrt(pow(basket2Pos[0] - ballPos[0], 2) +
		pow(basket2Pos[1] - ballPos[1], 2) +
		pow(basket2Pos[2] - ballPos[2], 2));
}
/*
END: Basketball-Spiel: Distanzen zwischen 2 Punkten
*/
/*
BEGIN: Basketball-Spiel: Zurücksetzen von Ballbewegung und -drehung
Author: Fabian Vogt (s0570800)
*/
void resetBallVelocity() {
	ballVelocity[0] = 0;
	ballVelocity[1] = 0;
	ballVelocity[2] = 0;
}
void resetBallSpin() {
	ballSpin[0] = 0;
	ballSpin[1] = 0;
	ballSpin[2] = 0;
	ballAngle[0] = 0;
	ballAngle[1] = 0;
	ballAngle[2] = 0;
}
/*
END: Basketball-Spiel: Zurücksetzen von Ballbewegung und -drehung
*/
/*
BEGIN: Basketball-Spiel: Ball werfen
Author: Fabian Vogt (s0570800)
*/
void throwBall() {
	if (playerBallDistance() > ballHitDistance)
		return;
	playerHasBall = false;
	ballVelocity[0] = myCamera.getDirection().x*10;
	ballVelocity[1] = myCamera.getDirection().y*10;
	ballVelocity[2] = myCamera.getDirection().z*10;
}
/*
END: Basketball-Spiel: Ball werfen
*/

void processMovement()
{
	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}
	/*
	BEGIN: Basketball-Spiel: Steuerung zum Springen und Werfen
	Author: Fabian Vogt (s0570800)
	*/
	if (pressedKeys[GLFW_KEY_R]) {
		throwBall();
	}

	if (pressedKeys[GLFW_KEY_SPACE]) {
		if (height <= initialHeight)
			isJumping = true;
	}
	/*
	END: Basketball-Spiel: Steuerung zum Springen und Werfen
	*/

	if (pressedKeys[GLFW_KEY_J]) {

		lightAngle += 1.0f;
		if (lightAngle > 360.0f)
			lightAngle -= 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle -= 1.0f;
		if (lightAngle < 0.0f)
			lightAngle += 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}
}

/*
BEGIN: Basketball-Spiel: Spieler/Kamera-Höhe updaten (ermöglicht das Springen)
Author: Fabian Vogt (s0570800)
*/
void updateHeight() {
	float heightDiff = (maxJumpHeight - height) * 0.1 + 0.01;
	if (isJumping)
		if (height < maxJumpHeight)
			height += heightDiff;
		else
			isJumping = false;
	else
		if (height > initialHeight)
			height -= heightDiff;
}
/*
END: Basketball-Spiel: Spieler/Kamera-Höhe updaten (ermöglicht das Springen)
*/
bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	//for Mac OS X
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwMakeContextCurrent(glWindow);

	glfwWindowHint(GLFW_SAMPLES, 4);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
						  //glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initFBOs()
{
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix()
{
	const GLfloat near_plane = 1.0f, far_plane = 10.0f;
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);

	glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
	glm::mat4 lightView = glm::lookAt(lightDirTr, myCamera.getCameraTarget(), glm::vec3(0.0f, 1.0f, 0.0f));

	return lightProjection * lightView;
}

void initModels()
{
	
    myModel = gps::Model3D("objects/sala/sala.obj","objects/sala/");
	//lightCube = gps::Model3D("objects/blub/blub.obj", "objects/blub/");
	/*
	BEGIN: Basketball-Spiel: Ball laden und initialisieren 
	Author: Fabian Vogt (s0570800)
	*/
	ball = gps::Model3D("objects/ball/new_ball.obj", "objects/ball/");
	ballVelocity[1] = 10;
	ballSpin[1] = 90;
	/*
	END: Basketball-Spiel: Ball laden und initialisieren
	*/
}

void loadTextures() {
	faces.push_back("textures/skybox/hangingstone_rt.tga");
	faces.push_back("textures/skybox/hangingstone_lf.tga");
	faces.push_back("textures/skybox/hangingstone_up.tga");
	faces.push_back("textures/skybox/hangingstone_dn.tga");
	faces.push_back("textures/skybox/hangingstone_bk.tga");
	faces.push_back("textures/skybox/hangingstone_ft.tga");
}

void initShaders()
{
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	depthMapShader.loadShader("shaders/simpleDepthMap.vert", "shaders/simpleDepthMap.frag");

	// skybox 
	loadTextures();
	mySkyBox.Load(faces);
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();

	// Font 

	fontShader.loadShader("shaders/fontShader.vert", "shaders/fontShader.frag");
	fontShader.useShaderProgram();
}

void initUniforms()
{
	myCustomShader.useShaderProgram();

	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");

	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");

	lightDirMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDirMatrix");

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 2.0f);
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	// init view and projection matrix for skybox
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
		glm::value_ptr(view));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
		glm::value_ptr(projection));
}

/*
BEGIN: Basketball-Spiel: Berechnung der nächsten Position des Balls
Author: Fabian Vogt (s0570800)
*/
clock_t tm1 = 0, tm2 = 0;

void nextBallPosition(){
	float dt = ((float)(tm2 - tm1)) / ((float)CLOCKS_PER_SEC);
	if (dt > 0.02f)
		dt = 0.02f;

	double rate = 0.9;
	ballVelocity[1] += -9.87f * dt;
	for (int i = 0; i < 3; i++)
	{
		ballPos[i] += ballVelocity[i] * dt;
	}
	if (ballPos[1] - ballRadius <= planeY)
	{
		ballPos[1] = planeY + ballRadius;
		ballVelocity[1] = fabs(ballVelocity[1]) * rate;
	}
	if (ballPos[0] + ballRadius >= boundsL)
	{
		ballVelocity[0] = -fabs(ballVelocity[0]) * rate;
	}
	if (ballPos[0] - ballRadius <= -boundsR)
	{
		ballVelocity[0] = fabs(ballVelocity[0]) * rate;
	}
	if (ballPos[2] + ballRadius >= boundsFB)
	{
		ballVelocity[2] = -fabs(ballVelocity[2]) * rate;
	}
	if (ballPos[2] - ballRadius <= -boundsFB)
	{
		ballVelocity[2] = fabs(ballVelocity[2]) * rate;
	}
}
/*
END: Basketball-Spiel: Berechnung der nächsten Position des Balls
*/

/*
BEGIN: Basketball-Spiel: Berechnung der Drehung des Balls mit Quaternionen

Die Berechnung der Drehung mit Quaternionen funktioniert leider nicht richtig.
Hier sind meine gescheiterten Versuche:
	
Author: Fabian Vogt (s0570800)
*/
glm::mat4 getRotationMatrix(float angle, glm::vec3 axis) {
	// Build Quaternion
	Quaternion rotation, conjugate, w;
	rotation.CreateFromAxisAngle(axis.x, axis.y, axis.z, glm::radians(angle));

	conjugate = rotation.conjugate();

	w = rotation.multiplyVec3(ballAngle) * conjugate;
	ballAngle[0] = w.getX();
	ballAngle[1] = w.getY();
	ballAngle[2] = w.getZ();

	/*if (ballAngle[0] == 0 &&
		ballAngle[1] == 0 &&
		ballAngle[2] == 0) {
		ballAngle[0] = axis.x;
		ballAngle[1] = axis.y;
		ballAngle[2] = axis.z;
	}*/

	return w.CreateMatrix();
	//return vec3(w.getX(), w.getY(), w.getZ());
}
/*
END: Basketball-Spiel: Berechnung der Drehung des Balls
*/
/*
BEGIN: Basketball-Spiel: Erzeugung des Ball-Models
Author: Fabian Vogt (s0570800)
*/
void buildBallModel() {
	//glm::mat4 _model;

	if (playerHasBall) {
		ballModel = glm::translate(glm::mat4(1.0f), myCamera.getPosition() + glm::normalize(myCamera.getDirection()) * 2.0f);//->LookingDirection().normalized() * 5)
		ballPos[0] = ballModel[3].x;
		ballPos[1] = ballModel[3].y;
		ballPos[2] = ballModel[3].z;
		resetBallVelocity();
		resetBallSpin();
	}
	else {
		if (playerBallDistance() < ballPickUpDistance) {
			ballIsFallingInBasket = false;
			playerHasBall = true;
			ballNotPickedUpYet = false;
			buildBallModel();
		}
		if (!ballIsFallingInBasket) {
			if (basket1BallDistance() < winningBallBasketDistance) {
				ballPos[0] = basket1Pos[0];
				ballPos[2] = basket1Pos[2];
				resetBallVelocity();
				ballVelocity[1] = ballBasketJumpVelocity;
				ballIsFallingInBasket = true;
			}
			else if (basket2BallDistance() < winningBallBasketDistance) {
				ballPos[0] = basket2Pos[0];
				ballPos[2] = basket2Pos[2];
				resetBallVelocity();
				ballVelocity[1] = ballBasketJumpVelocity;
				ballIsFallingInBasket = true;
			}
		}
		glm::vec3 lastPos = vec3(ballPos[0], ballPos[1], ballPos[2]);
		nextBallPosition();
		glm::vec3 currentPos = vec3(ballPos[0], ballPos[1], ballPos[2]);
		glm::vec3 diff = currentPos - lastPos;

		// Ball Rotieren
		// Diese Logik funktioniert nur bedingt bis zu einem gewissen Punkt
		// Sobald der Ball ein paar mal an der Wand abprallt, gerät die Rotation durcheinander
		// Ich vermute, dass dies an dem Gimbal Lock Problem liegt.
		// Um das Problem zu lösen wollte ich Quaternionen einsetzen, habe es aber leider bisher nicht hingekriegt
		// (siehe Methode "getRotationMatrix()")
		if (ballNotPickedUpYet) {
			ballModel = glm::rotate(ballModel, ballSpin[1], vec3(0,1,0));
			ballSpin[1] *= 0.99f;
		}
		else if (!ballIsFallingInBasket) {
			const float angle = (diff).length() / ballRadius / 50;

			ballModel[3].x = 0;
			ballModel[3].y = 0;
			ballModel[3].z = 0;
			// Assuming the table is on the XZ plane
			const glm::vec3 axis = cross(diff, glm::vec3(0, 1, 0));
			
			//Quaternion test
			//ballModel = ballModel * getRotationMatrix(angle, glm::normalize(axis));
			
			ballModel = glm::rotate(ballModel, -angle, axis);
		}

		ballModel[3].x = ballPos[0];
		ballModel[3].y = ballPos[1];
		ballModel[3].z = ballPos[2];
	}
}
/*
END: Basketball-Spiel: Erzeugung des Ball-Models
*/

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*
	BEGIN: Basketball-Spiel: Updaten der Kamerahöhe 
	Author: Fabian Vogt (s0570800)
	*/
	// Handle player jumps
	updateHeight();
	myCamera.setHeight(height);
	/*
	END: Basketball-Spiel: Updaten der Kamerahöhe
	*/

	processMovement();

	//render the scene to the depth buffer (first pass)

	depthMapShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	//create model matrix for nanosuit
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle1), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(angle2), glm::vec3(1, 0, 0));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));

	//myModel.Draw(depthMapShader);

	//create model matrix for ground
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));

	//ground.Draw(depthMapShader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//render the scene (second pass)

	myCustomShader.useShaderProgram();

	//send lightSpace matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	//send view matrix to shader
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "view"),
		1,
		GL_FALSE,
		glm::value_ptr(view));

	//compute light direction transformation matrix
	lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
	//send lightDir matrix data to shader
	glUniformMatrix3fv(lightDirMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightDirMatrix));

	glViewport(0, 0, retina_width, retina_height);
	myCustomShader.useShaderProgram();

	//bind the depth map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

	//create model matrix for nanosuit
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle1), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(angle2), glm::vec3(1, 0, 0));
	//send model matrix data to shader	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	myModel.Draw(myCustomShader);

	/*
	BEGIN: Basketball-Spiel: Ball-Matrizen erstellen und Ball zeichnen
	Author: Fabian Vogt (s0570800)
	*/
	buildBallModel();
	model = ballModel;
	//send model matrix data to shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//create normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	ball.Draw(myCustomShader);
	/*
	END: Basketball-Spiel: Ball-Matrizen erstellen und Ball zeichnen
	*/

	//create model matrix for ground
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	//send model matrix data to shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//create normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	ground.Draw(myCustomShader);

	//draw a white cube around the light

	lightShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

	model = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, lightDir);
	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	lightCube.Draw(lightShader);
	mySkyBox.Draw(skyboxShader, view, projection);

	/*
	BEGIN: Basketball-Spiel: Clock updaten
	Author: Fabian Vogt (s0570800)
	*/
	// Timer
	tm1 = tm2;
	tm2 = clock();
	/*
	END: Basketball-Spiel: Clock updaten
	*/
}

int main(int argc, char **argv) {
	
	initOpenGLWindow();
	initOpenGLState();
	initFBOs();
	initModels();
	initShaders();
	initUniforms();
	glCheckError();
	while (!glfwWindowShouldClose(glWindow)) {
		renderScene();
		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}
	

	//close GL context and any other GLFW resources
	glfwTerminate();

	return 0;
}
