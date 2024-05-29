#include "Engine_Shader_Defines.hlsli"

/* Àü¿ªº¯¼ö : ½¦ÀÌ´õ ¿ÜºÎ¿¡ ÀÖ´Â µ¥ÀÌÅÍ¸¦ ½¦ÀÌ´õ ¾ÈÀ¸·Î ¹Þ¾Æ¿Â´Ù. */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
//matrix g_LightViewMatrix, g_LightProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

texture2D g_Texture;
TextureCubeArray g_CubeTexture;
Texture3D g_3DTexture;

texture2D g_NormalTexture;
texture2D g_DiffuseTexture;
texture2D g_ShadeTexture;
texture2D g_DepthTexture;
texture2D g_MaterialTexture;
texture2D g_OriginalTexture;

texture2D g_SpecularTexture;
texture2D g_LightResultTexture;

texture2D g_AmbientDiffuseTexture;
texture2D g_AmbientShadeTexture;

texture2D g_DistortionTexture;
texture2D g_EmissiveTexture;

texture2D g_AdditionalLightTexture;

texture2D g_PostprocessingDiffuseTexture;
texture2D g_PostprocessingShadeTexture;

Texture3D d;

bool g_isRadialBlurActive = { false };
float2 g_vRadialBlurUV;
float g_fRadialAmount;

//¿¹Àº Ãß°¡
float		g_fCutOff;
float		g_fOutCutOff;
//

// ÇöÁø Ãß°¡
bool g_isShadowDirLight;
texture2D g_DirLightDepthTexture;
matrix g_DirLightViewMatrix;
matrix g_DirLightProjMatrix;
float4 g_vDirLightDirection;

TextureCubeArray g_PointLightDepthTexture;
int g_iNumShadowPointLight;
float4 g_vShadowPointLightPos[2];
float4 g_fShadowPointLightRange[2];

matrix g_ShadowLightViewMatrix[2][6];
matrix g_ShadowLightProjMatrix[2];

bool g_isShadowSpotLight;
texture2D g_SpotLightDepthTexture;
matrix g_SpotLightViewMatrix;
matrix g_SpotLightProjMatrix;
float4 g_vSpotLightPosition;
float4 g_vSpotLightDirection;
float g_fSpotLightCutOff;
float g_fSpotLightOutCutOff;
float g_fSpotLightRange;

//matrix g_LightViewMatrix[6];
////matrix g_LightViewMatrix[2][6];
////matrix g_LightProjMatrix[2];
//matrix g_LightProjMatrix;


//

float4 g_vLightDir;
float4 g_vLightPos;
float g_fLightRange;

float4 g_vLightDiffuse;
float4 g_vLightAmbient;
float4 g_vLightSpecular;

float4 g_vWaterColor;

float4 g_vMtrlAmbient = float4(1.f, 1.f, 1.f, 1.f);
float4 g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);

float4 g_vCamPosition;

float g_fLightDepthFar;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

/* Á¤Á¡ ½¦ÀÌ´õ */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD;
};

struct PS_SHADOW_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 FragPos : FragPos;
    uint RTIndex : SV_RenderTargetArrayIndex;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

struct PS_OUT_EMISSIVE
{
    float4 vEmissive : SV_TARGET0;
};

struct PS_OUT_PRE_POST
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vMaterial : SV_TARGET3;
};

struct PS_OUT_POST_PROCESSING
{
    float4 vDiffuse : SV_TARGET0;
    float4 vShade : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

PS_OUT PS_MAIN_CUBE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float3 vTex = normalize(float3(1.0, In.vTexcoord.y * -2 + 1.f, In.vTexcoord.x * -2.f + 1.f));
    //float3 vTex = normalize(float3(In.vTexcoord.x * 2 - 1.f, In.vTexcoord.y * -2.f + 1.f, 1.f));
    //float3 vTex = normalize(float3(-1.0, In.vTexcoord.y * -2 + 1.f, In.vTexcoord.x * -2.f + 1.f));
    
    Out.vColor = g_CubeTexture.Sample(LinearSampler, float4(vTex, 0));

    return Out;
}

int g_iNumZ;
int g_iNumSizeZ;

