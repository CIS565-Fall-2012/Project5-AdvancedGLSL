#version 330

uniform float u_Far;

in vec3 fs_Normal;
in vec4 fs_Position;

out vec4 out_Normal;
out vec4 out_Position;

void main(void)
{
    out_Normal = vec4(normalize(fs_Normal),0.0f);
	out_Position = vec4(fs_Position.xyz,1.0f); //Tuck position into 0 1 range
}
