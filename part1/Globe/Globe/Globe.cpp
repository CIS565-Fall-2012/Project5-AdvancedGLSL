#include "Globe.h"

#include "Utility.h"

#include <GL/glut.h>
#include "SOIL.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_projection.hpp>
#include <glm/gtc/matrix_operation.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform2.hpp>

#include <cmath>
#include <iostream>
#include <fstream>
#include <string>

const float PI = std::atan(1.0f)*4;

using namespace std;
using namespace glm;

static device_mesh_t current_mesh;
static device_mesh_t device_sphere;

static GLuint current_prog;
static GLuint lambert_prog;
static GLuint blinnphong_prog;
static GLuint fresnel_prog;
static GLuint globe_prog;

static GLuint daydiffuse_tex;
static GLuint night_tex;
static GLuint cloud_tex;
static GLuint cloudtrans_tex;
static GLuint earthspec_tex;
static GLuint disp_tex;
static GLuint noise_tex;

static const int LONGITUDE_DIVISIONS = 75;
static const int LATITUDE_DIVISIONS = 75;
static const int NUM_LONGITUDE_PTS = LONGITUDE_DIVISIONS + 1;
static const int NUM_LATITUDE_PTS = LATITUDE_DIVISIONS + 1;
static const float RADIUS = 1;

//r theta phi
vec3 computeSpherical(vec2 uv, float radius) {
   vec3 out;
   out.x = radius;
   float phi = -2.0f*PI*(uv.s);
   float theta = PI*(uv.t);
   out.y = theta;
   out.z = phi;
   return out;
}

vec3 computePosition(vec3 spherical) {
	float radius = spherical.x;
    float theta = spherical.y;
    float phi = spherical.z;
    float x = radius * sin(theta) * sin(phi);
    float y = radius * sin(theta) * cos(phi);
	float z = radius * cos(theta);
    return vec3(x,y,z);
}

vec3 computeNormal(vec3 spherical) {
	float radius = 1;
	vec3 new_spherical(radius,spherical.y,spherical.z);
	return computePosition(new_spherical);
}

void appendPoint(mesh_t * mesh, vec2 uv, float radius) {
	vec3 spherical = computeSpherical(uv, radius);
	vec3 position = computePosition(spherical);
	vec3 normal = computeNormal(spherical);
	mesh->vertices.push_back(position);
	mesh->uvs.push_back(uv);
	mesh->normals.push_back(normal);
}

