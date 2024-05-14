
SamplerState	g_LinearSmp	: register(s0);
SamplerState	g_PointSmp		: register(s1);

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

cbuffer Debug	: register(b9)
{
	matrix	g_matDebugWVP;
}

Texture2D		g_DiffuseTex	: register(t0);
SamplerState	g_DiffuseSmp	: register(s0);

VS_OUTPUT_UV DebugVS(VS_INPUT_UV input)
{
	VS_OUTPUT_UV output = (VS_OUTPUT_UV)0;

	output.vPos = mul(float4(input.vPos, 1.f), g_matDebugWVP);
	output.vUV = input.vUV;

	return output;
}

PS_OUTPUT DebugPS(VS_OUTPUT_UV input)
{
	PS_OUTPUT	output = (PS_OUTPUT)0;

	float4 vColor = g_DiffuseTex.Sample(g_LinearSmp, input.vUV);

	if (vColor.a == 0.f)
		clip(-1);

	output.vColor = vColor;

	return output;
}