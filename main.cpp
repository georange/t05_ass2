#define WITH_GLFW
#define WITH_OPENGP
#define WITH_EIGEN

#include <OpenGP/GL/Eigen.h>
#include <GL/glew.h>

#include "common/icg_common.h"
#include "common/imshow.h"

#include <math.h>
#include <time.h>

#include "Triangle/Triangle.h"
#include "Quad/Quad.h"
#include "Wings.h"

using namespace OpenGP;

// transformation matrix and speed of time
typedef Eigen::Transform<float, 3, Eigen::Affine> Transform;
static const float SpeedFactor = 0.5;

// scene objects
Quad background;
Triangle triangle;
Quad quad;
Wings wings;

struct point
{
	float x;
	float y;
};

// simple linear interpolation between two points
void lerp(point& dest, const point& a, const point& b, const float t)
{
	dest.x = a.x + (b.x - a.x)*t;
	dest.y = a.y + (b.y - a.y)*t;
}

// evaluate a point on a bezier-curve (t goes from 0 to 1)
void bezier(point &dest, const point& a, const point& b, const point& c, const point& d, const float t)
{
	point ab, bc, cd, abbc, bccd;
	lerp(ab, a, b, t);           // point between a and b 
	lerp(bc, b, c, t);           // point between b and c 
	lerp(cd, c, d, t);           // point between c and d 
	lerp(abbc, ab, bc, t);       // point between ab and bc
	lerp(bccd, bc, cd, t);       // point between bc and cd
	lerp(dest, abbc, bccd, t);   // point on the bezier-curve
}

void display() {
	// alow for alpha channel textures
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	// set up time 
	float timeCount = glfwGetTime();
	float t = timeCount * SpeedFactor;
	Transform TRS = Transform::Identity();

	// control points for bezier curve
	point a = { 1, -3 };
	point b = { -3, -1 };
	point c = { 3, 1 };
	point d = { -1, 3 };

	point p;
	bezier(p, a, b, c, d, t);

	TRS *= Eigen::AlignedScaling3f(1.6, 1.6, 1);
	TRS *= Eigen::Translation3f(0, 1.35, 0);
	background.draw(TRS.matrix());

	//triangle.draw();

	float xcord = p.x;
	float ycord = p.y;

	// transformation matrix for quad
	TRS = Transform::Identity();
	TRS *= Eigen::AlignedScaling3f(abs(0.2 * sin(t * 4.0f)) + 0.3, abs(0.2 * sin(t * 4.0f)) + 0.3, 1);
	TRS *= Eigen::Translation3f(xcord, ycord, 0);
	quad.draw(TRS.matrix());

	// transformation matrix for wings
	//TRS *= Eigen::Translation3f(xcord, ycord, 0);
	TRS *= Eigen::AngleAxisf(0.05* (sin(t * 20)), Eigen::Vector3f::UnitZ());
	TRS *= Eigen::Translation3f(-xcord, -ycord, 0);
	//TRS *= Eigen::AlignedScaling3f(abs(0.2 * sin(t * 4.0f)) + 0.3, abs(0.2 * sin(t * 4.0f)) + 0.3, 1);
	TRS *= Eigen::Translation3f(xcord, ycord, 0);
	wings.draw(TRS.matrix());

	glDisable(GL_BLEND);
}

///
/// You can use this sub-project as a  starting point for your second
/// assignemnt. See the files triangle.h and quad.h for examples of
/// basic OpenGL code.
///

int main(int, char**) {
	// opens a window
	OpenGP::glfwInitWindowSize(840, 660);
	OpenGP::glfwMakeWindow("Assignment 2");

	// resets screen to black
	glClearColor(0.0, 0.0, 0.0, 0.0);

	// initialize objects
	background.init("Quad/bg.tga");
	triangle.init();
	quad.init("Quad/raft.tga");
	wings.init("Quad/paddle.tga");

	// invokes display() every frame
	OpenGP::glfwDisplayFunc(&display);

	// autopilot for mainloop
	OpenGP::glfwMainLoop();

	return EXIT_SUCCESS;

}