//Trig Time
void initSphere() {
	mesh_t sphere;
	//Check assumptions
	assert(LATITUDE_DIVISIONS >= 2);
	assert(LONGITUDE_DIVISIONS >= 3);
    //drop starting row.  Notice num_pts = num_dvisions + 1
	for (int i = 0; i < NUM_LONGITUDE_PTS; i ++) {
		vec2 uv(i / (float)(NUM_LONGITUDE_PTS - 1),0);
        appendPoint(&sphere, uv, RADIUS);
	}
	
	for (int j = 0; j < LATITUDE_DIVISIONS; j++) {
		float v = (j + 1)/((float)NUM_LATITUDE_PTS-1);
        //Set up first point
		vec2 first_uv(0.0f,v);
		appendPoint(&sphere, first_uv,RADIUS);
		//Iterate over divisions
		for (int i = 0; i < LONGITUDE_DIVISIONS; i++) {
			//Setup new point
			vec2 new_uv((i+1)/((float)NUM_LONGITUDE_PTS - 1),v);
			appendPoint(&sphere,new_uv, RADIUS);
            unsigned short length = (unsigned short)sphere.vertices.size();
			unsigned short upper_right = length - 1;
			unsigned short lower_right = upper_right - NUM_LONGITUDE_PTS;
			unsigned short lower_left = lower_right - 1;
			unsigned short upper_left = lower_left + NUM_LONGITUDE_PTS;
			unsigned short added [] = {upper_left, lower_right, upper_right,
				upper_left, lower_left, lower_right};
			for (int i = 0; i < 6; ++i) { sphere.indices.push_back(added[i]); }
		}
	}
	device_sphere = uploadMesh(sphere);
}

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
	glGenBuffers(1,&(out.vbo_uvs));
	glGenBuffers(1,&(out.vbo_indices));
    
	//Upload vertex data
	glBindBuffer(GL_ARRAY_BUFFER, out.vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size()*sizeof(vec3), &mesh.vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(attributes::POSITION, 3, GL_FLOAT, GL_FALSE,0,0);
	glEnableVertexAttribArray(attributes::POSITION);

    //VBO for normal data
	glBindBuffer(GL_ARRAY_BUFFER, out.vbo_normals);
	glBufferData(GL_ARRAY_BUFFER, mesh.normals.size()*sizeof(vec3), &mesh.normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(attributes::NORMAL, 3, GL_FLOAT, GL_FALSE,0,0);
	glEnableVertexAttribArray(attributes::NORMAL);

    //texture data
	glBindBuffer(GL_ARRAY_BUFFER, out.vbo_uvs);
	glBufferData(GL_ARRAY_BUFFER, mesh.uvs.size()*sizeof(vec2), &mesh.uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(attributes::UV, 2, GL_FLOAT, GL_FALSE,0,0);
	glEnableVertexAttribArray(attributes::UV);

    //indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out.vbo_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size()*sizeof(GLushort), &mesh.indices[0], GL_STATIC_DRAW);
    out.num_indices = mesh.indices.size();
	//Unplug Vertex Array
    glBindVertexArray(0);
    return out;
}

GLuint initShader(char * vert_path,
				  char * frag_path,
				  const unsigned int * additional_params, 
				  const char ** additional_names,
				  int num_params) {
  
	Utility::shaders_t shaders = Utility::loadShaders(vert_path,frag_path);

	GLuint p = glCreateProgram();

	glBindAttribLocation(p,attributes::POSITION, "Position");
	glBindAttribLocation(p,attributes::NORMAL, "Normal");
	glBindAttribLocation(p,attributes::UV, "Texcoord");

	for (int i = 0; i < num_params; i++) {
		glBindAttribLocation(p,additional_params[i], additional_names[i]);
	}

	glAttachShader(p,shaders.vertex);
	glAttachShader(p,shaders.fragment);
	
	Utility::attachAndLinkProgram( p, shaders);
	
	return p;
}

const float ROTATION_STEP_STEP = 0.1f;
float rotation_step = 0.0f;
void adjustRotation(float adjustment) {
    rotation_step += adjustment;
}

void speedUpRotation() {
    adjustRotation(ROTATION_STEP_STEP);
}

void slowDownRotation() {
    adjustRotation(-ROTATION_STEP_STEP);
}

static float object_rotation;
static float slow_rotation;
mat4 update_rotation() {
	object_rotation += rotation_step;
    slow_rotation += rotation_step / 8.0f;
	if (object_rotation >= 360.0f) object_rotation = 0.0f;
    vec3 axis(0.0f,0.0f,1.0f);
    mat4 incr = glm::rotate(mat4(), object_rotation, axis);
	vec3 tilt(1.0f,0.0f,0.0f);
    mat4 tilt_mat = glm::rotate(mat4(), 23.5f, tilt);
    mat4 rot = glm::rotate(mat4(), slow_rotation, axis);
    return rot * tilt_mat * incr;
    
}


float rx;
float ry;
float dist;
const float ZOOM_STEP = 0.01f;

void zoom(float dz) {
	dist = clamp(dist - ZOOM_STEP*dz, 1.5f, 10.0f);
}

void rotate(float dx, float dy) {
	if (abs(dx) > 0.0f) {
        rx += dx;
        rx = fmod(rx,360.0f);
	}
	if (abs(dy) > 0.0f) {
        ry += dy;
        ry = clamp(ry, - (4.0f/5.0f)*90.0f, (4.0f/5.0f)*90.0f);
	}
}


void initView() {
    dist = 4.0f;
	rx = -140.0f;
	ry = 0.0f;
}

mat4 get_view() {
    vec3 eye_start(1.0f,0.0f,0.0f);
    vec3 out_eye = dist*eye_start;
	vec3 inclin = glm::gtx::rotate_vector::rotate(out_eye,ry,vec3(0.0f,1.0f,0.0f));
	vec3 around = glm::gtx::rotate_vector::rotate(inclin,rx,vec3(0.0f,0.0f,1.0f));
	vec3 center(0.0f,0.0f,0.0f);
    vec3 up(0.0f,0.0f,1.0f);
    return lookAt(around,center,up);
}

float time = 0.0;

void display(void)
{
	time += 0.0001f;

	// clear the screen
    glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(current_mesh.vertex_array);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_mesh.vbo_indices);

	mat4 model = update_rotation();
	mat4 view = get_view();
	mat4 persp = perspective(45.0f,1.0f,0.1f,100.0f);
	mat4 inverse_transposed = transpose(inverse(view*model));

    vec3 worlddirlight(-1,-1,0);
    vec4 cameraspacedirlight4 = view * vec4(worlddirlight,0.0f);
    vec3 cameraspacedirlight = normalize(vec3(cameraspacedirlight4));
	glUniform3fv(glGetUniformLocation(current_prog,"u_CameraSpaceDirLight"),1, &cameraspacedirlight[0]);
	glUniformMatrix4fv(glGetUniformLocation(current_prog,"u_Model"),1,GL_FALSE,&model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(current_prog,"u_View"),1,GL_FALSE,&view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(current_prog,"u_Persp"),1,GL_FALSE,&persp[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(current_prog,"u_InvTrans") ,1,GL_FALSE,&inverse_transposed[0][0]);
	glUniform1f(glGetUniformLocation(current_prog,"u_time"), time);

	glDrawElements(GL_TRIANGLES, current_mesh.num_indices, GL_UNSIGNED_SHORT,0);

	glBindVertexArray(0);
    glutPostRedisplay();
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0,0,(GLsizei)w,(GLsizei)h);
}

void initGlobeShader() {
	globe_prog = initShader("vs.glsl", "fs.glsl",NULL,NULL,0);
	daydiffuse_tex = (unsigned int)SOIL_load_OGL_texture("earthmap1k.jpg",0,0,0);
	night_tex = (unsigned int)SOIL_load_OGL_texture("earthlights1k.jpg",0,0,0);
	cloud_tex = (unsigned int)SOIL_load_OGL_texture("earthcloudmap.jpg",0,0,0);
    cloudtrans_tex = (unsigned int)SOIL_load_OGL_texture("earthcloudmaptrans.jpg",0,0,0);
	earthspec_tex = (unsigned int) SOIL_load_OGL_texture("earthspec1k.jpg",0,0,0);
	disp_tex = (unsigned int) SOIL_load_OGL_texture("earthbump1k.jpg",0,0,0);
	noise_tex = (unsigned int) SOIL_load_OGL_texture("Noise.jpg",0,0,0);
   	glBindTexture(GL_TEXTURE_2D, cloudtrans_tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
   	glBindTexture(GL_TEXTURE_2D, cloud_tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void setGlobeShader() {
	current_prog = globe_prog;
	glUseProgram(current_prog);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, daydiffuse_tex);
	glUniform1i(glGetUniformLocation(current_prog, "u_DayDiffuse"),0);
    glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, night_tex);
    glUniform1i(glGetUniformLocation(current_prog, "u_Night"),1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, cloud_tex);
    glUniform1i(glGetUniformLocation(current_prog, "u_Cloud"),2);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, cloudtrans_tex);
    glUniform1i(glGetUniformLocation(current_prog, "u_CloudTrans"),3);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, earthspec_tex);
    glUniform1i(glGetUniformLocation(current_prog, "u_EarthSpec"),4);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, disp_tex);
    glUniform1i(glGetUniformLocation(current_prog, "u_Bump"),5);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, noise_tex);
    glUniform1i(glGetUniformLocation(current_prog, "u_Noise"),6);

}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
	   case '+':
           speedUpRotation();
		   break;
	   case '-':
           slowDownRotation();
		   break;	
	}
}

int mouse_buttons = 0;
int mouse_old_x = 0;
int mouse_old_y = 0;
void mouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN) {
        mouse_buttons |= 1<<button;
    } 
	else if (state == GLUT_UP) {
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
		zoom(dy);
    }
    else {
        rotate(-dx*0.2f,-dy*0.2f);
    }
 
    mouse_old_x = x;
    mouse_old_y = y;
}

void setCurrentMesh(device_mesh_t next) {
	current_mesh = next;
}

void init() {
	glEnable(GL_DEPTH_TEST);
}

int main (int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(600,600);
	glutCreateWindow("Multi-Textured Globe");
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
    
    init();
    initView();
	initSphere();
    initGlobeShader();
    setGlobeShader();
	setCurrentMesh(device_sphere);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);	
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
	glutMainLoop();
	return 0;
}
