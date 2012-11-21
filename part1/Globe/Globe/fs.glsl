#version 120
//View-Space directional light
//A unit vector
uniform vec3 u_CameraSpaceDirLight;

//Diffuse texture map for the day
uniform sampler2D u_DayDiffuse;
//Ambient texture map for the night side
uniform sampler2D u_Night;
//Color map for the clouds
uniform sampler2D u_Cloud;
//Transparency map for the clouds.  Note that light areas are where clouds are NOT
//Dark areas are were clouds are present
uniform sampler2D u_CloudTrans;
//Mask of which areas of the earth have specularity
//Oceans are specular, landmasses are not
uniform sampler2D u_EarthSpec;
//Bump map
uniform sampler2D u_Bump;

uniform float u_time;
uniform mat4 u_InvTrans;

varying vec3 v_Normal;              // surface normal in camera coordinates
varying vec2 v_Texcoord;
varying vec3 v_Position;            // position in camera coordinates
varying vec3 v_positionMC;          // position in model coordinates

mat3 eastNorthUpToEyeCoordinates(vec3 positionMC, vec3 normalEC);

const float HeightUnit = 1.0/500.0;
const float WidthUnit = 1.0/1000.0;

void main(void)
{
    vec3 normal = normalize(v_Normal);            // surface normal - normalized after rasterization
    vec3 eyeToPosition = normalize(v_Position);   // normalized eye-to-position vector in camera coordinates

	//Diffuse
	float diffuse = clamp(dot(u_CameraSpaceDirLight, normal), 0.0,1.0);

	//Specular
    vec3 toReflectedLight = reflect(-u_CameraSpaceDirLight, normal);
    float specular = max(dot(toReflectedLight, -eyeToPosition), 0.0);
    specular = pow(specular, 20.0);
	
	//SpecularMask
	vec4 earthSpecMask = texture2D(u_EarthSpec, v_Texcoord);
	if( length(earthSpecMask.rgb) < 0.01){
		specular = 0.0f;

   }
	
    //Cloud and CloudTrans
    float gammaCorrect = 1/2.2;	//gamma correct by 1/1.8

	vec4 cloud =  texture2D(u_Cloud, v_Texcoord);
	vec4 cloudTrans = texture2D(u_CloudTrans, v_Texcoord);

    vec4 daySampleColor = texture2D(u_DayDiffuse, v_Texcoord);
    vec4 nightColor = pow(texture2D(u_Night, v_Texcoord),vec4(vec3(gammaCorrect),1.0));    //apply gamma correction to nighttime texture
	
	vec4 dayColor=0.6 * diffuse * daySampleColor; //diffuse

	if( length(cloudTrans.rgb -vec3(1.0)) >0.01){//with cloud
		dayColor  = mix(cloud, dayColor, cloudTrans);
		nightColor = mix(vec4(vec3(0.0),1.0),nightColor, cloudTrans);
	}

	dayColor += (0.4 * specular)*daySampleColor; // specular

	vec4 finalColor=vec4(0.0);//Day and Night Mix
	if(abs(diffuse)<0.1){
		if(abs(diffuse)>0.01)
			finalColor =mix(nightColor,dayColor,(diffuse-0.01)*10);
		else finalColor =  nightColor;
		 
	}else{
		
		//BumpMap 
		vec2 BumpCoord = vec2(0.0); 
		int y=int(v_Texcoord.t*500.0);
		BumpCoord=v_Texcoord;
		float center=  texture2D(u_Bump, BumpCoord).r;
		vec2 rightCoord= vec2(mod((BumpCoord.s+WidthUnit),1.0),BumpCoord.t); 
		vec2 topCoord =vec2(BumpCoord.s,mod((BumpCoord.t+HeightUnit),1.0));
		if(y==499)
			topCoord =BumpCoord;
		float right= clamp( texture2D(u_Bump, rightCoord).r,0.0,1.0);
		float top=  clamp(texture2D(u_Bump, topCoord).r,0.0,1.0);

		vec3 perturbedNormal= normalize(vec3(center-right,center-top,0.1));
		mat3 transMat=eastNorthUpToEyeCoordinates(v_positionMC,normal);
		perturbedNormal = normalize( transMat * perturbedNormal);
		diffuse = clamp(dot(u_CameraSpaceDirLight, perturbedNormal), 0.0,1.0);

		dayColor=0.6 * diffuse * daySampleColor;
		if( length(cloudTrans.rgb -vec3(1.0)) >0.01){//with cloud
			dayColor  = mix(cloud, dayColor, cloudTrans);
			}
		dayColor += (0.4 * specular)*daySampleColor;
		finalColor = dayColor;
    }

	 //rim
	 float rimfacter= dot(v_Normal,v_Position)+1.0;

	 if(rimfacter>0.001)
		finalColor+= vec4( rimfacter/4.0, rimfacter/2.0, rimfacter/2.0, 1.0);

	 gl_FragColor = finalColor;
}

mat3 eastNorthUpToEyeCoordinates(vec3 positionMC, vec3 normalEC)
{
    vec3 tangentMC = normalize(vec3(-positionMC.y, positionMC.x, 0.0));  // normalized surface tangent in model coordinates
    vec3 tangentEC = normalize(mat3(u_InvTrans) * tangentMC);            // normalized surface tangent in eye coordiantes
    vec3 bitangentEC = normalize(cross(normalEC, tangentEC));            // normalized surface bitangent in eye coordinates

    return mat3(
        tangentEC.x,   tangentEC.y,   tangentEC.z,
        bitangentEC.x, bitangentEC.y, bitangentEC.z,
        normalEC.x,    normalEC.y,    normalEC.z);
}