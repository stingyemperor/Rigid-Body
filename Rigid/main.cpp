#include "Mesh.h"

#include <GL/glut.h>
#include <omp.h>
#include "objloader.hpp"

using namespace std;

float h = 0.01; //step size
static const int FPS =  60;
int rotateon;
int lastx, lasty;
int xchange, ychange;
float spin = 0.0;
float spinup = 0.0;

Mesh body;
std::vector<glm::vec3> vertices_o;

std::vector <glm::vec3> Vert;
std::vector<std::vector<int>> indices;
bool bounding = true;
glm::vec3 collisionPoint;




void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 1.0, 1, 600);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0, 0, 300, 0, 0, 0, 0, 1, 0);


	GLfloat light0color[] = { 0.5, 0.5, 0.5 };
	GLfloat light0pos[] = { 500, 500, 300 };
	GLfloat light1color[] = { 0.5, 0.5, 0.5 };
	GLfloat light1pos[] = { 1000, 1000, 1000 };
	glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0color);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0color);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0color);
	glLightfv(GL_LIGHT1, GL_POSITION, light1pos);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1color);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1color);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1color);
	
	if (bounding) {

		std::vector< glm::vec3 > normals;
		//bool res = loadOBJfacenormal("dodecahedron.obj", Vert, normals);
		//bool res = loadOBJfacenormal("dodecahedron.obj", Vert, normals);
		bool res = loadOBJfacenormal("isocahedron.obj", Vert, normals);
		//bool res = loadOBJfacenormal("gourd.obj", Vert, normals);
		Point point;

	
		for (int i = 0; i < Vert.size()/3; i++)
		{	
			float scale = 150;
			Vert[3 * i][0] *= scale;
			Vert[3 * i+1][0] *= scale;
			Vert[3 * i+2][0] *= scale;

			Vert[3 * i][1] *= scale;
			Vert[3 * i + 1][1] *= scale;
			Vert[3 * i + 2][1] *= scale;

			Vert[3 * i][2] *= scale;
			Vert[3 * i + 1][2] *= scale;
			Vert[3 * i + 2][2] *= scale;

			indices.push_back({ 3 * i,3 * i + 1,3 * i + 2 });
		}

		bounding = false;

		body.X = { -30.0,50.0,0.0 };
		body.L = { 600.0,300.0,0.0 };
		body.p = { 0.0,-30.0,2.0 };

	}

}

void draw_cube(Mesh body) {

	glPushMatrix();
	vertices_o = body.vertices_o();
	for (int i = 0; i < body.faces.size();i++) {

		glLineWidth(5.0);
		GLfloat lineColor[3] = { 0.4,0.0,0.0 };
		glBegin(GL_LINE_STRIP);
		glMaterialfv(GL_FRONT, GL_AMBIENT, lineColor);
		glVertex3f(vertices_o[body.faces[i][0]][0], vertices_o[body.faces[i][0]][1], vertices_o[body.faces[i][0]][2]);
		glVertex3f(vertices_o[body.faces[i][1]][0], vertices_o[body.faces[i][1]][1], vertices_o[body.faces[i][1]][2]);
		glVertex3f(vertices_o[body.faces[i][2]][0], vertices_o[body.faces[i][2]][1], vertices_o[body.faces[i][2]][2]);
		glVertex3f(vertices_o[body.faces[i][3]][0], vertices_o[body.faces[i][3]][1], vertices_o[body.faces[i][3]][2]);
		glVertex3f(vertices_o[body.faces[i][0]][0], vertices_o[body.faces[i][0]][1], vertices_o[body.faces[i][0]][2]);
		glEnd();
	
	}

}

void draw_environment() {
	glPushMatrix();
	
	for (int i = 0; i < indices.size(); i++) {

		glLineWidth(1.0);
		GLfloat lineColor[3] = { 0.5,0.3,0.5 };
		glBegin(GL_TRIANGLES);
		//glBegin(GL_LINE_STRIP);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, lineColor);

		for (int j = 0; j < indices[i].size(); j++) {
			glVertex3f(Vert[indices[i][j]][0], Vert[indices[i][j]][1], Vert[indices[i][j]][2]);
		}
		glVertex3f(Vert[indices[i][0]][0], Vert[indices[i][0]][1], Vert[indices[i][0]][2]);

		glEnd();
	}
	glPopMatrix();
}

void display() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	glRotatef(spinup, 1.0, 0.0, 0.0);
	glRotatef(spin, 0.0, 1.0, 0.0);

	draw_cube(body);
	draw_environment();

	glPopMatrix();
	glutSwapBuffers();
}

void timer(int v) {
	glutTimerFunc(1000 / FPS, timer, v);
}

void reshapeFunc(GLint newWidth, GLint newHeight)
{

	glViewport(0, (newHeight - newWidth) / 2, newWidth, newWidth);
	init();
	glutPostRedisplay();
}

void idle(void)
{
	
	if (rotateon) {
		spin += xchange / 250.0;
		if (spin >= 360.0) spin -= 360.0;
		if (spin < 0.0) spin += 360.0;
		spinup -= ychange / 250.0;
		if (spinup > 89.0) spinup = 89.0;
		if (spinup < -89.0) spinup = -89.0;
	}

	for (float t = 0; t < 1.0 / FPS; t += h) {
		
		collisionPoint = body.collision_check(Vert, indices, h);
		body.RK4step(h);
		body.update();

	}
	glutPostRedisplay();
}

void motion(int x, int y)
{
	xchange = x - lastx;
	ychange = y - lasty;
}

void mouse(int button, int state, int x, int y)
{
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {
			lastx = x;
			lasty = y;
			xchange = 0;
			ychange = 0;
			rotateon = 1;
		}
		else if (state == GLUT_UP) {
			xchange = 0;
			ychange = 0;
			rotateon = 0;
		}
		break;

	default:
		break;
	}
}

int main(int argc, char** argv) {
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(1920, 1080);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Hope");

	init();
	glutDisplayFunc(display);
	glutTimerFunc(100, timer, 0);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutIdleFunc(idle);
	glutReshapeFunc(reshapeFunc);

	glutMainLoop();

	return 0;
}