#include "pch.h"
#include "GLRenderer.h"
#define _USE_MATH_DEFINES
#include <cmath>
//#pragma comment(lib, "GL\\glut32.lib")

CGLRenderer::CGLRenderer(void)
{
	grayMaterial = new CGLMaterial();
	blueMaterial = new CGLMaterial();
	lightBlueMaterial = new CGLMaterial();
	axisMaterial = new CGLMaterial();


	grayMaterial->SetAmbient(0.1, 0.1, 0.1, 1.0);
	grayMaterial->SetDiffuse(0.8, 0.8, 0.8, 1.0);
	grayMaterial->SetSpecular(0.2, 0.2, 0.2, 1.0);
	grayMaterial->SetEmission(0.1,0.1,0.1,1.0);
	grayMaterial->SetShininess(0);

	blueMaterial->SetAmbient(0.5, 0.0, 0.5, 1.0 );
	blueMaterial->SetDiffuse( 1.0, 0.0, 1.0, 1.0);
	blueMaterial->SetSpecular(0.5, 0.5, 0.5, 1.0);
	blueMaterial->SetShininess(0.0);
	
	lightBlueMaterial->SetAmbient(0.0, 0.5, 0.5, 1.0);
	lightBlueMaterial->SetDiffuse(0.0, 1.0, 1.0, 1.0);
	lightBlueMaterial->SetSpecular(0.5, 0.5, 0.5, 1.0);
	lightBlueMaterial->SetShininess(0.0);

	axisMaterial->SetAmbient(0.0, 0.0, 0.0, 1.0);
	axisMaterial->SetShininess(128);
}

CGLRenderer::~CGLRenderer(void)
{
	delete grayMaterial;
	delete blueMaterial;
	delete lightBlueMaterial;
	delete axisMaterial;
}

bool CGLRenderer::CreateGLContext(CDC* pDC)
{
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int nPixelFormat = ChoosePixelFormat(pDC->m_hDC, &pfd);

	if (nPixelFormat == 0) return false;

	BOOL bResult = SetPixelFormat(pDC->m_hDC, nPixelFormat, &pfd);

	if (!bResult) return false;

	m_hrc = wglCreateContext(pDC->m_hDC);

	if (!m_hrc) return false;

	return true;
}

