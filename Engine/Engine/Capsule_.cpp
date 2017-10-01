#include "Capsule_.h"
#include "MathGeoLib/Geometry/Capsule.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"

Capsule_::Capsule_() :Primitive_(PRIMITIVE_CAPSULE)
{
	geometry = new math::Capsule();
}

Capsule_::Capsule_(const Capsule_ & _cpy) : Primitive_(_cpy)
{
	geometry = new math::Capsule(*_cpy.geometry);
}

Capsule_::~Capsule_()
{
	RELEASE(geometry);
}

void Capsule_::Initialize()
{
	double i, resolution = 0.1;
	double height = 1, radius = 0.5;
	glPushMatrix();
	glTranslatef(0, -0.5, 0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0, height, 0);  /* center */
	for (i = 0; i <= 2 * HAVE_M_PI; i += resolution)
	{
		glVertex3f(radius * cos(i), height, radius * sin(i));
	}
	glEnd();

	/* bottom triangle: note: for is in reverse order */
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0, 0, 0);  /* center */
	for (i = 2 * HAVE_M_PI; i >= 0; i -= resolution)
	{
		glVertex3f(radius * cos(i), 0, radius * sin(i));
	}
	/* close the loop back to 0 degrees */
	glVertex3f(radius, height, 0);
	glEnd();

	/* middle tube */
	glBegin(GL_QUAD_STRIP);
	for (i = 0; i <= 2 * HAVE_M_PI; i += resolution)
	{
		glVertex3f(radius * cos(i), 0, radius * sin(i));
		glVertex3f(radius * cos(i), height, radius * sin(i));
	}
	/* close the loop back to zero degrees */
	glVertex3f(radius, 0, 0);
	glVertex3f(radius, height, 0);
	glEnd();
	glPopMatrix();
}

void Capsule_::Draw()
{

	
}
