#include "Engine_Shader_Defines.hlsli"

/* 전역변수 : 쉐이더 외부에 있는 데이터를 쉐이더 안으로 받아온다. */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_WorldMatrix_Inv, g_ViewMatrix_Inv, g_ProjMatrix_Inv;

texture2D	g_Texture;
texture2D	g_DepthTexture;

float g_fMinUV_X;
float g_fMinUV_Y;
float g_fMaxUV_X;
float g_fMaxUV_Y;
float g_fAlpha;
float g_fAlpha_Delta;
float3 g_vExtent;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	
};

/* 정점 쉐이더 */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;

	return Out;
}


struct VS_OUT_ALPHABLEND
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};


VS_OUT_ALPHABLEND VS_MAIN_ALPHABLEND(VS_IN In)
{
	VS_OUT_ALPHABLEND		Out = (VS_OUT_ALPHABLEND)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

struct PS_OUT_EFFECT
{
	float4		vDiffuse : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	
	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

PS_OUT PS_NORMAL(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	if (Out.vColor.a < 0.01f)
		discard;

	Out.vColor.a -= g_fAlpha_Delta;

	return Out;
}

PS_OUT PS_BLOOD(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	Out.vColor.r -= 0.1f;
	Out.vColor.g -= 0.1f;
	Out.vColor.b -= 0.1f;

	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

struct PS_IN_ALPHABLEND
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};


PS_OUT PS_MAIN_ALPHABLEND(PS_IN_ALPHABLEND In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	float2		vTexcoord = (float2)0.f;

	vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	float4		vDepthDesc = g_DepthTexture.Sample(PointSampler, vTexcoord);
	float		fOldViewZ = vDepthDesc.y * 1000.f;

	Out.vColor.a = Out.vColor.a  * saturate(fOldViewZ - In.vProjPos.w);

	return Out;
}

PS_OUT_EFFECT PS_EFFECT(PS_IN In)
{
	PS_OUT_EFFECT      Out = (PS_OUT_EFFECT)0;

	float       fTexcoordLinearX = g_fMinUV_X + (In.vTexcoord.x * (g_fMaxUV_X - g_fMinUV_X));
	float       fTexcoordLinearY = g_fMinUV_Y + (In.vTexcoord.y * (g_fMaxUV_Y - g_fMinUV_Y));
	float2      vTexcoordLinear = float2(fTexcoordLinearX, fTexcoordLinearY);

	Out.vDiffuse = g_Texture.Sample(LinearSampler, vTexcoordLinear);

	if (Out.vDiffuse.a <= 0.1f)
		discard;

	Out.vDiffuse.a -= g_fAlpha_Delta;

	if (Out.vDiffuse.a <= 0.1f)
		discard;

	//Out.vDiffuse.a *= g_fAlpha;

	//Out.vMaterial = 0.f;
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);

	return Out;
}

PS_OUT_EFFECT PS_EFFECT_NORMAL(PS_IN In)
{
	PS_OUT_EFFECT      Out = (PS_OUT_EFFECT)0;

	float       fTexcoordLinearX = g_fMinUV_X + (In.vTexcoord.x * (g_fMaxUV_X - g_fMinUV_X));
	float       fTexcoordLinearY = g_fMinUV_Y + (In.vTexcoord.y * (g_fMaxUV_Y - g_fMinUV_Y));
	float2      vTexcoordLinear = float2(fTexcoordLinearX, fTexcoordLinearY);

	Out.vDiffuse = g_Texture.Sample(LinearSampler, vTexcoordLinear);

	if (Out.vDiffuse.a < 0.3f)
		discard;

	return Out;
}

PS_OUT_EFFECT PS_MUZZLE_LIGHT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	if (Out.vColor.a < 0.01f)
		discard;

	Out.vColor.a = 0.4f;

	float2 center = float2(0.5, 0.5);

	float distance = length(In.vTexcoord - center);

	float alpha = saturate(1.0 - distance*2.f);

	Out.vColor.a *= alpha;

	return Out;
}

VS_OUT VS_DECAL(VS_IN In)
{
	VS_OUT output = (VS_OUT)0.f;

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	output.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	output.vTexcoord = In.vTexcoord;

	return output;
}

PS_OUT PS_DECAL(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	float4 vColor = (float4)0.f;

	float2 vScreenUV = (float2)0.f;
	vScreenUV.x = In.vPosition.x / SCREEN_SIZE_X;
	vScreenUV.y = In.vPosition.y / SCREEN_SIZE_Y;

	vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
	float fViewZ = vDepthDesc.y * 1000.0f;
	float4 vWorldPos;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / View.z */
	vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
	vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	vWorldPos *= fViewZ;

	matrix InverViewProj = mul(g_ProjMatrix_Inv, g_ViewMatrix_Inv);

	vWorldPos = mul(vWorldPos, InverViewProj);

	/*if (vViewPos.z == 0.f)
	{
		clip(-1);
	}*/

	float4 vLocalPos = mul(vWorldPos, g_WorldMatrix_Inv);

	if ((-g_vExtent.x <= vLocalPos.x && vLocalPos.x <= g_vExtent.x) &&
		(-g_vExtent.y <= vLocalPos.y && vLocalPos.y <= g_vExtent.y) &&
		(-g_vExtent.z <= vLocalPos.z && vLocalPos.z <= g_vExtent.z))
	{
		float2 decalTextureUV = (vLocalPos.xy / (2.0f * float2(g_vExtent.x, g_vExtent.y))) + 0.5f;
		float3 vDiffuseColor = g_Texture.Sample(LinearSampler, decalTextureUV).xyz;

		Out.vColor = float4(vDiffuseColor, 1.f);
	}
	else
	{
		clip(-1);
	}


	return Out;
}

technique11 DefaultTechnique
{
	//0
	pass Default
	{
		SetRasterizerState(RS_NoCulling);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
		HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
		DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	//1
	pass Blend
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_ALPHABLEND();
		GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
		HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
		DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHABLEND();
	}

	//2
	pass SingleSprite
	{
		SetRasterizerState(RS_NoCulling);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
		HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
		DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
		PixelShader = compile ps_5_0 PS_EFFECT();
	}

	//3
	pass SSD_Decal
	{
		SetRasterizerState(RS_NoCulling);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
		HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
		DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
		PixelShader = compile ps_5_0 PS_DECAL();
	}

	//4
	pass Blood
	{
		SetRasterizerState(RS_NoCulling);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
	
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
		HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
		DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
		PixelShader = compile ps_5_0 PS_BLOOD();
	}

	//5
	pass Normal
	{
		SetRasterizerState(RS_NoCulling);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
		HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
		DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
		PixelShader = compile ps_5_0 PS_EFFECT_NORMAL();
	}

	//6
	pass Muzzle_Light
	{
		SetRasterizerState(RS_NoCulling);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
		HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
		DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
		PixelShader = compile ps_5_0 PS_MUZZLE_LIGHT();
	}

	//7
	pass Normal_AlphaBlend
	{
		SetRasterizerState(RS_NoCulling);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
	
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
		HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
		DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
		PixelShader = compile ps_5_0 PS_NORMAL();
	}
}