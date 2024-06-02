#include "Engine_Shader_Defines.hlsli"

/* 전역변수 : 쉐이더 외부에 있는 데이터를 쉐이더 안으로 받아온다. */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_CamViewMatrix, g_CamProjMatrix;
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

bool g_isRadialBlurActive = { false };
float2 g_vRadialBlurUV;
float g_fRadialAmount;

//예은 추가
float		g_fCutOff;
float		g_fOutCutOff;
//

// 현진 추가
bool g_isSSAO;
texture2D g_RandomNormalTexture;
texture2D g_VelocityTexture;

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

/* 정점 쉐이더 */
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

    Out.vColor = g_Texture.Sample(PointSamplerClamp, In.vTexcoord);
    return Out;
}

PS_OUT PS_MAIN_CUBE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float3 vTex = normalize(float3(1.0, In.vTexcoord.y * -2 + 1.f, In.vTexcoord.x * -2.f + 1.f));
    //float3 vTex = normalize(float3(In.vTexcoord.x * 2 - 1.f, In.vTexcoord.y * -2.f + 1.f, 1.f));
    //float3 vTex = normalize(float3(-1.0, In.vTexcoord.y * -2 + 1.f, In.vTexcoord.x * -2.f + 1.f));
    
    Out.vColor = g_CubeTexture.Sample(PointSamplerClamp, float4(vTex, 0));

    return Out;
}

// for 3D Texture
int g_iNumZ;
int g_iNumSizeZ;

PS_OUT PS_MAIN_3D(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //Out.vColor = g_3DTexture.Load(int4(In.vTexcoord * 128, (float) g_iNumZ, 0));
    Out.vColor = g_3DTexture.Sample(PointSampler, float3(In.vTexcoord, (float) g_iNumZ / g_iNumSizeZ));
    return Out;
}

PS_OUT PS_MAIN_DISCARD(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(PointSampler, In.vTexcoord);
    if (Out.vColor.a == 0)
        discard;
    
    return Out;
}


struct PS_OUT_LIGHT
{
    float4 vShade : SV_TARGET0;
    float4 vSpecular : SV_TARGET1;
};

float4 ConvertoTexcoordToWorldPosition(in float2 uv)
{
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, uv);
    float fViewZ = vDepthDesc.y * 1000.0f;
    float4 vWorldPos;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / View.z */
    vWorldPos.x = uv.x * 2.f - 1.f;
    vWorldPos.y = uv.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos *= fViewZ;
    
    matrix InverViewProj = mul(g_ProjMatrixInv, g_ViewMatrixInv);
    
    vWorldPos = mul(vWorldPos, InverViewProj);
    
    return vWorldPos;
}

float doAmbientOcclusion(in float2 tcoord, in float2 uv, in float3 p, in float3 cnorm)
{
    float3 diff = ConvertoTexcoordToWorldPosition(tcoord + uv);
    diff = mul(float4(diff, 1.f), g_ViewMatrix).xyz - p;
    const float3 v = normalize(diff);
    const float d = length(diff) * 0.5; //g_scale
    return max(0.0, dot(cnorm, v) - 0.05) * (1.0 / (1.0 + d)) * 1.5f;
        // g_bias, g_intensity
}

float CalcAmbientOcclusion(float2 Texcoord)
{
    const float2 vec[4] = { float2(1, 0), float2(-1, 0), float2(0, 1), float2(0, -1) };
    float4 p = ConvertoTexcoordToWorldPosition(Texcoord);
    p = mul(p, g_ViewMatrix);
    float4 n = g_NormalTexture.Sample(PointSampler, Texcoord) * 2.f - 1.f;
    float2 rand = g_RandomNormalTexture.Sample(PointSampler, float2(1920, 1080) * Texcoord / 900).xy * 2.f - 1.f;
    float ao = 0.0f;
    float rad = 1 / p.z; // 샘플링 반경 : 1 임시값
  
    //**SSAO Calculation**// 
    int iterations = 4;
    for (int j = 0; j < iterations; ++j)
    {
        float2 coord1 = reflect(vec[j], rand) * rad;
        float2 coord2 = float2(coord1.x * 0.707 - coord1.y * 0.707, coord1.x * 0.707 + coord1.y * 0.707);
        
        ao += doAmbientOcclusion(Texcoord, coord1 * 0.25, p, n.xyz);
        ao += doAmbientOcclusion(Texcoord, coord1 * 0.5, p, n.xyz);
        ao += doAmbientOcclusion(Texcoord, coord1 * 0.75, p, n.xyz);
        ao += doAmbientOcclusion(Texcoord, coord1, p, n.xyz);
        
        ao += doAmbientOcclusion(Texcoord, coord2 * 0.25, p, n.xyz);
        ao += doAmbientOcclusion(Texcoord, coord2 * 0.5, p, n.xyz);
        ao += doAmbientOcclusion(Texcoord, coord2 * 0.75, p, n.xyz);
        ao += doAmbientOcclusion(Texcoord, coord2, p, n.xyz);
    }
  
    ao /= (float) iterations * 8.0;

    return ao;
}

