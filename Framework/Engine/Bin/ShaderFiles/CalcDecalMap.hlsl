#include "Globals.hlsli"

matrix g_DecalMat_Inv;
uint g_NumVertices;
RWStructuredBuffer<float2> g_DecalMap;
float3  g_Extent;

RWStructuredBuffer<SKINNING_OUTPUT> g_Skinnig_Output;

StructuredBuffer<float3>    g_VertexBuffer;
#define CALC_DECAL_MAP_THREAD_GROUP_SIZE 64 

[numthreads(CALC_DECAL_MAP_THREAD_GROUP_SIZE, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint vertexIndex = dispatchThreadID.x;
    if (vertexIndex < g_NumVertices)
    {
        float4 vPosition = float4(g_Skinnig_Output[vertexIndex].vPosition.x, g_Skinnig_Output[vertexIndex].vPosition.y, g_Skinnig_Output[vertexIndex].vPosition.z, 1.f); 
        vPosition = mul(vPosition, g_DecalMat_Inv);

        float2 DecalUV = (vPosition.xz / (2.0f * float2(g_Extent.x, g_Extent.z))) + 0.5f;

        if ((-g_Extent.x <= vPosition.x && vPosition.x <= g_Extent.x) &&
            (-g_Extent.y <= vPosition.y && vPosition.y <= g_Extent.y) &&
            (-g_Extent.z <= vPosition.z && vPosition.z <= g_Extent.z))
        {
            if(g_DecalMap[vertexIndex].x - 0.f < 0.001f && g_DecalMap[vertexIndex].y - 0.f < 0.001f)
                g_DecalMap[vertexIndex] = DecalUV;
            else
            {
                float2 center = float2(0.5f, 0.5f);
                float distance = length(DecalUV - center);

                if (distance < 0.1f)
                {
                    g_DecalMap[vertexIndex] = DecalUV;
                }
            }
        }
    }
}

[numthreads(CALC_DECAL_MAP_THREAD_GROUP_SIZE, 1, 1)]
void main_static(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint vertexIndex = dispatchThreadID.x;
    if (vertexIndex < g_NumVertices)
    {
        float4 vPosition = float4(g_VertexBuffer[vertexIndex].x, g_VertexBuffer[vertexIndex].y, g_VertexBuffer[vertexIndex].z, 1.f);
        vPosition = mul(vPosition, g_DecalMat_Inv);

        float2 DecalUV = (vPosition.xz / (2.0f * float2(g_Extent.x, g_Extent.z))) + 0.5f;

        if ((-g_Extent.x <= vPosition.x && vPosition.x <= g_Extent.x) &&
            (-g_Extent.y <= vPosition.y && vPosition.y <= g_Extent.y) &&
            (-g_Extent.z <= vPosition.z && vPosition.z <= g_Extent.z))
        {
            /*if (g_DecalMap[vertexIndex].x - 0.f < 0.001f && g_DecalMap[vertexIndex].y - 0.f < 0.001f)
                g_DecalMap[vertexIndex] = DecalUV;
            else
            {
                float2 center = float2(0.5f, 0.5f);
                float distance = length(DecalUV - center);

                if (distance < 0.1f)
                {
                    g_DecalMap[vertexIndex] = DecalUV;
                }
            }*/

            g_DecalMap[vertexIndex] = DecalUV;
        }
    }
}

technique11 Default
{
    pass Default
    {
        SetComputeShader(CompileShader(cs_5_0, main()));
    }

    pass Static
    {
        SetComputeShader(CompileShader(cs_5_0, main_static()));
    }
}