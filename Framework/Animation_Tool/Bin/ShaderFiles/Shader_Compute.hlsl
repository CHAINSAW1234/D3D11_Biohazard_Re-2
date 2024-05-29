#include "Engine_Shader_Defines.hlsli"

<<<<<<< HEAD
RWTexture3D<float4> OutputTexture;  // Output

float Epsilon = 1e-9;
float Pi = 3.14;
float UniformDensity = 0.975f;
float Intensity = 1;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

float4 g_vCamPosition;

texture2D g_DiffuseTexture;
texture2D g_DepthTexture;
Texture3D g_VolumeTexture;

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
    float depthPackExponent = 1;
    float nearPlaneDist = 0.1;
    float farPlaneDist = 1000;

    return pow(saturate((depth - nearPlaneDist) / (farPlaneDist - nearPlaneDist)), 1 / depthPackExponent);
}
=======
RWTexture3D<float4> OutputTexture : register(u0);


float4 g_fTest;

>>>>>>> parent of 122df45 (ìž‘ì—…ì¤‘)

// ¾²·¹µå ID¸¦ Directx ÁÂÇ¥°è·ÎÀÇ º¯È¯
float3 ConvertThreadIdToNdc( uint3 id, uint3 dims )
{
	float3 ndc = id;
	ndc += 0.5f;
	ndc *= float3( 2.f / dims.x, -2.f / dims.y, 1.f / dims.z );
	ndc += float3( -1.f, 1.f, 0.f );
	return ndc;
}

float2 ConvertTreahdIdToTexcoord(uint3 id, uint3 dims)
{
    float2 Texcoord = id.xy;
    Texcoord *= float2(1.f / dims.x, 1.f / dims.y);
    return Texcoord;
}

float ConvertNdcZToDepth(float ndcZ)
{
<<<<<<< HEAD
    float depthPackExponent = 1.f;
    float nearPlaneDist = 0.1f;
    float farPlaneDist = 1000.f;
=======
    //float depthPackExponent = VolumetricFogParam.DepthPackExponent;
    //float nearPlaneDist = VolumetricFogParam.NearPlaneDist;
    //float farPlaneDist = VolumetricFogParam.FarPlaneDist;
>>>>>>> parent of 122df45 (ìž‘ì—…ì¤‘)

    //return pow(ndcZ, depthPackExponent) * (farPlaneDist - nearPlaneDist) + nearPlaneDist;
    return 1;
}

float3 ConvertToWorldPosition( float3 ndc, float depth )
{
<<<<<<< HEAD
	// view ray 
	// ndcÁÂÇ¥¿¡ ´ëÇÑ Ä«¸Þ¶ó °ø°£ ±¤¼±À» °è»ê
	float4 viewRay = mul( float4( ndc, 1.f ), g_ProjMatrixInv );
	viewRay /= viewRay.w;
	viewRay /= viewRay.z; // z°ªÀÌ 1ÀÌ µÇµµ·Ï

	// ndc -> world position
    float4 worldPosition = mul(float4(viewRay.xyz * depth, 1.f), g_ViewMatrixInv);

	return worldPosition.xyz;
=======
	//// view ray 
	//// ndcÁÂÇ¥¿¡ ´ëÇÑ Ä«¸Þ¶ó °ø°£ ±¤¼±À» °è»ê
    //float4 viewRay = mul(float4(ndc, 1.f), InvProjectionMatrix);
    //viewRay /= viewRay.w;
    //viewRay /= viewRay.z; // z°ªÀÌ 1ÀÌ µÇµµ·Ï
    //
	//// ndc -> world position
    //float4 worldPosition = mul(float4(viewRay.xyz * depth, 1.f), InvViewMatrix);
    //
    //return worldPosition.xyz;
    return float3(1.f, 1.f, 1.f);

>>>>>>> parent of 122df45 (ìž‘ì—…ì¤‘)
}

float3 ConvertThreadIdToWorldPosition(uint3 id, uint3 dims)
{
	// id -> ndc
    float3 ndc = ConvertThreadIdToNdc(id, dims);
    float depth = ConvertNdcZToDepth(ndc.z);

    return ConvertToWorldPosition(ndc, depth);
}

<<<<<<< HEAD
float HenyeyGreensteinPhaseFunction(float3 wi, float3 wo, float g)
{
    float cosTheta = dot(wi, wo);
    float g2 = g * g;
    float denom = pow(1.f + g2 - 2.f * g * cosTheta, 3.f / 2.f);
    return (1.f / (4.f * Pi)) * ((1.f - g2) / max(denom, Epsilon));
}

