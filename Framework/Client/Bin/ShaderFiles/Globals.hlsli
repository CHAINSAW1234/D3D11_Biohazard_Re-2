#define MAX_NUM_SUBMODELS 16
#define MAX_NUM_SKINNED_DECALS 256
#define MAX_NUM_DECAL_MATERIALS 8
#define MAX_NUM_DECAL_TEXTURES (MAX_NUM_DECAL_MATERIALS * 3)
#define PI 3.14159265f

typedef float4 Quat;

struct CameraConstData
{
    matrix viewMatrix;
    matrix projMatrix;
    matrix viewProjMatrix;
    matrix invViewProjMatrix;
    float3 position;
    float aspectRatio;
    float nearClipDistance;
    float farClipDistance;
    float nearFarClipDistance;
};

struct ModelConstData
{
    matrix transformMatrix;
    uint numVertices;
    uint debugDecalMask;
};

struct SubModelInfo
{
    uint firstIndex;
    uint numTris;
    float decalLookupMapWidth;
    float decalLookupMapHeight;
};

struct LightingConstData
{
    float4 dirLightDirection;
    float4 dirLightColor;
};

struct DrawIndirectCmd
{
    uint indexCountPerInstance;
    uint instanceCount;
    uint startIndexLocation;
    int baseVertexLocation;
    uint startInstanceLocation;
};

// workaround for compiler bug
struct Index
{
    uint value;
};

struct DecalInfo
{
    uint decalHitMask; // bit 0-19: triangleIndex, bit 20-23: subModelIndex, bit 24-31: hitDistance
    uint decalIndex;
    float2 decalScale;
    float2 decalBias;
    float3 decalNormal;
    float4x4 decalMatrix;
    uint isHit;
};

struct DecalMeshInfo
{
    uint decalMeshIndex;
};

struct WeightIndex
{
    uint firstIndex;
    uint numIndices;
};

struct Weight
{
    uint jointIndex;
    float weight;
    float3 position;
    float3 normal;
    float3 tangent;
};

struct Joint
{
    float3 translation;
    Quat rotation;
};

float3 MulQuatVec(in Quat quat, in float3 vec)
{
    float3 t = 2.0f * cross(quat.xyz, vec);
    return (vec + t * quat.w + cross(quat.xyz, t));
}

uint EncodeDecalInfo(in float2 decalTC, in float fade, in uint decalIndex)
{
    uint2 iDecalTC = uint2(decalTC * 255.0f);
    uint iFade = uint(fade * 255.0f);
    return ((decalIndex << 24u) | (iFade << 16u) | (iDecalTC.x << 8u) | iDecalTC.y);
}

void DecodeDecalInfo(in uint decalInfo, out float3 decalTCFade, out uint decalIndex)
{
    uint4 decalValue;
    decalValue.x = (decalInfo >> 8u) & 0xff;
    decalValue.y = decalInfo & 0xff;
    decalValue.z = (decalInfo >> 16u) & 0xff;
    decalValue.w = (decalInfo >> 24u) & 0xff;
    decalTCFade = float3(decalValue.xyz) / 255.0f;
    decalIndex = decalValue.w;
}

uint GetDecalIndex(in uint decalInfo)
{
    return ((decalInfo >> 24u) & 0xff);
}

typedef
struct
{
    float3 vPosition;
    float3 vNormal;
    float3 vTangent;

} SKINNING_OUTPUT;