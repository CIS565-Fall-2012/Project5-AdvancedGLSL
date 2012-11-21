#version 330
uniform float u_time;
uniform mat4x4 u_Model;
uniform mat4x4 u_View;
uniform mat4x4 u_Persp;
uniform mat4x4 u_InvTrans;

in  vec3 Normal;


in vec3 Position;
in vec2 Texcoord;

out vec2 fs_Texcoord;

void main() {
	fs_Texcoord = Texcoord;
	float disp = (sin(u_time * Position.y / 10.0) + 1.0) / 0.20;
	gl_Position = vec4(Position, 1.0f);
}