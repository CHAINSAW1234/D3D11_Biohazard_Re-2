#include "Engine_Shader_Defines.hlsli"


/* 전역변수 : 쉐이더 외부에 있는 데이터를 쉐이더 안으로 받아온다. */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_PrevWorldMatrix, g_PrevViewMatrix, g_PrevProjMatrix;

matrix g_BoneMatrices[512];

bool g_isAlphaTexture;
bool g_isAOTexture;
bool g_isEmissiveTexture;

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_AlphaTexture;
Texture2D g_AOTexture;
Texture2D g_EmissiveTexture;
Texture2D g_NoiseTexture;
Texture2D g_DissolveDiffuseTexture;
Texture2D g_DecalTexture;

/* For.Dissolve */
float g_fDissolveRatio = { 0.f };
float g_fDissolveRange = { 0.f };
vector g_vDissolveColor = { 1.f, 1.f, 1.f, 1.f };

/* For.Emissive */
float4 g_vEmissiveLightColor = { 0.f, 0.f, 0.f, 0.f };

// HyeonJin 추가
int g_LightIndex;

matrix g_LightViewMatrix[6];
matrix g_LightProjMatrix;

StructuredBuffer<float2> g_DecalMap;
bool		g_DecalRender;
bool		g_Cloth;
bool		g_Hair;
bool		g_bPlayer;

float4 g_vCamPosition;
float4 g_vLightDiffuse;
float4 g_vLightDir;

TextureCubeArray g_PrevIrradianceTexture;
TextureCubeArray g_CurIrradianceTexture;
TextureCubeArray g_PrevEnvironmentTexture;
TextureCubeArray g_CurEnvironmentTexture;
Texture2D g_SpecularLUTTexture;
float g_PBRLerpTime;
float4 g_Color;

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;
	float3 vTangent : TANGENT;

	uint4 vBlendIndices : BLENDINDEX;
	float4 vBlendWeights : BLENDWEIGHT;
	uint   iIndex : INDEX;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float3 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;

	float3 vTangent : TANGENT;
	float3 vBinormal : BINORMAL;
	uint   iIndex : COLOR0;
	float2 vDecalUV : TEXCOORD4;
};

struct VS_OUT_CUBE
{
	float4 vPosition : TEXCOORD0;
	float2 vTexcoord : TEXCOORD1;
};

/* 정점 쉐이더 */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	//  float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);
	float fTotalWeight = In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z + In.vBlendWeights.w;
	//float fTotalWeight = 1.f;

	matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x / fTotalWeight +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y / fTotalWeight +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z / fTotalWeight +
		g_BoneMatrices[In.vBlendIndices.w] * In.vBlendWeights.w / fTotalWeight;

	vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
	vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix)).xyz;
	Out.vTexcoord = In.vTexcoord;
	Out.vWorldPos = mul(vPosition, g_WorldMatrix);
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
	Out.vBinormal = normalize(cross(Out.vNormal, Out.vTangent));
	Out.iIndex = In.iIndex;
	Out.vDecalUV = g_DecalMap[Out.iIndex];

	return Out;
}

/* 정점 쉐이더 */
VS_OUT VS_LIGHT(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	//  float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);
	  float fTotalWeight = In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z + In.vBlendWeights.w;
	//float fTotalWeight = 1.f;

	matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x / fTotalWeight +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y / fTotalWeight +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z / fTotalWeight +
		g_BoneMatrices[In.vBlendIndices.w] * In.vBlendWeights.w / fTotalWeight;

	vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
	vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vTexcoord = In.vTexcoord;

	return Out;
}

VS_OUT_CUBE VS_CUBE(VS_IN In)
{
	VS_OUT_CUBE Out = (VS_OUT_CUBE)0;

    float fTotalWeight = In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z + In.vBlendWeights.w;
	
    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x / fTotalWeight +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y / fTotalWeight +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z / fTotalWeight +
		g_BoneMatrices[In.vBlendIndices.w] * In.vBlendWeights.w / fTotalWeight;

	vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix);
	Out.vTexcoord = In.vTexcoord;

	return Out;
}

struct GS_IN
{
	float4 vPosition : TEXCOORD0;
	float2 vTexcoord : TEXCOORD1;
};