/* 빛 하나당 480000 수행되는 쉐이더. */
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
	
    // for specular
    float4 vWorldPos;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / View.z */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos *= fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    float4 vLook = vWorldPos - g_vCamPosition;
    float4 vReflect = reflect(normalize(g_vLightDir), vNormal);
    float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 3.f);

    if (g_isSSAO)
        Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + (1 - CalcAmbientOcclusion(In.vTexcoord)) * g_vLightAmbient * g_vMtrlAmbient);
    else
        Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + g_vLightAmbient * g_vMtrlAmbient);
    
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

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / View.z */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

    vWorldPos *= fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    vector vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);
    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);

    float4 vLook = vWorldPos - g_vCamPosition;
    float4 vReflect = reflect(normalize(vLightDir), vNormal);
    float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 3.f);
    
    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + g_vLightAmbient * g_vMtrlAmbient);
    Out.vShade *= fAtt;
    
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

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / View.z */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

    vWorldPos *= fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    vector vLightDir = vWorldPos - g_vLightPos;
    vector vSpotDir = g_vLightDir;
    float fResult = acos(dot(normalize(vLightDir), normalize(vSpotDir)));
	
 
    if (g_fOutCutOff >= fResult) // 빛이 번질 범위 안에 있을 때
    {
        float fDistance = length(vLightDir);
        float fIntensity = (fResult - g_fOutCutOff) / (g_fCutOff - g_fOutCutOff);
        float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange) * fIntensity; //범위 줘서 끝 범위에서는 연해지게 

        float4 vLook = vWorldPos - g_vCamPosition;
        float4 vReflect = reflect(normalize(vLightDir), vNormal);
        float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);
        
        Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + g_vLightAmbient * g_vMtrlAmbient);
        Out.vShade *= fAtt;
        
        Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
        Out.vSpecular *= fAtt;
    }
		
    return Out;
}

float2 poissonDisk[16] =
{
    float2(-0.94201624, -0.39906216),
    float2(0.94558609, -0.76890725),
    float2(-0.094184101, -0.92938870),
    float2(0.34495938, 0.29387760),
    float2(-0.91588581, 0.45771432),
    float2(-0.81544232, -0.87912464),
    float2(-0.38277543, 0.27676845),
    float2(0.97484398, 0.75648379),
    float2(0.44323325, -0.97511554),
    float2(0.53742981, -0.47373420),
    float2(-0.26496911, -0.41893023),
    float2(0.79197514, 0.19090188),
    float2(-0.24188840, 0.99706507),
    float2(-0.81409955, 0.91437590),
    float2(0.19984126, 0.78641367),
    float2(0.14383161, -0.14100790)
};

float ShadowPCF(texture2D ShadowTexture, float SampleRadius, float fOriginDepth, float2 vTexcoord)
{
    float Shadow = 0.f;
    
    for (int i = 0; i < 16; ++i)
    {
        float2 offset = poissonDisk[i] * SampleRadius;
        float2 sampleCoord = vTexcoord + offset;
        float fDepth = ShadowTexture.Sample(PointSamplerClamp, sampleCoord).r;
        if (fOriginDepth - 0.1f > (fDepth * 1000.f))
            Shadow += 1.f;
    }
    Shadow /= 16; // 샘플 수로 나누어 평균 계산
    
    return Shadow;
}
float ShadowPCF_Point(TextureCubeArray ShadowTexture, int iLightIndex, float SampleRadius, float fDistance, float3 fDirection)
{
    float Shadow = 0.f;
    
    for (int i = 0; i < 16; ++i)
    {
        float3 Offset = float3(poissonDisk[i] * SampleRadius, 0);
        float3 SamplePos = normalize(fDirection + Offset);
        float fDepth = g_PointLightDepthTexture.Sample(PointSamplerClamp, float4(SamplePos, iLightIndex)).r;
        if (fDistance > (fDepth * 1000.f))
            Shadow += 1.f;
    }
    Shadow /= 16; // 샘플 수로 나누어 평균 계산
    
    return Shadow;
}