PS_OUT PS_MAIN_3D(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    
<<<<<<< HEAD
    //Out.vColor = g_3DTexture.Load(int4(In.vTexcoord * 128, (float) g_iNumZ, 0));
    Out.vColor = g_3DTexture.Sample(PointSampler, float3(In.vTexcoord, (float) g_iNumZ / g_iNumSizeZ));
=======
    //Out.vColor = g_3DTexture.Load(int4(0,0,0, 0));
    Out.vColor = g_3DTexture.Sample(LinearSampler, float3(In.vTexcoord, In.vTexcoord.y));
    //Out.vColor = g_3DTexture.Load(int4(In.vTexcoord * 64, 6.4, 0));
>>>>>>> parent of 122df45 (ìž‘ì—…ì¤‘)
    return Out;
}

struct PS_OUT_LIGHT
{
    float4 vShade : SV_TARGET0;
    float4 vSpecular : SV_TARGET1;
};

/* ºû ÇÏ³ª´ç 480000 ¼öÇàµÇ´Â ½¦ÀÌ´õ. */

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.0f;
    
    vector vMaterialDesc = g_MaterialTexture.Sample(PointSampler, In.vTexcoord);
    float fMaterialMetalic = vMaterialDesc.x;
    float fMaterialRoughness = vMaterialDesc.y;
    float fMaterialSpecular = vMaterialDesc.z;
	
    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + g_vLightAmbient * g_vMtrlAmbient);

    float4 vWorldPos;

	/* ·ÎÄÃÀ§Ä¡ * ¿ùµåÇà·Ä * ºäÇà·Ä * Åõ¿µÇà·Ä / View.z */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* ·ÎÄÃÀ§Ä¡ * ¿ùµåÇà·Ä * ºäÇà·Ä * Åõ¿µÇà·Ä */
    vWorldPos *= fViewZ;

	/* ·ÎÄÃÀ§Ä¡ * ¿ùµåÇà·Ä * ºäÇà·Ä */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* ·ÎÄÃÀ§Ä¡ * ¿ùµåÇà·Ä */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    float4 vLook = vWorldPos - g_vCamPosition;
    float4 vReflect = reflect(normalize(g_vLightDir), vNormal);

    float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 3.f);

    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.0f;

    float4 vWorldPos;

	/* ·ÎÄÃÀ§Ä¡ * ¿ùµåÇà·Ä * ºäÇà·Ä * Åõ¿µÇà·Ä / View.z */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* ·ÎÄÃÀ§Ä¡ * ¿ùµåÇà·Ä * ºäÇà·Ä * Åõ¿µÇà·Ä */
    vWorldPos *= fViewZ;

	/* ·ÎÄÃÀ§Ä¡ * ¿ùµåÇà·Ä * ºäÇà·Ä */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* ·ÎÄÃÀ§Ä¡ * ¿ùµåÇà·Ä */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    vector vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);

    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);

    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + g_vLightAmbient * g_vMtrlAmbient);
    Out.vShade *= fAtt;
    
    float4 vLook = vWorldPos - g_vCamPosition;
    float4 vReflect = reflect(normalize(vLightDir), vNormal);

    float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 3.f);

    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
    Out.vSpecular *= fAtt;

    return Out;
}

PS_OUT_LIGHT PS_MAIN_SPOT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.0f;
    float4 vWorldPos;

	/* ·ÎÄÃÀ§Ä¡ * ¿ùµåÇà·Ä * ºäÇà·Ä * Åõ¿µÇà·Ä / View.z */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* ·ÎÄÃÀ§Ä¡ * ¿ùµåÇà·Ä * ºäÇà·Ä * Åõ¿µÇà·Ä */
    vWorldPos *= fViewZ;

	/* ·ÎÄÃÀ§Ä¡ * ¿ùµåÇà·Ä * ºäÇà·Ä */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* ·ÎÄÃÀ§Ä¡ * ¿ùµåÇà·Ä */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    vector vLightDir = vWorldPos - g_vLightPos;
	
    vector vSpotDir = g_vLightDir;
	
    float fResult = acos(dot(normalize(vLightDir), normalize(vSpotDir)));
	
 
    if (g_fOutCutOff >= fResult) // ºûÀÌ ¹øÁú ¹üÀ§ ¾È¿¡ ÀÖÀ» ¶§
    {
        float fDistance = length(vLightDir);

        float fIntensity = (fResult - g_fOutCutOff) / (g_fCutOff - g_fOutCutOff);
		
        float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange) * fIntensity; //¹üÀ§ Áà¼­ ³¡ ¹üÀ§¿¡¼­´Â ¿¬ÇØÁö°Ô 

        Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + g_vLightAmbient * g_vMtrlAmbient);
        Out.vShade *= fAtt;
	
        float4 vLook = vWorldPos - g_vCamPosition;
        float4 vReflect = reflect(normalize(vLightDir), vNormal);

        float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

        Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
        Out.vSpecular *= fAtt;

    }
		
    return Out;
}

