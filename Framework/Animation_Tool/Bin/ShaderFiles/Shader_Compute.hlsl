#include "Engine_Shader_Defines.hlsli"

RWTexture3D<float4> OutputTexture : register(u0);


float4 g_fTest;


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
    //float depthPackExponent = VolumetricFogParam.DepthPackExponent;
    //float nearPlaneDist = VolumetricFogParam.NearPlaneDist;
    //float farPlaneDist = VolumetricFogParam.FarPlaneDist;

    //return pow(ndcZ, depthPackExponent) * (farPlaneDist - nearPlaneDist) + nearPlaneDist;
    return 1;
}

float3 ConvertToWorldPosition(float3 ndc, float depth)
{
	//// view ray 
	//// ndc좌표에 대한 카메라 공간 광선을 계산
    //float4 viewRay = mul(float4(ndc, 1.f), InvProjectionMatrix);
    //viewRay /= viewRay.w;
    //viewRay /= viewRay.z; // z값이 1이 되도록
    //
	//// ndc -> world position
    //float4 worldPosition = mul(float4(viewRay.xyz * depth, 1.f), InvViewMatrix);
    //
    //return worldPosition.xyz;
    return float3(1.f, 1.f, 1.f);

}

float3 ConvertThreadIdToWorldPosition(uint3 id, uint3 dims)
{
	// id -> ndc
    float3 ndc = ConvertThreadIdToNdc(id, dims);
    float depth = ConvertNdcZToDepth(ndc.z);

    return ConvertToWorldPosition(ndc, depth);
}


[numthreads(8, 8, 8)]
void CS_Volume( uint3 DTid : SV_DispatchThreadID )
{
    uint3 dims;
    
    OutputTexture.GetDimensions(dims.x, dims.y, dims.z);
    
    if (all(DTid < dims))
    {
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