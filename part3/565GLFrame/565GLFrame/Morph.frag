#version 330


uniform sampler2D u_Normaltex;
uniform sampler2D u_Positiontex;
in vec2 fs_Texcoord;
out vec4 out_Color;

uniform int u_ScreenWidth;
uniform int u_ScreenHeight;


//Helper function to automatically sample and unpack normals
vec3 sampleNrm(vec2 texcoords) {
	return texture(u_Normaltex,texcoords).xyz;
}

//Helper function to automicatlly sample and unpack normals
vec3 samplePos(vec2 texcoords) {
	return texture(u_Positiontex,texcoords).xyz;
}
///////////////////////////////////
// MAIN, Shouldn't really need to mess with this much
//////////////////////////////////

void main() {


	float occlusion = 0.0f;

	vec3 normal = sampleNrm(fs_Texcoord);
	vec3 position = samplePos(fs_Texcoord);


	vec3 to_eye = -normalize(position);
	float diffuse = dot(to_eye,normal);
	float ambient = 0.5;
	float val = diffuse*0.5 + ambient*(1.0f - occlusion); 
	out_Color = vec4(vec3(val),1.0f);

	
                                    vec2 halfres = vec2(u_ScreenWidth,u_ScreenHeight )/2.0;
                                    vec2 cPos = gl_FragCoord.xy;
                                
                                    cPos.x -= 0.5*halfres.x*sin(_Time.y/2.0)+0.3*halfres.x*cos(_Time.y)+halfres.x;
                                    cPos.y -= 0.4*halfres.y*sin(_Time.y/5.0)+0.3*halfres.y*cos(_Time.y)+halfres.y;
                                    float cLength = length(cPos);
                                
                                    vec2 uv = gl_FragCoord.xy/_ScreenParams.xy+(cPos/cLength)*sin(cLength/30.0-_Time.y*10.0)/25.0;
                                    vec3 col = texture2D(_MainTex,uv).xyz*50.0/cLength;
                                
                                    gl_FragColor = vec4(col,1.0);
		

	return;
}

