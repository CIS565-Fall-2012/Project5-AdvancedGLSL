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

uniform sampler2D u_Moon;

varying vec3 v_Normal;
varying vec2 v_Texcoord;

void main(void)
{
	 gl_FragColor = texture2D(u_Moon, v_Texcoord);;
}