struct GS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;

	uint    RTIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
void GS_MAIN(triangle GS_IN In[3], inout TriangleStream<GS_OUT> Output)
{
	GS_OUT Out = (GS_OUT)0;

	for (int i = 0; i < 6; ++i)
	{
		Out.RTIndex = i + 6 * g_LightIndex;
		matrix LightViewProjMatrix = mul(g_LightViewMatrix[i], g_LightProjMatrix);
		for (uint j = 0; j < 3; j++)
		{
			Out.vPosition = mul(In[j].vPosition, LightViewProjMatrix);
			Out.vTexcoord = In[j].vTexcoord;
			Out.vProjPos = Out.vPosition;
			Output.Append(Out);
		}
		Output.RestartStrip();
	}

}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float3 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;

	float3 vTangent : TANGENT;
	float3 vBinormal : BINORMAL;
	uint   iIndex : COLOR0;
	float2 vDecalUV : TEXCOORD4;
};

struct PS_IN_CUBE
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;

	uint RTIndex : SV_RenderTargetArrayIndex;
};

struct PS_OUT
{
	float4 vDiffuse : SV_TARGET0;
	float4 vNormal : SV_TARGET1;
	float4 vDepth : SV_TARGET2;
	float4 vMaterial : SV_TARGET3;
	float4 vEmissive : SV_TARGET4;
};

struct PS_OUT_EMISSIVE
{
	float4 vEmissive : SV_TARGET0;
};

struct PS_OUT_COLOR
{
    float4 vColor : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

	float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

	float3 vWorldNormal = mul(vNormal, WorldMatrix);

	Out.vDiffuse = vMtrlDiffuse;
	Out.vNormal = vector(vWorldNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);

	if (g_isAlphaTexture)
	{
		vector vAlphaDesc = g_AlphaTexture.Sample(LinearSampler, In.vTexcoord);
		Out.vDiffuse.a = vAlphaDesc.r;
		if (0.1f >= Out.vDiffuse.a)
			discard;
	}
	else
	{
		Out.vDiffuse.a = 1.f;
	}

	Out.vMaterial.r = vMtrlDiffuse.a;
	Out.vMaterial.g = vNormalDesc.a;

	if (g_isAOTexture)
	{
		vector vAODesc = g_AOTexture.Sample(LinearSampler, In.vTexcoord);
		Out.vMaterial.b = vAODesc.b;
	}
	else
	{
		Out.vMaterial.b = 1.f;
	}

    if (g_isEmissiveTexture)
    {
        vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
        if (vEmissive.r != 0 || vEmissive.g != 0 || vEmissive.b != 0)
        {
            Out.vEmissive = vEmissive;
        }

    }
	
	
    if (g_DecalRender)
    {
        float2 DecalTexcoord;
        DecalTexcoord.x = g_DecalMap[In.iIndex].x;
        DecalTexcoord.y = g_DecalMap[In.iIndex].y;

        float2 DecalUV = In.vDecalUV;

        if (DecalUV.x >= 0.0f && DecalUV.x <= 1.0f && DecalUV.y >= 0.0f && DecalUV.y <= 1.0f)
        {
            float4 decalColor = g_DecalTexture.Sample(LinearSampler, DecalUV);

            if (decalColor.a > 0.01f)
            {
                float2 center = float2(0.5f, 0.5f);
                float distance = length(DecalUV - center);

				if(g_Cloth)
				{
					if (distance < 0.1f)
					{
						if(g_bPlayer == false)
							discard;
						else
						{
							decalColor = float4(0.3f, 0.0f, 0.0f, 1.f);
							Out.vDiffuse = decalColor;
						}
					}
					else
					{
						if (decalColor.a > 0.01f)
						{
							decalColor = float4(0.3f, 0.0f, 0.0f, 1.f);
							Out.vDiffuse = decalColor;
						}
					}
				}
				else
				{
					if (distance < 0.5f)
					{
						if(g_Hair == false)
							discard;
					}
					else
					{
						if (decalColor.a > 0.01f)
						{
							decalColor = float4(0.3f, 0.0f, 0.0f, 1.f);
							Out.vDiffuse = decalColor;
						}
					}
				}
			}
		}
	}

