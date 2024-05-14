
#include "Share.fx"

cbuffer	Button	: register(b11)
{
	float4	g_vButtonColor;
}

VS_OUTPUT_UV ButtonVS(VS_INPUT_UV input)
{
	VS_OUTPUT_UV	output = (VS_OUTPUT_UV)0;

	output.vPos = mul(float4(input.vPos, 1.f), g_matWVP);
	output.vUV = input.vUV;

	return output;
}

PS_OUTPUT ButtonPS(VS_OUTPUT_UV input)
{
	PS_OUTPUT	output = (PS_OUTPUT)0;

	output.vColor = g_DiffuseTex.Sample(g_LinearSmp, input.vUV) *
		g_vButtonColor;

	return output;
}