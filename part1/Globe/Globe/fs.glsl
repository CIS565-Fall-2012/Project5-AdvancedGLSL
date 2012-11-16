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

void main(void)
{
    vec3 normal = normalize(v_Normal);            // surface normal - normalized after rasterization
    vec3 eyeToPosition = normalize(v_Position);   // normalized eye-to-position vector in camera coordinates

	float diffuse = dot(u_CameraSpaceDirLight, normal);
	float diffuse2 = diffuse;

    vec3 toReflectedLight = reflect(-u_CameraSpaceDirLight, normal);
    float specular = dot(toReflectedLight, -eyeToPosition);
    specular = pow(specular, 20.0);

    float gammaCorrect = 1/1.8; //gamma correct by 1/1.8

    vec4 dayColor = texture2D(u_DayDiffuse, v_Texcoord);

	//Calculating night time texture and gamma correction
	vec4 nightTextureColor = texture2D(u_Night, v_Texcoord);
	vec4 nightColor;
	for(int i = 0; i < 4; ++i)
	{
		nightColor[i] = pow(nightTextureColor[i], gammaCorrect);
	}

	//--------------------------------------------------------------------------------------------------------------
	//Cloud color
	//--------------------------------------------------------------------------------------------------------------
	vec4 cloudTransmittance = texture2D(u_CloudTrans, vec2(v_Texcoord.s + u_time / 3.0, v_Texcoord.t));
	vec4 cloudColor = texture2D(u_Cloud, vec2(v_Texcoord.s + u_time / 3.0, v_Texcoord.t));
	if(diffuse2 < 0)
	{
		dayColor = mix(cloudColor, dayColor, cloudTransmittance.r);
	}
	else
	{
		nightColor = mix(vec4(0), nightColor, cloudTransmittance.r);
	}


	//To decide whether this fragment is land or water
	float earthDecider = length(vec3(texture2D(u_EarthSpec, v_Texcoord)));
	
	//--------------------------------------------------------------------------------------------------------------
	//Bump Mapping
	//--------------------------------------------------------------------------------------------------------------
	if(earthDecider < 0.01)   //Works only on earth, not ocean
	{
		float centerBump = (texture2D(u_Bump, v_Texcoord)).r;
		float rightBump = (texture2D(u_Bump, vec2(v_Texcoord.s + 1.0 / 1000.0, v_Texcoord.t))).r;
		float topBump = (texture2D(u_Bump, vec2(v_Texcoord.s, v_Texcoord.t + 1.0/ 500.0))).r;
		//Normal in tangent space
		vec3 normalInTangentSpace = normalize(vec3(centerBump - rightBump, centerBump - topBump, 0.2));
		//Normal in Eye space
		vec3 normalInEyeSpace = eastNorthUpToEyeCoordinates(v_positionMC, normal) * normalInTangentSpace;
		normalInEyeSpace = normalize(normalInEyeSpace);
		diffuse = dot(u_CameraSpaceDirLight, normalInEyeSpace);
		toReflectedLight = reflect(-u_CameraSpaceDirLight, normalInEyeSpace);
		specular = dot(toReflectedLight, -eyeToPosition);
		specular = pow(specular, 20.0);
	}

	//--------------------------------------------------------------------------------------------------------------
	//Calculating Diffuse and Specular Colors
	//--------------------------------------------------------------------------------------------------------------
	vec4 diffuseAndSpecular = vec4(0, 0, 0, 0);
	if(diffuse2 < -0.1)
	{
		if(earthDecider > 0.01)
		{
			diffuseAndSpecular = (-0.6 * diffuse + 0.4 * specular) * dayColor;
		}
		else
		{
			diffuseAndSpecular = -1.0 * diffuse * dayColor;
		}
	}
	else if(diffuse2 < 0)
	{
		vec4 interpolatedColor = mix(dayColor, nightColor, (diffuse2 + 0.1) / 0.2);
		if(earthDecider > 0.01)
		{
			diffuseAndSpecular = (-0.6 * diffuse + 0.4 * specular) * interpolatedColor;
		}
		else
		{
			diffuseAndSpecular = -1.0 * diffuse * interpolatedColor;
		}

	}
	else if(diffuse2 < 0.1)
	{
		vec4 interpolatedColor = mix(dayColor, nightColor, (diffuse2 + 0.1) / 0.2);
		diffuseAndSpecular = 1.2 * diffuse * interpolatedColor;
	}
	else
	{
		diffuseAndSpecular = 1.2 * diffuse * nightColor;
	}

	gl_FragColor = diffuseAndSpecular;

	////--------------------------------------------------------------------------------------------------------------
	////Cloud color
	////--------------------------------------------------------------------------------------------------------------
	//vec4 cloudTransmittance = texture2D(u_CloudTrans, vec2(v_Texcoord.s + u_time / 3.0, v_Texcoord.t));
	//vec4 cloudColor = texture2D(u_Cloud, vec2(v_Texcoord.s + u_time / 3.0, v_Texcoord.t));
	////Day Time
	//if(diffuse2 < 0)
	//{
	//	gl_FragColor = mix(cloudColor, diffuseAndSpecular, cloudTransmittance.r);
	//}
	////Night Time
	//else
	//{
	//	gl_FragColor = mix(vec4(0), diffuseAndSpecular, cloudTransmittance.r);
	//}

	////--------------------------------------------------------------------------------------------------------------
	////Rim Lighting
	////--------------------------------------------------------------------------------------------------------------
	//float rimFactor = dot(v_Normal, v_Position) + 1.0;
	//if(rimFactor > 0)
	//{
	//	gl_FragColor += vec4(rimFactor / 4.0, rimFactor / 2.0, rimFactor / 2.0, 0.0);
	//}
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