	return Out;
}

PS_OUT PS_DISSOLVE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	vector vNoiseDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
	vector vDissolveDiffuseDesc = g_DissolveDiffuseTexture.Sample(LinearSampler, In.vTexcoord);

	if (0.3f >= vMtrlDiffuse.a)
		discard;

	float fInvRatio = 1.f - g_fDissolveRatio;
	if (fInvRatio + g_fDissolveRange * 0.5f < vNoiseDesc.x)
		discard;
	else if (fInvRatio - g_fDissolveRange * 0.5f <= vNoiseDesc.x)
	{
		float fBlendRatio = (fInvRatio - g_fDissolveRange * 0.5f) / g_fDissolveRange;

		//  Out.vDiffuse = vDissolveDiffuseDesc;
		Out.vDiffuse = vMtrlDiffuse + (vDissolveDiffuseDesc - vMtrlDiffuse) * fBlendRatio;
	}
	else
	{
		Out.vDiffuse = vMtrlDiffuse;
	}

	Out.vNormal = vector((In.vNormal * 0.5f + 0.5f).xyz, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000, 0.0f, 0.0f);
	Out.vMaterial = 1;

	return Out;
}

PS_OUT_EMISSIVE PS_EMISSIVE(PS_IN In)
{
	PS_OUT_EMISSIVE Out = (PS_OUT_EMISSIVE)0;

	Out.vEmissive = g_vEmissiveLightColor;

	return Out;
}

struct PS_OUT_LIGHTDEPTH
{
	float4 vLightDepth : SV_TARGET0;
};

PS_OUT_LIGHTDEPTH PS_MAIN_LIGHTDEPTH(PS_IN In)
{
	PS_OUT_LIGHTDEPTH Out = (PS_OUT_LIGHTDEPTH)0;

	Out.vLightDepth = float4(In.vPosition.w / 1000.f, 0.f, 0.f, 0.f);

	return Out;
}


PS_OUT_LIGHTDEPTH PS_LIGHTDEPTH_CUBE(PS_IN_CUBE In)
{
	PS_OUT_LIGHTDEPTH Out = (PS_OUT_LIGHTDEPTH)0;

	Out.vLightDepth = float4(In.vProjPos.w / 1000.f, 0.f, 0.f, 0.f);

	return Out;
}


uint querySpecularTextureLevels()
{
    uint width, height, elements, levels;
    g_CurEnvironmentTexture.GetDimensions(0, width, height, elements, levels);
    return levels;
}

