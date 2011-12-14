/**
 * Modified By: Arian Karbasi
 * Student Id: 20308751
 * Userid: a3karbas
 */

#ifndef CS488_MESH_HPP
#define CS488_MESH_HPP

#include <GL/glew.h>
#include "algebra.hpp"
#include <vector>
#include <iosfwd>
#include "primitive.hpp"
#include "material.hpp"
//#include <GL/gl.h>
//#include <GL/glu.h>

// A polygonal mesh.
class Mesh : public Primitive {
public:
	  struct Face {
		  std::vector<int> vertice_index;
		  std::vector<int> texture_index;
		  std::vector<int> normal_index;
		  double area;
	  };
	  struct VBO_vert {
		  float x;
		  float y;
		  float z;
	  };
	  struct VBO_text {
		  float x;
		  float y;
	  };
	  struct VBO_normal {
		  float x;
		  float y;
		  float z;
	  };
	  struct VBO_velocity {
		  float x;
		  float y;
		  float z;
	  };
  Mesh(const std::vector<Point3D>& verts,
       const std::vector< Face >& faces,
       const std::vector<Vector3D>& normals,
       const std::vector<Point2D>&textures,
       const std::vector< std::vector<Face> > &neighbouring_faces,
       const std::vector<float>& occlusion_factors);
  
  virtual void walk_gl(bool picking, int shader_id);
  virtual void calculate_occlusion_factors();
  virtual double clamp(double val, double min, double max);

private:

  //used for ambient occlusion
  struct Disk {
	  double area;
	  Vector3D normal;
	  Vector3D normal2;
	  Point3D position;
	  float total_attenuation;
  };

  std::vector<Point3D> m_verts;
  std::vector<Face> m_faces;
  std::vector<Vector3D> m_normals;
  std::vector<Point2D> m_textures;
  std::vector< std::vector<Face> >m_neighbouring_faces;
  std::vector<struct Disk> disks;
  GLuint call_list_id;
  bool should_make_list;

  //VBO stuff
  unsigned int m_vbo_verts_name;
  unsigned int m_vbo_texture_name;
  unsigned int m_vbo_normal_name;
  unsigned int m_vbo_occlusion_name;

  struct VBO_vert* m_vbo_vert;
  struct VBO_text* m_vbo_texture;
  struct VBO_normal* m_vbo_normal;
  float* m_vbo_occlusion;

};

#endif
