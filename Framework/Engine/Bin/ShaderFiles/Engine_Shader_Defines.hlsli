
#define SCREEN_SIZE_X 1600
#define SCREEN_SIZE_Y 900
#define PI 3.14159265359f

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = wrap;
    AddressV = wrap;
};

sampler LinearSamplerClamp = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = clamp;
    AddressV = clamp;
};

sampler PointSamplerClamp = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = clamp;
    AddressV = clamp;
};
RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = false;
};

RasterizerState RS_NoCulling
{
    FillMode = Solid;
    CullMode = None;
    FrontCounterClockwise = false;
};

RasterizerState RS_CullReverse
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = true;
};

RasterizerState RS_Wireframe
{
    FillMode = Wireframe;
    CullMode = None;
    FrontCounterClockwise = false;
};

RasterizerState RS_Sky
{
    FillMode = Solid;
    CullMode = Front;
    FrontCounterClockwise = false;
};

RasterizerState RS_2D
{
    FillMode = Solid;
    CullMode = None;
};

DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;
};


DepthStencilState DSS_Sky
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

DepthStencilState DSS_NO_TEST_WRITE
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

DepthStencilState DSS_NO_WRITE
{
    DepthEnable = true;
    DepthWriteMask = zero;
};

BlendState BS_Default
{
    BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;

    SrcBlend = src_alpha;
    DestBlend = inv_src_alpha;
    BlendOp = Add;
};

BlendState BS_Blend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = one;
    DestBlend = one;
    BlendOp = Add;
};


float DistributeGGX(float3 N, float3 H, float a)
{
    float a_square = a * a;
    float a_square_square = a_square * a_square;
    float NdotH = max(dot(N, H), 0.f);
    float NdotH_square = NdotH * NdotH;

    float nom = a_square;
    float denom = (NdotH * a_square - NdotH) * NdotH + 1;
    //float denom = (NdotH * NdotH * (a_square - 1.f) + 1.f);
    denom = PI * denom * denom;
    
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float k)
{
    float nom = NdotV;
    float denom = NdotV * (1.f - k) + k;
    
    return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float k)
{
    float r = k + 1.0;
    float k2 = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
    
    float NdotV = max(dot(N, V), 0.f);
    float NdotL = max(dot(N, L), 0.f);
    float ggx1 = GeometrySchlickGGX(NdotV, k2);
    float ggx2 = GeometrySchlickGGX(NdotL, k2);

    return ggx1 * ggx2;
}

float3 FresnelSchlick(float cos_theta, float3 F0)
{
    // cos_theta : dot(N. V)
    return F0 + (1.f - F0) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.f);
}
