#version 330

uniform float u_time;
uniform mat4x4 u_Model;
uniform mat4x4 u_View;
uniform mat4x4 u_Persp;
uniform mat4x4 u_InvTrans;

in  vec3 Position;
in  vec3 Normal;

out vec3 fs_Normal;
out vec4 fs_Position;

void main(void) 
{
	fs_Normal = (u_InvTrans*vec4(Normal,0.0f)).xyz;

	vec3 pos = Position;

	float blip = 10*abs( sin( u_time ) );

	pos.xz *= 1.0 + 0.1 * exp( -( blip - pos.y )*( blip - pos.y )/2 );

	vec4 world = u_Model * vec4(pos, 1.0);
    vec4 camera = u_View * world;
	fs_Position = camera;
	gl_Position = u_Persp * camera;
}