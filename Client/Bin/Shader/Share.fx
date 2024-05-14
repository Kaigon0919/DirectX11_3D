
struct VS_INPUT_COLOR
{
	float3	vPos	: POSITION;
	float4	vColor	: COLOR;
};

struct VS_OUTPUT_COLOR
{
	float4	vPos	: SV_POSITION;
	float4	vColor	: COLOR;
};

struct VS_INPUT_NORMAL_COLOR
{
	float3	vPos	: POSITION;
	float3	vNormal	: NORMAL;
	float4	vColor	: COLOR;
};

struct VS_OUTPUT_NORMAL_COLOR
{
	float4	vPos	: SV_POSITION;
	float3	vNormal	: NORMAL;
	float4	vColor	: COLOR;
	float3	vViewPos	: POSITION;
	float4	vProjPos	: POSITION1;
};

struct VS_INPUT_3D
{
	float3	vPos	: POSITION;
	float3	vNormal	: NORMAL;
	float2	vUV		: TEXCOORD;
	float3	vTangent	: TANGENT;
	float3	vBinormal	: BINORMAL;
	float4	vWeights	: BLENDWEIGHTS;
	float4	vIndices	: BLENDINDICES;
};

struct VS_OUTPUT_3D
{
	float4	vPos	: SV_POSITION;
	float3	vNormal	: NORMAL;
	float2	vUV		: TEXCOORD;
	float3	vTangent	: TANGENT;
	float3	vBinormal	: BINORMAL;
	float3	vViewPos	: POSITION;
	float4	vProjPos	: POSITION1;
};

struct VS_INPUT_3DINSTANCING
{
	float3	vPos	: POSITION;
	float3	vNormal	: NORMAL;
	float2	vUV		: TEXCOORD;
	float3	vTangent	: TANGENT;
	float3	vBinormal	: BINORMAL;
	float4	vWeights	: BLENDWEIGHTS;
	float4	vIndices	: BLENDINDICES;
	matrix	matWVP		: WORLD;
	matrix	matWV		: WORLDVIEW;
	matrix	matWVRot	: WORLDVIEWROT;
};

struct VS_OUTPUT_3DINSTANCING
{
	float4	vPos	: SV_POSITION;
	float3	vNormal	: NORMAL;
	float2	vUV		: TEXCOORD;
	float3	vTangent	: TANGENT;
	float3	vBinormal	: BINORMAL;
	float3	vViewPos	: POSITION;
	float4	vProjPos	: POSITION1;
	matrix	matWVP		: WORLD;
	matrix	matWV		: WORLDVIEW;
	matrix	matWVRot	: WORLDVIEWROT;
};

struct VS_INPUT_POS
{
	float3	vPos	: POSITION;
};

struct VS_OUTPUT_POS
{
	float4	vPos	: SV_POSITION;
};

struct VS_OUTPUT_SKY
{
	float4	vPos	: SV_POSITION;
	float3	vUV		: TEXCOORD;
};

struct VS_INPUT_UV
{
	float3	vPos	: POSITION;
	float2	vUV		: TEXCOORD;
};

struct VS_OUTPUT_UV
{
	float4	vPos	: SV_POSITION;
	float2	vUV		: TEXCOORD;
};

struct PS_OUTPUT
{
	float4	vColor	: SV_TARGET;
};

struct PS_OUTPUT_GBUFFER
{
	float4	vAlbedo		: SV_TARGET;
	float4	vNormal		: SV_TARGET1;
	float4	vDepth		: SV_TARGET2;
	float4	vMaterial	: SV_TARGET3;
};

cbuffer Transform	: register(b0)
{
	matrix	g_matWorldRot;
	matrix	g_matWVRot;
	matrix	g_matWorld;
	matrix	g_matView;
	matrix	g_matProj;
	matrix	g_matInvProj;
	matrix	g_matWV;
	matrix	g_matWVP;
	matrix	g_matVP;
}

cbuffer	Material	: register(b1)
{
	float4	g_vMtrlDif;
	float4	g_vMtrlAmb;
	float4	g_vMtrlSpc;
	float4	g_vMtrlEmv;
	int		g_iMtrlNormal;
	int		g_iMtrlSpecular;
	int		g_iSkinning;
	float	g_fMtrlEmpty;
}

