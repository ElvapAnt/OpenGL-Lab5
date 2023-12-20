#pragma once
#include "pch.h"
#include "GLMaterial.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
class CGLRenderer
{
public:
	CGLRenderer(void);
	virtual ~CGLRenderer(void);

	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC, double angle);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,

protected:
	void DrawSphere(double r, int nSegAlpha, int nSegBeta);
	void DrawCylinder(double h, double r1, double r2, int nSeg);
	void DrawAxis(double width);
	

	void DrawWall(double width, double height, int nSeg);
	void DrawRoom(double width, double height, double depth);
	void DrawBase();
	void DrawVase();
	void DrawCube(double size);
	void DrawVaseLayer(bool dir, double h, double radius, double diff, int nSeg, const double color[3]);
	//true = up
	//false = down
	void DrawLayerNormals(double h, double r1, double r2, int nSeg, double normalLength);


	void RedLightSource();
	void BlueLightSource();
	void GreenLightSource();
	void AmbientLight();
	void ViewPointLight();


protected:
	HGLRC	 m_hrc;

	const int rLight = GL_LIGHT1;
	const int bLight = GL_LIGHT2;
	const int gLight = GL_LIGHT3;
	CGLMaterial* grayMaterial;
	CGLMaterial* blueMaterial;
	CGLMaterial* lightBlueMaterial;
	CGLMaterial* axisMaterial;

public:
	float angle = 0.0f;
	float m_yaw = 0.0f;
	float m_pitch = 0.0f; //OpenGL Rendering Context 
	bool displayNormals = false;
	bool redLight = false;
	bool blueLight = false;
	bool greenLight = false;
};

