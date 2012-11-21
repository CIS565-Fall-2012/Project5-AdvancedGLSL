#version 330


uniform mat4x4 u_Model;
uniform mat4x4 u_View;
uniform mat4x4 u_Persp;
uniform mat4x4 u_InvTrans;
uniform float u_Time;

in  vec3 Position;
in  vec3 Normal;

out vec3 fs_Normal;
out vec4 fs_Position;

void main(void) {
	float frequency = 0.5;
	float scaleFactor = 0.01;
	float displacement = 0;
	if (u_Time > 0)
		displacement = scaleFactor * (0.5*sin(Position.y * frequency * u_Time) + 0.5);

	vec3 tempPos = Position + Normal*displacement;
	vec3 tempNormal = (u_InvTrans*vec4(Normal,0.0f)).xyz;
	tempNormal = normalize(tempNormal);
	fs_Normal = tempNormal;
	vec4 world = u_Model * vec4(tempPos, 1.0);
    vec4 camera = u_View * world;
	fs_Position = camera;
	vec4 pos = u_Persp * camera;
	pos = pos/pos.w;
	gl_Position = pos;
}