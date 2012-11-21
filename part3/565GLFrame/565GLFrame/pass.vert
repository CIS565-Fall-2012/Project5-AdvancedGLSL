#version 330


uniform mat4x4 u_Model;
uniform mat4x4 u_Model2;
uniform mat4x4 u_View;
uniform mat4x4 u_Persp;
uniform mat4x4 u_InvTrans;
uniform float u_time;
uniform int u_effect;

in  vec3 Position;
in  vec3 Normal;
in int boneNumber;

out vec3 fs_Normal;
out vec4 fs_Position;

void main(void) {
	fs_Normal = (u_InvTrans*vec4(Normal,0.0f)).xyz;
	vec4 world = u_Model * vec4(Position, 1.0);
    vec4 camera = u_View * world;
	fs_Position = camera;
	gl_Position = u_Persp * camera;
	if(u_effect == 5)
	{
		gl_Position = u_Persp * camera + 0.05 * sin(0.1 * u_time * Position.y) * vec4(fs_Normal, 0);
	}
	else if(u_effect == 6)
	{
		gl_Position = u_Persp * camera;
		if(boneNumber == 0)
		{
			//Need to do nothing
		}
		else// if(boneNumber == 1)
		{
			world = u_Model2 * vec4(Position, 1.0);
			camera = u_View * world;
			//fs_Position = vec4(1);
			fs_Position = camera;
			gl_Position = u_Persp * camera;
		}
	}
}