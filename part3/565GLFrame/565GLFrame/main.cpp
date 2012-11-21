#include "main.h"

#include "Utility.h"
#include "objloader.h"

#include <GL/glut.h>
#include "SOIL.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_projection.hpp>
#include <glm/gtc/matrix_operation.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/verbose_operator.hpp>

#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;
using namespace glm;

const float PI = 3.14159f;

int width, height;

enum DisplayVertex displayVertex_type = DISPLAY_V_NORMAL;
vec4 boneMatrix[9];

vector<device_mesh_t> draw_meshes;

device_mesh_t uploadMesh(const mesh_t & mesh) {
	device_mesh_t out;
	//Allocate vertex array
	//Vertex arrays encapsulate a set of generic vertex attributes and the buffers they are bound too
	//Different vertex array per mesh.
	glGenVertexArrays(1, &(out.vertex_array));
    glBindVertexArray(out.vertex_array);
    
	//Allocate vbos for data
	glGenBuffers(1,&(out.vbo_vertices));
	glGenBuffers(1,&(out.vbo_normals));
	glGenBuffers(1,&(out.vbo_indices));
    
	//Upload vertex data
	glBindBuffer(GL_ARRAY_BUFFER, out.vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size()*sizeof(vec3), &mesh.vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(mesh_attributes::POSITION, 3, GL_FLOAT, GL_FALSE,0,0);
	glEnableVertexAttribArray(mesh_attributes::POSITION);

    //VBO for normal data
	glBindBuffer(GL_ARRAY_BUFFER, out.vbo_normals);
	glBufferData(GL_ARRAY_BUFFER, mesh.normals.size()*sizeof(vec3), &mesh.normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(mesh_attributes::NORMAL, 3, GL_FLOAT, GL_FALSE,0,0);
	glEnableVertexAttribArray(mesh_attributes::NORMAL);

	//VBO for bone weights Index
	glBindBuffer(GL_ARRAY_BUFFER, out.vbo_boneIndex);
	glBufferData(GL_ARRAY_BUFFER, mesh.boneWeightIndex.size()*sizeof(vec2), &mesh.boneWeightIndex[0], GL_STATIC_DRAW);
	glVertexAttribPointer(mesh_attributes::BONEINDEX, 2, GL_FLOAT, GL_FALSE,0,0);
	glEnableVertexAttribArray(mesh_attributes::BONEINDEX);

	//VBO for bone weights
	glBindBuffer(GL_ARRAY_BUFFER, out.vbo_boneWeights);
	glBufferData(GL_ARRAY_BUFFER, mesh.boneWeights.size()*sizeof(vec2), &mesh.boneWeights[0], GL_STATIC_DRAW);
	glVertexAttribPointer(mesh_attributes::BONEWEIGHT, 2, GL_FLOAT, GL_FALSE,0,0);
	glEnableVertexAttribArray(mesh_attributes::BONEWEIGHT);

    //indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out.vbo_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size()*sizeof(GLushort), &mesh.indices[0], GL_STATIC_DRAW);
    out.num_indices = mesh.indices.size();
	//Unplug Vertex Array
    glBindVertexArray(0);
    return out;
}


int num_boxes = 3;
const int DUMP_SIZE = 1024;

void initMesh() {
	int totalsize = objmesh->getFaces()->size();
	int f = 0;

	/*vector<vec3> bonePos;
	bonePos.push_back(vec3(-1.0, 0.5, 0));
	boneMatrix[0] = vec4(cos(5*PI/180.0),-sin(5*PI/180.0),0,0);
	boneMatrix[1] = vec4(sin(5*PI/180.0),cos(5*PI/180.0),0,0);
	boneMatrix[2] = vec4(0,0,1,0);

	bonePos.push_back(vec3(-0.25, 0.5, 0));
	boneMatrix[3] = vec4(cos(10*PI/180.0),-sin(10*PI/180.0),0,0);
	boneMatrix[4] = vec4(sin(10*PI/180.0),cos(10*PI/180.0),0,0);
	boneMatrix[5] = vec4(0,0,1,0);

	bonePos.push_back(vec3(0, 0.5, 0));
	boneMatrix[9] = vec4(1,0,0,0);
	boneMatrix[10] = vec4(0,1,0,0);
	boneMatrix[11] = vec4(0,0,1,0);

	bonePos.push_back(vec3(0.25, 0.5, 0));
	boneMatrix[6] = vec4(1,0,0,0);
	boneMatrix[7] = vec4(0,1,0,0);
	boneMatrix[8] = vec4(0,0,1,0);

	bonePos.push_back(vec3(1.0, 0.5, 0));
	boneMatrix[12] = vec4(1,0,0,0);
	boneMatrix[13] = vec4(0,1,0,0);
	boneMatrix[14] = vec4(0,0,1,0);*/

	vector<vec3> bonePos;
	bonePos.push_back(vec3(0, -0.25, 0));
	boneMatrix[0] = vec4(1,0,0,0);
	boneMatrix[1] = vec4(0,cos(5*PI/180.0),-sin(5*PI/180.0),0);
	boneMatrix[2] = vec4(0,sin(5*PI/180.0),cos(5*PI/180.0),0);

	bonePos.push_back(vec3(0, 0, 0));
	boneMatrix[3] = vec4(cos(10*PI/180.0),-sin(10*PI/180.0),0,0);
	boneMatrix[4] = vec4(sin(10*PI/180.0),cos(10*PI/180.0),0,0);
	boneMatrix[5] = vec4(0,0,1,0);

	bonePos.push_back(vec3(0, 0.25, 0));
	boneMatrix[6] =  vec4(1,0,0,0);
	boneMatrix[7] =  vec4(0,1,0,0);  
	boneMatrix[8] =  vec4(0,0,1,0);  


	while(f<totalsize){
		mesh_t mesh;
		int process = std::min(10000, totalsize-f);
		int points = 0;
		for(int i=f; i<process+f; i++){
			
			vector<int> face = objmesh->getFaces()->operator[](i);
			//vector<int> facenormal =  objmesh->getFaceNormals()->operator[](i);

			mesh.vertices.push_back(glm::vec3(objmesh->getPoints()->operator[](face[0])));
			mesh.vertices.push_back(glm::vec3(objmesh->getPoints()->operator[](face[1])));
			mesh.vertices.push_back(glm::vec3(objmesh->getPoints()->operator[](face[2])));

			mesh.boneWeights.push_back(glm::vec2(0,0));
			mesh.boneWeights.push_back(glm::vec2(0,0));
			mesh.boneWeights.push_back(glm::vec2(0,0));

			mesh.boneWeightIndex.push_back(glm::vec2(-1,-1));
			mesh.boneWeightIndex.push_back(glm::vec2(-1,-1));
			mesh.boneWeightIndex.push_back(glm::vec2(-1,-1));

			int vertSize = mesh.vertices.size();
			float minValue = 100.0;
			float secondMin = minValue;
			int weightIndex = 0;
			for (int vertIndex = vertSize-1;  vertIndex >= vertSize - 3; --vertIndex)
			{
				for (int boneIndex = 0; boneIndex < bonePos.size(); ++boneIndex)
				{
					if (mesh.vertices[vertIndex].y == bonePos[boneIndex].y)
					{
						mesh.boneWeightIndex[vertIndex].x = boneIndex;
						mesh.boneWeights[vertIndex].x = 1.0;
						break;
					}
					else if (mesh.vertices[vertIndex].y < bonePos[boneIndex].y)
					{
						if (boneIndex == 0)
						{
							mesh.boneWeightIndex[vertIndex].x = boneIndex;
							mesh.boneWeights[vertIndex].x = 1.0;
							//mesh.vertices[vertIndex] = vec3(0);
							break;
						}
						else
						{
							mesh.boneWeights[vertIndex].x = abs(bonePos[boneIndex].y - mesh.vertices[vertIndex].y);
							mesh.boneWeights[vertIndex].y = abs(bonePos[boneIndex-1].y - mesh.vertices[vertIndex].y);
							float tot = mesh.boneWeights[vertIndex].x + mesh.boneWeights[vertIndex].y;

							// Bring the values to between 0 and 1
							mesh.boneWeights[vertIndex].x /= tot;
							mesh.boneWeights[vertIndex].y /= tot;

							mesh.boneWeightIndex[vertIndex].x = boneIndex;
							mesh.boneWeightIndex[vertIndex].y = boneIndex-1;
							break;
						}
					}
					else if((boneIndex == bonePos.size() - 1) && mesh.vertices[vertIndex].y > bonePos[boneIndex].y)
					{
						mesh.boneWeightIndex[vertIndex].x = boneIndex;
						mesh.boneWeights[vertIndex].x = 1.0;
						//mesh.vertices[vertIndex] = vec3(0);
						break;
					}
				}
			}
			
			/*mesh.normals.push_back(glm::vec3(objmesh->getNormals()->operator[](facenormal[0])));
			mesh.normals.push_back(glm::vec3(objmesh->getNormals()->operator[](facenormal[1])));
			mesh.normals.push_back(glm::vec3(objmesh->getNormals()->operator[](facenormal[2])));*/

			mesh.normals.push_back(glm::vec3(objmesh->getSmoothNormals()->operator[](face[0])));
			mesh.normals.push_back(glm::vec3(objmesh->getSmoothNormals()->operator[](face[1])));
			mesh.normals.push_back(glm::vec3(objmesh->getSmoothNormals()->operator[](face[2])));

			mesh.indices.push_back(points);
			mesh.indices.push_back(points+1);
			mesh.indices.push_back(points+2);
			points = points + 3;
		}
		draw_meshes.push_back(uploadMesh(mesh));

		// Temp - To find Max
		// TODO - Please Remove
		vec3 max = mesh.vertices[0];
		vec3 min = max;
		for (unsigned int i = 0; i<mesh.vertices.size(); ++i)
		{
			if (max.x < mesh.vertices[i].x) max.x = mesh.vertices[i].x;
			if (max.y < mesh.vertices[i].y) max.y = mesh.vertices[i].y;
			if (max.z < mesh.vertices[i].z) max.z = mesh.vertices[i].z;

			if (min.x > mesh.vertices[i].x) min.x = mesh.vertices[i].x;
			if (min.y > mesh.vertices[i].y) min.y = mesh.vertices[i].y;
			if (min.z > mesh.vertices[i].z) min.z = mesh.vertices[i].z;
		}

		std::cout<<"Max: " << max.x << ", " << max.y << ", " << max.z << std::endl;
		std::cout<<"Min: " << min.x << ", " << min.y << ", " << min.z << std::endl;

		f=f+process;
	}
}


device_mesh2_t device_quad;
void initQuad() {
	vertex2_t verts [] = { {vec3(-1,1,0),vec2(0,1)},
	{vec3(-1,-1,0),vec2(0,0)},
	{vec3(1,-1,0),vec2(1,0)},
	{vec3(1,1,0),vec2(1,1)}};

	unsigned short indices[] = { 0,1,2,0,2,3};

	//Allocate vertex array
	//Vertex arrays encapsulate a set of generic vertex attributes and the buffers they are bound too
	//Different vertex array per mesh.
	glGenVertexArrays(1, &(device_quad.vertex_array));
    glBindVertexArray(device_quad.vertex_array);

    
	//Allocate vbos for data
	glGenBuffers(1,&(device_quad.vbo_data));
	glGenBuffers(1,&(device_quad.vbo_indices));
    
	//Upload vertex data
	glBindBuffer(GL_ARRAY_BUFFER, device_quad.vbo_data);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    //Use of strided data, Array of Structures instead of Structures of Arrays
	glVertexAttribPointer(quad_attributes::POSITION, 3, GL_FLOAT, GL_FALSE,sizeof(vertex2_t),0);
	glVertexAttribPointer(quad_attributes::TEXCOORD, 2, GL_FLOAT, GL_FALSE,sizeof(vertex2_t),(void*)sizeof(vec3));
	glEnableVertexAttribArray(quad_attributes::POSITION);
	glEnableVertexAttribArray(quad_attributes::TEXCOORD);

    //indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, device_quad.vbo_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(GLushort), indices, GL_STATIC_DRAW);
    device_quad.num_indices = 6;
	//Unplug Vertex Array
    glBindVertexArray(0);
}


GLuint depthTexture = 0;
GLuint normalTexture = 0;
GLuint positionTexture = 0;
GLuint FBO = 0;

GLuint ssaoTexture = 0;

GLuint pass_prog;
void initPass() {
	Utility::shaders_t shaders = Utility::loadShaders("pass.vert", "pass.frag");

	pass_prog = glCreateProgram();

	glBindAttribLocation(pass_prog,mesh_attributes::POSITION, "Position");
	glBindAttribLocation(pass_prog,mesh_attributes::NORMAL, "Normal");

	Utility::attachAndLinkProgram(pass_prog,shaders);
	
}

GLuint pulse_prog;
void initPulse() {
	Utility::shaders_t shaders = Utility::loadShaders("pulse.vert", "pulse.frag");

	pulse_prog = glCreateProgram();

	glBindAttribLocation(pulse_prog,mesh_attributes::POSITION, "Position");
	glBindAttribLocation(pulse_prog,mesh_attributes::NORMAL, "Normal");

	Utility::attachAndLinkProgram(pulse_prog,shaders);
	
}

GLuint skin_prog;
void initSkining() {
	Utility::shaders_t shaders = Utility::loadShaders("skining.vert", "skining.frag");

	skin_prog = glCreateProgram();

	glBindAttribLocation(skin_prog, mesh_attributes::POSITION, "Position");
	glBindAttribLocation(skin_prog, mesh_attributes::NORMAL, "Normal");
	glBindAttribLocation(skin_prog, mesh_attributes::BONEWEIGHT, "BoneWeight");
	glBindAttribLocation(skin_prog, mesh_attributes::BONEINDEX, "BoneIndex");

	Utility::attachAndLinkProgram(skin_prog,shaders);
	
}

GLuint morph_prog;
void initMorph() {
	Utility::shaders_t shaders = Utility::loadShaders("morph.vert", "morph.frag");

	morph_prog = glCreateProgram();

	glBindAttribLocation(morph_prog,mesh_attributes::POSITION, "Position");
	glBindAttribLocation(morph_prog,mesh_attributes::NORMAL, "Normal");

	Utility::attachAndLinkProgram(morph_prog,shaders);
	
}

GLuint ssao_prog;
void initSSAO() {
	Utility::shaders_t shaders = Utility::loadShaders("ssao.vert", "ssao.frag");

    ssao_prog = glCreateProgram();

	glBindAttribLocation(ssao_prog, quad_attributes::POSITION, "Position");
	glBindAttribLocation(ssao_prog, quad_attributes::TEXCOORD, "Texcoord");
 
	Utility::attachAndLinkProgram(ssao_prog, shaders);
}

GLuint blur_prog;
void initBlur() {
	Utility::shaders_t shaders = Utility::loadShaders("blur.vert", "blur.frag");

    blur_prog = glCreateProgram();

	glBindAttribLocation(blur_prog, 0, "Position");
	glBindAttribLocation(blur_prog, 1, "Texcoord");
 
	Utility::attachAndLinkProgram(blur_prog, shaders);
}

void freeFBO() {
	glDeleteTextures(1,&depthTexture);
    glDeleteTextures(1,&normalTexture);
    glDeleteTextures(1,&positionTexture);
    glDeleteFramebuffers(1,&FBO);
}

void checkFramebufferStatus(GLenum framebufferStatus) {
	switch (framebufferStatus) {
        case GL_FRAMEBUFFER_COMPLETE_EXT: break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
            printf("Attachment Point Unconnected");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            printf("Missing Attachment");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            printf("Dimensions do not match");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            printf("Formats");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            printf("Draw Buffer");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            printf("Read Buffer");
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            printf("Unsupported Framebuffer Configuration");
            break;
        default:
            printf("Unkown Framebuffer Object Failure");
            break;
    }
}


GLuint random_normal_tex;
GLuint random_scalar_tex;
void initNoise() {  
	random_normal_tex = (unsigned int)SOIL_load_OGL_texture("random_normal.png",0,0,0);
	glBindTexture(GL_TEXTURE_2D, random_normal_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	random_scalar_tex = (unsigned int)SOIL_load_OGL_texture("random.png",0,0,0);
	glBindTexture(GL_TEXTURE_2D, random_scalar_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void initFBO(int w, int h) {
    GLenum FBOstatus;
	
	glActiveTexture(GL_TEXTURE0);
	
	glGenTextures(1, &depthTexture);
    glGenTextures(1, &normalTexture);
	glGenTextures(1, &positionTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);

	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glBindTexture(GL_TEXTURE_2D, normalTexture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB32F , w, h, 0, GL_RGBA, GL_FLOAT,0);

	glBindTexture(GL_TEXTURE_2D, positionTexture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB32F , w, h, 0, GL_RGBA, GL_FLOAT,0);
		
	// creatwwe a framebuffer object
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	
	// Instruct openGL that we won't bind a color texture with the currently binded FBO
	glReadBuffer(GL_NONE);

	GLuint curr_prog = pass_prog;
	switch(displayVertex_type)
	{
	case DISPLAY_V_PULSE:
		curr_prog = pulse_prog;
		break;
	case DISPLAY_V_SKINING:
		curr_prog = skin_prog;
		break;
	case DISPLAY_V_MORPH:
		curr_prog = morph_prog;
		break;
	}
    GLint normal_loc = glGetFragDataLocation(curr_prog,"out_Normal");
    GLint position_loc = glGetFragDataLocation(curr_prog,"out_Position");
    GLenum draws [2];
    draws[normal_loc] = GL_COLOR_ATTACHMENT0;
    draws[position_loc] = GL_COLOR_ATTACHMENT1;
	glDrawBuffers(2, draws);
	
	// attach the texture to FBO depth attachment point
    int test = GL_COLOR_ATTACHMENT0;
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
	glBindTexture(GL_TEXTURE_2D, normalTexture);    
	glFramebufferTexture(GL_FRAMEBUFFER, draws[normal_loc], normalTexture, 0);
	glBindTexture(GL_TEXTURE_2D, positionTexture);    
	glFramebufferTexture(GL_FRAMEBUFFER, draws[position_loc], positionTexture, 0);

	// check FBO status
	FBOstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(FBOstatus != GL_FRAMEBUFFER_COMPLETE) {
		printf("GL_FRAMEBUFFER_COMPLETE failed, CANNOT use FBO\n");
        checkFramebufferStatus(FBOstatus);
	}

	
	// switch back to window-system-provided framebuffer
	glClear(GL_DEPTH_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void bindFBO() {
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,0); //Bad mojo to unbind the framebuffer using the texture
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    //glColorMask(false,false,false,false);
    glEnable(GL_DEPTH_TEST);
}

void setTextures() {
    glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,0); 
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//glColorMask(true,true,true,true);
    glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);
}


// Dragon
//Camera cam(vec3(27,0,-3),
//		   normalize(vec3(-1,0,0)),
//		   normalize(vec3(0,0,1)));

Camera cam(vec3(20,4,4),
		   normalize(vec3(-1,0,0)),
		   normalize(vec3(0,0,1)));

void
Camera::adjust(float dx, // look left right
      float dy, //look up down
      float dz,
      float tx, //strafe left right
      float ty,
      float tz)//go forward) //strafe up down
{

	if (abs(dx) > 0) {
		rx += dx;
        rx = fmod(rx,360.0f);
	}

	if (abs(dy) > 0) {
        ry += dy;
        ry = clamp(ry,-70.0f, 70.0f);
	}

	if (abs(tx) > 0) {
		vec3 dir = glm::gtx::rotate_vector::rotate(start_dir,rx + 90,up);
        vec2 dir2(dir.x,dir.y);
        vec2 mag = dir2 * tx;
        pos += mag;	
	}

	if (abs(ty) > 0) {
		z += ty;
	}

	if (abs(tz) > 0) {
		vec3 dir = glm::gtx::rotate_vector::rotate(start_dir,rx,up);
        vec2 dir2(dir.x,dir.y);
        vec2 mag = dir2 * tz;
        pos += mag;
	}
}

mat4x4 Camera::get_view() {
	vec3 inclin = glm::gtx::rotate_vector::rotate(start_dir,ry,start_left);
	vec3 spun = glm::gtx::rotate_vector::rotate(inclin,rx,up);
    vec3 cent(pos, z);
	return lookAt(cent, cent + spun, up);
}

mat4x4 get_mesh_world() {
	vec3 tilt(1.0f,0.0f,0.0f);
	mat4 translate_mat = glm::translate(glm::vec3(0.0f,.8f,0.0f));
	mat4 tilt_mat = glm::rotate(mat4(), 0.0f, tilt);
	mat4 scale_mat = glm::scale(glm::vec3(8,8,8));
	/*mat4 translate_mat = glm::translate(glm::vec3(0.0f,.5f,0.0f));
	mat4 tilt_mat = glm::rotate(mat4(), 90.0f, tilt);
	mat4 scale_mat = glm::scale(glm::vec3(1,1,1));*/
	return scale_mat*tilt_mat * translate_mat;
}


float FARP;
float NEARP;
float timeVal;
void draw_mesh_common(GLuint& currProg, float timeValue) {
    FARP = 100.0f;
	NEARP = 1.0f;

	glUseProgram(currProg);
	

	mat4 model = get_mesh_world();
	mat4 view = cam.get_view();
	mat4 persp = perspective(45.0f,(float)width/(float)height,NEARP,FARP);
	mat4 inverse_transposed = transpose(inverse(view*model));
	
    glUniform1f(glGetUniformLocation(currProg, "u_Far"), FARP);
	glUniformMatrix4fv(glGetUniformLocation(currProg,"u_Model"),1,GL_FALSE,&model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(currProg,"u_View"),1,GL_FALSE,&view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(currProg,"u_Persp"),1,GL_FALSE,&persp[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(currProg,"u_InvTrans") ,1,GL_FALSE,&inverse_transposed[0][0]);
	glUniform1f(glGetUniformLocation(currProg, "u_Time"), timeValue);
	glUniform4fv(glGetUniformLocation(currProg,"u_BoneMatrix"),9, &boneMatrix[0][0]);

	for(int i=0; i<draw_meshes.size(); i++){
		glBindVertexArray(draw_meshes[i].vertex_array);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw_meshes[i].vbo_indices);
		glDrawElements(GL_TRIANGLES, draw_meshes[i].num_indices, GL_UNSIGNED_SHORT,0);
	}
	glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}

void draw_mesh()
{
	draw_mesh_common(pass_prog, 0);
}

void draw_pulsing() {
	if (timeVal > 0.001/* && timeVal < 600*/)
		timeVal += 0.5;
	else 
		timeVal = 0;
	
	draw_mesh_common(pulse_prog, timeVal);
}

void draw_skining() {
	draw_mesh_common(skin_prog, 0);
}

void draw_morph() {
	draw_mesh_common(skin_prog, 0);
}

enum Display display_type = DISPLAY_TOTAL;
enum Occlusion occlusion_type = OCCLUSION_NONE;
void draw_quad() {
    glUseProgram(ssao_prog);

	glBindVertexArray(device_quad.vertex_array);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, device_quad.vbo_indices);

    glEnable(GL_TEXTURE_2D);

	mat4 persp = perspective(45.0f,1.0f,NEARP,FARP);
    vec4 test(-2,0,10,1);
    vec4 testp = persp * test;
    vec4 testh = testp / testp.w;
    vec2 coords = vec2(testh.x, testh.y) / 2.0f + 0.5f;
    glUniform1i(glGetUniformLocation(ssao_prog, "u_ScreenHeight"), height);
    glUniform1i(glGetUniformLocation(ssao_prog, "u_ScreenWidth"), width);
    glUniform1f(glGetUniformLocation(ssao_prog, "u_Far"), FARP);
    glUniform1f(glGetUniformLocation(ssao_prog, "u_Near"), NEARP);
    glUniform1i(glGetUniformLocation(ssao_prog, "u_OcclusionType"), occlusion_type);
    glUniform1i(glGetUniformLocation(ssao_prog, "u_DisplayType"), display_type);
    glUniformMatrix4fv(glGetUniformLocation(ssao_prog, "u_Persp"),1, GL_FALSE, &persp[0][0] );
    
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glUniform1i(glGetUniformLocation(ssao_prog, "u_Depthtex"),0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glUniform1i(glGetUniformLocation(ssao_prog, "u_Normaltex"),1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, positionTexture);
    glUniform1i(glGetUniformLocation(ssao_prog, "u_Positiontex"),2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, random_normal_tex);
    glUniform1i(glGetUniformLocation(ssao_prog, "u_RandomNormaltex"),3);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, random_scalar_tex);
    glUniform1i(glGetUniformLocation(ssao_prog, "u_RandomScalartex"),4);
    
    glDrawElements(GL_TRIANGLES, device_quad.num_indices, GL_UNSIGNED_SHORT,0);

    glBindVertexArray(0);
}

void updateDisplayText(char * disp) {
	switch(display_type) {
			case(DISPLAY_DEPTH):
				sprintf(disp, "Displaying Depth");
				break;
			case(DISPLAY_NORMAL):
				sprintf(disp, "Displaying Normal");
				break;
			case(DISPLAY_POSITION):
				sprintf(disp, "Displaying Position");
				break;
			case(DISPLAY_OCCLUSION):
				sprintf(disp, "Displaying Occlusion");
				break;
			case(DISPLAY_TOTAL):
				sprintf(disp, "Displaying Diffuse+Occlusion");
				break;
	}
}

void updateOcclusionText(char * disp) {
	switch(occlusion_type) {
			case(OCCLUSION_NONE):
				sprintf(disp, "with No Occlusion");
				break;
			case(OCCLUSION_REGULAR_SAMPLES):
				sprintf(disp, "with Regular Grid Occlusion");
				break;
			case(OCCLUSION_POISSON_SS_SAMPLES):
				sprintf(disp, "with Poisson Disk SS Occlusion");
				break;
			case(OCCLUSION_WORLD_SPACE_SAMPLES):
				sprintf(disp, "with World Space Occlusion");
				break;
	}
}



int frame = 0;
int currenttime = 0;
int timebase = 0;
char title[1024];
char disp[1024];
char occl[1024];
void updateTitle() {
    updateDisplayText(disp);
    updateOcclusionText(occl);
//calculate the frames per second
	frame++;

	//get the current time
	currenttime = glutGet(GLUT_ELAPSED_TIME);
	
	//check if a second has passed
	if (currenttime - timebase > 1000) 
	{
		
		sprintf(title, "CIS565 OpenGL Frame | %s %s FPS: %4.2f", disp, occl,frame*1000.0/(currenttime-timebase));
		//sprintf(title, "CIS565 OpenGL Frame | %4.2f FPS", frame*1000.0/(currenttime-timebase));
		glutSetWindowTitle(title);
	 	timebase = currenttime;		
		frame = 0;
	}
}

void display(void)
{
	// clear the screen
    bindFBO();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (displayVertex_type)
	{
	case DISPLAY_V_PULSE:
		draw_pulsing();
		break;
	case DISPLAY_V_SKINING:
		draw_skining();
		break;
	case DISPLAY_V_MORPH:
		draw_morph();
		break;
	default:
		draw_mesh();
	}

    setTextures();
	draw_quad();

    updateTitle();

    glutPostRedisplay();
	glutSwapBuffers();
}



void reshape(int w, int h)
{
    width = w;
    height = h;
    glBindFramebuffer(GL_FRAMEBUFFER,0);
	glViewport(0,0,(GLsizei)w,(GLsizei)h);
	if (FBO != 0 || depthTexture != 0 || normalTexture != 0 ) {
		freeFBO();
	}
    initFBO(w,h);
}


int mouse_buttons = 0;
int mouse_old_x = 0;
int mouse_old_y = 0;
void mouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN) {
        mouse_buttons |= 1<<button;
    } else if (state == GLUT_UP) {
        mouse_buttons = 0;
    }
 
    mouse_old_x = x;
    mouse_old_y = y;
}
 
void motion(int x, int y)
{
    float dx, dy;
    dx = (float)(x - mouse_old_x);
    dy = (float)(y - mouse_old_y);
 
    if (mouse_buttons & 1<<GLUT_RIGHT_BUTTON) {
		cam.adjust(0,0,dx,0,0,0);;
    }
    else {
        cam.adjust(-dx*0.2f,-dy*0.2f,0,0,0,0);
    }
 
    mouse_old_x = x;
    mouse_old_y = y;
}

void keyboard(unsigned char key, int x, int y) {
    float tx = 0;
    float tz = 0;
	float ty = 0;
	switch(key) {
	case('w'):
      tz = 0.1;
	  break;
	case('s'):
      tz = -0.1;
	  break;
	case('d'):
      tx = -0.1;
	  break;
	case('a'):
      tx = 0.1;
	  break;
	case('q'):
      ty = +0.1;
	  break;
	case('z'):
      ty = -0.1;
	  break;
	case('1'):
      occlusion_type = OCCLUSION_NONE;
	  break;
	case('2'):
      occlusion_type = OCCLUSION_REGULAR_SAMPLES;
	  break;
	case('3'):
      occlusion_type = OCCLUSION_POISSON_SS_SAMPLES;
	  break;
	case('4'):
      occlusion_type = OCCLUSION_WORLD_SPACE_SAMPLES;
	  break;
	case('6'):
      display_type = DISPLAY_DEPTH;
	  break;
	case('7'):
      display_type = DISPLAY_NORMAL;
	  break;
	case('8'):
      display_type = DISPLAY_POSITION;
	  break;
	case('9'):
      display_type = DISPLAY_OCCLUSION;
	  break;
	case('0'):
      display_type = DISPLAY_TOTAL;
	  break;
	case('o'):
	case('O'):
		displayVertex_type = DISPLAY_V_NORMAL;
		break;
	case('p'):
	case('P'):
		displayVertex_type = DISPLAY_V_PULSE;
		break;
	case('i'):
	case('I'):
		displayVertex_type = DISPLAY_V_SKINING;
		break;
	case('u'):
	case('U'):
		displayVertex_type = DISPLAY_V_MORPH;
		break;
}

	if (abs(tx) > 0 ||  abs(tz) > 0 || abs(ty) > 0) {
		cam.adjust(0,0,0,tx,ty,tz);
	}
}

void init() {
	glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f,1.0f);
}



