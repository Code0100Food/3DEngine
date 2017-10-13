#include "Grid_.h"

#include <gl/GL.h>


// Constructors =================================
Grid_::Grid_():Primitive_(PRIMITIVE_TYPE::PRIMITIVE_GRID)
{

}

Grid_::Grid_(const Grid_ & cpy) : Primitive_(cpy)
{

}

// Destructors ==================================
Grid_::~Grid_()
{

}

// Game Loop ====================================
void Grid_::Draw()
{
	glLineWidth(2.5f);

	glBegin(GL_LINES);
	
	int i = (divisions * -1);
	int k = divisions;
	
	glColor4f(color.r, color.g, color.b, color.a);

	while (i <= k)
	{
		glVertex3f(i, 0.0f, -k);
		glVertex3f(i, 0.0f, k);
		glVertex3f(-k, 0.0f, i);
		glVertex3f(k, 0.0f, i);

		i++;
	}

	glEnd();

	if (axis)DrawAxis();

}