cbuffer	Light	: register(b2)
{
	float4	g_vLightDif;
	float4	g_vLightAmb;
	float4	g_vLightSpc;
	float3	g_vLightPos;
	int		g_iLightType;
	float3	g_vLightDir;
	float	g_fLightDist;
	float	g_fLightInAngle;
	float	g_fLightOutAngle;
	float2	g_vLightEmpty;
}

#define	RENDER_FORWARD	0
#define	RENDER_DEFERRED	1

cbuffer	Rendering	: register(b3)
{
	int		g_iRenderMode;
	float3	g_vRenderEmpty;
}

#define	AFT_ATLAS	0
#define	AFT_FRAME	1

cbuffer AnimationFrame	: register(b8)
{
	int		g_iAnimFrameType;
	int		g_iAnimFrameOption;
	float2	g_vAnimFrameTextureSize;
	float2	g_vAnimFrameStart;
	float2	g_vAnimFrameEnd;
	int		g_iAnimFrameFrame;
	float3	g_vAnimFrameEmpty;
}


Texture2D		g_DiffuseTex	: register(t0);
Texture2D		g_NormalTex		: register(t1);
Texture2D		g_SpecularTex	: register(t2);
Texture2D		g_BoneTex		: register(t3);
SamplerState	g_LinearSmp		: register(s0);
SamplerState	g_PointSmp		: register(s1);





VS_OUTPUT_COLOR StandardVertex3DVS(VS_INPUT_3D input)
{
	VS_OUTPUT_COLOR	output = (VS_OUTPUT_COLOR)0;

	return output;
}

PS_OUTPUT StandardVertex3DPS(VS_OUTPUT_COLOR input)
{
	PS_OUTPUT output = (PS_OUTPUT)0;

	return output;
}

struct LightInfo
{
	float4	vDif;
	float4	vAmb;
	float4	vSpc;
	float4	vEmv;
};

#define	LIGHT_DIR	0
#define	LIGHT_POINT	1
#define	LIGHT_SPOT	2

LightInfo ComputeLight(float3 vViewPos, float3 vViewNormal)
{
	LightInfo	tInfo = (LightInfo)0;

	float3	vLightDir;
	float	fIntensity = 1.f;

	if (g_iLightType == LIGHT_DIR)
	{
		// 조명 방향을 뷰공간으로 변환한다.
		// 하지만 조명 방향의 역방향을 구해야 한다.
		//vLightDir = -normalize(mul(float4(g_vLightDir, 0.f), g_matView).xyz);
		vLightDir = g_vLightDir;
	}

	if (g_iLightType == LIGHT_POINT)
	{
		// 정점에서 조명을 향하는 벡터를 구해준다.
		vLightDir = g_vLightPos - vViewPos;
		vLightDir = normalize(vLightDir);

		// 정점과 조명간의 거리를 구해준다.
		float	fDist = distance(g_vLightPos, vViewPos);

		if (fDist > g_fLightDist)
			fIntensity = 0.f;

		else
		{
			//fIntensity = 1.f - fDist / g_fLightDist;
			fIntensity = (1.f - fDist / g_fLightDist) * 0.7f + 0.3f;
		}
	}

	if (g_iLightType == LIGHT_SPOT)
	{
		// 정점에서 조명을 향하는 벡터를 구해준다.
		vLightDir = g_vLightPos - vViewPos;
		vLightDir = normalize(vLightDir);

		// 정점과 조명간의 거리를 구해준다.
		float	fDist = distance(g_vLightPos, vViewPos);

		if (fDist > g_fLightDist)
			fIntensity = 0.f;

		else
		{
			// 바깥쪽 반경 안에 들어왔는지 판단한다.
			float3	vDir = -vLightDir;

			// vDir과 조명이 비추는 벡터를 구해서 조명의 반경 안에 들어오는지
			// 판단한다.
			float	fDot = dot(vDir, g_vLightDir);

			if (fDot < g_fLightOutAngle)
				fIntensity = 0.f;

			// 안쪽 반경에 있는지 판단한다.
			else if (fDot >= g_fLightInAngle)
				fIntensity = 1.f;

			// 바깥쪽 반경일 경우
			else
			{
				fIntensity = (g_fLightInAngle - fDot) / (g_fLightInAngle - g_fLightOutAngle);
			}

			fIntensity = fIntensity * 0.3f + 0.7f;
			float fDistIntensity = (1.f - fDist / g_fLightDist) * 0.3f + 0.7f;

			fIntensity *= fDistIntensity;
		}
	}

	tInfo.vDif = g_vLightDif * g_vMtrlDif * max(0, dot(vViewNormal, vLightDir)) *
		fIntensity;
	tInfo.vAmb = g_vLightAmb * g_vMtrlAmb;

	// 반사벡터를 구한다.
	float3	vR = 2.f * dot(vViewNormal, vLightDir) * vViewNormal - vLightDir;
	vR = normalize(vR);

	// View 벡터를 구한다.
	float3	vView = -normalize(vViewPos);

	tInfo.vSpc = g_vLightSpc * g_vMtrlSpc * pow(max(0, dot(vR, vView)), g_vMtrlSpc.w) *
		fIntensity;

	return tInfo;
}

