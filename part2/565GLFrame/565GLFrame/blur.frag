#version 330

uniform sampler2D u_SSAOtex;

in vec2 fs_Texcoord;

out vec4 out_Color;

void main(void)
{
    out_Color = vec4(texture(u_SSAOtex,fs_Texcoord).xyz, 1.0);
}
