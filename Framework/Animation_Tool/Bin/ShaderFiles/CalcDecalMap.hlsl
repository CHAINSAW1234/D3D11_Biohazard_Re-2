#include "Globals.hlsli"

matrix g_DecalMat_Inv;
uint g_NumVertices;
RWStructuredBuffer<float2> g_DecalMap;
StructuredBuffer<float2> g_Texcoords;
float3  g_Extent;

RWStructuredBuffer<SKINNING_OUTPUT> g_Skinnig_Output;

#define CALC_DECAL_MAP_THREAD_GROUP_SIZE 64 

[numthreads(CALC_DECAL_MAP_THREAD_GROUP_SIZE, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint vertexIndex = dispatchThreadID.x;
    if (vertexIndex < g_NumVertices)
    {
        float4 vPosition = float4(g_Skinnig_Output[vertexIndex].vPosition, 1.f);
        vPosition = mul(g_DecalMat_Inv, vPosition);

        float2 DecalUV = vPosition.xy*0.5f + 0.5f;

        if ((-g_Extent.x <= vPosition.x && vPosition.x <= g_Extent.x) &&
            (-g_Extent.y <= vPosition.y && vPosition.y <= g_Extent.y) &&
            (-g_Extent.z <= vPosition.z && vPosition.z <= g_Extent.z))
        {
            g_DecalMap[vertexIndex] = float2(2.f,2.f);
        }
    }
}


technique11 Default
{
    pass Default
    {
        SetComputeShader(CompileShader(cs_5_0, main()));
    }
}