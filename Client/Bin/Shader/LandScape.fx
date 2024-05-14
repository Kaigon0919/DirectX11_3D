
#include "Share.fx"

cbuffer LandScape	: register(b10)
{
	float	g_fDetailLevelX;
	float	g_fDetailLevelY;
	int		g_iSplatCount;
	float	g_fLandScapeEmpty;
}

Texture2DArray	g_DiffuseSplatTex	: register(t10);
Texture2DArray	g_NormalSplatTex	: register(t11);
Texture2DArray	g_SpecularSplatTex	: register(t12);
Texture2DArray	g_AlphaSplatTex		: register(t13);

VS_OUTPUT_3D LandScapeVS(VS_INPUT_3D input)
{
	VS_OUTPUT_3D	output = (VS_OUTPUT_3D)0;

	output.vProjPos = mul(float4(input.vPos, 1.f), g_matWVP);
	output.vPos = output.vProjPos;
	output.vUV = input.vUV;

	// 뷰공간의 위치를 구한다.
	output.vViewPos = mul(float4(input.vPos, 1.f), g_matWV).xyz;
	// 뷰공간의 Normal을 구한다.
	output.vNormal = normalize(mul(float4(input.vNormal, 0.f), g_matWVRot).xyz);

	//if (g_iMtrlNormal == 1)
	{
		output.vTangent = normalize(mul(float4(input.vTangent, 0.f), g_matWVRot).xyz);
		output.vBinormal = normalize(mul(float4(input.vBinormal, 0.f), g_matWVRot).xyz);
	}

	return output;
}

PS_OUTPUT_GBUFFER LandScapePS(VS_OUTPUT_3D input)
{
	PS_OUTPUT_GBUFFER	output = (PS_OUTPUT_GBUFFER)0;

	float2	vDetailUV = input.vUV;
	vDetailUV.x *= g_fDetailLevelX;
	vDetailUV.y *= g_fDetailLevelY;

	if (g_iRenderMode == RENDER_FORWARD)
	{
		LightInfo	tInfo = ComputeLight(input.vViewPos, input.vNormal);

		float4	vColor = g_DiffuseTex.Sample(g_LinearSmp, vDetailUV);

		// 스플래팅 컬러를 구한다.
		for (int i = 0; i < g_iSplatCount; ++i)
		{
			float4	vAlpha = g_AlphaSplatTex.Sample(g_LinearSmp, float3(input.vUV, i));
			float4	vSplatDif = g_DiffuseSplatTex.Sample(g_LinearSmp, float3(vDetailUV, i));

			vColor = vColor * (1.f - vAlpha.r) + vSplatDif * vAlpha.r;
		}

		output.vAlbedo = vColor * (tInfo.vDif + tInfo.vAmb) + tInfo.vSpc;
	}

	else
	{
		float4	vColor = g_DiffuseTex.Sample(g_LinearSmp, vDetailUV);

		float3	vNormal = input.vNormal;

		float4	vNormalCol = g_NormalTex.Sample(g_LinearSmp, vDetailUV);

		float4	vSpc = g_vMtrlSpc;

		//if (g_iMtrlSpecular == 1)
		{
			float4	vSpecular = g_SpecularTex.Sample(g_LinearSmp, vDetailUV);

			vSpc.xyz = vSpecular.xyz;
		}

		float4	vAlpha = (float4)0.f;
		float4	vSplatDif = (float4)0.f;
		float4	vSplatNrm = (float4)0.f;
		float4	vSplatSpc = (float4)0.f;
		float	fAlpha = 0.f;
		float	fInvAlpha = 0.f;

		// 스플래팅 컬러를 구한다.
		for (int i = 0; i < g_iSplatCount; ++i)
		{
			vAlpha = g_AlphaSplatTex.Sample(g_LinearSmp, float3(input.vUV, i));
			vSplatDif = g_DiffuseSplatTex.Sample(g_LinearSmp, float3(vDetailUV, i));
			vSplatNrm = g_NormalSplatTex.Sample(g_LinearSmp, float3(vDetailUV, i));
			vSplatSpc = g_SpecularSplatTex.Sample(g_LinearSmp, float3(vDetailUV, i));

			fAlpha = vAlpha.r;
			fInvAlpha = 1.f - vAlpha.r;

			vColor = vColor * float4(fInvAlpha, fInvAlpha, fInvAlpha, fInvAlpha) + 
				vSplatDif * float4(fAlpha, fAlpha, fAlpha, fAlpha);
			vNormalCol = vNormalCol * float4(fInvAlpha, fInvAlpha, fInvAlpha, fInvAlpha) +
				vSplatNrm * float4(fAlpha, fAlpha, fAlpha, fAlpha);
			vSpc.xyz = vSpc.xyz * float3(fInvAlpha, fInvAlpha, fInvAlpha) +
				vSplatSpc.xyz * float3(fAlpha, fAlpha, fAlpha);
		}

		vNormal = vNormalCol.xyz * 2.f - 1.f;
		//vNormal = normalize(vNormal);

		float3x3	mat =
		{
			input.vTangent,
			input.vBinormal,
			input.vNormal
		};

		vNormal = normalize(mul(vNormal, mat));

		output.vAlbedo = vColor;

		output.vNormal.xyz = vNormal;
		output.vNormal.w = g_vMtrlSpc.w;
		output.vDepth.xyz = (float3)(input.vProjPos.z / input.vProjPos.w);
		output.vDepth.w = input.vProjPos.w;

		output.vMaterial.r = ConvertColor(g_vMtrlDif);
		output.vMaterial.g = ConvertColor(g_vMtrlAmb);
		output.vMaterial.b = ConvertColor(vSpc);
		output.vMaterial.a = ConvertColor(g_vMtrlEmv);
	}

	return output;
}