float Cal_Shadow(float2 vTexcoord)
{
    /* Shadow */
    /* ProjPos.w == View.Z */
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.0f;

    float4 vWorldPos;
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / View.z */
    vWorldPos.x = vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
    vWorldPos *= fViewZ;
	/* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
	/* 로컬위치 * 월드행렬 */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    // 광원 여러개에 대해서 처리해야함
    
    float fShadow = 0;
    
    // 1. DirLight
    if (g_isShadowDirLight)
    {
        vector vPosition = mul(vWorldPos, g_DirLightViewMatrix);
        vPosition = mul(vPosition, g_DirLightProjMatrix);
  
        float2 vTexcoord;
        vTexcoord.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
        vTexcoord.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;

        fShadow += ShadowPCF(g_DirLightDepthTexture, 0.0002f, vPosition.w, vTexcoord) * 0.5;
    }
    
    // 2. PointLight
    for (int i = 0; i < g_iNumShadowPointLight; ++i)
    {
        float3 vLightDir = vWorldPos - g_vShadowPointLightPos[i];
        float fDistance = length(vLightDir);
        vLightDir = normalize(vLightDir);
    
        float fAtt = saturate((g_fShadowPointLightRange[i].x - fDistance) / g_fShadowPointLightRange[i].x);
        
        fShadow += ShadowPCF_Point(g_PointLightDepthTexture, i, 0.0001f, fDistance, vLightDir) * fAtt;
    }
    
    // 3. SpotLight
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

        if (g_fOutCutOff >= fResult) // 빛이 번질 범위 안에 있을 때
        {
            float fDistance = length(vLightDir);
            float fIntensity = (fResult - g_fSpotLightOutCutOff) / (g_fSpotLightCutOff - g_fSpotLightOutCutOff);
            float fAtt = saturate((g_fSpotLightRange - fDistance) / g_fSpotLightRange) * fIntensity; //범위 줘서 끝 범위에서는 연해지게 
        
            fShadow += ShadowPCF(g_SpotLightDepthTexture, 0.0001f, vPosition.w, vTexcoord) * fAtt;
        }
    }
    
    return fShadow;
}

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

/* 최종적으로 480000 수행되는 쉐이더. */
PS_OUT_PRE_POST PS_MAIN_LIGHT_RESULT(PS_IN In)
{
    PS_OUT_PRE_POST Out = (PS_OUT_PRE_POST) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(PointSamplerClamp, In.vTexcoord);
    if (0.0f == vDiffuse.a)
        discard;
    vector vShade = g_ShadeTexture.Sample(PointSamplerClamp, In.vTexcoord);
    vector vSpecular = g_SpecularTexture.Sample(PointSamplerClamp, In.vTexcoord);
    
    vector vDepth = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormal = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vMaterial = g_MaterialTexture.Sample(PointSampler, In.vTexcoord);

    float fMetalness = vMaterial.r;
    
    vSpecular = lerp(0.04, vSpecular, fMetalness);
    
    Out.vDiffuse = vDiffuse * vShade + vSpecular;
    Out.vDiffuse.a = 1.f;
    Out.vNormal = vNormal;
    Out.vDepth = vDepth;
    Out.vMaterial = vMaterial;

    // 그림자 연산
    vector vOrigin = g_OriginalTexture.Sample(PointSampler, In.vTexcoord);
    if (vOrigin.r == 0)
    {
        float fShadow = Cal_Shadow(In.vTexcoord);
        Out.vDiffuse *= (1 - fShadow);
        Out.vDiffuse.a = 1;
    }

    //float3 uv = float3(In.vTexcoord, vDepth.r * vDepth.g);

    //float4 scatteringColorAndTransmittance = g_3DTexture.Sample(PointSampler, uv);
    //float3 scatteringColor = HDR(scatteringColorAndTransmittance.rgb);

    //Out.vDiffuse = Out.vDiffuse * float4(scatteringColor, scatteringColorAndTransmittance.a);
    
    return Out;
}

PS_OUT PS_MOTIONBLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    int iNumBlurSample = 5;
    
    float4 vVelocity = g_VelocityTexture.Sample(LinearSamplerClamp, In.vTexcoord);
    vVelocity.xy /= (float) iNumBlurSample;
    
    
    Out.vColor = g_DiffuseTexture.Sample(PointSamplerClamp, In.vTexcoord);g_DiffuseTexture.Sample(PointSamplerClamp, In.vTexcoord);
    int iCnt = 1;
    
    float4 vColor;
    
    for (int i = iCnt; i < iNumBlurSample; ++i)
    {
        vColor = g_DiffuseTexture.Sample(PointSamplerClamp, In.vTexcoord + vVelocity.xy * (float) i);
        
        if (vVelocity.a != 0)
        {
            iCnt++;
            Out.vColor += vColor;
        }

    }
    
    Out.vColor /= (float)iCnt;
    
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
    
    //  현재 깊이가 원본위치에서의 깊이가 디스토션이적용된 깊이보다 멀다면
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
        float2 vTexOffset = 1.0 / float2(fWidth, fHeight); // 텍스처 각 요소에 대한 오프셋 계산 
    
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

