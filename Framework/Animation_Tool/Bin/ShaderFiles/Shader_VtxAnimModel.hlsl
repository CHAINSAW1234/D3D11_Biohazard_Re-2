#include "Engine_Shader_Defines.hlsli"


/* �������� : ���̴� �ܺο� �ִ� �����͸� ���̴� ������ �޾ƿ´�. */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_PrevWorldMatrix, g_PrevViewMatrix, g_PrevProjMatrix;

matrix g_BoneMatrices[256];
matrix g_PrevBoneMatrices[256];

texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_ATOSTexture;
texture2D g_NoiseTexture;
texture2D g_DissolveDiffuseTexture;

/* For.Dissolve */
float g_fDissolveRatio = { 0.f };
float g_fDissolveRange = { 0.f };
vector g_vDissolveColor = { 1.f, 1.f, 1.f, 1.f };

/* For.Emissive */
float4 g_vEmissiveLightColor = { 0.f, 0.f, 0.f, 0.f };

// HyeonJin �߰�
int g_LightIndex;

matrix g_LightViewMatrix[6];
matrix g_LightProjMatrix;

bool g_isMotionBlur;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;

    uint4 vBlendIndices : BLENDINDEX;
    float4 vBlendWeights : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vVelocity : TEXCOORD3;
    
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;

};

struct VS_OUT_CUBE
{
    float4 vPosition : TEXCOORD0;
    float2 vTexcoord : TEXCOORD1;
};

/* ���� ���̴� */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    //  float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);
    //  float fTotalWeight = In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z + In.vBlendWeights.w;
    float fTotalWeight = 1.f;
    
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
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vBinormal = normalize(cross(Out.vNormal, Out.vTangent));

    if (g_isMotionBlur)
    {
        matrix PrevBoneMatrix = g_PrevBoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x / fTotalWeight +
		g_PrevBoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y / fTotalWeight +
		g_PrevBoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z / fTotalWeight +
		g_PrevBoneMatrices[In.vBlendIndices.w] * In.vBlendWeights.w / fTotalWeight;

        matrix matPrevWV, matPrevWVP;

        matPrevWV = mul(g_PrevWorldMatrix, g_PrevViewMatrix);
        matPrevWVP = mul(matPrevWV, g_PrevProjMatrix);
    
    // Velocity ���
        float4 vNewPosition = Out.vPosition;
        vector vOldPosition = mul(vector(In.vPosition, 1.f), PrevBoneMatrix);
        vOldPosition = mul(vOldPosition, matPrevWVP);
    
        float3 vDir = vNewPosition.xyz - vOldPosition.xyz;
    
        float a = dot(normalize(vDir), normalize(Out.vNormal));
        if (a < 0.f)
            Out.vPosition = vOldPosition;
        else
        {
            Out.vPosition = vNewPosition;
            Out.vNormal = normalize(mul(mul(vector(In.vNormal, 0.f), PrevBoneMatrix), g_PrevWorldMatrix));
            Out.vWorldPos = mul(vOldPosition, g_PrevWorldMatrix);
            Out.vProjPos = Out.vPosition;
            Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_PrevWorldMatrix)).xyz;
            Out.vBinormal = normalize(cross(Out.vNormal, Out.vTangent));
        }

    
        float2 vVelocity = (vNewPosition.xy / vNewPosition.w) - (vOldPosition.xy / vOldPosition.w);
        Out.vVelocity.xy = vVelocity * 0.5f;
        Out.vVelocity.z = Out.vPosition.z;
        Out.vVelocity.w = Out.vPosition.w;
    }
        
    
    return Out;
}

/* ���� ���̴� */
VS_OUT VS_LIGHT(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    //  float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);
    //  float fTotalWeight = In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z + In.vBlendWeights.w;
    float fTotalWeight = 1.f;
    
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
    VS_OUT_CUBE Out = (VS_OUT_CUBE) 0;

    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

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
    
    uint    RTIndex  : SV_RenderTargetArrayIndex; 
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
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vVelocity : TEXCOORD3;
    
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
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
    float4 vVelocity : SV_TARGET4;
    float4 vOrigin : SV_TARGET5;
};

struct PS_OUT_EMISSIVE
{
    float4 vEmissive : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

    float3 vWorldNormal = mul(vNormal, WorldMatrix);

  
    Out.vDiffuse.xyz = vMtrlDiffuse.xyz;
    //  Out.vDiffuse.xyz = pow(vMtrlDiffuse.xyz, 2.2f);
    Out.vDiffuse.a = 1.f;
    Out.vNormal = vector(vWorldNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000, 0.0f, 0.0f);
    Out.vMaterial.r = vMtrlDiffuse.a;
    Out.vMaterial.g = vNormalDesc.a;
    Out.vOrigin = vector(1.f, 0.f, 0.f, 1.f);
    Out.vVelocity = vector(In.vVelocity.xy, 1, In.vVelocity.z / In.vVelocity.w);
    
    return Out;
}

PS_OUT PS_MAIN_ATOS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vATOSDesc = g_ATOSTexture.Sample(LinearSampler, In.vTexcoord);
    
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

    float3 vWorldNormal = mul(vNormal, WorldMatrix);

    Out.vDiffuse = vMtrlDiffuse;
    Out.vDiffuse.a = vATOSDesc.r;
    Out.vNormal = vector(vWorldNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000, 0.0f, 0.0f);
    Out.vMaterial.r = vMtrlDiffuse.a;
    Out.vMaterial.g = vNormalDesc.a;
    Out.vOrigin = vector(1.f, 0.f, 0.f, 1.f);
    Out.vVelocity = vector(In.vVelocity.xy, 1, In.vVelocity.z / In.vVelocity.w);
    if (0.01f >= Out.vDiffuse.a)
        discard;
	
    return Out;
}

PS_OUT PS_DISSOLVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
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
    PS_OUT_EMISSIVE Out = (PS_OUT_EMISSIVE) 0;
        
    Out.vEmissive = g_vEmissiveLightColor;
    
    return Out;
}

struct PS_OUT_LIGHTDEPTH
{
    float4 vLightDepth : SV_TARGET0;
};

PS_OUT_LIGHTDEPTH PS_MAIN_LIGHTDEPTH(PS_IN In)
{
    PS_OUT_LIGHTDEPTH Out = (PS_OUT_LIGHTDEPTH) 0;

    Out.vLightDepth = float4(In.vPosition.w / 1000.f, 0.f, 0.f, 0.f);

    return Out;
}


PS_OUT_LIGHTDEPTH PS_LIGHTDEPTH_CUBE(PS_IN_CUBE In)
{
    PS_OUT_LIGHTDEPTH Out = (PS_OUT_LIGHTDEPTH) 0;
    
    Out.vLightDepth = float4(In.vProjPos.w / 1000.f, 0.f, 0.f, 0.f);
    
    return Out;
}

technique11 DefaultTechnique
{
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

    pass Default_Atos
    {
        SetRasterizerState(RS_Default);
        //  SetRasterizerState(RS_Wireframe);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        //  SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ATOS();
    }

    pass Dissolve
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

    pass LightDepth
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

    pass Emissive
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

}