void CGLRenderer::PrepareScene(CDC* pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	AmbientLight();

	glEnable(GL_LIGHTING);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC* pDC, double angle)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float distance = 6;

	float azimuthRad = m_pitch * (M_PI / 180.0f);
	float elevationRad = m_yaw * (M_PI / 180.0f);

	float x = distance * cos(elevationRad) * sin(azimuthRad);
	float y = distance * sin(elevationRad);
	float z = distance * cos(elevationRad) * cos(azimuthRad);
	glLoadIdentity();

	gluLookAt(x, y, z,
		0.0, 2.0, 0.0,
		0.0, 1.0, 0.0);


	AmbientLight();
	ViewPointLight();

	glDisable(GL_LIGHTING);
	
	DrawAxis(50.0);

	glEnable(GL_LIGHTING);

	if (!redLight) {
		RedLightSource();
	}
	if (!blueLight) {
		//BlueLightSource();
	}

	glPushMatrix();

	grayMaterial->Select();
	DrawBase();
	glPushMatrix();
	grayMaterial->Select();
	glTranslatef(0, 5, 0);
	glColor3f(0.8, 0.8, 0.8);
	DrawRoom(10.0, 10.0, 10.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 1.7, 0.0);
	DrawVase();

	glPopMatrix();

	glPopMatrix();


	glFlush();
	//---------------------------------
	SwapBuffers(pDC->m_hDC);
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::Reshape(CDC* pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (double)w / (double)h, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC* pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHTING);

	wglMakeCurrent(NULL, NULL);
	if (m_hrc)
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::DrawSphere(double r, int nSegAlpha, int nSegBeta)
{
	for (int i = 0; i <= nSegAlpha; i++) {
		double lat0 = M_PI * (-0.5 + (double)(i - 1) / nSegAlpha);
		double z0 = sin(lat0);
		double zr0 = cos(lat0);

		double lat1 = M_PI * (-0.5 + (double)i / nSegAlpha);
		double z1 = sin(lat1);
		double zr1 = cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= nSegBeta; j++) {
			double lng = 2 * M_PI * (double)j / nSegBeta;
			double x = cos(lng);
			double y = sin(lng);

			glNormal3d(x * zr0, y * zr0, z0);
			glVertex3d(r * x * zr0, r * y * zr0, r * z0);
			glNormal3d(x * zr1, y * zr1, z1);
			glVertex3d(r * x * zr1, r * y * zr1, r * z1);
		}
		glEnd();
	}
}
void CGLRenderer::DrawCylinder(double h, double r1, double r2, int nSeg)
{
	double step = 2 * M_PI / nSeg;

	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= nSeg; i++) {
		double angle = i * step;
		double x = cos(angle);
		double z = sin(angle);

		glNormal3d(x, 0.0, z);
		glVertex3d(r2 * x, h, r2 * z);
		glVertex3d(r1 * x, 0.0, r1 * z);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0.0, 1.0, 0.0);
	glVertex3d(0.0, h, 0.0); 
	for (int i = 0; i <= nSeg; i++) {
		double angle = i * step;
		glVertex3d(r2 * cos(angle), h, r2 * sin(angle));
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0.0, -1.0, 0.0);
	glVertex3d(0.0, 0.0, 0.0); 
	for (int i = 0; i <= nSeg; i++) {
		double angle = i * step;
		glVertex3d(r1 * cos(angle), 0.0, r1 * sin(angle));
	}
	glEnd();
}
void CGLRenderer::DrawAxis(double width)
{
	glBegin(GL_LINES);
	// x-axis
	axisMaterial->SetAmbient(1,0,0,1);
	axisMaterial->SetDiffuse(1, 0, 0, 1);
	axisMaterial->SetEmission(1, 0, 0, 1);
	axisMaterial->Select();
	glColor3d(1.0, 0.0, 0.0);
	glVertex3d(0, 0, 0);
	glVertex3d(-width, 0, 0);

	// y-axis
	axisMaterial->SetAmbient(0, 1, 0, 1);
	axisMaterial->SetDiffuse(0, 1, 0, 1);
	axisMaterial->SetEmission(0, 1, 0, 1);
	axisMaterial->Select();
	glColor3d(0.0, 1.0, 0.0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, -width, 0);
	
	// z-axis
	axisMaterial->SetAmbient(0, 0, 1, 1);
	axisMaterial->SetDiffuse(0, 0, 1, 1);
	axisMaterial->SetEmission(0, 0, 1, 1);
	axisMaterial->Select();
	glColor3d(0.0, 0.0, 1.0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, width);
	glEnd();
}
void CGLRenderer::DrawCube(double size)
{
	float halfSize = size / 2.0f;
	// Define the 8 vertices of the cube
	float vertices[8][3] = {
		{-halfSize, -halfSize, -halfSize}, // 0. left-bottom-back
		{halfSize, -halfSize, -halfSize},  // 1. right-bottom-back
		{halfSize, halfSize, -halfSize},   // 2. right-top-back
		{-halfSize, halfSize, -halfSize},  // 3. left-top-back
		{-halfSize, -halfSize, halfSize},  // 4. left-bottom-front
		{halfSize, -halfSize, halfSize},   // 5. right-bottom-front
		{halfSize, halfSize, halfSize},    // 6. right-top-front
		{-halfSize, halfSize, halfSize}    // 7. left-top-front
	};

	// Define the indices for the 6 faces, each with 4 vertices (counter-clockwise order)
	int faces[6][4] = {
		{4, 5, 6, 7}, // front
		{1, 0, 3, 2}, // back
		{5, 1, 2, 6}, // right
		{0, 4, 7, 3}, // left
		{7, 6, 2, 3}, // top
		{0, 1, 5, 4}  // bottom
	};

	// Define the normals for each face (pointing outward)
	float normals[6][3] = {
		{0.0f, 0.0f, 1.0f},  // front
		{0.0f, 0.0f, -1.0f}, // back
		{1.0f, 0.0f, 0.0f},  // right
		{-1.0f, 0.0f, 0.0f}, // left
		{0.0f, 1.0f, 0.0f},  // top
		{0.0f, -1.0f, 0.0f}  // bottom
	};
	glColor3f(0.8, 0.8, 0.8);
	grayMaterial->Select();
	// Draw each face using a loop
	for (int i = 0; i < 6; i++) {
		glBegin(GL_QUADS);
		glNormal3fv(normals[i]); // Set the normal for the current face
		for (int j = 0; j < 4; j++) {
			glVertex3fv(vertices[faces[i][j]]); // Draw each vertex of the face
		}
		glEnd();
	}
}
void CGLRenderer::DrawVaseLayer(bool dir, double h, double radius, double diff, int nSeg, const double color[3])
{
	double step = 2 * M_PI / nSeg;
	double r2 = radius;
	double r1 = radius - diff;
	double slope = h / diff; // The slope of the layer side
	double normal_length;

	glColor3dv(color);
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= nSeg; i++) {
		double angle = i * step;
		double x = cos(angle);
		double z = sin(angle);

		// Calculate normals
		double normal_x = x / radius; // radial component
		double normal_z = z / radius; // radial component
		double normal_y = dir ? -slope : slope; // vertical component based on the slope of the layer

		// Normalize the normal vector
		normal_length = sqrt(normal_x * normal_x + normal_y * normal_y + normal_z * normal_z);
		normal_x /= normal_length;
		normal_y /= normal_length;
		normal_z /= normal_length;

		if (dir) {
			glNormal3d(normal_x, normal_y, normal_z);
			glVertex3d(r2 * x, h, r2 * z);
			glNormal3d(normal_x, normal_y, normal_z);
			glVertex3d(r1 * x, 0.0, r1 * z);
		}
		else {
			glNormal3d(normal_x, normal_y, normal_z);
			glVertex3d(r1 * x, h, r1 * z);
			glNormal3d(normal_x, normal_y, normal_z);
			glVertex3d(r2 * x, 0.0, r2 * z);
		}
		// Apply the normal to both vertices of the strip
	}
	glEnd();
	/*double step = 2 * M_PI / nSeg;
	double r2 = radius;
	double r1 = radius - diff;
	
	int v = r2 > r1 ? -1 : 1;
	double r = abs(r1 - r2);
	double L = sqrt(r * r + h * h);
	double ny = r / L;
	double nr = h / L;

	glColor3dv(color);
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= nSeg; i++) {
		double angle = i * step;
		double x = cos(angle);
		double z = sin(angle);

		if (dir) {
			glVertex3d(r2 * x, h, r2 * z);
			glVertex3d(r1 * x, 0.0, r1 * z);
		}
		else { 
			glVertex3d(r1 * x, h, r1 * z);
			glVertex3d(r2 * x, 0.0, r2 * z);
		}
	}*/
	//glEnd();

	if (displayNormals) {
		if (dir) {
			DrawLayerNormals(h, r1, r2, nSeg, 0.2);
		}
		else {
			DrawLayerNormals(h, r2, r1, nSeg, 0.2);
		}
	}
}
void CGLRenderer::DrawLayerNormals(double h, double r1, double r2, int nSeg, double normalLength)
{	
	axisMaterial->SetEmission(0, 1, 0, 1);
	axisMaterial->SetAmbient(0, 1, 0, 1);
	axisMaterial->Select();
	glColor3f(0.0, 1.0, 0.0);
	double step = 2 * M_PI / nSeg;
	double hDiff = h; 

	glBegin(GL_LINES);
	for (int i = 0; i < nSeg; i++) {
		double angle = i * step;
		double x = cos(angle);
		double z = sin(angle);

		// Calculate the normal direction based on the slant of the vase's layer
		double nx = x; // horizontal component
		double nz = z; // horizontal component
		double ny = (r2 - r1) / hDiff; // vertical component based on the slope of the layer

		// Normalize the normal vector
		double length = sqrt(nx * nx + ny * ny + nz * nz);
		nx /= length;
		ny /= length;
		nz /= length;

		ny = -ny; // flip the normal vector to point in the correct direction

		// Draw the normal for the outer edge
		glVertex3d(r2 * x, h, r2 * z);
		glVertex3d(r2 * x + normalLength * nx, h + normalLength * ny, r2 * z + normalLength * nz);

		// Draw the normal for the inner edge if r1 is greater than 0
		if (r1 > 0) {
			glVertex3d(r1 * x, 0.0, r1 * z);
			glVertex3d(r1 * x + normalLength * nx, normalLength * ny, r1 * z + normalLength * nz);
		}
	}
	glEnd();

}
void CGLRenderer::RedLightSource()
{
	GLfloat red_light_ambient[] = { 0.5, 0.0, 0.0, 1.0 };
	GLfloat red_light_diffuse[] = { 0.75, 0.0, 0.0, 1.0 }; // Red color
	GLfloat red_light_specular[] = { 0, 0, 0.5, 1.0 };

	glLightfv(rLight, GL_AMBIENT, red_light_ambient);
	glLightfv(rLight, GL_DIFFUSE, red_light_diffuse);
	glLightfv(rLight, GL_SPECULAR, red_light_specular);

	//slabljenje
	glLightf(rLight, GL_CONSTANT_ATTENUATION, 1);
	//usmerenje izvora
	glLightf(rLight, GL_SPOT_CUTOFF, 25.0); //ugao
	glLightf(rLight, GL_SPOT_EXPONENT, 2);  //eksponent

	//aktiviranje

	float light_position[] = { 0, 4, 3, 1.0 }; ////poziciono svetlo
	float ligth_spot_direction[] = { 0.0, 0.0, -1.0 }; //iz centra lopte postavljeno, y osa ce biti usmerena ka zidu
	glLightfv(rLight, GL_POSITION, light_position);
	glLightfv(rLight, GL_SPOT_DIRECTION, ligth_spot_direction);

	if (!redLight)
	{
		glPushMatrix();
		glTranslatef(0, 4.5, 4);
		axisMaterial->SetAmbient(1, 0, 0, 1);
		axisMaterial->SetDiffuse(1, 0, 0, 1);
		axisMaterial->SetEmission(1, 0, 0, 1);
		axisMaterial->SetShininess(128);
		axisMaterial->Select();
		DrawSphere(0.25, 30, 30);
		glPopMatrix();
		glEnable(rLight);
	}
	else {
		glDisable(rLight);
	}


}
void CGLRenderer::BlueLightSource()
{
	glPushMatrix();
	glTranslatef(0, 5, -5);
	axisMaterial->SetAmbient(0, 0, 1, 1);
	axisMaterial->SetDiffuse(0, 0, 1, 1);
	axisMaterial->SetEmission(0, 0, 1, 1);
	axisMaterial->SetShininess(64.0);
	axisMaterial->Select();
	DrawSphere(0.25, 30, 30);
	
	float light_ambient[] = { 0.5, 0.0, 0.5, 1.0 };
	float light_diffuse[] = { 1.0, 0.0, 1.0, 1.0 };
	float light_specular[] = { 1.0, 0.0, 1.0, 1.0 };
	GLfloat light_position[] = {0, 0, 0, 1 };

	//Boja i intenzitet svetlosti
	glLightfv(bLight, GL_AMBIENT, light_ambient);
	glLightfv(bLight, GL_DIFFUSE, light_diffuse);
	glLightfv(bLight, GL_SPECULAR, light_specular);

	glLightfv(bLight, GL_POSITION, light_position);
	GLfloat spot_direction[] = { 0, 0, 1.0 };
	glLightfv(bLight, GL_SPOT_DIRECTION, spot_direction);
	//Slabljenje
	glLightf(bLight, GL_CONSTANT_ATTENUATION, 1.0);

	//Usmerenje izvora
	glLightf(bLight, GL_SPOT_CUTOFF, 20);
	//glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 0);
	glEnable(bLight);

	glPopMatrix();
}
void CGLRenderer::GreenLightSource()
{

}
void CGLRenderer::AmbientLight()
{
	GLfloat lmodel_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
}
void CGLRenderer::ViewPointLight()
{
	float light0_ambient[] = { 0.5, 0.5,0.5, 1.0 };
	float light0_diffuse[] = { 0.5, 0.5,0.5, 1.0 };
	float light0_specular[] = { 0.5, 0.5,0.5, 1.0 };
	float light0_position[] = { 13,39,13, 0.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	glEnable(GL_LIGHT0);
}
void CGLRenderer::DrawWall(double width, double height, int nSeg)
{
	float wStep = width / nSeg;
	float hStep = height / nSeg;

	glBegin(GL_QUADS);
	for (int i = 0; i < nSeg; ++i) {
		for (int j = 0; j < nSeg; ++j) {
			float x = i * wStep - width / 2;
			float y = j * hStep - height / 2;

			glNormal3f(0.0f, 0.0f, 1.0f); // Assuming wall is facing +z direction
			glVertex3f(x, y, 0.0f);
			glVertex3f(x + wStep, y, 0.0f);
			glVertex3f(x + wStep, y + hStep, 0.0f);
			glVertex3f(x, y + hStep, 0.0f);
		}
	}
	glEnd();
}
void CGLRenderer::DrawRoom(double width, double height, double depth)
{
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, depth / 2);
	DrawWall(width, height, 30); // Adjust grid size as needed
	glPopMatrix();

	// Draw Back Wall
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -depth / 2);
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	DrawWall(width, height, 30);
	glPopMatrix();

	// Draw Left Wall
	glPushMatrix();
	glTranslatef(-width / 2, 0.0f, 0.0f);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	DrawWall(width, height, 30);
	glPopMatrix();

	// Draw Right Wall
	glPushMatrix();
	glTranslatef(width / 2, 0.0f, 0.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	DrawWall(width, height, 30);
	glPopMatrix();

	// Draw Floor
	glPushMatrix();
	glTranslatef(0.0f, -height / 2, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	DrawWall(width, height, 30);
	glPopMatrix();
}
void CGLRenderer::DrawBase()
{
	glPushMatrix();
	glColor3f(0.8, 0.8, 0.8);
	DrawSphere(0.8, 30, 30);
	glTranslatef(0.0, 0.5, 0.0);
	DrawCylinder(0.8, 0.4, 0.4, 8);
	glTranslatef(0.0, 0.8, 0.0);

	glPushMatrix();
	glRotatef(45, 0.0, 1.0, 0.0);
	DrawCylinder(0.4, 1.2, 1.2, 4);
	glPopMatrix();
	glPopMatrix();
}
void CGLRenderer::DrawVase()
{
	double diff = 0.2;
	double radiusLarge = 0.75;
	double radiusMedium = radiusLarge - diff;
	double radiusSmall = radiusMedium - diff;
	double h = 0.2;
	int nSeg = 30;

	double vaseColor1[3] = { 1.0, 0.0, 1.0 };
	double vaseColor2[3] = { 0.0, 1.0, 1.0 };

	lightBlueMaterial->Select();
	DrawVaseLayer(false, h, radiusLarge, diff, nSeg, vaseColor2);
	glTranslatef(0.0, h, 0.0);
	blueMaterial->Select();
	DrawVaseLayer(false, h, radiusMedium, diff, nSeg, vaseColor1);
	glTranslatef(0.0, h, 0.0);
	lightBlueMaterial->Select();
	DrawVaseLayer(false, h, radiusSmall, 0, nSeg, vaseColor2);
	glTranslatef(0.0, h, 0.0);
	blueMaterial->Select();
	DrawVaseLayer(true, h, radiusSmall, 0, nSeg, vaseColor1);
	glTranslatef(0.0, h, 0.0);
	lightBlueMaterial->Select();
	DrawVaseLayer(true, h, radiusMedium, diff, nSeg, vaseColor2);
	glTranslatef(0.0, h, 0.0);
	blueMaterial->Select();
	DrawVaseLayer(false, h, radiusMedium, diff, nSeg, vaseColor1);
	glTranslatef(0.0, h, 0.0);
	lightBlueMaterial->Select();
	DrawVaseLayer(true, h, radiusMedium, diff, nSeg, vaseColor2);
	glTranslatef(0.0, h, 0.0);
	blueMaterial->Select();
	DrawVaseLayer(true, h, radiusLarge, diff, nSeg, vaseColor1);
	glTranslatef(0.0, h, 0.0);
	lightBlueMaterial->Select();
	DrawVaseLayer(false, h, radiusLarge, diff, nSeg, vaseColor2);
	glTranslatef(0.0, h, 0.0);
	blueMaterial->Select();
	DrawVaseLayer(false, h, radiusMedium, diff, nSeg, vaseColor1);
	glTranslatef(0, h, 0);
	lightBlueMaterial->Select();
	DrawVaseLayer(true, h, radiusMedium, diff, nSeg, vaseColor2);
	glTranslatef(0.0, h, 0.0);
	blueMaterial->Select();
	DrawVaseLayer(false, h, radiusMedium, diff, nSeg, vaseColor1);
	glTranslatef(0.0, h, 0.0);
	lightBlueMaterial->Select();
	DrawVaseLayer(true, h, radiusMedium, diff, nSeg, vaseColor2);
	glTranslatef(0.0, h, 0.0);
	blueMaterial->Select();
	DrawVaseLayer(true, h, radiusLarge, diff, nSeg, vaseColor1);

}