float Compute_Gaussian(float fX, float fSigma)
{
    float fResult = exp(-((fX * fX) / (2.0 * fSigma * fSigma))) / (fSigma * sqrt(2.0 * 3.14159));
    
    return fResult;
}

// 가우시안 블러를 적용할 픽셀 셰이더 함수
PS_OUT PS_MAIN_BLURX(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float fWidth, fHeight;
    g_Texture.GetDimensions(fWidth, fHeight);
    
    float4 vResult = float4(0, 0, 0, 0);
    float2 vTexOffset = 1.0 / float2(fWidth, fHeight); // 텍스처 각 요소에 대한 오프셋 계산 
    float fRadius = 10.f;

    // 주변 픽셀들을 반복하여 블러 처리
    for (int iX = (int) fRadius * -1.f; iX <= (int) fRadius; iX++)
    {
        float fDistance = sqrt(float(iX * iX));
        float fWeight = Compute_Gaussian(fDistance, 1.f); // 가우시안 함수를 이용해 가중치 계산
        // 가중치가 적용된 색상을 결과에 더함
        vResult += fWeight * g_Texture.Sample(PointSamplerClamp, In.vTexcoord + float2(iX, 0) * vTexOffset);
    }
    
    Out.vColor = vResult;

    return Out;
}

PS_OUT PS_MAIN_BLURY(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float fWidth, fHeight;
    g_Texture.GetDimensions(fWidth, fHeight);
    
    float4 vResult = float4(0, 0, 0, 0);
    float2 vTexOffset = 1.0 / float2(fWidth, fHeight); // 텍스처 각 요소에 대한 오프셋 계산 
    float fRadius = 10.f;

    // 주변 픽셀들을 반복하여 블러 처리
    for (int iY = (int) fRadius * -1.f; iY <= (int) fRadius; iY++)
    {
        float fDistance = sqrt(float(iY * iY));
        float fWeight = Compute_Gaussian(fDistance, 1.f); // 가우시안 함수를 이용해 가중치 계산
        // 가중치가 적용된 색상을 결과에 더함
        vResult += fWeight * g_Texture.Sample(PointSamplerClamp, In.vTexcoord + float2(0, iY) * vTexOffset);
    }
    
    Out.vColor = vResult;

    return Out;
}

static const int SSR_MAX_STEPS = 16;
static const int SSR_BINARY_SEARCH_STEPS = 16;
static const float RayStep = 1.f;                   // 1 ~ 3
static const float RayHitThreshold = .25f;           // 0.25 ~ 5

float4 SSRBinarySearch(float3 vDir, inout float3 hitCoord)
{
    for (int i = 0; i < SSR_BINARY_SEARCH_STEPS; i++)
    {
        float4 vProjectedCoord = mul(float4(hitCoord, 1.0f), g_CamProjMatrix);
        vProjectedCoord /= vProjectedCoord.w;
        vProjectedCoord.xy = vProjectedCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
        
        float4 ViewPosition = ConvertoTexcoordToWorldPosition(vProjectedCoord.xy);
        ViewPosition = mul(ViewPosition, g_CamViewMatrix);

        float depthDifference = hitCoord.z - ViewPosition.z;
        
        if (depthDifference < 0.f)
            hitCoord += vDir;

        vDir *= 0.5f;
        hitCoord -= vDir;
    }

    float4 vProjectedCoord = mul(float4(hitCoord, 1.0f), g_CamProjMatrix);
    vProjectedCoord.xy /= vProjectedCoord.w;
    vProjectedCoord.xy = vProjectedCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
   
    float fDepth = g_DepthTexture.Sample(PointSampler, vProjectedCoord.xy).r;
    
    float4 ViewPosition = ConvertoTexcoordToWorldPosition(vProjectedCoord.xy);
    ViewPosition = mul(ViewPosition, g_CamViewMatrix);
    

    float depthDifference = hitCoord.z - ViewPosition.z;

    return float4(vProjectedCoord.xy, fDepth, abs(depthDifference) < RayHitThreshold ? 1.0f : 0.0f);
}
float4 SSRRayMarch(float3 vDir, inout float3 hitCoord)
{
    for (int i = 0; i < SSR_MAX_STEPS; i++)
    {
        hitCoord += vDir;
        float4 projectedCoord = mul(float4(hitCoord, 1.0f), g_CamProjMatrix);
        projectedCoord /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);

        float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, projectedCoord.xy);

        float4 ViewPosition = ConvertoTexcoordToWorldPosition(projectedCoord.xy);
        ViewPosition = mul(ViewPosition, g_CamViewMatrix);

        float fDepthDifference = hitCoord.z - ViewPosition.z;

		[branch]
        if (fDepthDifference > 0.f)
        {
            return SSRBinarySearch(vDir, hitCoord);
        }

        vDir *= RayStep;
    }
    
    return float4(0.0f, 0.0f, 0.0f, 0.0f);
}