float Visibility_Dir(float3 vWorldPosition, float3 toLight)
{
    //vector vPosition = mul(float4(vWorldPosition, 1.f), g_DirLightViewMatrix);
    //vPosition = mul(vPosition, g_ProjMatrix);
  
    //float2 vTexcoord;
    //vTexcoord.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
    //vTexcoord.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;

    //float4 vDepth = g_DepthTexture.SampleLevel(PointSamplerClamp, vTexcoord, 0);
    
    //if (vPosition.w >= vDepth.r * 1000)
    //{
    //    return 0;
    //}
    
    //vector vPosition2 = mul(float4(vWorldPosition, 1.f), g_ViewMatrix);
    //vPosition2 = mul(vPosition2, g_ProjMatrix);
  
    //float2 vTexcoord2;
    //vTexcoord2.x = (vPosition2.x / vPosition2.w) * 0.5f + 0.5f;
    //vTexcoord2.y = (vPosition2.y / vPosition2.w) * -0.5f + 0.5f;

    //float4 vDepth2 = g_DepthTexture.SampleLevel(PointSamplerClamp, vTexcoord2, 0);
    
    //if (vPosition2.w <= vDepth2.r * 1000)
    //{
    //    return 0;
    //}

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
    //vector vPosition = mul(float4(vWorldPosition, 1.f), g_SpotLightViewMatrix);
    //vPosition = mul(vPosition, g_SpotLightProjMatrix);
  
    //float2 vTexcoord;
    //vTexcoord.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
    //vTexcoord.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;
    
    //float4 vDepth = g_SpotLightDepthTexture.SampleLevel(PointSamplerClamp, vTexcoord, 0);
    
    //if (vPosition.w > vDepth.r * 1000)
    //{
    //    return 1;
    //}
    
    //return 0;
    
    
    //float3 viewPos = mul(float4(vWorldPosition, 1.f), g_ViewMatrix).xyz;
    
    float4 shadowCoord = mul(float4(vWorldPosition, 1.0f), g_SpotLightViewMatrix);
    shadowCoord = mul(float4(vWorldPosition, 1.0f), g_SpotLightProjMatrix);
    
    float3 uv_depth = shadowCoord.xyz / shadowCoord.w;
    uv_depth.y = -uv_depth.y;
    uv_depth.xy = uv_depth.xy * 0.5f + 0.5f;

    vector vDepth = g_SpotLightDepthTexture.SampleLevel(LinearSamplerClamp, uv_depth.xy, 0);
    
    return uv_depth.z > vDepth.r;

}
=======
>>>>>>> parent of 122df45 (ìž‘ì—…ì¤‘)

[numthreads(8, 8, 8)]
void CS_Volume( uint3 DTid : SV_DispatchThreadID )
{
    uint3 dims;
    
    OutputTexture.GetDimensions(dims.x, dims.y, dims.z);
    
    if (all(DTid < dims))
    {
<<<<<<< HEAD
        
        float3 worldPosition = ConvertThreadIdToWorldPosition(DTid, dims);
        float3 toCamera = normalize(g_vCamPosition.xyz - worldPosition);
        

        float3 lighting = float3(0.f, 0.f, 0.f);
        
        if (g_isShadowDirLight)
        {
            float3 lightDirection = normalize(g_vDirLightDirection.xyz);
            
            float3 toLight = -lightDirection;
            
            float visibility = Visibility_Dir(worldPosition, toLight); // ¼¨µµ¿ì ¸ÊÀ¸·Î °¡½Ã¼º °Ë»ç
            float phaseFunction = HenyeyGreensteinPhaseFunction(lightDirection, toCamera, 0);

            lighting += visibility * g_vDirLightDiffuse;
            //lighting += visibility * light.m_diffuse.rgb * light.m_diffuse.a * phaseFunction;
        }
      
        if (g_isShadowSpotLight)
        {
            
            //float3 lightDirection = normalize(worldPosition - g_vSpotLightPosition.xyz);
            //float visibility = Visibility_Spot(worldPosition); // ¼¨µµ¿ì ¸ÊÀ¸·Î °¡½Ã¼º °Ë»ç
            //float phaseFunction = HenyeyGreensteinPhaseFunction(lightDirection, toCamera, 0);
            
            //lighting += visibility * g_vSpotLightDiffuse;
        }
        
    
        
        OutputTexture[DTid] = float4(lighting, 1);
        //OutputTexture[DTid] = float4(float(DTid.x) / 128, float(DTid.y) / 128, float(DTid.z) / 128, 1);
        
    }
}

float SliceTickness(float ndcZ, uint dimZ)
{
    return ConvertNdcZToDepth(ndcZ + 1.f / float(dimZ)) - ConvertNdcZToDepth(ndcZ);
}

static const float DensityScale = 0.01f;

float4 ScatterStep(float3 accumulatedLight, float accumulatedTransmittance, float3 sliceLight, float sliceDensity, float tickness)
{
    sliceDensity = max(sliceDensity, 0.000001f);
    sliceDensity *= DensityScale;
    float sliceTransmittance = exp(-sliceDensity * tickness);

	// FrostbiteÀÇ ´©Àû ¹æ½Ä
    float3 sliceLightIntegral = sliceLight * (1.f - sliceTransmittance) / sliceDensity;

    accumulatedLight += sliceLightIntegral * accumulatedTransmittance;
    accumulatedTransmittance *= sliceTransmittance;

    return float4(accumulatedLight, accumulatedTransmittance);
}

[numthreads(8, 8, 8)]
void CS_Volume2(uint3 DTid : SV_DispatchThreadID)
{
    uint3 dims;
    OutputTexture.GetDimensions(dims.x, dims.y, dims.z);

    if (all(DTid < dims))
    {
        float4 accum = float4(0.f, 0.f, 0.f, 1.f);
=======
>>>>>>> parent of 122df45 (ìž‘ì—…ì¤‘)
        uint3 pos = uint3(DTid.xy, 0);

        //OutputTexture[pos] = float4(dims.x/1920 , dims.y/1080, dims.z, 1);
        OutputTexture[DTid] = g_fTest;

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
}