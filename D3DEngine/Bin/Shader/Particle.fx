
#include "Share.fx"

struct VS_INPUT_PARTICLE
{
	float3	vPos	: POSITION;
};

struct GS_OUTPUT_PARTICLE
{
	float4	vPos	: SV_POSITION;
	float4	vProjPos	: POSITION;
	float2	vUV		: TEXCOORD;
};

cbuffer Particle	: register(b10)
{
	float3	g_vParticlePos;
	float	g_fSizeX;
	float3	g_vAxisX;
	float	g_fSizeY;
	float3	g_vAxisY;
	float	g_fEmpty;
}

Texture2DArray	g_FrameTex	: register(t10);
Texture2D		g_GBufferDepth	: register(t11);

VS_INPUT_PARTICLE ParticleVS(VS_INPUT_PARTICLE input)
{
	return input;
}

[maxvertexcount(4)]
void ParticleGS(point VS_INPUT_PARTICLE input[1],
	inout TriangleStream<GS_OUTPUT_PARTICLE> stream)
{
	float	fHalfX = g_fSizeX * 0.5f;
	float	fHalfY = g_fSizeY * 0.5f;

	float3	vPos[4];
	vPos[0] = g_vParticlePos - g_vAxisX * fHalfX - g_vAxisY * fHalfY;
	vPos[1] = g_vParticlePos - g_vAxisX * fHalfX + g_vAxisY * fHalfY;
	vPos[2] = g_vParticlePos + g_vAxisX * fHalfX - g_vAxisY * fHalfY;
	vPos[3] = g_vParticlePos + g_vAxisX * fHalfX + g_vAxisY * fHalfY;

	float2	vUV[4]	=
	{
		float2(0.f, 1.f),
		float2(0.f, 0.f),
		float2(1.f, 1.f),
		float2(1.f, 0.f)
	};

	GS_OUTPUT_PARTICLE	output = (GS_OUTPUT_PARTICLE)0;

	for (int i = 0; i < 4; ++i)
	{
		output.vProjPos = mul(float4(vPos[i], 1.f), g_matVP);
		output.vPos = output.vProjPos;
		output.vUV = ComputeFrameUV(vUV[i]);

		stream.Append(output);
	}
}

PS_OUTPUT ParticlePS(GS_OUTPUT_PARTICLE input)
{
	PS_OUTPUT	output = (PS_OUTPUT)0;

	if (g_iAnimFrameType == AFT_ATLAS)
		output.vColor = g_DiffuseTex.Sample(g_LinearSmp, input.vUV);

	else
		output.vColor = g_FrameTex.Sample(g_LinearSmp, float3(input.vUV, g_iAnimFrameFrame));

	if (output.vColor.a == 0.f)
		clip(-1);

	// 투영공간의 좌표를 이용해서 화면에서의 UV좌표를 구한다.
	float2	vDepthUV = input.vProjPos.xy / input.vProjPos.w;
	vDepthUV.x = vDepthUV.x * 0.5f + 0.5f;
	vDepthUV.y = vDepthUV.y * -0.5f + 0.5f;

	float4	vDepth = g_GBufferDepth.Sample(g_PointSmp, vDepthUV);

	// 뷰공간의 Z값을 이용해서 비교를 한다.
	float	fDepthDist = vDepth.w - input.vProjPos.w;

	if (vDepth.w == 0.f)
		fDepthDist = 1.f;

	else if (fDepthDist < 0.f)
		clip(-1);

	else if (fDepthDist == 0.f)
		fDepthDist = 0.4f;

	float fAlpha = fDepthDist / 0.4f;
	fAlpha = min(fAlpha, 1.f);

	output.vColor.a *= fAlpha;

	return output;
}