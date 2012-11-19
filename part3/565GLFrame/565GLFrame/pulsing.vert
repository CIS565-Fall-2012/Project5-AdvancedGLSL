#version 330

////////////////////////////
//       ENUMERATIONS
////////////////////////////
#define VERTEX_SHADER_PASSTHROUGH	0
#define VERTEX_SHADER_PULSING		1



uniform mat4x4 u_Model;
uniform mat4x4 u_View;
uniform mat4x4 u_Persp;
uniform mat4x4 u_InvTrans;
uniform int u_VertexShaderType;
uniform float u_time;

in  vec3 Position;
in  vec3 Normal;

out vec3 fs_Normal;
out vec4 fs_Position;

void main(void) {
	fs_Normal = (u_InvTrans*vec4(Normal,0.0f)).xyz;
		
	vec3 newPosition = Position.xyz;
	switch (u_VertexShaderType)
	{
	case VERTEX_SHADER_PASSTHROUGH:
		newPosition = Position;
		break;
	case VERTEX_SHADER_PULSING:
		float displacement = 0.005*(sin(Position.y * u_time));
		newPosition = Position + displacement*normalize(Normal);
	}

	vec4 world = u_Model * vec4(newPosition, 1.0);
    vec4 camera = u_View * world;
	fs_Position = camera;
	gl_Position = u_Persp * camera;
}