float ShadowPCFSample_Dir(float fOriginDepth, float2 vTexcoord)
{
    float SampleRadius = 0.0005;
    float Shadow = 0.f;
    // PCF
    int Samples = 10; // »ùÇÃ ¼ö, ÇÊ¿ä¿¡ µû¶ó Á¶Á¤
    float SampleStep = SampleRadius / Samples; // »ùÇÃ °£°Ý

    for (int x = -Samples / 2; x <= Samples / 2; ++x)
    {
        for (int y = -Samples / 2; y <= Samples / 2; ++y)
        {
            float2 Offset = float2(x * SampleStep, y * SampleStep);
            float2 SampleTexcoord = vTexcoord + Offset;
            float fDepth = g_DirLightDepthTexture.Sample(PointSamplerClamp, SampleTexcoord).r;

            if (fOriginDepth - 1.f > (fDepth * 1000.f))
                Shadow += 1.f;
        }
    }

    Shadow /= (Samples * Samples); // »ùÇÃ ¼ö·Î ³ª´©¾î Æò±Õ °è»ê
    
    return Shadow;
}


float ShadowPCFSample_Spot(float fOriginDepth, float2 vTexcoord)
{
    float SampleRadius = 0.005;
    float Shadow = 0.f;
    // PCF
    int Samples = 10; // »ùÇÃ ¼ö, ÇÊ¿ä¿¡ µû¶ó Á¶Á¤
    float SampleStep = SampleRadius / Samples; // »ùÇÃ °£°Ý

    for (int x = -Samples / 2; x <= Samples / 2; ++x)
    {
        for (int y = -Samples / 2; y <= Samples / 2; ++y)
        {
            float2 Offset = float2(x * SampleStep, y * SampleStep);
            float2 SampleTexcoord = vTexcoord + Offset;
            float fDepth = g_SpotLightDepthTexture.Sample(PointSamplerClamp, SampleTexcoord).r;

            if (fOriginDepth -1.f > (fDepth * 1000.f))
                Shadow += 1.f;
        }
    }

    Shadow /= (Samples * Samples); // »ùÇÃ ¼ö·Î ³ª´©¾î Æò±Õ °è»ê
    
    return Shadow;
}

float ShadowPCFSample_Point(float fDistance, float3 fDirection, int iLightIndex)
{
    float SampleRadius = 0.01;
    float Shadow = 0.f;
    // PCF
    int Samples = 10; // »ùÇÃ ¼ö, ÇÊ¿ä¿¡ µû¶ó Á¶Á¤
    float SampleStep = SampleRadius / Samples; // »ùÇÃ °£°Ý

    for (int x = -Samples / 2; x <= Samples / 2; ++x)
    {
        for (int y = -Samples / 2; y <= Samples / 2; ++y)
        {
            float3 Offset = float3(x * SampleStep, y * SampleStep, 0.0);
            float3 SamplePos = normalize(fDirection + Offset);
            float fDepth = g_PointLightDepthTexture.Sample(PointSamplerClamp, float4(SamplePos, iLightIndex)).r;

            if (fDistance > fDepth * 1000)
            {
                Shadow += 1.0;
            }
        }
    }

    Shadow /= (Samples * Samples); // »ùÇÃ ¼ö·Î ³ª´©¾î Æò±Õ °è»ê
    
    return Shadow;
}

