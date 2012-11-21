uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Persp;
uniform mat4 u_InvTrans;
uniform float u_time;

attribute vec3 Position;
attribute vec3 Normal;
attribute vec2 Texcoord;

varying vec3 v_Normal;
varying vec2 v_Texcoord;
varying vec3 v_Position;
varying vec3 v_positionMC;
varying vec3 v_positionWorld;

void main(void)
{
	vec3 pos = Position;

	if (Position.y > 1.1)
	{
		pos.x -= 1.7*sin(u_time);
		pos.z += 1.7*cos(u_time);
		//Position.z += cos(u_time);
	}
	v_Normal = (u_InvTrans*vec4(Normal,0.0)).xyz;
    v_Texcoord = Texcoord;
	vec4 world = u_Model * vec4(pos, 1.0);
    vec4 camera = u_View * world;
    v_Position = camera.xyz;
	v_positionMC = pos;
	v_positionWorld = world.xyz;
	gl_Position = u_Persp * camera;
}
