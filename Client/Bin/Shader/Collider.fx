
#include "Share.fx"

cbuffer Collider	: register(b10)
{
	float4	g_vColliderColor;
}

VS_OUTPUT_POS ColliderVS(VS_INPUT_POS input)
{
	VS_OUTPUT_POS	output = (VS_OUTPUT_POS)0;

	output.vPos = mul(float4(input.vPos, 1.f), g_matWVP);

	return output;
}

PS_OUTPUT ColliderPS(VS_OUTPUT_POS input)
{
	PS_OUTPUT	output = (PS_OUTPUT)0;

	output.vColor = g_vColliderColor;

	return output;
}
