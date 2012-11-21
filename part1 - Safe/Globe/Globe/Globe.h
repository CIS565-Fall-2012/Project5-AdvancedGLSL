#ifndef GLOBE_H
#define GLOBE_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <vector>

//Struct for building sphere on host
typedef struct {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> u_tangent;
	std::vector<glm::vec3> v_tangent;
	std::vector<unsigned short> indices;
} mesh_t;

//Struct to represent sphere on device
typedef struct {
	unsigned int vertex_array;
	unsigned int vbo_vertices;
	unsigned int vbo_normals;
	unsigned int vbo_uvs;
	unsigned int vbo_u_tangent;
	unsigned int vbo_v_tangent;
	unsigned int vbo_indices;
	int num_indices;
} device_mesh_t;

//Structure to track the numbers attributes are bound to
namespace attributes {
	enum {
		POSITION,
		NORMAL,
		UV
	};
}

//Loading and compiling shaders
GLuint initShader(char * vert_path,
				  char * frag_path,
				  const unsigned int * additional_params, 
				  const char ** additional_names,
				  int num_params);
void initGlobeShader();
void setGlobeShader();

glm::vec3 computeSpherical(glm::vec2 uv, float radius);
glm::vec3 computePosition(glm::vec3 spherical) ;
glm::vec3 computeNormal(glm::vec3 spherical);
void appendPoint(mesh_t * mesh, glm::vec2 uv, float radius);
void initSphere();
device_mesh_t uploadMesh(const mesh_t & mesh);

void adjustRotation(float adjustment);
void speedUpRotation();
void slowDownRotation();
glm::mat4 update_rotation();
glm::mat4 get_view();

void display(void);
void keyboard(unsigned char, int, int);
void reshape(int, int);

#endif GLOBE_H