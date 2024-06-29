#include "Globals.hlsli"

cbuffer decalCB : register(b0)
{
    SubModelInfo subModelInfo;
    float4 rayOrigin;
    float4 rayDir;
    float4 decalTangent;
    float4 hitDistances;
    float decalLookupRtWidth;
    float decalLookupRtHeight;
    float decalSizeX;
    float decalSizeY;
    float decalSizeZ;
    uint decalIndex;
    uint decalMaterialIndex;
    uint iMeshIndex;
};

StructuredBuffer<uint> indexBuffer;

RWStructuredBuffer<DecalInfo> decalInfoBuffer;
RWStructuredBuffer<SKINNING_OUTPUT> positionBuffer;

uint g_NumTris;

#define RAYCAST_THREAD_GROUP_SIZE 64 
#define EPSILON 0.0001f

float RayTriIntersect(in float3 rayOrigin, in float3 rayDir, in float3 pos0, in float3 pos1, in float3 pos2)
{
    float3 edge1 = pos1 - pos0;
    float3 edge2 = pos2 - pos0;

    float3 pvec = cross(rayDir, edge2);
    float det = dot(edge1, pvec);
  
  // back-face culling
    if (det < EPSILON)
        return -1.0f;

    float3 tvec = rayOrigin - pos0;
    float u = dot(tvec, pvec);
    if ((u < 0.0f) || (u > det))
        return -1.0f;

    float3 qvec = cross(tvec, edge1);
    float v = dot(rayDir, qvec);
    if ((v < 0.0f) || ((u + v) > det))
        return -1.0f;

    return (dot(edge2, qvec) / det);
}

[numthreads(RAYCAST_THREAD_GROUP_SIZE, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint triangleOffset = dispatchThreadID.x;

    if (triangleOffset < g_NumTris)
    {
        uint offset = triangleOffset * 3;
        uint vertexIndex0 = indexBuffer[offset];
        uint vertexIndex1 = indexBuffer[offset + 1];
        uint vertexIndex2 = indexBuffer[offset + 2];
   
        float3 pos0 = positionBuffer[vertexIndex0].vPosition;
        float3 pos1 = positionBuffer[vertexIndex1].vPosition;
        float3 pos2 = positionBuffer[vertexIndex2].vPosition;
    
        float hitDistance = RayTriIntersect(rayOrigin.xyz, rayDir.xyz, pos0, pos1, pos2);
        if (hitDistance >= 0.0f)
        {
            float hitDistanceN = (hitDistance - hitDistances.x) * hitDistances.z;
            uint iHitDistance = uint(hitDistanceN * 255.0f);

            uint decalHitMask = (iHitDistance << 24u) | (iMeshIndex << 20u) | triangleOffset;
            InterlockedMin(decalInfoBuffer[0].decalHitMask, decalHitMask);

            InterlockedMin(decalInfoBuffer[0].isHit, 0xfff);
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