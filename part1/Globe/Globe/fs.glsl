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
vec2 u_bumpStep = vec2(1.0/1000, 1.0/500);

uniform float u_time;
uniform mat4 u_InvTrans;
uniform int u_renderType;
// make sure that these numbers are matched to enum in Globe.cpp
const int TEXTURE = 0; 
const int ALTITUDE = 1;

varying vec3 v_Normal;              // surface normal in camera coordinates
varying vec2 v_Texcoord;
varying vec3 v_Position;            // position in camera coordinates
varying vec3 v_positionMC;          // position in model coordinates

mat3 eastNorthUpToEyeCoordinates(vec3 positionMC, vec3 normalEC);

void main(void)
{
	float top_bump = texture2D(u_Bump, vec2(v_Texcoord.s, v_Texcoord.t + u_bumpStep.t)).r;
	float right_bump = texture2D(u_Bump, vec2(v_Texcoord.s + u_bumpStep.s, v_Texcoord.t)).r;
	float center_bump = texture2D(u_Bump, v_Texcoord).r;
	vec3 bumpNormalMC = normalize(vec3(center_bump - right_bump, center_bump - top_bump, 0.2));	

	if (u_renderType == ALTITUDE) {
		const vec3 orange = vec3(1.0, 0.565, 0.0);
		const vec3 red = vec3(1.0, 0.0, 0.0);
		const vec3 green = vec3(0.0, 1.0, 0.0);
		const vec3 blue = vec3(0.0, 0.0, 1.0);
		if (center_bump < 0.0001) {
			gl_FragColor = vec4(blue, 1.0);
		} else if (center_bump > 0.99) {
			gl_FragColor = vec4(red, 1.0);
		} else if (center_bump >= 0.5) {
			gl_FragColor = mix(vec4(orange, 1.0), vec4(red, 1.0), (1.0/-log(2*(center_bump-0.5))));
		} else {
			gl_FragColor = mix(vec4(green, 1.0), vec4(orange, 1.0), 1.0/-log(2*center_bump));
		}
		return;
	} 

    vec3 normal = normalize(eastNorthUpToEyeCoordinates(v_positionMC, v_Normal) * bumpNormalMC);
    vec3 eyeToPosition = normalize(v_Position);   // normalized eye-to-position vector in camera coordinates

    float diffuse = max(dot(u_CameraSpaceDirLight, normal), 0.0);

    vec3 toReflectedLight = reflect(-u_CameraSpaceDirLight, normal);
    float specular = max(dot(toReflectedLight, -eyeToPosition), 0.0);
    specular = pow(specular, 20.0);
	   
    vec4 dayColor = texture2D(u_DayDiffuse, v_Texcoord);

    //apply gamma correction to nighttime texture
	float gammaCorrect = 1/1.8;
	vec4 nightColor = texture2D(u_Night, v_Texcoord);
	nightColor.r = pow(nightColor.r, gammaCorrect);
	nightColor.g = pow(nightColor.g, gammaCorrect);
    nightColor.b = pow(nightColor.b, gammaCorrect);

    float earthSpec = texture2D(u_EarthSpec, v_Texcoord).r;

	vec4 dayColor_cloud_blends = mix(texture2D(u_Cloud, v_Texcoord), 
									((0.6 * diffuse) + (0.4 * specular * earthSpec)) * dayColor, 
									texture2D(u_CloudTrans, v_Texcoord));
	vec4 nightColor_cloud_blends = mix(vec4(0.0, 0.0, 0.0, 1.0),
										nightColor,
										texture2D(u_CloudTrans, v_Texcoord));

	float light_normal_dot = max(dot(u_CameraSpaceDirLight, normalize(v_Normal)), 0.0);
    if (light_normal_dot > 0.1) {
        gl_FragColor = dayColor_cloud_blends;
    } else if (light_normal_dot > 0.0) {
        gl_FragColor = mix(nightColor_cloud_blends, dayColor_cloud_blends, 10.0*light_normal_dot);
    } else {
        gl_FragColor = nightColor_cloud_blends;
    }

	float rimFactor = dot(v_Normal, v_Position) + 1.0;
	if (rimFactor > 0.0) {
		gl_FragColor = clamp(gl_FragColor + vec4(rimFactor/4, rimFactor/2, rimFactor/2, 1), 0.0, 1.0);
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