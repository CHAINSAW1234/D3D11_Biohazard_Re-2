#include "globals.shi"

matrix g_BoneMatrices[512];
matrix g_WorldMatrix;
uint g_NumVertices;


StructuredBuffer<float3> g_VertexPositions;
StructuredBuffer<float3> g_VertexNormals;
StructuredBuffer<float3> g_VertexTangents;
StructuredBuffer<uint4> g_VertexBlendIndices;
StructuredBuffer<float4> g_VertexBlendWeights;

RWStructuredBuffer<SKINNING_OUTPUT> g_Skinnig_Output;

#define SKINNING_THREAD_GROUP_SIZE 64 

[numthreads(SKINNING_THREAD_GROUP_SIZE, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint vertexIndex = dispatchThreadID.x;
    if (vertexIndex < g_NumVertices)
    {
        float3 vertexPosition = float3(0.0f, 0.0f, 0.0f);
        float3 vertexNormal = float3(0.0f, 0.0f, 0.0f);
        float3 vertexTangent = float3(0.0f, 0.0f, 0.0f);
        
        float fTotalWeight = 1.f;
    
        matrix BoneMatrix = g_BoneMatrices[g_VertexBlendIndices[vertexIndex].x] * g_VertexBlendWeights[vertexIndex].x / fTotalWeight +
		g_BoneMatrices[g_VertexBlendIndices[vertexIndex].y] * g_VertexBlendWeights[vertexIndex].y / fTotalWeight +
		g_BoneMatrices[g_VertexBlendIndices[vertexIndex].z] * g_VertexBlendWeights[vertexIndex].z / fTotalWeight +
		g_BoneMatrices[g_VertexBlendIndices[vertexIndex].w] * g_VertexBlendWeights[vertexIndex].w / fTotalWeight;

        vector vPosition = mul(vector(g_VertexPositions[vertexIndex], 1.f), BoneMatrix);
        vector vNormal = mul(vector(g_VertexNormals[vertexIndex], 0.f), BoneMatrix);

        matrix matWV, matWVP;

        vertexPosition = mul(vPosition, g_WorldMatrix);
        vertexNormal = normalize(mul(vNormal, g_WorldMatrix));
        vertexTangent = normalize(mul(float4(g_VertexTangents[vertexIndex], 0.f), g_WorldMatrix)).xyz;
        
        g_Skinnig_Output[vertexIndex].vPosition = vertexPosition;
        g_Skinnig_Output[vertexIndex].vNormal = vertexNormal;
        g_Skinnig_Output[vertexIndex].vTangent = vertexTangent;
    }
}


technique11 Default
{
    pass Default
    {
        SetComputeShader(CompileShader(cs_5_0, main()));
    }
}