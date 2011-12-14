/**
 * Modified By: Arian Karbasi
 * Student Id: 20308751
 * Userid: a3karbas
 */

#include "scene.hpp"
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>


SceneNode::SceneNode(const std::string& name) :
	m_name(name)
{
}

SceneNode::~SceneNode()
{
}

void SceneNode::walk_gl(bool picking, int shader_id) const
{
	  glMatrixMode(GL_MODELVIEW);
	  glPushMatrix();
	  glPushName(m_id);

	  //lets multiply the opengl matrix by the translation of our scene
	  glMultMatrixd(m_trans.transpose().begin());
	  glMatrixMode(GL_TEXTURE);
	  glActiveTexture(GL_TEXTURE7);
	  glPushMatrix();
	  glMultMatrixd(m_trans.transpose().begin());

	  //now we go through and call walk on the children
	  for (ChildList::const_iterator children = m_children.begin(); children != m_children.end(); ++children)
	  {
	    (*children)->walk_gl(picking, shader_id);
	  }
	  glPopName();
	  glMatrixMode(GL_MODELVIEW);
	  glPopMatrix();
	  glMatrixMode(GL_TEXTURE);
	  glPopMatrix();
}

void SceneNode::rotate(char axis, double angle)
{
	angle = (angle/180.0)*M_PI;

	  Matrix4x4 r = Matrix4x4();

	  if (axis == 'x') {
		  r[1][1] = cos(angle);
		  r[1][2] = -sin(angle);
		  r[2][1] = sin(angle);
		  r[2][2] = cos(angle);
	  }
	  else if (axis == 'y') {
		  r[0][0] = cos(angle);
		  r[0][2] = sin(angle);
		  r[2][0] = -sin(angle);
		  r[2][2] = cos(angle);
	  }
	  else if (axis == 'z') {
		  r[0][0] = cos(angle);
		  r[0][1] = -sin(angle);
		  r[1][0] = sin(angle);
		  r[1][1] = cos(angle);
	  }
	  m_trans = m_trans * r;
	  set_transform(m_trans);
}

void SceneNode::scale(const Vector3D& amount)
{
	Matrix4x4 s = Matrix4x4();
	//here we are scaling in the x direction
	s[0][0] = amount[0];
	//here we are scaling in the y direction
	s[1][1] = amount[1];
	//here we are scaling in the z direction
	s[2][2] = amount[2];

	m_trans = m_trans * s;
	set_transform(m_trans);

}

void SceneNode::translate(const Vector3D& displacement)
{

	Matrix4x4 t = Matrix4x4();

	//this is the x displacementm_children
	t[0][3] = displacement[0];// Fill me in
	//this is the y displacement
	t[1][3] = displacement[1];
	//this is the z displacement
	t[2][3] = displacement[2];

	m_trans = m_trans * t;
	set_transform(m_trans);
}

bool SceneNode::is_joint() const
{
	return false;
}

JointNode::JointNode(const std::string& name) :
	SceneNode(name)
{
	cur_angle_x = 0;
	cur_angle_y = 0;
	m_rotation = Matrix4x4();
	cur_angle_x_stack.push(cur_angle_x);
	cur_angle_y_stack.push(cur_angle_y);
}

JointNode::~JointNode()
{
}

void JointNode::walk_gl(bool picking, int shader_id) const
{
	  glPushMatrix();
	  glPushName(m_id);
	  //multiply the current open gl matrix by the translation of the joint
	  glMultMatrixd(m_trans.transpose().begin());
	  for (ChildList::const_iterator children = m_children.begin();children != m_children.end(); ++children)
	  {
	    (*children)->walk_gl(picking, shader_id);
	  }
	  glPopName();
	  glPopMatrix();

}