float Cal_Shadow(float2 vTexcoord)
{
    /* Shadow */
    /* ProjPos.w == View.Z */
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.0f;

    float4 vWorldPos;
	/* ·ÎÄÃÀ§Ä¡ * ¿ùµåÇà·Ä * ºäÇà·Ä * Åõ¿µÇà·Ä / View.z */
    vWorldPos.x = vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* ·ÎÄÃÀ§Ä¡ * ¿ùµåÇà·Ä * ºäÇà·Ä * Åõ¿µÇà·Ä */
    vWorldPos *= fViewZ;
	/* ·ÎÄÃÀ§Ä¡ * ¿ùµåÇà·Ä * ºäÇà·Ä */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
	/* ·ÎÄÃÀ§Ä¡ * ¿ùµåÇà·Ä */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    // ±¤¿ø ¿©·¯°³¿¡ ´ëÇØ¼­ Ã³¸®ÇØ¾ßÇÔ
    
        // 1. Point Lights 
    float fShadow = 0;
    
    // 1. DirLight
    if (g_isShadowDirLight)
    {
        vector vPosition = mul(vWorldPos, g_DirLightViewMatrix);
        vPosition = mul(vPosition, g_DirLightProjMatrix);
  
        float2 vTexcoord;
        vTexcoord.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
        vTexcoord.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;

        fShadow += ShadowPCFSample_Dir(vPosition.w, vTexcoord) * 0.5;
    }
    
    // 2. PointLight
    for (int i = 0; i < g_iNumShadowPointLight; ++i)
    {
        float3 vLightDir = vWorldPos - g_vShadowPointLightPos[i];
        float fDistance = length(vLightDir);
        vLightDir = normalize(vLightDir);
    
        float fAtt = saturate((g_fShadowPointLightRange[i].x - fDistance) / g_fShadowPointLightRange[i].x);
    
        fShadow += ShadowPCFSample_Point(fDistance, vLightDir, i) * fAtt;

    }
    
    // 2. SpotLight
    if (g_isShadowSpotLight)
    {
        vector vPosition = mul(vWorldPos, g_SpotLightViewMatrix);
        vPosition = mul(vPosition, g_SpotLightProjMatrix);
  
        float2 vTexcoord;
        vTexcoord.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
        vTexcoord.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;
    
        vector vLightDir = vWorldPos - g_vSpotLightPosition;
        vector vSpotDir = g_vSpotLightDirection;
	
        float fResult = acos(dot(normalize(vLightDir), normalize(vSpotDir)));

        if (g_fOutCutOff >= fResult) // ºûÀÌ ¹øÁú ¹üÀ§ ¾È¿¡ ÀÖÀ» ¶§
        {
            float fDistance = length(vLightDir);
            float fIntensity = (fResult - g_fSpotLightOutCutOff) / (g_fSpotLightCutOff - g_fSpotLightOutCutOff);
            float fAtt = saturate((g_fSpotLightRange - fDistance) / g_fSpotLightRange) * fIntensity; //¹üÀ§ Áà¼­ ³¡ ¹üÀ§¿¡¼­´Â ¿¬ÇØÁö°Ô 
        
            fShadow += ShadowPCFSample_Spot(vPosition.w, vTexcoord) * fAtt;
        }
    }
    
    return fShadow;
}

<<<<<<< HEAD
float ConvertDepthToNdcZ(float depth)
{
    float depthPackExponent = 2;
    float nearPlaneDist = 0.1;
    float farPlaneDist = 1000;

    return pow(saturate((depth - nearPlaneDist) / (farPlaneDist - nearPlaneDist)), 1 / depthPackExponent);
}

float3 HDR(float3 l)
{
    float Exposure = 1.f;
    l = l * Exposure;
    l.r = l.r < 1.413f ? pow(abs(l.r * 0.38317f), 1.f / 2.2f) : 1.f - exp(-l.r);
    l.g = l.g < 1.413f ? pow(abs(l.g * 0.38317f), 1.f / 2.2f) : 1.f - exp(-l.g);
    l.b = l.b < 1.413f ? pow(abs(l.b * 0.38317f), 1.f / 2.2f) : 1.f - exp(-l.b);
    return l;
}

