/**
 * Modified By: Arian Karbasi
 * Student Id: 20308751
 * Userid: a3karbas
 */

#include "primitive.hpp"

#include <GL/gl.h>
#include <GL/glu.h>


Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

void Primitive::walk_gl(bool picking, int shader_id)
{

}

void Sphere::walk_gl(bool picking, int shader_id)
{
	  glCallList(1);
//	  glBegin(GL_TRIANGLES);
//	  glTranslatef(50,50,30);
//	  glVertex3f(1,0,0);
//	  glVertex3f(0,1,0);
//	  glVertex3f(0,0,1);
//	  glEnd();
	  glFlush();
}
