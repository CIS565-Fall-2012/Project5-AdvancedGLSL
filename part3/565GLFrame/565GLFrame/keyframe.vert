#version 330


uniform mat4x4 u_Model;
uniform mat4x4 u_View;
uniform mat4x4 u_Persp;
uniform mat4x4 u_InvTrans;

in  vec3 Position;
in  vec3 Normal;

uniform float u_time;
out vec3 fs_Normal;
out vec4 fs_Position;

#define CONSIZE 8 //2*(size-1)
//const vec3 Key[4]={ vec3(0.0);
//const vec3 Key2= vec3(1.0,0.5,0.0);
//const vec3 Key3= vec3(2.0,-0.5,0.0);
//const vec3 Key4= vec3(3.0,0.0,0.0);
#define SIZE 5
const vec3 Key[SIZE]=vec3[](vec3(-1.0,0.0,0.0),vec3(0.0,1.0,0.0),vec3(2.0,0.0,0.0),vec3(0.0,-1.0,0.0),vec3(-1.0,0.0,0.0));

void main(void) {
    int count=0;
    vec3 controlPoints[CONSIZE];
	//for(int i=0;i<CONSIZE;i++)
	//controlPoints[i]=vec3(0.0);
    for(int i=0;i<SIZE-1;i++)
		{
			if(i==0){
			vec3 pm1=Key[0]+(Key[0]-Key[1]);
			controlPoints[count++]=Key[i]+(Key[i+1]-pm1)/6.0f;
			}else{
			controlPoints[count++]=Key[i]+(Key[i+1]-Key[i-1])/6.0f;
			}
			
		    if(i==(SIZE-2)){
				vec3 pa1=Key[SIZE-1]+(Key[SIZE-2]-Key[SIZE-1]);
				controlPoints[count++]=Key[i+1]-(pa1-Key[i])/6.0f;
			}
			else
			    controlPoints[count++]=Key[i+1]-(Key[i+2]-Key[i])/6.0f;
			
		}
	float time=0;
	time= mod(u_time,float(SIZE-1));
	int i=int(time/1.0);
	float u=time-float(i);
	vec3 newPosition=Position + 5.0*(Key[i]*(1-u)*(1-u)*(1-u)
			+controlPoints[2*i]*3*u*(1-u)*(1-u)
			+controlPoints[2*i+1]*3*u*u*(1-u)
			+Key[i+1]*u*u*u);

	//vec3 newPosition = Position * mod(u_time,1.0) + (Position + vec3(1.0,0.0,0.0)) *(1.0 - mod(u_time,1.0));

	fs_Normal = (u_InvTrans*vec4(Normal,0.0f)).xyz;
	vec4 world = u_Model * vec4(newPosition, 1.0);
    vec4 camera = u_View * world;
	fs_Position = camera;
	gl_Position = u_Persp * camera;
}