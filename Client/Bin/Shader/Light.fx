
#include "Share.fx"

Texture2D	g_GBufferAlbedo	: register(t10);
Texture2D	g_GBufferNormal	: register(t11);
Texture2D	g_GBufferDepth	: register(t12);
Texture2D	g_GBufferMaterial	: register(t13);

Texture2D	g_LightDif	: register(t14);
Texture2D	g_LightSpc	: register(t15);

static const float2	vNullPos[4] =
{
	float2(-1.f, 1.f),
	float2(1.f, 1.f),
	float2(-1.f, -1.f),
	float2(1.f, -1.f)
};

static const float2	vNullUV[4] =
{
	float2(0.f, 0.f),
	float2(1.f, 0.f),
	float2(0.f, 1.f),
	float2(1.f, 1.f)
};

struct PS_OUTPUT_LIGHTACC
{
	float4	vDif	: SV_TARGET;
	float4	vSpc	: SV_TARGET1;
};

VS_OUTPUT_UV LightAccDirVS(uint iVertexID : SV_VertexID)
{
	VS_OUTPUT_UV	output = (VS_OUTPUT_UV)0;

	output.vPos = float4(vNullPos[iVertexID], 0.f, 1.f);
	output.vUV = vNullUV[iVertexID];

	return output;
}

VS_OUTPUT_UV LightAccPointVS(VS_INPUT_POS input)
{
	VS_OUTPUT_UV	output = (VS_OUTPUT_UV)0;

	float4	vPos = mul(float4(input.vPos, 1.f), g_matWVP);
	output.vPos = vPos;

	output.vUV.x = (vPos.x / vPos.w) * 0.5f + 0.5f;
	output.vUV.y = (vPos.y / vPos.w) * -0.5f + 0.5f;

	return output;
}

VS_OUTPUT_UV LightAccSpotVS(uint iVertexID : SV_VertexID)
{
	VS_OUTPUT_UV	output = (VS_OUTPUT_UV)0;

	output.vPos = float4(vNullPos[iVertexID], 0.f, 1.f);
	output.vUV = vNullUV[iVertexID];

	return output;
}

PS_OUTPUT_LIGHTACC LightAccPS(VS_OUTPUT_UV input)
{
	PS_OUTPUT_LIGHTACC	output = (PS_OUTPUT_LIGHTACC)0;

	float4	vDepth = g_GBufferDepth.Sample(g_LinearSmp, input.vUV);

	if (vDepth.w == 0.f)
		clip(-1);

	// UV좌표를 이용해서 w로 나눈 좌표를 구한다.
	float3	vPos;
	vPos.x = input.vUV.x * 2.f - 1.f;
	vPos.y = input.vUV.y * -2.f + 1.f;
	vPos.z = vDepth.x;

	vPos *= vDepth.w;

	// 투영 역행렬을 곱해서 뷰공간으로 변환한다.
	vPos = mul(float4(vPos, vDepth.w), g_matInvProj).xyz;

	float4	vNormalCol = g_GBufferNormal.Sample(g_PointSmp, input.vUV);
	float3	vNormal = vNormalCol.xyz;

	float4	vMaterial = g_GBufferMaterial.Sample(g_PointSmp, input.vUV);
	float4	vDif = ConvertColor(vMaterial.r);
	float4	vAmb = ConvertColor(vMaterial.g);
	float4	vSpc = ConvertColor(vMaterial.b);
	float4	vEmv = ConvertColor(vMaterial.a);

	LightInfo	tInfo = ComputeLight(vPos, vNormal, vDif,
		vAmb, vSpc, vEmv, vNormalCol.w);

	output.vDif = tInfo.vDif + tInfo.vAmb;
	output.vSpc = tInfo.vSpc + tInfo.vEmv;

	return output;
}

PS_OUTPUT LightBlendPS(VS_OUTPUT_UV input)
{
	PS_OUTPUT	output = (PS_OUTPUT)0;

	float4	vAlbedo = g_GBufferAlbedo.Sample(g_LinearSmp, input.vUV);
	float4	vDif = g_LightDif.Sample(g_LinearSmp, input.vUV);
	float4	vSpc = g_LightSpc.Sample(g_LinearSmp, input.vUV);

	if (vAlbedo.a == 0.f)
		clip(-1);

	output.vColor = vAlbedo * vDif + vSpc;
	output.vColor.a = vAlbedo.a;

	return output;
}

PS_OUTPUT LightBlendRenderPS(VS_OUTPUT_UV input)
{
	PS_OUTPUT	output = (PS_OUTPUT)0;

	output.vColor = g_DiffuseTex.Sample(g_LinearSmp, input.vUV);

	return output;
}
