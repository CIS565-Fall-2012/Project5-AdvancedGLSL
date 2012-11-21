#version 330


uniform float u_time;
uniform mat4x4 u_Model;
uniform mat4x4 u_View;
uniform mat4x4 u_Persp;
uniform mat4x4 u_InvTrans;

in vec3 Position;
in vec3 Normal;

out vec3 fs_Normal;
out vec4 fs_Position;

void main(void)
{

	 float scaleFactor = 5.2;
	 float frequency = 0.005;
	 float displacement = scaleFactor * 0.5 *sin(Position.z * frequency * u_time) + 1;
	 vec3 newPosition = Position + displacement *Normal;

  //vec4 newPosition = vec4(Position.x , Position.y+displacement ,Position.z + displacement, 1.0);

             
	fs_Normal = (u_InvTrans*vec4(Normal,0.0f)).xyz;
	
	vec4 world = u_Model * vec4(newPosition,1.0);
    vec4 camera = u_View * world;
	fs_Position = camera;
	gl_Position = u_Persp * camera;
}