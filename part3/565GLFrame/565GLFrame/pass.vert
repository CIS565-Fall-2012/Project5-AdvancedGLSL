#version 330


uniform mat4x4 u_Model;
uniform mat4x4 u_View;
uniform mat4x4 u_Persp;
uniform mat4x4 u_InvTrans;

uniform float u_time;

in  vec3 Position;
in  vec3 Normal;

out vec3 fs_Normal;
out vec4 fs_Position;

void main(void) {

	float scaleFactor = 0.003;
	float frequency = 0.3;
	
	float displacement = scaleFactor * (0.5 * sin(Position.y * frequency * u_time) + 1);
	vec4 displacementDirection = vec4(Normal.x, Normal.y, Normal.z, 0);
	vec4 newPosition = vec4(Position, 1.0) + displacement * displacementDirection;
	
	fs_Position = (u_View * u_Model) * newPosition;
	gl_Position = (u_Persp * u_View * u_Model) * newPosition;

	fs_Normal = (u_InvTrans*vec4(Normal,0.0f)).xyz;
	
	//vec4 world = u_Model * vec4(newPosition.xyz, 1.0);
    //vec4 camera = u_View * world;
	//fs_Position = camera;
	//gl_Position = u_Persp * camera;
}