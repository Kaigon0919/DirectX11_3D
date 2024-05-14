
#include "Share.fx"

TextureCube	g_CubeTex	: register(t10);

VS_OUTPUT_COLOR StandardColorVS(VS_INPUT_COLOR input)
{
	VS_OUTPUT_COLOR	output = (VS_OUTPUT_COLOR)0;

	output.vPos = mul(float4(input.vPos, 1.f), g_matWVP);
	output.vColor = input.vColor;

	return output;
}

PS_OUTPUT StandardColorPS(VS_OUTPUT_COLOR input)
{
	PS_OUTPUT output = (PS_OUTPUT)0;

	output.vColor = input.vColor;

	return output;
}

VS_OUTPUT_NORMAL_COLOR StandardNormalColorVS(VS_INPUT_NORMAL_COLOR input)
{
	VS_OUTPUT_NORMAL_COLOR	output = (VS_OUTPUT_NORMAL_COLOR)0;

	output.vProjPos = mul(float4(input.vPos, 1.f), g_matWVP);
	output.vPos = output.vProjPos;
	output.vColor = input.vColor;

	// 뷰공간의 위치를 구한다.
	output.vViewPos = mul(float4(input.vPos, 1.f), g_matWV).xyz;
	// 뷰공간의 Normal을 구한다.
	output.vNormal = normalize(mul(float4(input.vNormal, 0.f), g_matWVRot).xyz);

	return output;
}

PS_OUTPUT_GBUFFER StandardColorNormalPS(VS_OUTPUT_NORMAL_COLOR input)
{
	PS_OUTPUT_GBUFFER output = (PS_OUTPUT_GBUFFER)0;

	if (g_iRenderMode == RENDER_FORWARD)
	{
		LightInfo	tInfo = ComputeLight(input.vViewPos, input.vNormal);

		output.vAlbedo = input.vColor * (tInfo.vDif + tInfo.vAmb) + tInfo.vSpc;
	}

	else
	{
		output.vAlbedo = input.vColor;
		output.vNormal.xyz = input.vNormal;
		output.vNormal.w = g_vMtrlSpc.w;
		output.vDepth.xyz = (float3)(input.vProjPos.z / input.vProjPos.w);
		output.vDepth.w = input.vProjPos.w;
		output.vMaterial.r = ConvertColor(g_vMtrlDif);
		output.vMaterial.g = ConvertColor(g_vMtrlAmb);
		output.vMaterial.b = ConvertColor(g_vMtrlSpc);
		output.vMaterial.a = ConvertColor(g_vMtrlEmv);
	}

	return output;
}

VS_OUTPUT_SKY SkyVS(VS_INPUT_POS input)
{
	VS_OUTPUT_SKY	output = (VS_OUTPUT_SKY)0;

	output.vPos = mul(float4(input.vPos, 1.f), g_matWVP).xyww;
	output.vUV = input.vPos;

	return output;
}

PS_OUTPUT SkyPS(VS_OUTPUT_SKY input)
{
	PS_OUTPUT	output = (PS_OUTPUT)0;

	output.vColor = g_CubeTex.Sample(g_LinearSmp, input.vUV);

	return output;
}

VS_OUTPUT_3D StandardTexNormalVS(VS_INPUT_3D input)
{
	VS_OUTPUT_3D	output = (VS_OUTPUT_3D)0;

	output.vProjPos = mul(float4(input.vPos, 1.f), g_matWVP);
	output.vPos = output.vProjPos;
	output.vUV = input.vUV;

	// 뷰공간의 위치를 구한다.
	output.vViewPos = mul(float4(input.vPos, 1.f), g_matWV).xyz;
	// 뷰공간의 Normal을 구한다.
	output.vNormal = normalize(mul(float4(input.vNormal, 0.f), g_matWVRot).xyz);

	return output;
}

VS_OUTPUT_3D Standard3DVS(VS_INPUT_3D input)
{
	VS_OUTPUT_3D	output = (VS_OUTPUT_3D)0;

	_tagSkinning	tSkinning = (_tagSkinning)0;

	tSkinning.vPos = input.vPos;
	tSkinning.vNormal = input.vNormal;
	tSkinning.vTangent = input.vTangent;
	tSkinning.vBinormal = input.vBinormal;

	if (g_iSkinning == 1)
	{
		if (g_iMtrlNormal == 1)
			tSkinning = Skinning(input.vPos, input.vNormal, input.vTangent, input.vBinormal, input.vWeights, input.vIndices);

		else
			tSkinning = Skinning(input.vPos, input.vNormal, input.vWeights, input.vIndices);
	}

	output.vProjPos = mul(float4(tSkinning.vPos, 1.f), g_matWVP);
	output.vPos = output.vProjPos;
	output.vUV = input.vUV;

	// 뷰공간의 위치를 구한다.
	output.vViewPos = mul(float4(tSkinning.vPos, 1.f), g_matWV).xyz;
	// 뷰공간의 Normal을 구한다.
	output.vNormal = normalize(mul(float4(tSkinning.vNormal, 0.f), g_matWVRot).xyz);

	if (g_iMtrlNormal == 1)
	{
		output.vTangent = normalize(mul(float4(tSkinning.vTangent, 0.f), g_matWVRot).xyz);
		output.vBinormal = normalize(mul(float4(tSkinning.vBinormal, 0.f), g_matWVRot).xyz);
	}

	return output;
}