LightInfo ComputeLight(float3 vViewPos, float3 vViewNormal,
	float4 vDif, float4 vAmb, float4 vSpc, float4 vEmv, 
	float fSpcPower)
{
	LightInfo	tInfo = (LightInfo)0;

	float3	vLightDir;
	float	fIntensity = 1.f;

	if (g_iLightType == LIGHT_DIR)
	{
		// 조명 방향을 뷰공간으로 변환한다.
		// 하지만 조명 방향의 역방향을 구해야 한다.
		//vLightDir = -normalize(mul(float4(g_vLightDir, 0.f), g_matView).xyz);
		vLightDir = g_vLightDir;
	}

	if (g_iLightType == LIGHT_POINT)
	{
		// 정점에서 조명을 향하는 벡터를 구해준다.
		vLightDir = g_vLightPos - vViewPos;
		vLightDir = normalize(vLightDir);

		// 정점과 조명간의 거리를 구해준다.
		float	fDist = distance(g_vLightPos, vViewPos);

		if (fDist > g_fLightDist)
			fIntensity = 0.f;

		else
		{
			//fIntensity = 1.f - fDist / g_fLightDist;
			fIntensity = (1.f - fDist / g_fLightDist) * 0.7f + 0.3f;
		}
	}

	if (g_iLightType == LIGHT_SPOT)
	{
		// 정점에서 조명을 향하는 벡터를 구해준다.
		vLightDir = g_vLightPos - vViewPos;
		vLightDir = normalize(vLightDir);

		// 정점과 조명간의 거리를 구해준다.
		float	fDist = distance(g_vLightPos, vViewPos);

		if (fDist > g_fLightDist)
			fIntensity = 0.f;

		else
		{
			// 바깥쪽 반경 안에 들어왔는지 판단한다.
			float3	vDir = -vLightDir;

			// vDir과 조명이 비추는 벡터를 구해서 조명의 반경 안에 들어오는지
			// 판단한다.
			float	fDot = dot(vDir, g_vLightDir);

			if (fDot < g_fLightOutAngle)
				fIntensity = 0.f;

			// 안쪽 반경에 있는지 판단한다.
			else if (fDot >= g_fLightInAngle)
				fIntensity = 1.f;

			// 바깥쪽 반경일 경우
			else
			{
				fIntensity = (g_fLightInAngle - fDot) / (g_fLightInAngle - g_fLightOutAngle);
			}

			fIntensity = fIntensity * 0.3f + 0.7f;
			float fDistIntensity = (1.f - fDist / g_fLightDist) * 0.3f + 0.7f;

			fIntensity *= fDistIntensity;
		}
	}

	tInfo.vDif = g_vLightDif * vDif * max(0, dot(vViewNormal, vLightDir)) *
		fIntensity;
	tInfo.vAmb = g_vLightAmb * vAmb;

	// 반사벡터를 구한다.
	float3	vR = 2.f * dot(vViewNormal, vLightDir) * vViewNormal - vLightDir;
	vR = normalize(vR);

	// View 벡터를 구한다.
	float3	vView = -normalize(vViewPos);

	tInfo.vSpc = g_vLightSpc * vSpc * pow(max(0, dot(vR, vView)), fSpcPower) *
		fIntensity;
	tInfo.vEmv = vSpc * vEmv;

	return tInfo;
}




