#version 330

uniform mat4x4 u_BigModel;
uniform mat4x4 u_SmallModel;
uniform mat4x4 u_View;
uniform mat4x4 u_Persp;
uniform mat4x4 u_InvTrans;
uniform float u_time;

in  vec3 Position;
in  vec3 Normal;

out vec3 fs_Normal;
out vec4 fs_Position;

void main(void) {
	fs_Normal = (u_InvTrans*vec4(Normal,0.0f)).xyz;

	vec4 bigWorld = u_BigModel * vec4(Position, 1.0);
    vec4 bigCamera = u_View * bigWorld;

	vec4 smallWorld = u_SmallModel * vec4(Position, 1.0);
    vec4 smallCamera = u_View * smallWorld;

	float interp = (sin(u_time*0.05)+1)*0.5;

	fs_Position = interp*bigCamera + (1-interp)*smallCamera;
	gl_Position = u_Persp * fs_Position;
}