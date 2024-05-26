

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