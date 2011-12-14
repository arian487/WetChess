/**
 * Modified By: Arian Karbasi
 * Student Id: 20308751
 * Userid: a3karbas
 */

#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include <stack>
#include "algebra.hpp"
#include "primitive.hpp"
#include "material.hpp"

class SceneNode {
public:
  SceneNode(const std::string& name);
  virtual ~SceneNode();

  virtual void walk_gl(bool picking = false, int shader_id = 0) const;

  const Matrix4x4& get_transform() const { return m_trans; }
  const Matrix4x4& get_inverse() const { return m_invtrans; }
  int get_id() { return m_id; }
  void set_id(int id) { m_id = id; }

  int get_pos() { return pos; }
  void set_pos(int new_pos) { pos = new_pos; }

  
  void set_transform(const Matrix4x4& m)
  {
    m_trans = m;
    m_invtrans = m.invert();
  }

  void set_transform(const Matrix4x4& m, const Matrix4x4& i)
  {
    m_trans = m;
    m_invtrans = i;
  }

  void add_child(SceneNode* child)
  {
	child->set_id(m_children.size());

	/*
	 * setting the position of the tile
	 */
	if (m_name == "white_tiles")
	{
		if ((int)(m_children.size() / 4) % 2 == 0)
			child->pos = m_children.size()*2+1;
		else
			child->pos = m_children.size()*2;
	}
	else if (m_name == "black_tiles")
	{
		if ((int)(m_children.size() / 4) % 2 == 0)
			child->pos = m_children.size()*2;
		else
			child->pos = m_children.size()*2+1;
	}
	else if (m_name == "water_pieces") {
		//there is no magic here, this is just the initial positions
		//of the pawns
		if (m_children.size() <= 7) {
			child->pos = m_children.size() + 8;
		}
		//the rest is the initial positions of the other pieces
		//based on the order that i draw them in lua
		else {
			if (m_children.size() == 8)
				child->pos = 0;
			if (m_children.size() == 9)
				child->pos = 7;
			if (m_children.size() == 10)
				child->pos = 2;
			if (m_children.size() == 11)
				child->pos = 5;
			if (m_children.size() == 12)
				child->pos = 1;
			if (m_children.size() == 13)
				child->pos = 6;
			if (m_children.size() == 14)
				child->pos = 3;
			if (m_children.size() == 15)
				child->pos = 4;
		}
	}
	else if (m_name == "fire_pieces") {
		//there is no magic here, this is just the initial positions
		//of the pawns
		if (m_children.size() <= 7) {
			child->pos = m_children.size() + 48;
		}
		//the rest is the initial positions of the other pieces
		//based on the order that i draw them in lua
		else {
			if (m_children.size() == 8)
				child->pos = 56;
			if (m_children.size() == 9)
				child->pos = 63;
			if (m_children.size() == 10)
				child->pos = 58;
			if (m_children.size() == 11)
				child->pos = 61;
			if (m_children.size() == 12)
				child->pos = 57;
			if (m_children.size() == 13)
				child->pos = 62;
			if (m_children.size() == 14)
				child->pos = 59;
			if (m_children.size() == 15)
				child->pos = 60;
		}
	}
    m_children.push_back(child);
  }

  void remove_child(SceneNode* child)
  {
    m_children.remove(child);
  }

  // Callbacks to be implemented.
  // These will be called from Lua.
  void rotate(char axis, double angle);
  void scale(const Vector3D& amount);
  void translate(const Vector3D& amount);

  // Returns true if and only if this node is a JointNode
  virtual bool is_joint() const;
  typedef std::list<SceneNode*> ChildList;
  ChildList m_children;
  std::string m_name;
  Matrix4x4 m_trans;
  Matrix4x4 orig_trans;

protected:

  int m_id;
  int pos;

  // Transformations
  Matrix4x4 m_invtrans;
  Matrix4x4 m_rotation;
  double cur_angle;

  // Hierarch
};

class JointNode : public SceneNode {
public:
  JointNode(const std::string& name);
  virtual ~JointNode();

  virtual void walk_gl(bool bicking = false, int shader_id = 0) const;

  virtual bool is_joint() const;

  virtual void rotate(char axis, double angle);
  virtual void translate(const Vector3D& amount);

  void set_joint_x(double min, double init, double max);
  void set_joint_y(double min, double init, double max);

  struct JointRange {
    double min, init, max;
  };
  std::stack<double> cur_angle_x_stack;
  std::stack<double> cur_angle_y_stack;
  std::stack<double> cur_angle_x_redo_stack;
  std::stack<double> cur_angle_y_redo_stack;
  double cur_angle_x;
  double cur_angle_y;

  
protected:

  JointRange m_joint_x, m_joint_y;
};

class GeometryNode : public SceneNode {
public:
  GeometryNode(const std::string& name,
               Primitive* primitive);
  virtual ~GeometryNode();

  virtual void walk_gl(bool picking = false, int shader_id = 0) const;

  const Material* get_material() const;
  Material* get_material();

  void set_material(Material* material)
  {
    m_material = material;
  }

  bool is_selected;


protected:
  Material* m_material;
  Primitive* m_primitive;
};

#endif