void JointNode::translate(const Vector3D& displacement)
{
	Matrix4x4 t = Matrix4x4();

	//this is the x displacementm_children
	t[0][3] = displacement[0];// Fill me in
	//this is the y displacement
	t[1][3] = displacement[1];
	//this is the z displacement
	t[2][3] = displacement[2];

	m_trans = m_trans * t;
	set_transform(m_trans);
}


void JointNode::rotate(char axis, double angle)
{
	if (m_name == "upper_neck_joint") {
		m_rotation = Matrix4x4();
		Matrix4x4 r = Matrix4x4();
		  Matrix4x4 r1 = Matrix4x4();
		if (angle + cur_angle_x > m_joint_y.min && angle + cur_angle_x < m_joint_y.max && axis == 'x') {
			cur_angle_x += angle;
		//	std::cout<<"in here x" << std::endl;
		}
	//	std::cout<<"cur angle x:" <<cur_angle_x<<std::endl;

			double new_angle = (cur_angle_x/180.0)*M_PI;


			r[1][1] = cos(new_angle);
			r[1][2] = -sin(new_angle);
			r[2][1] = sin(new_angle);
			r[2][2] = cos(new_angle);

		if (angle + cur_angle_y > m_joint_x.min && angle + cur_angle_y < m_joint_x.max && axis == 'y') {
		//	std::cout<<"in here y" << std::endl;
			cur_angle_y += angle;
		}
	//	std::cout<<"cur_angle_y: "<<cur_angle_y<<std::endl;

			angle = (cur_angle_y/180.0)*M_PI;

			  r1[0][0] = cos(angle);
			  r1[0][2] = sin(angle);
			  r1[2][0] = -sin(angle);
			  r1[2][2] = cos(angle);

		m_rotation = r*r1;
		  m_trans = orig_trans * m_rotation;
		  set_transform(m_trans);
	}
	else {
		if (axis == 'x') {
			if (angle + cur_angle_x > m_joint_y.min && angle + cur_angle_x < m_joint_y.max) {
				cur_angle_x += angle;

				angle = (angle/180.0)*M_PI;

				  Matrix4x4 r = Matrix4x4();

				  r[1][1] = cos(angle);
				  r[1][2] = -sin(angle);
				  r[2][1] = sin(angle);
				  r[2][2] = cos(angle);

				  m_trans = m_trans * r;
				  set_transform(m_trans);
			}
		}
		else if (axis == 'y')
		{
			if (angle + cur_angle_y > m_joint_x.min && angle + cur_angle_y < m_joint_x.max) {
				cur_angle_y += angle;

				angle = (angle/180.0)*M_PI;

				  Matrix4x4 r = Matrix4x4();

				  r[0][0] = cos(angle);
				  r[0][2] = sin(angle);
				  r[2][0] = -sin(angle);
				  r[2][2] = cos(angle);

				  m_trans = m_trans * r;
				  set_transform(m_trans);
			}
		}
	}
}

bool JointNode::is_joint() const
{
	return true;
}

void JointNode::set_joint_x(double min, double init, double max)
{
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;
}

void JointNode::set_joint_y(double min, double init, double max)
{
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive) :
	SceneNode(name), m_primitive(primitive)
{
	is_selected = false;
}

GeometryNode::~GeometryNode()
{
}

//TODO:  need to code material stuff
void GeometryNode::walk_gl(bool picking, int shader_id) const
{
	glMatrixMode(GL_MODELVIEW);
	//after pushing the matrix on stack, we multiply, call walk on others, and pop
	glPushMatrix();
	//doing transpose because open opengl expects column matrix while matrix4x4
	//is row matrix.  Begin returns the double pointer we want
	glMultMatrixd(m_trans.transpose().begin());
	glMatrixMode(GL_TEXTURE);
	glActiveTexture(GL_TEXTURE7);
	glPushMatrix();
	glMultMatrixd(m_trans.transpose().begin());
//	m_material->apply_gl();
	m_primitive->walk_gl(picking, shader_id);
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

Material* GeometryNode::get_material()
{
	return m_material;
}


