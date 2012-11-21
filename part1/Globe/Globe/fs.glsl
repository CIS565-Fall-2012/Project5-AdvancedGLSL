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
float rand(vec2 tex);

void main(void)
{
    vec3 normal = normalize(v_Normal);            // surface normal - normalized after rasterization
    vec3 eyeToPosition = normalize(v_Position);   // normalized eye-to-position vector in camera coordinates

    float diffuse = max(dot(u_CameraSpaceDirLight, normal), 0.0);

	
    vec3 toReflectedLight = reflect(-u_CameraSpaceDirLight, normal);
    float specular = max(dot(toReflectedLight, -eyeToPosition), 0.0);
    specular = pow(specular, 50.0);
	vec4 spec= texture2D(u_EarthSpec, v_Texcoord);
	specular= spec*specular;

	float w_center= texture2D(u_EarthSpec, v_Texcoord);
	float w_top=texture2D(u_EarthSpec, v_Texcoord+vec2(0.0, 1.0/500.0));
	float w_right=texture2D(u_EarthSpec, v_Texcoord+vec2(1.0/1000.0,0));

	vec3 water_normal= normalize(eastNorthUpToEyeCoordinates(v_positionMC, normal) *normalize(vec3(w_center-w_right, w_center-w_top ,0.1)));

	diffuse = max(dot(u_CameraSpaceDirLight+5*rand(v_Texcoord.x+u_time), water_normal), 0.0);

	specular= mix(specular,spec* diffuse, 0.1);

    float gammaCorrect = 1/1.2; //gamma correct by 1/1.8

    vec4 dayColor = texture2D(u_DayDiffuse, v_Texcoord) + specular/2 ;
	
    vec4 nightColor = pow(texture2D(u_Night, v_Texcoord),gammaCorrect);    //apply gamma correction to nighttime texture
	vec4 cloud_color= pow(texture2D(u_Cloud,v_Texcoord+ vec2(u_time,0.0)),gammaCorrect) ;
	//cloud_color= cloud_color+vec2( u_time, 0.0 );
	vec4 cloudTrans= texture2D(u_CloudTrans,v_Texcoord+ vec2(u_time,0.0));
	float b_center= texture2D(u_Bump, v_Texcoord);
	float b_top=texture2D(u_Bump, v_Texcoord+vec2(0.0, 1.0/500.0));
	float b_right=texture2D(u_Bump, v_Texcoord+vec2(1.0/1000.0,0));

	float dot_product= dot(v_Normal, v_Position);  // for rim lighting
	float rim_factor=dot_product+1;


	vec4 rimcolor;
	if (rim_factor>=0)
	rimcolor=vec4(rim_factor/5,rim_factor/2,rim_factor/2,1.0);

	vec4 mix_night_day;

	float center= b_center;
	float right= b_right;
	float top= b_top;
	//vec3 right= vTexcoord+1;

	vec3 bump_normal= normalize(eastNorthUpToEyeCoordinates(v_positionMC, normal) *normalize(vec3(center-right, center-top ,0.1)));

	diffuse = max(dot(u_CameraSpaceDirLight, bump_normal), 0.0);

	dayColor= mix(cloud_color, dayColor*diffuse, cloudTrans);

	//printf("diffuse %f \n", diffuse);


	// for bump mapping
	//if( bump.a>0)
	//{
	
	

	//if (diffuse ==0) // calculating the night color
	//{
		//diffuse = nightColor;
		 mix_night_day=mix(nightColor*2,dayColor ,0.1);
		

	//}
	//else
	{

		//vec4 mix_cloud= 
		//if ( spec.rgb==0.00 )
		{
		//	gl_FragColor = mix((( diffuse)*cloud_color), dayColor,cloudTrans);
		}
		//else
		{
			gl_FragColor= rimcolor+ mix( diffuse*1.5*dayColor, mix_night_day, 0.5);
		//gl_FragColor = mix(( diffuse*2)*cloud_color*dayColor , (specular) *mix_night_day , cloudTrans);
		}
	}

	//if (rim_factor>=0)
	//{
//		gl_FragColor = mix(vec4(rim_factor/5,rim_factor/2,rim_factor/2,1.0)*3, gl_FragColor,0.7);

	//}

}

float rand(vec2 co)
{
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
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