=======
>>>>>>> parent of 122df45 (ìž‘ì—…ì¤‘)
/* ÃÖÁ¾ÀûÀ¸·Î 480000 ¼öÇàµÇ´Â ½¦ÀÌ´õ. */
PS_OUT_PRE_POST PS_MAIN_LIGHT_RESULT(PS_IN In)
{
    PS_OUT_PRE_POST Out = (PS_OUT_PRE_POST) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (0.0f == vDiffuse.a)
        discard;
    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vDepth = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormal = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vMaterial = g_MaterialTexture.Sample(PointSampler, In.vTexcoord);

    float fMetalness = vMaterial.r;
    
    vSpecular = lerp(0.04, vSpecular, fMetalness);
    
    Out.vDiffuse = vDiffuse * vShade + vSpecular;
    Out.vNormal = vNormal;
    Out.vDepth = vDepth;
    Out.vMaterial = vMaterial;

    // ±×¸²ÀÚ ¿¬»ê
    vector vOrigin = g_OriginalTexture.Sample(PointSampler, In.vTexcoord);
    if (vOrigin.r == 0)
    {
        float fShadow = Cal_Shadow(In.vTexcoord);
        Out.vDiffuse *= (1 - fShadow);
        Out.vDiffuse.a = 1;
    }

<<<<<<< HEAD
    

    float3 uv = float3(In.vTexcoord, vDepth.r * vDepth.g);

    float4 scatteringColorAndTransmittance = g_3DTexture.Sample(PointSampler, uv);
    float3 scatteringColor = HDR(scatteringColorAndTransmittance.rgb);

    Out.vDiffuse = Out.vDiffuse * float4(scatteringColor, scatteringColorAndTransmittance.a);
    return Out;
  
    
    
    
    
    
    
    
    

    float4 vVolumeLight = g_3DTexture.Sample(LinearSamplerClamp, float3(In.vTexcoord, 0));
    
    //Out.vDiffuse = float4(vVolumeLight, 1);
    Out.vDiffuse = Out.vDiffuse *  vVolumeLight;
    Out.vDiffuse.a = 1.f;
    
=======
>>>>>>> parent of 122df45 (ìž‘ì—…ì¤‘)
    return Out;
}

PS_OUT_EMISSIVE PS_MAIN_EMISSIVE(PS_IN In)
{
    PS_OUT_EMISSIVE Out = (PS_OUT_EMISSIVE) 0;
    
    vector MaterialDesc = g_MaterialTexture.Sample(PointSampler, In.vTexcoord);
    vector DiffuseDesc = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    
    Out.vEmissive = DiffuseDesc * MaterialDesc.a;
    
    return Out;
}

PS_OUT_POST_PROCESSING PS_MAIN_POSTPROCESSING(PS_IN In)
{
    PS_OUT_POST_PROCESSING Out = (PS_OUT_POST_PROCESSING) 0;
    
    vector vAmbientDiffuseDesc = g_AmbientDiffuseTexture.Sample(PointSampler, In.vTexcoord);
    vector vAmbientShadeDesc = g_AmbientShadeTexture.Sample(PointSampler, In.vTexcoord);
    
    Out.vDiffuse = vAmbientDiffuseDesc;
    Out.vShade = vAmbientShadeDesc;
    
    return Out;
}

PS_OUT PS_POST_PROCESSING_RESULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector DistortionDesc = g_DistortionTexture.Sample(PointSampler, In.vTexcoord);
    DistortionDesc.xyz *= 2.f;
    DistortionDesc.xyz -= 1.f;
    
    DistortionDesc.xy = normalize(DistortionDesc.xy);
    float2 vTexcoord = float2(In.vTexcoord.x + (DistortionDesc.x * DistortionDesc.w), In.vTexcoord.y + (DistortionDesc.y * DistortionDesc.w));

    vector DepthDescSrc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector DepthDescDst = g_DepthTexture.Sample(PointSampler, vTexcoord);
    
    //  ÇöÀç ±íÀÌ°¡ ¿øº»À§Ä¡¿¡¼­ÀÇ ±íÀÌ°¡ µð½ºÅä¼ÇÀÌÀû¿ëµÈ ±íÀÌº¸´Ù ¸Ö´Ù¸é
    if (DepthDescSrc.y >= DepthDescDst.y)
        vTexcoord = In.vTexcoord;
    
    vector AdditionalLightDesc = g_AdditionalLightTexture.Sample(PointSampler, In.vTexcoord);
    vector DiffuseDesc = g_DiffuseTexture.Sample(PointSamplerClamp, vTexcoord);
    vector PostprocessingDiffuseDesc = g_PostprocessingDiffuseTexture.Sample(PointSamplerClamp, vTexcoord);
    vector PostprocessingShadeDesc = g_PostprocessingShadeTexture.Sample(PointSamplerClamp, vTexcoord);
    
    float fSrcAlpha = PostprocessingDiffuseDesc.a;
    float fDstAlpha = 1.f - fSrcAlpha;
   
   
    //Out.vColor = DiffuseDesc * fDstAlpha + PostprocessingDiffuseDesc * fSrcAlpha;
    //Out.vColor *= PostprocessingShadeDesc;
    Out.vColor = DiffuseDesc;
    
    Out.vColor += AdditionalLightDesc;
    
    //  Out.vColor.xyz = pow(Out.vColor.xyz, 1.f / 2.2f);
    
    return Out;
}

