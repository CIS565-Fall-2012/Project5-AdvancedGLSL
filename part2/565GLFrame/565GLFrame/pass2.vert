#version 330

//uniform mat4 bonesmat[20];
//attribute vec4 bonesidx;
//attribute vec4 bonesweights;

//varying vec3 normal;
//varying vec3 viewvec;


uniform mat4x4 u_Model;
uniform mat4x4 u_View;
uniform mat4x4 u_Persp;
uniform mat4x4 u_InvTrans;

in  vec3 Position;
in  vec3 Normal;

out vec3 fs_Normal;
out vec4 fs_Position;

void main(void) {

	//gl_Position = vec4 (0, 0, 0, 0);
 //   ivec4 ibonesidx = ivec4(bonesidx);
 //   for (int i = 0; i < 4; i++)
 //   {
 //       gl_Position += bonesmat[ibonesidx[i]] * gl_Vertex * bonesweights[i];
 //   }

 //   gl_Position = gl_ModelViewProjectionMatrix * gl_Position;

   // normal = gl_NormalMatrix * gl_Normal;
   // viewvec = gl_NormalMatrix * vec3 (gl_Vertex);
	fs_Normal = (u_InvTrans*vec4(Normal,0.0f)).xyz;
	vec4 world = u_Model * vec4(Position, 1.0);
    vec4 camera = u_View * world;
	fs_Position = camera;
	gl_Position = u_Persp * (camera);
	gl_Position = u_Persp * (camera+vec4(vec3(fs_Normal)/8,0));
}