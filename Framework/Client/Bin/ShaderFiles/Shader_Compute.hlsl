#include "Engine_Shader_Defines.hlsli"

RWTexture3D<float4> OutputTexture;  // Output

float Epsilon = 1.0e-5f;
float Pi = 3.14;
float UniformDensity = 0.975f;
float Intensity = 1;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

float4 g_vCamPosition;

bool g_isShadowDirLight;
texture2D g_DirLightDepthTexture;
matrix g_DirLightViewMatrix;
matrix g_DirLightProjMatrix;
float4 g_vDirLightDirection;
float4 g_vDirLightDiffuse;


bool g_isShadowSpotLight;
texture2D g_SpotLightDepthTexture;
matrix g_SpotLightViewMatrix;
matrix g_SpotLightProjMatrix;
//float4 g_vSpotLightDirection;
float4 g_vSpotLightPosition;
float4 g_vSpotLightDiffuse;
//float4 g_vLightDir;
//float4 g_vLightPos;
//float g_fLightRange;

//float4 g_vLightDiffuse;
//float4 g_vLightAmbient;
//float4 g_vLightSpecular;


float ConvertDepthToNdcZ(float depth)
{
    float depthPackExponent = 2;
    float nearPlaneDist = 0.1;
    float farPlaneDist = 1000;

    return pow(saturate((depth - nearPlaneDist) / (farPlaneDist - nearPlaneDist)), 1 / depthPackExponent);
}

// 쓰레드 ID를 Directx 좌표계로의 변환
float3 ConvertThreadIdToNdc(uint3 id, uint3 dims)
{
    float3 ndc = id;
    ndc += 0.5f;
    ndc *= float3(2.f / dims.x, -2.f / dims.y, 1.f / dims.z);
    ndc += float3(-1.f, 1.f, 0.f);
    return ndc;
}

float ConvertNdcZToDepth(float ndcZ)
{
    float depthPackExponent = 2.f;
    float nearPlaneDist = 0.1f;
    float farPlaneDist = 1000.f;

    return pow(ndcZ, depthPackExponent) * (farPlaneDist - nearPlaneDist) + nearPlaneDist;
}

float3 ConvertToWorldPosition(float3 ndc, float depth)
{
    
	// view ray 
	 //ndc좌표에 대한 카메라 공간 광선을 계산
    float4 viewRay = mul(float4(ndc, 1.f), g_ProjMatrixInv);
    viewRay /= viewRay.w;
    viewRay /= viewRay.z; // z값이 1이 되도록

	// ndc -> world position
    float4 worldPosition = mul(float4(viewRay.xyz * depth, 1.f), g_ViewMatrixInv);

    return worldPosition.xyz;
}

float3 ConvertThreadIdToWorldPosition(uint3 id, uint3 dims)
{
	// id -> ndc
    float3 ndc = ConvertThreadIdToNdc(id, dims);
    float depth = ConvertNdcZToDepth(ndc.z);

    return ConvertToWorldPosition(ndc, depth);
}

// 산란 게산
float HenyeyGreensteinPhaseFunction(float3 wi, float3 wo, float g)
{
    float cosTheta = dot(wi, wo);
    float g2 = g * g;
    float denom = pow(1.f + g2 - 2.f * g * cosTheta, 3.f / 2.f);
    return (1.f / (4.f * Pi)) * ((1.f - g2) / max(pow(denom, 1.5f), Epsilon));
}


//float CalcShadowVisibility(float3 worldPos, float3 viewPos, float depthBias)
//{
//    float4 shadowCoord = mul(float4(worldPos, 1.0f), ShadowViewProjection[cascadeIndex]);
	
//    float3 uv_depth = shadowCoord.xyz / shadowCoord.w;
//    uv_depth.y = -uv_depth.y;
//    uv_depth.xy = uv_depth.xy * 0.5f + 0.5f;

//    return ShadowTexture.SampleCmpLevelZero(ShadowSampler, float3(uv_depth.xy, cascadeIndex), uv_depth.z + depthBias);
//}

//float Visibility(float3 worldPos, float3 biasDir)
//{
//    float3 viewPos = mul(float4(worldPos, 1.f), g_ViewMatrix).xyz;
    
//    float4 shadowCoord = mul(float4(worldPos, 1.0f), g_DirLightViewMatrix);
//    shadowCoord = mul(shadowCoord, g_DirLightProjMatrix);
    
//    float3 uv_depth = shadowCoord.xyz / shadowCoord.w;
//    uv_depth.y = -uv_depth.y;
//    uv_depth.xy = uv_depth.xy * 0.5f + 0.5f;

//    return g_DirLightDepthTexture.SampleLevel(PointSamplerClamp, uv_depth.xy, uv_depth.z + depthBias);
    
    
    
//    return (CalcShadowVisibility(worldPos, viewPos, ShadowBias) == 1.f) ? 1.0f : 0.f;
//}



