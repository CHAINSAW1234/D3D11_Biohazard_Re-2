#include "Engine_Shader_Defines.hlsli"

/* 전역변수 : 쉐이더 외부에 있는 데이터를 쉐이더 안으로 받아온다. */
matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D	g_DiffuseTexture;
Texture2D	g_NormalTexture;
Texture2D   g_AlphaTexture;
Texture2D   g_AOTexture;
Texture2D   g_EmissiveTexture;

bool g_isAlphaTexture;
bool g_isAOTexture;
bool g_isEmissiveTexture;

bool g_DecalRender;

// HyeonJin 추가
int g_LightIndex;

matrix g_LightViewMatrix[6];
matrix g_LightProjMatrix;
float g_fShadowFar;

StructuredBuffer<float2>    g_DecalMap;
Texture2D                   g_DecalTexture;
Texture2D                   g_Texture_Dissolve;
float                       g_fDissolveAmount;
bool                        g_bDissolve;

float4      g_vCamPosition;
float4      g_vLightDiffuse;
float4      g_vLightDir;
TextureCubeArray    g_PrevIrradianceTexture;
TextureCubeArray    g_CurIrradianceTexture;
TextureCubeArray    g_PrevEnvironmentTexture;
TextureCubeArray    g_CurEnvironmentTexture;
Texture2D           g_SpecularLUTTexture;
float               g_PBRLerpTime;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float3		vTangent : TANGENT;

    uint        iIndex : INDEX;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;

	float3		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;

    uint   iIndex : COLOR0;
    float2 vDecalUV : TEXCOORD3;
};

struct VS_OUT_CUBE
{
    float4 vPosition : TEXCOORD0;
    float2 vTexcoord : TEXCOORD1;
};


/* 정점 쉐이더 */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix)).xyz;
	Out.vTexcoord = In.vTexcoord;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vBinormal = normalize(cross(Out.vNormal, Out.vTangent));

	return Out;
}


VS_OUT_CUBE VS_CUBE(VS_IN In)
{
    VS_OUT_CUBE Out = (VS_OUT_CUBE) 0;

    vector vPosition = vector(In.vPosition, 1);

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
    
    uint RTIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
void GS_MAIN(triangle GS_IN In[3], inout TriangleStream<GS_OUT> Output)
{
    GS_OUT Out = (GS_OUT) 0;
    
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
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;

	float3		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;

    uint   iIndex : COLOR0;
    float2 vDecalUV : TEXCOORD3;
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

struct PS_OUT_LIGHTDEPTH
{
    float4 vLightDepth : SV_TARGET0;
};

struct PS_OUT_COLOR
{
    float4 vColor : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

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
        if (0.01f >= Out.vDiffuse.a)
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
        Out.vMaterial.b = vAODesc.r;
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
    
    return Out;
}

PS_OUT PS_BLOOD(PS_IN In)
{
    PS_OUT			Out = (PS_OUT)0;

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
        if (0.3 >= Out.vDiffuse.a)
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
        Out.vMaterial.b = vAODesc.r;
    }
    else
    {
        Out.vMaterial.b = 1.f;
    }

    if (g_isEmissiveTexture)
    {
        vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
        Out.vEmissive = vEmissive;
    }
    
    //Out.vDiffuse.r -= 0.3f;
    //Out.vDiffuse.g -= 0.3f;
    //Out.vDiffuse.b -= 0.3f;

    if (g_bDissolve)
    {
        vector vMtrlDissolve = g_Texture_Dissolve.Sample(LinearSampler, In.vTexcoord);

        float dissolve = vMtrlDissolve.r;
        dissolve *= 0.999;
        float isVisible = dissolve - g_fDissolveAmount;
        clip(isVisible);
        isVisible = saturate(isVisible);
        Out.vDiffuse.a *= isVisible;
    }

    return Out;
}

PS_OUT_LIGHTDEPTH PS_MAIN_LIGHTDEPTH(PS_IN In)
{
    PS_OUT_LIGHTDEPTH Out = (PS_OUT_LIGHTDEPTH) 0;

    Out.vLightDepth = float4(In.vPosition.w/1000, 0.f, 0.f, 0.f);

    return Out;
}

PS_OUT_LIGHTDEPTH PS_LIGHTDEPTH_CUBE(PS_IN_CUBE In)
{
    PS_OUT_LIGHTDEPTH Out = (PS_OUT_LIGHTDEPTH) 0;
    
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
    float3 Li = g_vLightDir.xyz;
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
    
    Out.vColor = vColor;

    return Out;
}

technique11 DefaultTechnique
{
    //0
	pass Default
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

    pass AlphaBlend
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

    //1
    pass LightDepth
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHTDEPTH();
    }

    //2
    pass LightDepth_Cube
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

    //3
    pass WireFrame
    {
        SetRasterizerState(RS_Wireframe);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    //4
    pass Blood
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_BLOOD();
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