PS_OUT PS_SSR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(PointSamplerClamp, In.vTexcoord);
    
    float4 vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    vNormal = mul(vNormal, g_CamViewMatrix);
    float fMetallic = g_MaterialTexture.Sample(PointSampler, In.vTexcoord).g;
    
    if (fMetallic < 0.01f)
    {
        Out.vColor = vDiffuse;
        return Out;
    }

    float4 ViewPosition = ConvertoTexcoordToWorldPosition(In.vTexcoord);
    ViewPosition = mul(ViewPosition, g_CamViewMatrix);

    float3 vReflectDir = normalize(reflect(normalize(ViewPosition.xyz), vNormal.xyz));
    float3 hitPosition = ViewPosition.xyz;
    float4 coords = SSRRayMarch(vReflectDir, hitPosition);
    float2 coordsEdgeFactor = float2(1, 1) - pow(saturate(abs(coords.xy - float2(0.5f, 0.5f)) * 2), 8);
    float screenEdgeFactor = saturate(min(coordsEdgeFactor.x, coordsEdgeFactor.y));
    float reflectionIntensity = saturate(screenEdgeFactor * saturate(vReflectDir.z) * (coords.w));
    float3 reflectionColor = reflectionIntensity * g_DiffuseTexture.Sample(PointSamplerClamp, coords.xy).rgb;

    Out.vColor = vDiffuse + fMetallic * max(0, float4(reflectionColor, 1.0f));   
    return Out;
}

float g_fTargetDepth = 3.f;
const float g_fDOFParam = 5.f;  // 블러 적용 여부에 대한 반경 
texture2D g_DOFTexture;
// 뷰 공간에서의 깊이 : z  = fNear ~ fFar
// 투영 공간에서의 깊이 : z = 0 ~ 1
// w 나누기 수행시z = 1
PS_OUT PS_DOF(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float vDepth = vDepthDesc.x * vDepthDesc.y * 1000;
    if (vDepth < g_fTargetDepth - g_fDOFParam ||
        vDepth >= g_fTargetDepth + g_fDOFParam)
        Out.vColor = abs(g_fTargetDepth - vDepth) / (vDepthDesc.y * 1000);
    // 블러를 먹일 픽셀은 깊이 차이를 저장
    Out.vColor.a = 1.f;
    return Out;
}

PS_OUT PS_DOF_BLURX(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float fWidth, fHeight;
    g_Texture.GetDimensions(fWidth, fHeight);
    
    float4 vResult = float4(0, 0, 0, 0);
    float2 vTexOffset = 1.0 / float2(fWidth, fHeight); // 텍스처 각 요소에 대한 오프셋 계산 
    float fRadius = clamp(g_DOFTexture.Sample(PointSampler, In.vTexcoord).r * 10, 0.f, 10.f);
    
    if (fRadius == 0)
    {
        Out.vColor = g_Texture.Sample(PointSamplerClamp, In.vTexcoord);
        return Out;
    }
    
    // 주변 픽셀들을 반복하여 블러 처리
    for (int iX = (int) fRadius * -1.f; iX <= (int) fRadius; iX++)
    {
        float fDistance = sqrt(float(iX * iX));
        float fWeight = Compute_Gaussian(fDistance, 1.f); // 가우시안 함수를 이용해 가중치 계산
        // 가중치가 적용된 색상을 결과에 더함
        if (0 != g_DOFTexture.Sample(PointSampler, In.vTexcoord + float2(iX, 0) * vTexOffset).r)
            vResult += fWeight * g_Texture.Sample(PointSamplerClamp, In.vTexcoord + float2(iX, 0) * vTexOffset);
    }
    
    Out.vColor = vResult;

    return Out;
}
PS_OUT PS_DOF_BLURY(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float fWidth, fHeight;
    g_Texture.GetDimensions(fWidth, fHeight);
    
    float4 vResult = float4(0, 0, 0, 0);
    float2 vTexOffset = 1.0 / float2(fWidth, fHeight); // 텍스처 각 요소에 대한 오프셋 계산 
    float fRadius = clamp(g_DOFTexture.Sample(PointSampler, In.vTexcoord).r * 10, 0.f, 10.f);
    
    if (fRadius == 0)
    {
        Out.vColor = g_Texture.Sample(PointSamplerClamp, In.vTexcoord);
        return Out;
    }
    // 주변 픽셀들을 반복하여 블러 처리
    for (int iY = (int) fRadius * -1.f; iY <= (int) fRadius; iY++)
    {
        float fDistance = sqrt(float(iY * iY));
        float fWeight = Compute_Gaussian(fDistance, 1.f); // 가우시안 함수를 이용해 가중치 계산
        // 가중치가 적용된 색상을 결과에 더함
        if (0 != g_DOFTexture.Sample(PointSampler, In.vTexcoord + float2(0, iY) * vTexOffset).r)
            vResult += fWeight * g_Texture.Sample(PointSamplerClamp, In.vTexcoord + float2(0, iY) * vTexOffset);
    }
    
    Out.vColor = vResult;

    return Out;
}

