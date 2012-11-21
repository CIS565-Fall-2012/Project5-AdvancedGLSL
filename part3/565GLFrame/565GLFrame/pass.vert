#version 330


uniform mat4x4 u_Model;
uniform mat4x4 u_View;
uniform mat4x4 u_Persp;
uniform mat4x4 u_InvTrans;

in  vec3 Position;
in  vec3 Normal;

out vec3 fs_Normal;
out vec4 fs_Position;

const float PI = 3.14159;
const float TWO_PI = PI * 2.0;

const float Radius = 1.0;
const float Blend = 0.5;
uniform float u_time;

vec3 sphere(vec2 domain)
{
    vec3 range;
    range.x = Radius * cos(domain.y) * sin(domain.x);
    range.y = Radius * sin(domain.y) * sin(domain.x);
    range.z = Radius * cos(domain.x);
    return range;
}

void main(void) {

   /* vec2 p0 = Position.xy * TWO_PI;
    vec3 normal = sphere(p0);;
    vec3 r0 = Radius * normal;
    vec3 vertex = r0;

    normal = normalize(mix(Normal, normal, Blend));
    vertex = mix(Position.xyz, vertex, Blend);*/

	vec3 vertex = Position;
	vertex.xyz *= 5./length(vertex.xyz);
	

	/*float SIDE = 2.;
	vec3 vertex = Position;
	vertex.xyz *= 3./length(vertex.xyz);
	vertex.xyz = clamp( vertex.xyz, -SIDE, SIDE );*/
	vertex = mix(Position.xyz, vertex, Blend);


 /* float delta = 0.2 * u_time;
  float scale = 0.2;
    vec3 p = Position;

    p.z += sin(2.0 * p.y + delta) * 5.0;

    p.z += cos(2.0 * p.z + delta / 2.0) * 5.0;

    p.z += cos(2.0 * p.x + delta) * 5.0;

    p.x += sin(p.y + delta / 2.0) * 10.0;*/

   // vec4 mvPosition = modelViewMatrix * vec4(scale * p, 1.0 );
	
	


	fs_Normal = (u_InvTrans*vec4(Normal,0.0f)).xyz;
	vec4 world = u_Model * vec4(Position, 1.0);
    vec4 camera = u_View * world;
	fs_Position = camera;
	gl_Position = u_Persp * camera;
}