#ifndef UTILITY_H_
#define UTILITY_H_

#include <GL/glew.h>

namespace Utility {

	typedef struct {
		GLuint vertex;
		GLuint fragment;
	} shaders_t;



shaders_t loadShaders(char * vert_path, char * frag_path);

void attachAndLinkProgram( GLuint program, shaders_t shaders);

char* loadFile(char *fname, GLint &fSize);

// printShaderInfoLog
// From OpenGL Shading Language 3rd Edition, p215-216
// Display (hopefully) useful error messages if shader fails to compile
void printShaderInfoLog(GLint shader);

void printLinkInfoLog(GLint prog) ;
}
 
#endif