PS_OUT PS_RADIALBLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (true == g_isRadialBlurActive)
    {
        float fWidth, fHeight;
        g_DiffuseTexture.GetDimensions(fWidth, fHeight);
        float2 vTexOffset = 1.0 / float2(fWidth, fHeight); // ÅØ½ºÃ³ °¢ ¿ä¼Ò¿¡ ´ëÇÑ ¿ÀÇÁ¼Â °è»ê 
    
        float4 vColor = float4(0.f, 0.f, 0.f, 0.f);
        int iSampleCnt = 10;
        float2 vStep = (In.vTexcoord - g_vRadialBlurUV) * vTexOffset * g_fRadialAmount;

        for (int i = -iSampleCnt / 2; i <= iSampleCnt / 2; i++)
        {
            float2 vSamplePos = In.vTexcoord + vStep * i;
            vColor += g_DiffuseTexture.Sample(PointSamplerClamp, vSamplePos);
        }

        vColor /= iSampleCnt + 1;
        Out.vColor = vColor;
    }
    else
    {
        vector DiffuseDesc = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
        Out.vColor = DiffuseDesc;
        
        if (DiffuseDesc.a == 0)
            discard;

    }
    
    return Out;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

float Compute_Gaussian(float fX, float fSigma)
{
    float fResult = exp(-((fX * fX) / (2.0 * fSigma * fSigma))) / (fSigma * sqrt(2.0 * 3.14159));
    
    return fResult;
}

// °¡¿ì½Ã¾È ºí·¯¸¦ Àû¿ëÇÒ ÇÈ¼¿ ¼ÎÀÌ´õ ÇÔ¼ö
PS_OUT PS_MAIN_BLOOM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float fWidth, fHeight;
    g_Texture.GetDimensions(fWidth, fHeight);
    
    float4 vResult = float4(0, 0, 0, 0);
    float2 vTexOffset = 1.0 / float2(fWidth, fHeight); // ÅØ½ºÃ³ °¢ ¿ä¼Ò¿¡ ´ëÇÑ ¿ÀÇÁ¼Â °è»ê 
    float fRadius = 15.f;

    // ÁÖº¯ ÇÈ¼¿µéÀ» ¹Ýº¹ÇÏ¿© ºí·¯ Ã³¸®
    for (int iY = (int) fRadius * -1.f; iY <= (int) fRadius; iY++)
    {
        for (int iX = (int) fRadius * -1.f; iX <= (int) fRadius; iX++)
        {
            float fDistance = sqrt(float(iX * iX + iY * iY));
            float fWeight = Compute_Gaussian(fDistance, 1.f); // °¡¿ì½Ã¾È ÇÔ¼ö¸¦ ÀÌ¿ëÇØ °¡ÁßÄ¡ °è»ê
            // °¡ÁßÄ¡°¡ Àû¿ëµÈ »ö»óÀ» °á°ú¿¡ ´õÇÔ
            vResult += fWeight * g_Texture.Sample(LinearSampler, In.vTexcoord + float2(iX, iY) * vTexOffset);
        }
    }
    
    Out.vColor = vResult;

    return Out;
}
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

technique11 DefaultTechnique
{
    pass Debug  // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Debug_Cube // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CUBE();
    }

    pass Debug_3D // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_3D();
    }

    pass Light_Directional // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }


    pass Light_Point    // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }
    
    pass Light_Spot     // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPOT();
    }
    
    pass Light_Result   // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_RESULT();
    }

    pass Emissive       // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EMISSIVE();
    }

    pass Bloom          //  6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLOOM();
    }


    pass Post_Processing //  7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POSTPROCESSING();
    }

    pass Post_Processing_Result //  8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_POST_PROCESSING_RESULT();
    }

    pass RadialBlur // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_RADIALBLUR();
    }
}