PS_OUT_COLOR PS_FORWARD_LIGHTING(PS_IN In)
{
    PS_OUT_COLOR Out = (PS_OUT_COLOR) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
    
    vector vNormal = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    
    float fMaterialRoughness = saturate(vNormal.a + 0.2f);
    
    vNormal = vector(mul(vNormal.xyz, WorldMatrix), 0.f);

    vector vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);

    float fMaterialMetalic = vDiffuse.r;
    float fMaterialAO = 1.f;
    
    if (g_isAlphaTexture)
    {
        vector vAlphaDesc = g_AlphaTexture.Sample(LinearSampler, In.vTexcoord);
        vDiffuse.a = vAlphaDesc.r;
        if (0.01f >= vDiffuse.a)
            discard;
    }
    else
    {
        vDiffuse.a = 1.f;
    }
    
    if (g_isAOTexture)
    {
        vector vAODesc = g_AOTexture.Sample(LinearSampler, In.vTexcoord);
        fMaterialAO = vAODesc.r;
    }

    if (g_isEmissiveTexture)
    {
        vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
        if (vEmissive.r != 0 || vEmissive.g != 0 || vEmissive.b != 0)
        {
            Out.vColor = vEmissive;
            return Out;
        }
    }
       
    // PBR
    
    vector vAlbedo = pow(vDiffuse, 2.2);
    
    float3 Lo = normalize(g_vCamPosition.xyz - In.vWorldPos.xyz);
    float cosLo = max(0.0f, dot(vNormal.xyz, Lo));
    float3 Lradiance = g_vLightDiffuse.xyz;
    
    float3 Lr = 2.0f * cosLo * vNormal.xyz - Lo;
    float3 F0 = lerp(0.04, vAlbedo, fMaterialMetalic).xyz;
    
    // 여기부터 for문이였음
   // float3 Li = -g_vLightDir.xyz;
    float3 Li = -g_vLightDir.xyz;
    float3 Lh = normalize(Li + Lo);
    
    float cosLi = max(0.0, dot(vNormal.xyz, Li));
    float cosLh = max(0.0, dot(vNormal.xyz, Lh));
    
    float3 F = FresnelSchlick(max(dot(Lh.xyz, Lo.xyz), 0.f), F0);
    float D = DistributeGGX(vNormal.xyz, Lh, fMaterialRoughness);
    float G = GeometrySmith(vNormal.xyz, Lo, Li, fMaterialRoughness);
    float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), fMaterialMetalic);
    
    float3 diffuseBRDF = kd * vAlbedo.xyz;
    float3 specularBRDF = (F * D * G) / max(0.00001f, 4.0 * cosLi * cosLo);
  
    float3 directLighting = (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
    
    float3 ambientLighting;
    {
        // Sample diffuse irradiance at normal direction.
        float3 Previrradiance = pow(g_PrevIrradianceTexture.Sample(LinearSampler, vNormal).rgb, 2.2f);
        float3 Curirradiance = pow(g_CurIrradianceTexture.Sample(LinearSampler, vNormal).rgb, 2.2f);

        float3 irradiance = lerp(Curirradiance, Previrradiance, g_PBRLerpTime); // ???????????????????????????
        
        // Calculate Fresnel term for ambient lighting.
        float3 F = FresnelSchlick(cosLo, F0);
        // Get diffuse contribution factor (as with direct lighting).
        float3 kd = lerp(1.0 - F, 0.0, fMaterialMetalic);

        // Irradiance map contains exitant radiance assuming Lambertian BRDF, no need to scale by 1/PI here either.
        float3 diffuseIBL = kd * vAlbedo.xyz * irradiance;

        // Sample pre-filtered specular reflection environment at correct mipmap level.
        uint specularTextureLevels = querySpecularTextureLevels();
        float3 PrevspecularIrradiance = pow(g_PrevEnvironmentTexture.SampleLevel(LinearSampler, float4(Lr, 0), fMaterialRoughness * specularTextureLevels), 2.2f).rgb;
        float3 CurspecularIrradiance = pow(g_CurEnvironmentTexture.SampleLevel(LinearSampler, float4(Lr, 0), fMaterialRoughness * specularTextureLevels), 2.2f).rgb;
        
        float3 specularIrradiance = lerp(PrevspecularIrradiance, CurspecularIrradiance, g_PBRLerpTime); // ???????????????????????????
        
        //// Split-sum approximation factors for Cook-Torrance specular BRDF.
        float2 specularBRDF = g_SpecularLUTTexture.Sample(LinearSamplerClamp, float2(cosLo, fMaterialRoughness)).rg;

        // Total specular IBL contribution.
        float3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;

        // Total ambient lighting contribution.
        ambientLighting = (diffuseIBL + specularIBL);
    }
    
    vector vColor = float4(directLighting + ambientLighting, 1);
    
    vColor = vColor / (vColor + 1);
    vColor = pow(vColor, 1 / 2.2f);
    
    Out.vColor = vColor * g_Color;

    return Out;
}

technique11 DefaultTechnique
{
	pass Default	// 0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
		HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
		DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass AlphaBlend // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }


	pass Dissolve		// 1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
		HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
		DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
		PixelShader = compile ps_5_0 PS_DISSOLVE();
	}

	pass LightDepth		// 2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_LIGHT();
		GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
		HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
		DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHTDEPTH();
	}

	pass Emissive		// 3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
		HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
		DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
		PixelShader = compile ps_5_0 PS_EMISSIVE();
	}

	pass LightDepth_Cube	// 4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CUBE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
		DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
		PixelShader = compile ps_5_0 PS_LIGHTDEPTH_CUBE();
	}

	//5
	pass NonCull
	{
		SetRasterizerState(RS_2D);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
		HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
		DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass PS_EXAMINE
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_FORWARD_LIGHTING();
    }

}