float RGBToLuma(float3 rgb)
{
    return sqrt(dot(rgb, float3(0.299, 0.587, 0.114)));
}

float Quality[12] = { 0.f, 0.f, 0.f, 0.f, 0.f, 1.5f, 2.f, 2.f, 2.f, 2.f, 4.f, 8.f };

PS_OUT PS_FXAA(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float fWidth, fHeight;
    g_Texture.GetDimensions(fWidth, fHeight);
    float2 vTexOffset = 1.0 / float2(fWidth, fHeight); // 텍스처 각 요소에 대한 오프셋 계산 
    
    float4 fColor_Center = g_Texture.Sample(LinearSamplerClamp, In.vTexcoord);
    float fLuma_Center = RGBToLuma(fColor_Center.rgb);
    // 1. 가장자리를 찾기 : 현재 픽셀과 4방향 이웃의 lumas 계산
    float fLuma_Down = RGBToLuma(g_Texture.Sample(LinearSamplerClamp, In.vTexcoord + float2(0, 1) * vTexOffset).rgb);
    float fLuma_Up = RGBToLuma(g_Texture.Sample(LinearSamplerClamp, In.vTexcoord + float2(0, -1) * vTexOffset).rgb);
    float fLuma_Left = RGBToLuma(g_Texture.Sample(LinearSamplerClamp, In.vTexcoord + float2(-1, 0) * vTexOffset).rgb);
    float fLuma_Right = RGBToLuma(g_Texture.Sample(LinearSamplerClamp, In.vTexcoord + float2(1, 0) * vTexOffset).rgb);
    
    // 차이값을 구해서 임계치(THREADHOLD)보다 크다면 앨리어싱 수행
    float fLuma_Min = min(fLuma_Center, min(min(fLuma_Down, fLuma_Up), min(fLuma_Left, fLuma_Right)));
    float fLuma_Max = max(fLuma_Center, max(max(fLuma_Down, fLuma_Up), max(fLuma_Left, fLuma_Right)));
    
    float fLumaRange = fLuma_Max - fLuma_Min;
    // 각각 권장값 EDGE_THRESHOLD_MIN = 0.0312, EDGE_THRESHOLD_MAX = 0.125.
    if (fLumaRange < max(0.0312, fLuma_Max * 0.125))
    {
        Out.vColor = fColor_Center;
        return Out;
    }
    
    // 2. 가장자리가 수평, 수직인지 확인
    // 수평: |(upleft - left) - (left - downleft)| + 2 * |(up - center) - (center - down)| + |(upright - right) - (right - downright)|
    // 수직: |(upright - up) - (up - upleft)| + 2 * |(right - center) - (center - left)| + |(downright - down) - (down - downleft)|
   
    // 대각선 좌표의 Luma 계산
    float fLuma_DownLeft = RGBToLuma(g_Texture.Sample(LinearSamplerClamp, In.vTexcoord + float2(-1, 1) * vTexOffset).rgb);
    float fLuma_UpRight = RGBToLuma(g_Texture.Sample(LinearSamplerClamp, In.vTexcoord + float2(1, -1) * vTexOffset).rgb);
    float fLuma_UpLeft = RGBToLuma(g_Texture.Sample(LinearSamplerClamp, In.vTexcoord + float2(-1, -1) * vTexOffset).rgb);
    float fLuma_DownRight = RGBToLuma(g_Texture.Sample(LinearSamplerClamp, In.vTexcoord + float2(1, 1) * vTexOffset).rgb);
    
    // 상하좌우, 대각선을 조합
    float fLuma_DownUp = fLuma_Down + fLuma_Up;
    float fLuma_LeftRight = fLuma_Left + fLuma_Right;

    float fLuma_DownCorners = fLuma_DownLeft + fLuma_DownRight;
    float fLuma_UpCorners = fLuma_UpLeft + fLuma_UpRight;
    float fLuma_LeftCorners = fLuma_DownLeft + fLuma_UpLeft;
    float fLuma_RightCorners = fLuma_DownRight + fLuma_UpRight;
    
    // 수평 수직 계산
    float fEdgeHorizontal = abs(-2.0 * fLuma_Left + fLuma_LeftCorners) + abs(-2.0 * fLuma_Center + fLuma_DownUp) * 2.0 + abs(-2.0 * fLuma_Right + fLuma_RightCorners);
    float fEdgeVertical = abs(-2.0 * fLuma_Up + fLuma_UpCorners) + abs(-2.0 * fLuma_Center + fLuma_LeftRight) * 2.0 + abs(-2.0 * fLuma_Down + fLuma_DownCorners);
    
    bool isHorizontal = (fEdgeHorizontal >= fEdgeVertical);
 
    // 가장자리의 방향에 직교하는 방향이 실제 가장자리의 경계인지 판정
    // 현제 픽셀의 각 측면도의 변화도(Gradient)를 계산, 차이가 가장 큰 곳이 가장자리 테두리로 판정될 예정
    
    float fLuma1 = isHorizontal ? fLuma_Down : fLuma_Left;
    float fLuma2 = isHorizontal ? fLuma_Up : fLuma_Right;
    
    // 변화도 계산
    float fGradient1 = fLuma1 - fLuma_Center;
    float fGradient2 = fLuma2 - fLuma_Center;
    
    // 더 가파른 방향을 결정
    bool is1Steepest = abs(fGradient1) >= abs(fGradient2);
    
    // 정규화
    float fGradientScaled = 0.25f * max(abs(fGradient1), abs(fGradient2));
    
    // 가장자리 방향에 따라 이동 거리 결정
    float fStepLength = isHorizontal ? vTexOffset.y : vTexOffset.x;
    
    // 이동한 방향에서의 평균 Luma 계산
    float fLumaLocalAverage = 0.f;
    
    if (is1Steepest)
    {
        fStepLength = -fStepLength;
        fLumaLocalAverage = 0.5f * (fLuma1 + fLuma_Center);
    }
    else
    {
        fLumaLocalAverage = 0.5f * (fLuma2 + fLuma_Center);
    }
    
    float2 vCurrentTexcoord = In.vTexcoord;
    if (isHorizontal)
    {
        vCurrentTexcoord.y += fStepLength * 0.5f;
    }
    else
    {
        vCurrentTexcoord.x += fStepLength * 0.5f;
    }
    
    // 가장자리의 주축(Main Axis)를 따라서 탐색
    // 두 방향으로 한 픽셀 이동, 새로운 좌표의 Lumas를 얻고 변화량을 계산
    // 차이가 Gradient 보다 큰 경우 가장자리의 끝에 도달한 것으로 판정 , 정지
    
    float2 vOffSet = isHorizontal ? float2(vTexOffset.x, 0.f) : float2(0.f, vTexOffset.y);
      
    float2 vTexcoord1 = vCurrentTexcoord - vOffSet;
    float2 vTexcoord2 = vCurrentTexcoord + vOffSet;
    
    float fLuma_End1 = RGBToLuma(g_Texture.Sample(LinearSamplerClamp, vTexcoord1).rgb);
    float fLuma_End2 = RGBToLuma(g_Texture.Sample(LinearSamplerClamp, vTexcoord2).rgb);
    fLuma_End1 -= fLumaLocalAverage;
    fLuma_End2 -= fLumaLocalAverage;
    
    bool isReached1 = abs(fLuma_End1) >= fGradientScaled;
    bool isReached2 = abs(fLuma_End2) >= fGradientScaled;
    bool isReachedBoth = isReached1 && isReached2;
    
    if (!isReached1)
    {
        vTexcoord1 -= vOffSet;
    }
    if (!isReached2)
    {
        vTexcoord2 += vOffSet;
    }
    
    // 가장 자리 양쪽에 도달할 때까지 또는 최대 반복 수에 도달할 때 까지 반복
    // 빠른 처리를 위해 이동 픽셀 양을 5번 반복후에 증가( 0,0,0,0,0,1.5, 2.0, 2.0, 2.0, 2.0, 4.0, 8.0)
    if (!isReachedBoth)
    {
        for (int i = 2; i < 12; ++i)
        {
            if (!isReached1)
            {
                fLuma_End1 = RGBToLuma(g_Texture.Sample(LinearSamplerClamp, vTexcoord1).rgb);
                fLuma_End1 -= fLumaLocalAverage;
            }
            if (!isReached2)
            {
                fLuma_End2 = RGBToLuma(g_Texture.Sample(LinearSamplerClamp, vTexcoord2).rgb);
                fLuma_End2 -= fLumaLocalAverage;
            }
            
            isReached1 = abs(fLuma_End1) >= fGradientScaled;
            isReached2 = abs(fLuma_End2) >= fGradientScaled;
            isReachedBoth = isReached1 && isReached2;
            
            if (!isReached1)
                vTexcoord1 -= vOffSet * Quality[i];
            if (!isReached2)
                vTexcoord2 += vOffSet;
            if (isReachedBoth)
                break;
        }
    }
    
    // 도달한 두 방향에 대한 거리 계산
    float fDistance1 = isHorizontal ? In.vTexcoord.x - vTexcoord1.x : In.vTexcoord.y - vTexcoord1.y;
    float fDistance2 = isHorizontal ? vTexcoord2.x - In.vTexcoord.x : vTexcoord2.y - In.vTexcoord.y;
    
    bool isDirection1 = fDistance1 < fDistance2;
    float fDistanceFinal = min(fDistance1, fDistance2);
    
    float fEdgeThickness = fDistance1 + fDistance2;
    
    float fPixelOffset = -fDistanceFinal / fEdgeThickness + 0.5f;
    
    // 현재 픽셀의 Luma와 끝점에서 관찰된 Luma의 차이가 일관되는지(CoHerent) 확인
    // 너무 멀리 이동했을 경우 오프셋 적용하지 않음
    bool isLumaCenterSmaller = fLuma_Center < fLumaLocalAverage;
    bool isCorrectVariation = ((isDirection1 ? fLuma_End1 : fLuma_End2) < 0.f) != isLumaCenterSmaller;
    
    // 만약 luma 차이가 잘못된 경우, offset 을 적용하지 않음
    float fFinalOffset = isCorrectVariation ? fPixelOffset : 0.f;
    
    // SubPixel Antialiasing
    float fLumaAverage = (1.f / 12.f) * (2.f * (fLuma_DownUp + fLuma_LeftRight) + fLuma_LeftCorners + fLuma_RightCorners);
   
    float fSubPixelOffset1 = clamp(abs(fLumaAverage - fLuma_Center) / fLumaRange, 0.0, 1.0);
    float fSubPixelOffset2 = (-2.0 * fSubPixelOffset1 + 3.0) * fSubPixelOffset1 * fSubPixelOffset1;
    
    //SUBPIXEL_QUALITY = 0.75
    float fSubPixelOffsetFinal = fSubPixelOffset2 * fSubPixelOffset2 * 0.75f;
    
    fFinalOffset = max(fFinalOffset, fSubPixelOffsetFinal);
    
    // 최종 계산
    float2 vFinalTexcoord = In.vTexcoord;
    if (isHorizontal)
    {
        vFinalTexcoord.y += fFinalOffset * fStepLength;
    }
    else
    {
        vFinalTexcoord.x += fFinalOffset * fStepLength;
    }

    // 새로운 UV 좌표에서 컬러를 읽고 사용함
    float4 vFinalColor = g_Texture.Sample(LinearSamplerClamp, vFinalTexcoord);
    Out.vColor = vFinalColor;

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

    pass Debug_3D // 2
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

    pass Copy_With_Discard // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISCARD();
    }

    pass Light_Directional // 3
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

    pass Light_Point    // 4
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
    
    pass Light_Spot     // 5
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
    
    pass Light_Result   // 6
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

    pass Emissive       // 7
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

    pass Motion_Blur // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MOTIONBLUR(); // 고치시오
    }

    pass BlurX          //  8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLURX();
    }

    pass BlurY //  9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLURY();
    }

    pass Post_Processing //  10
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

    pass Post_Processing_Result //  11
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

    pass RadialBlur // 12
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

    pass SSR // 13
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_SSR();
    }

    pass DOF // 14
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_DOF();
    }

    pass DOF_BlurX // 15
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_DOF_BLURX();
    }

    pass DOF_BlurY // 16
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_DOF_BLURY();
    }

    pass FXAA // 16
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_FXAA();
    }

}