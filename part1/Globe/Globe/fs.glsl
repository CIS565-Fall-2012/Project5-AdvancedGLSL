//View-Space directional light
//A unit vector
uniform vec3 u_CameraSpaceDirLight;
uniform vec3 u_WorldSpaceDirLight;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Persp;

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
//Moon map
uniform sampler2D u_Moon;

uniform float u_time;
uniform mat4 u_InvTrans;

varying vec3 v_Normal;              // surface normal in camera coordinates
varying vec2 v_Texcoord;
varying vec3 v_Position;            // position in camera coordinates
varying vec3 v_positionMC;          // position in model coordinates
varying vec3 v_positionWorld;          // position in model coordinates

mat3 eastNorthUpToEyeCoordinates(vec3 positionMC, vec3 normalEC);

void main(void)
{
    vec3 normal = normalize(v_Normal);            // surface normal - normalized after rasterization
    vec3 eyeToPosition = normalize(v_Position);   // normalized eye-to-position vector in camera coordinates

	float dotProd = dot(u_CameraSpaceDirLight, normal);
	
	vec4 center = texture2D(u_Bump, v_Texcoord);
	vec4 right = texture2D(u_Bump, vec2(v_Texcoord.x+1.0/1000.0, v_Texcoord.y));
	vec4 top = texture2D(u_Bump, vec2(v_Texcoord.x, v_Texcoord.y+1.0/500.0));
	vec3 newNormal = normalize(vec3(center.x - right.x, center.y - top.y, 0.2));
	mat3 matrix = eastNorthUpToEyeCoordinates(v_positionMC, normal);
	newNormal = matrix*newNormal;
	newNormal = normalize(newNormal);

	float newdotProd = dot(u_CameraSpaceDirLight, newNormal);
	float diffuse = max(newdotProd, 0.0);

    vec3 toReflectedLight = reflect(-u_CameraSpaceDirLight, newNormal);
    float specular = max(dot(toReflectedLight, -eyeToPosition), 0.0);
    specular = pow(specular, 20.0);

    float gammaCorrect = 1.0/1.8; //gamma correct by 1/1.8

    vec4 dayColor = texture2D(u_DayDiffuse, v_Texcoord);
	vec4 ocean = texture2D(u_EarthSpec, v_Texcoord);
    
	vec4 cloudColor = texture2D(u_Cloud, vec2(v_Texcoord.s+0.02*u_time, v_Texcoord.t));
	vec4 cloudTransp = texture2D(u_CloudTrans, vec2(v_Texcoord.s+0.02*u_time, v_Texcoord.t));

    vec4 finalDayColor = mix(cloudColor, ((0.6 * diffuse) + (0.4 * specular)*ocean) * dayColor, cloudTransp);
	vec4 nightColor = mix(0.0, pow(texture2D(u_Night, v_Texcoord),gammaCorrect), cloudTransp);   //apply gamma correction to nighttime texture

	// Rim lighting
	float rimFactor = dot(v_Normal, v_Position)+0.6;
	float rim = 0.6;
	vec4 newColor = mix(nightColor, finalDayColor, clamp(2.0*(dotProd+0.25),0.0,1.0));

	vec4 shadowColor = vec4(1);
	bool inShadow = false;
	//if (v_positionMC.y < 1.1)
	//{
	//	// Shadow
	//	// Imagine that the clouds are at a height of say 0.2 above the ground
	//	
	//	float cloudDistFromGround = 0.01;
	//	vec3 cloudHitPoint = v_Position + u_WorldSpaceDirLight * cloudDistFromGround; 

	//	// Transform this point to screen
	//	vec4 cloudScreen = u_Persp * (u_View* (u_Model * vec4(cloudHitPoint, 1.0)));
	//	vec2 cloudScreenPoint = cloudScreen.xy/cloudScreen.w;
	//	cloudScreenPoint = cloudScreenPoint*0.5 + 0.5;

	//	vec4 cloudShadow = texture2D(u_Cloud, cloudScreenPoint);
	//	vec4 cloudShadowTransp = texture2D(u_CloudTrans, cloudScreenPoint);

	//	if (cloudShadowTransp.x <0.8 || cloudShadowTransp.y <0.8)
	//	{
	//		inShadow = true;
	//		shadowColor = vec4(1.0-cloudShadowTransp);
	//	}
	//}

	
	// Mixing color and rim factor to get the final color of the fragment
	if (v_positionMC.y > 1.1)
		gl_FragColor = texture2D(u_Moon, v_Texcoord);
	else
	{
		vec4 finalColor = mix(newColor, vec4(rim/4.0, rim/2.0, rim/2.0, 1.0), clamp(2.0*(rimFactor+0.25),0.0,1.0));
		gl_FragColor = mix(vec4(0.0,0.0,0.0,finalColor.w), finalColor, shadowColor);
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