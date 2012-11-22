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
//Noise Texture
uniform sampler2D u_Noise;

uniform float u_time;
uniform mat4 u_InvTrans;

varying vec3 v_Normal;              // surface normal in camera coordinates
varying vec2 v_Texcoord;
varying vec3 v_Position;            // position in camera coordinates
varying vec3 v_positionMC;          // position in model coordinates

mat3 eastNorthUpToEyeCoordinates(vec3 positionMC, vec3 normalEC);

void main(void)
{
	vec3 normal = normalize(v_Normal);            // surface normal - normalized after rasterization
	vec3 eyeToPosition = normalize(v_Position);   // normalized eye-to-position vector in camera coordinates
	//vec3 v_PosMC = v_positionMC;

	//Add Bump Map
	float center = texture2D(u_Bump, v_Texcoord).x;
	float right  = texture2D(u_Bump, v_Texcoord + vec2(1.0/1000.0,0)).x;
	float top    = texture2D(u_Bump, v_Texcoord + vec2(0,1.0/500.0)).x;

	vec3 normal_TS = normalize(vec3(center - right, center - top, 0.2));
	vec3 newNormal = normalize(eastNorthUpToEyeCoordinates(v_positionMC, normal) * normal_TS);
	
	float diffuse = max(dot(u_CameraSpaceDirLight, newNormal), 0.0);

	vec3 toReflectedLight = reflect(-u_CameraSpaceDirLight, normal);
	float specular = max(dot(toReflectedLight, -eyeToPosition), 0.0);
	specular = pow(specular, 20.0);

	//Basic Day and Night Color
	float gammaCorrect = 1.0 / 1.5;
	vec4 dayColor = texture2D(u_DayDiffuse, v_Texcoord);
	//vec4 nightColor = pow(texture2D(u_Night, v_Texcoord), gammaCorrect);    //apply gamma correction to nighttime texture
	vec4 nightColor = vec4(pow(texture2D(u_Night, v_Texcoord).x, gammaCorrect), pow(texture2D(u_Night, v_Texcoord).y, gammaCorrect), pow(texture2D(u_Night, v_Texcoord).z, gammaCorrect), pow(texture2D(u_Night, v_Texcoord).w, gammaCorrect));
	
	//Add Specular only for Oceans
	vec4 dayColorFrag = vec4(1.0);
	if(length(vec3(texture2D(u_EarthSpec, v_Texcoord).rgb)) >= 0.125)
	{
		dayColorFrag = ((0.6 * diffuse) + (0.4 * specular)) * dayColor + texture2D(u_Noise, v_Texcoord + vec2(0.25 * u_time, 0.0)) / 4.0;
	}
	else
	{
		dayColorFrag = diffuse * dayColor;
	}
	
	//Add Clouds
	vec2 cloudTexcoord = v_Texcoord + vec2(u_time, 0.0); 
	vec4 cloudColor = texture2D(u_Cloud, cloudTexcoord);
	float transparency = texture2D(u_CloudTrans, cloudTexcoord).r;

	dayColorFrag = mix(cloudColor, dayColorFrag, transparency);
	vec4 nightColorFrag = mix(vec4(vec3(0.0), 1.0), nightColor, transparency); 

	gl_FragColor = mix(nightColorFrag, dayColorFrag, diffuse);

	//Rim Lighting
	float rimDot = 1.0 + dot(normal, v_Position);
	if(rimDot > 0)
	{
		gl_FragColor += vec4(rimDot/16.0, rimDot/8.0, rimDot/4.0, rimDot/2.0);
	}
	
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