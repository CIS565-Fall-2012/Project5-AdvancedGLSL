#ifndef MAIN_H
#define MAIN_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <vector>
#include "obj.h"

obj* objmesh;
int counter=0;

typedef struct {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<unsigned short> indices;
} mesh_t;

typedef struct {
	unsigned int vertex_array;
	unsigned int vbo_indices;
	unsigned int num_indices;
	//Don't need these to get it working, but needed for deallocation
	unsigned int vbo_vertices;
	unsigned int vbo_normals;
} device_mesh_t;



namespace mesh_attributes {
	enum {
		POSITION,
		NORMAL,
	};
}

typedef struct {
	unsigned int vertex_array;
	unsigned int vbo_indices;
	unsigned int num_indices;
	//Don't need these to get it working, but needed for deallocation
	unsigned int vbo_data;
} device_mesh2_t;

typedef struct {
	glm::vec3 pt;
	glm::vec2 texcoord;
} vertex2_t;

class Camera {
	public:
    float rx;
    float ry;
    float z;
	glm::vec2 pos;
	glm::vec3 up;
	glm::vec3 start_left;
	glm::vec3 start_dir;

	Camera(glm::vec3 start_pos,
		glm::vec3 start_dir,
		glm::vec3 up) : pos(start_pos.x, start_pos.y),
		z(start_pos.z),
		up(up),
        start_dir(start_dir),
		start_left(glm::cross(start_dir,up)),
        rx(0),
		ry(0) {}

	void adjust(float dx, // look left right
      float dy, //look up down
      float dz,
      float tx, //strafe left right
      float ty,
      float tz);//go forward) //strafe up down

	glm::mat4x4 get_view();
};

namespace quad_attributes {
	enum {
		POSITION,
		TEXCOORD
	};
}

enum Occlusion {
	OCCLUSION_NONE = 0,
	OCCLUSION_REGULAR_SAMPLES = 1,
	OCCLUSION_POISSON_SS_SAMPLES = 2,
	OCCLUSION_WORLD_SPACE_SAMPLES = 3
};

enum Display {
	DISPLAY_DEPTH = 0,
	DISPLAY_NORMAL = 1,
	DISPLAY_POSITION = 2,
	DISPLAY_OCCLUSION = 3,
	DISPLAY_TOTAL = 4
}	;


char* loadFile(char *fname, GLint &fSize);
void printShaderInfoLog(GLint shader);
void printLinkInfoLog(GLint prog);
void initLambert();
void initPassthrough();

void initMesh();
device_mesh_t uploadMesh(const mesh_t & mesh);

void display(void);
void keyboard(unsigned char, int, int);
void reshape(int, int);

int main (int argc, char* argv[]);


#endif Main_H