float ConvertColor(float4 vColor)
{
	uint4	vColor1 = (uint4)0;
	vColor1.r = uint(vColor.r * 255);
	vColor1.g = uint(vColor.g * 255);
	vColor1.b = uint(vColor.b * 255);
	vColor1.a = uint(vColor.a * 255);

	uint	iColor = 0;
	iColor = (uint)(vColor1.a);
	iColor = (iColor << 8) | vColor1.r;
	iColor = (iColor << 8) | vColor1.g;
	iColor = (iColor << 8) | vColor1.b;

	return asfloat(iColor);
}

float4 ConvertColor(float fColor)
{
	float4	vColor;
	uint	iColor = asuint(fColor);
	vColor.b = (iColor & 0x000000ff) / 255.f;
	vColor.g = ((iColor >> 8) & 0x000000ff) / 255.f;
	vColor.r = ((iColor >> 16) & 0x000000ff) / 255.f;
	vColor.a = ((iColor >> 24) & 0x000000ff) / 255.f;

	return vColor;
}


struct _tagSkinning
{
	float3	vPos;
	float3	vNormal;
	float3	vTangent;
	float3	vBinormal;
};

matrix GetBoneMatrix(int idx)
{
	matrix	matBone =
	{
		g_BoneTex.Load(int3(idx * 4, 0, 0)),
		g_BoneTex.Load(int3(idx * 4 + 1, 0, 0)),
		g_BoneTex.Load(int3(idx * 4 + 2, 0, 0)),
		g_BoneTex.Load(int3(idx * 4 + 3, 0, 0))
	};

	return matBone;
}

_tagSkinning Skinning(float3 vPos, float3 vNormal, float3 vTangent,
	float3 vBinormal, float4 vWeights, float4 vIndices)
{
	_tagSkinning	tSkinning = (_tagSkinning)0;

	float	fWeights[4];
	fWeights[0] = vWeights.x;
	fWeights[1] = vWeights.y;
	fWeights[2] = vWeights.z;
	fWeights[3] = 1.f - vWeights.x - vWeights.y - vWeights.z;

	for (int i = 0; i < 4; ++i)
	{
		matrix	matBone = GetBoneMatrix((int)vIndices[i]);

		tSkinning.vPos += fWeights[i] * mul(float4(vPos, 1.f), matBone).xyz;
		tSkinning.vNormal += fWeights[i] * mul(float4(vNormal, 0.f), matBone).xyz;
		tSkinning.vTangent += fWeights[i] * mul(float4(vTangent, 0.f), matBone).xyz;
		tSkinning.vBinormal += fWeights[i] * mul(float4(vBinormal, 0.f), matBone).xyz;
	}

	tSkinning.vNormal = normalize(tSkinning.vNormal);
	tSkinning.vTangent = normalize(tSkinning.vTangent);
	tSkinning.vBinormal = normalize(tSkinning.vBinormal);

	return tSkinning;
}

_tagSkinning Skinning(float3 vPos, float3 vNormal, float4 vWeights,
	float4 vIndices)
{
	_tagSkinning	tSkinning = (_tagSkinning)0;

	float	fWeights[4];
	fWeights[0] = vWeights.x;
	fWeights[1] = vWeights.y;
	fWeights[2] = vWeights.z;
	fWeights[3] = 1.f - vWeights.x - vWeights.y - vWeights.z;

	for (int i = 0; i < 4; ++i)
	{
		matrix	matBone = GetBoneMatrix((int)vIndices[i]);

		tSkinning.vPos += fWeights[i] * mul(float4(vPos, 1.f), matBone).xyz;
		tSkinning.vNormal += fWeights[i] * mul(float4(vNormal, 0.f), matBone).xyz;
	}

	tSkinning.vNormal = normalize(tSkinning.vNormal);

	return tSkinning;
}

float2 ComputeFrameUV(float2 vUV)
{
	float2	vResult = vUV;

	if (g_iAnimFrameType == AFT_ATLAS)
	{
		if (vUV.x == 0.f)
			vResult.x = g_vAnimFrameStart.x / g_vAnimFrameTextureSize.x;

		else
			vResult.x = g_vAnimFrameEnd.x / g_vAnimFrameTextureSize.x;


		if (vUV.y == 0.f)
			vResult.y = g_vAnimFrameStart.y / g_vAnimFrameTextureSize.y;

		else
			vResult.y = g_vAnimFrameEnd.y / g_vAnimFrameTextureSize.y;
	}
	
	return vResult;
}

