/**
 * Modified By: Arian Karbasi
 * Student Id: 20308751
 * Userid: a3karbas
 */

#include "mesh.hpp"
#include <iostream>
#include <fstream>

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector< Face >& faces,
           const std::vector<Vector3D>& normals,
           const std::vector<Point2D>& textures,
           const std::vector< std::vector<Face> > &neighbouring_faces,
           const std::vector<float>& occlusion_factors)
  : m_verts(verts),
    m_faces(faces),
    m_normals(normals),
    m_textures(textures),
    m_neighbouring_faces(neighbouring_faces)
{
	should_make_list = true;

	//calculate all of the disks for this object.
	//I take the vertice position to be the disk position,
	//the normal of the vertice to be the disk normal,
	//and then sum up the  disks of the neighbouring faces and
	//divide by num of neighbouring faces to get disk area
	disks.resize(verts.size());
	for (int i = 0; i < neighbouring_faces.size(); i++){
		Point3D vert_pos = verts[i];
		std::vector<Face> vertex_neighbours = neighbouring_faces[i];
		struct Disk vert_disk;
		vert_disk.area = 0;
		vert_disk.position = vert_pos;
		int index_of_normal;
		for (int j = 0; j < vertex_neighbours.size(); j++) {
			Face face = vertex_neighbours[j];
			vert_disk.area += face.area;
			//find the normal associated with this vertex
			for (int k = 0; k < face.vertice_index.size(); k++) {
				int index = face.vertice_index[k];
				if (index == i) {
					//once we find the matching vertice in the face
					index_of_normal = face.normal_index[k];
					break;
				}
			}
		}
		//inside we should have found the matching normal so we put it in our disk
		vert_disk.normal = normals[index_of_normal];
		vert_disk.normal2 = normals[index_of_normal];
		//the area of our disk is the sum of the areas of all the disks divided
		//by the number of neighbours
		vert_disk.area = vert_disk.area / vertex_neighbours.size();
		if (i < occlusion_factors.size())
			vert_disk.total_attenuation = occlusion_factors[i];
		disks[i] = vert_disk;
	}
	//now that we have our disks, lets dump the disk info into the shader
	//so we can calculate occlusion attenuations
	//calculate_occlusion_factors();
}

//calculate all of the occlusion factors for every disk
void Mesh::calculate_occlusion_factors()
{
	for (int i = 0; i < disks.size(); i++) {
		struct Disk receiver = disks[i];
		Vector3D receiver_to_emitter;
		float total_attenuation = 1;
		for (int j = 0; j < disks.size(); j++) {
			if (j != i) {
				struct Disk emitter = disks[j];
				receiver_to_emitter = Vector3D(emitter.position[0] - receiver.position[0],
						emitter.position[1] - receiver.position[1],
						emitter.position[2] - receiver.position[2]);

				//receiver_to_emitter.normalize();
				double distance_squared = receiver_to_emitter.length2();
				receiver_to_emitter.normalize();
				//this is the disk area approximation calculation
				double val = (1 - (1/sqrt(emitter.area/(distance_squared) + 1))) *
						clamp(emitter.normal.dot(receiver_to_emitter), 0, 1) *
						clamp(4 * receiver.normal.dot(receiver_to_emitter), 0, 1);
				//used for bent normals
				disks[i].normal = disks[i].normal - val*receiver_to_emitter;
//				std::cout<<4 * receiver.normal.dot(receiver_to_emitter)<<std::endl;
//				if (j == 20)
//					exit(-1);
				total_attenuation -= val;
			}
		}
		disks[i].total_attenuation = total_attenuation;
//		std::cout<<"the total attenuation of disk is "<<total_attenuation<<std::endl;
//		if (i == 50)
//			exit(-1);
	}
	//second pass.  What I'm doing here is literally the same calculation
	//except now I am multiplying the old emitter area by the new occlusion
	//factor.  This basically puts less of a credence on things that already
	//are in shadows so we don't get as many black portions in the mesh
	std::cout<<"first pass done"<<std::endl;
	for (int i = 0; i < disks.size(); i++) {
		struct Disk receiver = disks[i];
		Vector3D receiver_to_emitter;
		float total_attenuation = 1;
		for (int j = 0; j < disks.size(); j++) {
			if (j != i) {
				struct Disk emitter = disks[j];
				receiver_to_emitter = Vector3D(emitter.position[0] - receiver.position[0],
						emitter.position[1] - receiver.position[1],
						emitter.position[2] - receiver.position[2]);

				//receiver_to_emitter.normalize();
				double distance_squared = receiver_to_emitter.length2();
				receiver_to_emitter.normalize();
				emitter.area *= emitter.total_attenuation;
				if (emitter.area < 0)
					emitter.area = 0.000001;
				double val = (1 - (1/sqrt(emitter.area/(distance_squared) + 1))) *
						clamp(emitter.normal2.dot(receiver_to_emitter), 0, 1) *
						clamp(4 * receiver.normal2.dot(receiver_to_emitter), 0, 1);
				disks[i].normal2 = disks[i].normal2 - val*receiver_to_emitter;
//				std::cout<<4 * receiver.normal.dot(receiver_to_emitter)<<std::endl;
//				if (j == 20)
//					exit(-1);
				total_attenuation -= val;
			}
		}
		disks[i].total_attenuation = total_attenuation;
//		std::cout<<"the total attenuation of disk is "<<total_attenuation<<std::endl;
//		if (i == 50)
//			exit(-1);
	}
}

double Mesh::clamp(double val, double min, double max)
{
	return std::min(std::max(val, min), max);
}