PS_OUTPUT_GBUFFER Standard3DPS(VS_OUTPUT_3D input)
{
	PS_OUTPUT_GBUFFER output = (PS_OUTPUT_GBUFFER)0;

	if (g_iRenderMode == RENDER_FORWARD)
	{
		LightInfo	tInfo = ComputeLight(input.vViewPos, input.vNormal);

		float4	vColor = g_DiffuseTex.Sample(g_LinearSmp, input.vUV);

		output.vAlbedo = vColor * (tInfo.vDif + tInfo.vAmb) + tInfo.vSpc;
	}

	else
	{
		float4	vColor = g_DiffuseTex.Sample(g_LinearSmp, input.vUV);
		output.vAlbedo = vColor;

		float3	vNormal = input.vNormal;

		if (g_iMtrlNormal == 1)
		{
			float4	vNormalCol = g_NormalTex.Sample(g_LinearSmp, input.vUV);
			vNormal = vNormalCol.xyz * 2.f - 1.f;

			float3x3	mat	=
			{
				input.vTangent,
				input.vBinormal,
				input.vNormal
			};

			vNormal = normalize(mul(vNormal, mat));
		}

		output.vNormal.xyz = vNormal;
		output.vNormal.w = g_vMtrlSpc.w;
		output.vDepth.xyz = (float3)(input.vProjPos.z / input.vProjPos.w);
		output.vDepth.w = input.vProjPos.w;

		float4	vSpc = g_vMtrlSpc;

		if (g_iMtrlSpecular == 1)
		{
			float4	vSpecular = g_SpecularTex.Sample(g_LinearSmp, input.vUV);

			vSpc.xyz = vSpecular.xyz;
		}

		output.vMaterial.r = ConvertColor(g_vMtrlDif);
		output.vMaterial.g = ConvertColor(g_vMtrlAmb);
		output.vMaterial.b = ConvertColor(vSpc);
		output.vMaterial.a = ConvertColor(g_vMtrlEmv);
	}

	return output;
}

VS_OUTPUT_3DINSTANCING Standard3DInstancingVS(VS_INPUT_3DINSTANCING input)
{
	VS_OUTPUT_3DINSTANCING	output = (VS_OUTPUT_3DINSTANCING)0;

	_tagSkinning	tSkinning = (_tagSkinning)0;

	tSkinning.vPos = input.vPos;
	tSkinning.vNormal = input.vNormal;
	tSkinning.vTangent = input.vTangent;
	tSkinning.vBinormal = input.vBinormal;

	if (g_iSkinning == 1)
	{
		if (g_iMtrlNormal == 1)
			tSkinning = Skinning(input.vPos, input.vNormal, input.vTangent, input.vBinormal, input.vWeights, input.vIndices);

		else
			tSkinning = Skinning(input.vPos, input.vNormal, input.vWeights, input.vIndices);
	}

	output.vProjPos = mul(float4(tSkinning.vPos, 1.f), input.matWVP);
	output.vPos = output.vProjPos;
	output.vUV = input.vUV;

	// 뷰공간의 위치를 구한다.
	output.vViewPos = mul(float4(tSkinning.vPos, 1.f), input.matWV).xyz;
	// 뷰공간의 Normal을 구한다.
	output.vNormal = normalize(mul(float4(tSkinning.vNormal, 0.f), input.matWVRot).xyz);

	if (g_iMtrlNormal == 1)
	{
		output.vTangent = normalize(mul(float4(tSkinning.vTangent, 0.f), input.matWVRot).xyz);
		output.vBinormal = normalize(mul(float4(tSkinning.vBinormal, 0.f), input.matWVRot).xyz);
	}

	output.matWVP = input.matWVP;
	output.matWV = input.matWV;
	output.matWVRot = input.matWVRot;

	return output;
}

PS_OUTPUT_GBUFFER Standard3DInstancingPS(VS_OUTPUT_3DINSTANCING input)
{
	PS_OUTPUT_GBUFFER output = (PS_OUTPUT_GBUFFER)0;

	if (g_iRenderMode == RENDER_FORWARD)
	{
		LightInfo	tInfo = ComputeLight(input.vViewPos, input.vNormal);

		float4	vColor = g_DiffuseTex.Sample(g_LinearSmp, input.vUV);

		output.vAlbedo = vColor * (tInfo.vDif + tInfo.vAmb) + tInfo.vSpc;
	}

	else
	{
		float4	vColor = g_DiffuseTex.Sample(g_LinearSmp, input.vUV);
		output.vAlbedo = vColor;

		float3	vNormal = input.vNormal;

		if (g_iMtrlNormal == 1)
		{
			float4	vNormalCol = g_NormalTex.Sample(g_LinearSmp, input.vUV);
			vNormal = vNormalCol.xyz * 2.f - 1.f;

			float3x3	mat =
			{
				input.vTangent,
				input.vBinormal,
				input.vNormal
			};

			vNormal = normalize(mul(vNormal, mat));
		}

		output.vNormal.xyz = vNormal;
		output.vNormal.w = g_vMtrlSpc.w;
		output.vDepth.xyz = (float3)(input.vProjPos.z / input.vProjPos.w);
		output.vDepth.w = input.vProjPos.w;

		float4	vSpc = g_vMtrlSpc;

		if (g_iMtrlSpecular == 1)
		{
			float4	vSpecular = g_SpecularTex.Sample(g_LinearSmp, input.vUV);

			vSpc.xyz = vSpecular.xyz;
		}

		output.vMaterial.r = ConvertColor(g_vMtrlDif);
		output.vMaterial.g = ConvertColor(g_vMtrlAmb);
		output.vMaterial.b = ConvertColor(vSpc);
		output.vMaterial.a = ConvertColor(g_vMtrlEmv);
	}

	return output;
}