int main (int argc, char* argv[])
{
	bool loadedScene = false;
	for(int i=1; i<argc; i++){
		string header; string data;
		istringstream liness(argv[i]);
		getline(liness, header, '='); getline(liness, data, '=');
		if(strcmp(header.c_str(), "mesh")==0){
			//renderScene = new scene(data);
			objmesh = new obj();
			objLoader* loader = new objLoader(data, objmesh);
			glm::vec3 meshPos = glm::vec3(0,0,0);
			glm::vec3 meshRot = glm::vec3(90,90,0);
			glm::vec3 meshScale = glm::vec3(1,1,1);
			objmesh->setTransforms(meshPos, meshRot, meshScale);
			objmesh->buildVBOs();
			delete loader;
			loadedScene = true;
		}
	}

	if(!loadedScene){
		cout << "Usage: mesh=[obj file]" << endl;
		std::cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
		return 0;
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    width = 1280;
    height = 720;
	glutInitWindowSize(width,height);
	glutCreateWindow("CIS565 OpenGL Frame");
	glewInit();
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		cout << "glewInit failed, aborting." << endl;
		exit (1);
	}
	cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << endl;
	cout << "OpenGL version " << glGetString(GL_VERSION) << " supported" << endl;
    
	timeVal = 0.01;

    initNoise();
	initBlur();
    initSSAO();
    initPass();
	initPulse();
	initSkining();
	initMorph();
    initFBO(width,height);
    init();
	initMesh();
    initQuad();


	glutDisplayFunc(display);
	glutReshapeFunc(reshape);	
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glutMainLoop();
	return 0;
}