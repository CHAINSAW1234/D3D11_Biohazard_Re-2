#include "Globals.hlsli"

cbuffer decalCB
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


RWStructuredBuffer<SKINNING_OUTPUT> positionBuffer;
StructuredBuffer<uint> indexBuffer;
RWStructuredBuffer<DecalInfo> decalInfoBuffer;

void BuildViewMatrix(in float3 position, in float3 dir, in float3 right, out matrix viewMatrix)
{
	float3 up = normalize(cross(right, dir));
	right = normalize(cross(dir, up));

	matrix rotMatrix;
	rotMatrix[0][0] = right.x;
	rotMatrix[0][1] = right.y;
	rotMatrix[0][2] = right.z;
	rotMatrix[0][3] = 0.0f;
	rotMatrix[1][0] = up.x;
	rotMatrix[1][1] = up.y;
	rotMatrix[1][2] = up.z;
	rotMatrix[1][3] = 0.0f;
	rotMatrix[2][0] = -dir.x;
	rotMatrix[2][1] = -dir.y;
	rotMatrix[2][2] = -dir.z;
	rotMatrix[2][3] = 0.0f;
	rotMatrix[3][0] = 0.0f;
	rotMatrix[3][1] = 0.0f;
	rotMatrix[3][2] = 0.0f;
	rotMatrix[3][3] = 1.0f;

	matrix transMatrix;
	transMatrix[0][0] = 1.0f;
	transMatrix[1][0] = 0.0f;
	transMatrix[2][0] = 0.0f;
	transMatrix[3][0] = 0.0f;
	transMatrix[0][1] = 0.0f;
	transMatrix[1][1] = 1.0f;
	transMatrix[2][1] = 0.0f;
	transMatrix[3][1] = 0.0f;
	transMatrix[0][2] = 0.0f;
	transMatrix[1][2] = 0.0f;
	transMatrix[2][2] = 1.0f;
	transMatrix[3][2] = 0.0f;
	transMatrix[0][3] = -position.x;
	transMatrix[1][3] = -position.y;
	transMatrix[2][3] = -position.z;
	transMatrix[3][3] = 1.0f;

	viewMatrix = mul(rotMatrix, transMatrix);
}

void BuildOrthoProjMatrix(in float width, in float height, in float near, in float far, out matrix projMatrix)
{
	projMatrix[0][0] = 2.0f / width;
	projMatrix[1][0] = 0.0f;
	projMatrix[2][0] = 0.0f;
	projMatrix[3][0] = 0.0f;
	projMatrix[0][1] = 0.0f;
	projMatrix[1][1] = 2.0f / height;
	projMatrix[2][1] = 0.0f;
	projMatrix[3][1] = 0.0f;
	projMatrix[0][2] = 0.0f;
	projMatrix[1][2] = 0.0f;
	projMatrix[2][2] = -1.0f / (far - near);
	projMatrix[3][2] = 0.0f;
	projMatrix[0][3] = 0.0f;
	projMatrix[1][3] = 0.0f;
	projMatrix[2][3] = -near / (far - near);
	projMatrix[3][3] = 1.0f;
}

void BuildTextureMatrix(out matrix textureMatrix)
{
	textureMatrix[0][0] = 0.5f;
	textureMatrix[1][0] = 0.0f;
	textureMatrix[2][0] = 0.0f;
	textureMatrix[3][0] = 0.0f;
	textureMatrix[0][1] = 0.0f;
	textureMatrix[1][1] = -0.5f;
	textureMatrix[2][1] = 0.0f;
	textureMatrix[3][1] = 0.0f;
	textureMatrix[0][2] = 0.0f;
	textureMatrix[1][2] = 0.0f;
	textureMatrix[2][2] = 0.5f;
	textureMatrix[3][2] = 0.0f;
	textureMatrix[0][3] = 0.5f;
	textureMatrix[1][3] = 0.5f;
	textureMatrix[2][3] = 0.5f;
	textureMatrix[3][3] = 1.0f;
}

[numthreads(1, 1, 1)]
void main()
{
	uint decalHitMask = decalInfoBuffer[0].decalHitMask;

	uint triangleOffset = decalHitMask & 0xfffff;
	uint meshIndex = (decalHitMask >> 20u) & 0xf;
	uint iHitDistance = (decalHitMask >> 24u) & 0xff;

	decalInfoBuffer[0].decalIndex = decalIndex;

	float2 rtSize;
	rtSize.x = decalLookupRtWidth;
	rtSize.y = decalLookupRtHeight;
	float2 texSize;
	texSize.x = subModelInfo.decalLookupMapWidth;
	texSize.y = subModelInfo.decalLookupMapHeight;
	decalInfoBuffer[0].decalScale = texSize / rtSize;
	decalInfoBuffer[0].decalBias.x = (texSize.x - rtSize.x) / rtSize.x;
	decalInfoBuffer[0].decalBias.y = (rtSize.y - texSize.y) / rtSize.y;

	// calculate hit position
	float hitDistanceN = float(iHitDistance) / 255.0f;
	float hitDistance = hitDistanceN * hitDistances.y + hitDistances.x;
	float3 hitPosition = rayDir.xyz * hitDistance + rayOrigin.xyz;

	// calculate hit normal
	uint firstIndex = subModelInfo.firstIndex;
	uint offset = triangleOffset * 3 + firstIndex;
	uint vertexIndex0 = indexBuffer[offset];
	uint vertexIndex1 = indexBuffer[offset + 1];
	uint vertexIndex2 = indexBuffer[offset + 2];
	float3 pos0 = positionBuffer[vertexIndex0].vPosition;
	float3 pos1 = positionBuffer[vertexIndex1].vPosition;
	float3 pos2 = positionBuffer[vertexIndex2].vPosition;
	float3 edge1 = pos1 - pos2;
	float3 edge2 = pos0 - pos2;
	float3 normal = normalize(cross(edge2, edge1));
	decalInfoBuffer[0].decalNormal = normal;

	// calculate decal matrix
	matrix viewMatrix;
	float3 viewPosition = normal * decalSizeZ * 0.5f + hitPosition;
	BuildViewMatrix(viewPosition, -normal, decalTangent.xyz, viewMatrix);
	matrix projMatrix;
	BuildOrthoProjMatrix(decalSizeX, decalSizeY, 0.0f, decalSizeZ, projMatrix);
	matrix textureMatrix;
	BuildTextureMatrix(textureMatrix);
	decalInfoBuffer[0].decalMatrix = mul(textureMatrix, mul(projMatrix, viewMatrix));

}

technique11 Default
{
	pass Default
	{
		SetComputeShader(CompileShader(cs_5_0, main()));
	}
}