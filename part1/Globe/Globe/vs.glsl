uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Persp;
uniform mat4 u_InvTrans;

attribute vec3 Position;
attribute vec3 Normal;
attribute vec2 Texcoord;

varying vec3 v_Normal;
varying vec2 v_Texcoord;
varying vec3 v_Position;
varying vec3 v_positionMC;
varying vec3 v_viewMC;

void main(void)
{
	v_Normal = (u_InvTrans*vec4(Normal,0.0)).xyz;
    v_Texcoord = Texcoord;
	vec4 world = u_Model * vec4(Position, 1.0);
    vec4 camera = u_View * world;
    v_Position = camera.xyz;
	v_positionMC = Position;
	v_viewMC = (v_positionMC - (inverse( u_Model ) * inverse( u_View ) * vec4( 0, 0, 0, 1.0 ))).xyz;
	gl_Position = u_Persp * camera;
}