float Visibility_Dir(float3 vWorldPosition)
{
    vector vPosition = mul(float4(vWorldPosition, 1.f), g_DirLightViewMatrix);
    vPosition = mul(vPosition, g_DirLightProjMatrix);
  
    float2 vTexcoord;
    vTexcoord.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
    vTexcoord.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;

    float4 vDepth = g_DirLightDepthTexture.SampleLevel(PointSamplerClamp, vTexcoord, 0);
    
    if (vPosition.w > vDepth.r * 1000)
    {
        return 1;
    }
    
    return 0;
}

float Visibility_Spot(float3 vWorldPosition)
{
    vector vPosition = mul(float4(vWorldPosition, 1.f), g_SpotLightViewMatrix);
    vPosition = mul(vPosition, g_SpotLightProjMatrix);
  
    float2 vTexcoord;
    vTexcoord.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
    vTexcoord.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;

    float4 vDepth = g_SpotLightDepthTexture.SampleLevel(PointSamplerClamp, vTexcoord, 0);
    
    if (vPosition.w > vDepth.r * 1000)
    {
        return 1;
    }
    
    return 0;
}

[numthreads(8, 8, 8)]
void CS_Volume( uint3 DTid : SV_DispatchThreadID )
{
    uint3 dims;
    OutputTexture.GetDimensions(dims.x, dims.y, dims.z);

    if (all(DTid < dims))
    {
        float3 worldPosition = ConvertThreadIdToWorldPosition(DTid, dims);
        float3 toCamera = normalize(g_vCamPosition.xyz - worldPosition);

        float density = UniformDensity;

        float3 lighting = float3(0.f, 0.f, 0.f);
        
        if (g_isShadowDirLight)
        {
            
            float3 lightDirection = normalize(g_vDirLightDirection.xyz);
            
            float3 toLight = -lightDirection;
            
            float visibility = Visibility_Dir(worldPosition); // 섀도우 맵으로 가시성 검사
            float phaseFunction = HenyeyGreensteinPhaseFunction(lightDirection, toCamera, 0);
            

            lighting +=  visibility * g_vDirLightDiffuse;
            //lighting += visibility * light.m_diffuse.rgb * light.m_diffuse.a * phaseFunction;
        }
      
        if (g_isShadowSpotLight)
        {
            
            float3 lightDirection = normalize(worldPosition - g_vSpotLightPosition.xyz);
            float visibility = Visibility_Spot(worldPosition); // 섀도우 맵으로 가시성 검사
            float phaseFunction = HenyeyGreensteinPhaseFunction(lightDirection, toCamera, 0);
            
            //lighting += 1;
            //lighting += visibility * g_vSpotLightDiffuse * phaseFunction;
        }
        
        OutputTexture[DTid] = float4(lighting * Intensity * density, density);
        //OutputTexture[DTid] = float4(float(DTid.x) / 128, float(DTid.y) / 128, float(DTid.z) / 128, 1);
        
    }
}

static const float DensityScale = 0.01f;

float4 ScatterStep(float3 accumulatedLight, float accumulatedTransmittance, float3 sliceLight, float sliceDensity, float tickness)
{
    sliceDensity = max(sliceDensity, 0.000001f);
    sliceDensity *= DensityScale;
    float sliceTransmittance = exp(-sliceDensity * tickness);

	// Frostbite의 누적 방식
    float3 sliceLightIntegral = sliceLight * (1.f - sliceTransmittance) / sliceDensity;

    accumulatedLight += sliceLightIntegral * accumulatedTransmittance;
    accumulatedTransmittance *= sliceTransmittance;

    return float4(accumulatedLight, accumulatedTransmittance);
}

float SliceTickness(float ndcZ, uint dimZ)
{
    return ConvertNdcZToDepth(ndcZ + 1.f / float(dimZ)) - ConvertNdcZToDepth(ndcZ);
}


[numthreads(8, 8, 1)]
void CS_Volume2(uint3 DTid : SV_DispatchThreadID)
{
    uint3 dims;
    OutputTexture.GetDimensions(dims.x, dims.y, dims.z);

    if (all(DTid < dims))
    {
        float4 accum = float4(0.f, 0.f, 0.f, 1.f);
        uint3 pos = uint3(DTid.xy, 0);

		[loop]
        for (uint z = 0; z < dims.z; ++z)
        {
            pos.z = z;
            float4 slice = OutputTexture[pos];
            float tickness = SliceTickness((float) z / dims.z, dims.z);

            accum = ScatterStep(accum.rgb, accum.a, slice.rgb, slice.a, tickness);
            OutputTexture[pos] = accum;
        }
    }
}

technique11 Default
{
    pass Default
    {
        //SetRasterizerState(RS_Default);
        //SetDepthStencilState(DSS_Default, 0);
        //SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        
        SetComputeShader(CompileShader(cs_5_0, CS_Volume()));

    }

    pass Default2
    {
        SetComputeShader(CompileShader(cs_5_0, CS_Volume2()));
    }
}