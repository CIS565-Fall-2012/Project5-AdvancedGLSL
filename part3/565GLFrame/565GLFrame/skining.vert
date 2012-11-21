#version 330


uniform mat4x4 u_Model;
uniform mat4x4 u_View;
uniform mat4x4 u_Persp;
uniform mat4x4 u_InvTrans;
uniform vec4 u_BoneMatrix[9]; //3x3

in vec3 Position;
in vec3 Normal;
in vec2 BoneWeight;
in vec2 BoneIndex;

out vec3 fs_Normal;
out vec4 fs_Position;

void main(void) {
	vec3 netPosition = vec3(0);
	vec3 netNormal = vec3(0);
	for (int i = 0; i < 2; ++i)
	{
		if (BoneIndex[i] >= -0.8)
		{
			int index = 3*int(BoneIndex[i]*1.1);
			mat4 model = mat4(vec4(u_BoneMatrix[index + 0].x, u_BoneMatrix[index + 1].x, u_BoneMatrix[index + 2].x, 0),
			                  vec4(u_BoneMatrix[index + 0].y, u_BoneMatrix[index + 1].y, u_BoneMatrix[index + 2].y, 0),
							  vec4(u_BoneMatrix[index + 0].z, u_BoneMatrix[index + 1].z, u_BoneMatrix[index + 2].z, 0),
							  vec4(u_BoneMatrix[index + 0].w, u_BoneMatrix[index + 1].w, u_BoneMatrix[index + 2].w, 1));
			//mat4 model = mat4(u_BoneMatrix[6], u_BoneMatrix[7], u_BoneMatrix[8], vec4(0.0,0.0,0.0,1.0));
			//mat4 model = mat4(vec4(1.5,0,0,0), vec4(0,1.5,0,0), vec4(0,0,1.5,0), vec4(0,0,0,1));
			vec4 bonePosition = model * vec4(Position, 1);
			mat3 rotate = mat3(model[0].xyz, model[1].xyz, model[2].xyz);
			vec3 boneNormal = rotate * Normal;
			
				netPosition += bonePosition.xyz * BoneWeight[i];
				netNormal   += boneNormal.xyz * BoneWeight[i];
			
		}
	}

	

	netNormal = normalize(netNormal);

	fs_Normal = (u_InvTrans*vec4(netNormal,0.0f)).xyz;
	vec4 world = u_Model * vec4(netPosition, 1.0);
    vec4 camera = u_View * world;
	fs_Position = camera;
	gl_Position = u_Persp * camera;
}