void Mesh::walk_gl(bool picking, int shader_id)
{
	if (should_make_list) {
//		call_list_id = glGenLists(1);
////		std::cout<<index_display_list<<std::endl;
//		glNewList(call_list_id, GL_COMPILE);
////		int error = glGetError();
////		std::cout<<gluErrorString(error)<<std::endl;
//		for (int i = 0; i < m_faces.size(); i++) {
////			if (picking)
////				glPushName(i);
//			int vertex_index = -1;
//			int normal_index = -1;
//			int texture_index = -1;
//			Face face;
//			face = m_faces[i];
//			glBegin(GL_POLYGON);
//			for (int j = 0; j < m_faces[i].vertice_index.size(); j++) {
//				vertex_index = face.vertice_index[j];
//		//		std::cout<<face.vertice_index.size()<<std::endl;
//	//			std::cout<<face.vertice_index.size()<<std::endl;
//				if (face.normal_index.size() != 0)
//					normal_index = face.normal_index[j];
//				if (face.texture_index.size() != 0)
//					texture_index = face.texture_index[j];
//				if (normal_index != -1)
//					glNormal3f(m_normals[normal_index][0], m_normals[normal_index][1], m_normals[normal_index][2]);
//				if (texture_index != -1)
//					glTexCoord2f (m_textures[texture_index][0],m_textures[texture_index][1]);
//				glVertex3f(m_verts[vertex_index][0],m_verts[vertex_index][1],m_verts[vertex_index][2]);
//
//			}
//			glEnd();
////			if (picking)
////				glPopName();
//		}
//		glEndList();

		/*
		 * generate buffer data for vertices
		 */
		m_vbo_vert = new VBO_vert[m_faces.size() * 3];
		m_vbo_texture = new VBO_text[m_faces.size() * 3];
		m_vbo_normal = new VBO_normal[m_faces.size() * 3];
		m_vbo_occlusion = new float[m_faces.size() * 3];
		for (int i = 0; i < m_faces.size(); i++) {
			int vertex_index = -1;
			int texture_index = -1;
			int normal_index = -1;
			Face face;
			face = m_faces[i];
			for (int j = 0; j < m_faces[i].vertice_index.size(); j++) {
				vertex_index = face.vertice_index[j];
				if (face.normal_index.size() != 0)
					normal_index = face.normal_index[j];
				if (face.texture_index.size() != 0)
					texture_index = face.texture_index[j];

				Point3D vert = m_verts[vertex_index];
				Point2D texture = m_textures[texture_index];
				Vector3D normal = m_normals[normal_index];
				float occlusion = disks[vertex_index].total_attenuation;
//				std::cout<<vertex_index<<std::endl;
//				std::cout<<occlusion<<std::endl;
//				if (i == 20)
//					exit(-1);

				//set the vbo vert struct
				m_vbo_vert[(i * 3) + j].x = vert[0];
				m_vbo_vert[(i * 3) + j].y = vert[1];
				m_vbo_vert[(i * 3) + j].z = vert[2];

				//set the vbo texture struct
				m_vbo_texture[(i * 3) + j].x = texture[0];
				m_vbo_texture[(i * 3) + j].y = texture[1];

				//set the vbo normal struct
				m_vbo_normal[(i * 3) + j].x = normal[0];
				m_vbo_normal[(i * 3) + j].y = normal[1];
				m_vbo_normal[(i * 3) + j].z = normal[2];

				//set the vbo occlusion
				m_vbo_occlusion[(i * 3) + j] = occlusion;

				//set the occlusion factor
			}
		}
		// Generate And Bind The Vertex Buffer
		glGenBuffers( 1, &m_vbo_verts_name );                  // Get A Valid Name
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_verts_name );         // Bind The Buffer
		glBufferData( GL_ARRAY_BUFFER, m_faces.size()*9*sizeof(float), m_vbo_vert, GL_STATIC_DRAW );

		//generate and bind the texture buffer
		glGenBuffers( 1, &m_vbo_texture_name );                  // Get A Valid Name
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_texture_name );         // Bind The Buffer
		glBufferData( GL_ARRAY_BUFFER, m_faces.size()*6*sizeof(float), m_vbo_texture, GL_STATIC_DRAW );

		//generate and bind the normal buffer
		glGenBuffers( 1, &m_vbo_normal_name );                  // Get A Valid Name
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_normal_name );         // Bind The Buffer
		glBufferData( GL_ARRAY_BUFFER, m_faces.size()*9*sizeof(float), m_vbo_normal, GL_STATIC_DRAW );

		//generate and bind the normal buffer
		glGenBuffers( 1, &m_vbo_occlusion_name );                  // Get A Valid Name
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_occlusion_name );         // Bind The Buffer
		glBufferData( GL_ARRAY_BUFFER, m_faces.size()*3*sizeof(float), m_vbo_occlusion, GL_STATIC_DRAW );


		should_make_list = false;
	}

	int location_occlusion = glGetAttribLocation(shader_id, "occlusion");

	glBindBuffer( GL_ARRAY_BUFFER, m_vbo_verts_name );
	glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)((char*)NULL));
	glBindBuffer( GL_ARRAY_BUFFER, m_vbo_texture_name );
	glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)((char*)NULL));
	glBindBuffer( GL_ARRAY_BUFFER, m_vbo_normal_name );
	glNormalPointer(GL_FLOAT, 0, (GLvoid*)((char*)NULL));
	glBindBuffer ( GL_ARRAY_BUFFER, m_vbo_occlusion_name );
	glVertexAttribPointer(location_occlusion, 1, GL_FLOAT, GL_FALSE, 0, (GLvoid*)((char*)NULL));

	glEnableVertexAttribArray( location_occlusion );
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );

	glDrawArrays(GL_TRIANGLES,0, m_faces.size()*3);

	glDisableVertexAttribArray( location_occlusion );
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );

	//glCallList(call_list_id);
}
