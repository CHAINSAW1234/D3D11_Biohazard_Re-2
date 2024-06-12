
#include "Engine_Shader_Defines.hlsli"

/* �������� : ���̴� �ܺο� �ִ� �����͸� ���̴� ������ �޾ƿ´�. */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_Texture;
texture2D g_MaskTexture;

// PS
bool g_ColorChange;
float4 g_ColorValu;
bool g_AlpaChange;

//VS
bool g_Wave;
float gTime;
float time_factor = 0.5f;
float g_Speed = 0.05f;

float g_UVPush_Timer;
float2 g_UVSpeed;
bool g_isPush;
float g_RotationSpeed;
float g_Split = 1;
bool g_Blending;
float g_BlendingStrength = 0.5f;

// Mask
bool g_isMask;
float2 g_fMaskControl;
float g_fMaskSpeed;
float g_fMaskTime;
float2 g_MaskType;


// Client 
bool g_isLightMask;
float4 g_vLightMask_Color;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

/* ���� ���̴� */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    if (g_Wave)
    {
        Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
        Out.vPosition.x += cos(gTime + Out.vPosition.x + Out.vPosition.y) * g_Speed;
        Out.vPosition.y += sin(gTime + Out.vPosition.y + +Out.vPosition.x) * g_Speed;

        float2 distortedTexcoord = In.vTexcoord;

        distortedTexcoord.x += cos(gTime * time_factor) * g_Speed; // X�� �ﷷ��
        distortedTexcoord.y += sin(gTime * time_factor) * g_Speed; // Y�� �ﷷ��

        Out.vTexcoord = distortedTexcoord;
        Out.vTexcoord = In.vTexcoord;
    }

    if (g_isPush)
    {
        if (!g_Wave)
        {
            Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
            Out.vTexcoord = In.vTexcoord;
         // ���� UV ��ǥ
            float2 uv = In.vTexcoord;

         // ȸ�� ���� (���� ����)
            float angle = g_UVPush_Timer * g_RotationSpeed;

         // ȸ�� ��� ���
            float cosTheta = cos(angle);
            float sinTheta = sin(angle);
            float2x2 rotationMatrix = float2x2(cosTheta, -sinTheta, sinTheta, cosTheta);

         // UV ��ǥ ȸ�� ��ȯ
            uv = mul(uv, rotationMatrix);
         
            uv.r += g_UVPush_Timer * g_UVSpeed.x;
            uv.g += g_UVPush_Timer * g_UVSpeed.y;
         
            Out.vTexcoord.r = uv.r;
            Out.vTexcoord.g = uv.g;
        }
        else if (g_Wave)
        {
            float2 uv = Out.vTexcoord;
         // ȸ�� ���� (���� ����)
            float angle = g_UVPush_Timer * g_RotationSpeed;

         // ȸ�� ��� ���
            float cosTheta = cos(angle);
            float sinTheta = sin(angle);
            float2x2 rotationMatrix = float2x2(cosTheta, -sinTheta, sinTheta, cosTheta);

         // UV ��ǥ ȸ�� ��ȯ
            uv = mul(uv, rotationMatrix);
         
            uv.r += g_UVPush_Timer * g_UVSpeed.x;
            uv.g += g_UVPush_Timer * g_UVSpeed.y;
         
            Out.vTexcoord.r = uv.r;
            Out.vTexcoord.g = uv.g;
        }
    }

    if (!g_isPush && !g_Wave)
    {
        Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
        Out.vTexcoord = In.vTexcoord;
    }

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord * g_Split);
    
    if (g_AlpaChange) // ���ĸ� �ٲ� ��
        Out.vColor.a = g_ColorValu.a;
    
    else if (g_ColorChange)
    {
        if (g_Blending)
        {
            Out.vColor = lerp(Out.vColor, g_ColorValu, g_BlendingStrength);
        }
        else
            Out.vColor = g_ColorValu;
    }
         
    if (g_isMask)
    {
        float2 MaskTexcoord = In.vTexcoord;
        
        MaskTexcoord.r += g_fMaskTime * g_MaskType.x;
        MaskTexcoord.g += g_fMaskTime * g_MaskType.y;

        float value = g_MaskTexture.Sample(LinearSampler, MaskTexcoord).r;
         
        /* Light Mask */
        /* HP Bar�� ���Ƿ� ���� ��, : ���߿� �� �� ���� �� �ٲ� �� */
        float blendFactor = 1.0 - value;
        float4 LightColor = float4(min(g_vLightMask_Color.r, 1.0f), min(g_vLightMask_Color.g + 0.5f, 1.0f), min(g_vLightMask_Color.b + 0.2f, 1.0f), Out.vColor.a);
        float4 finalColor = lerp(Out.vColor, LightColor, blendFactor);
        
        if (true == g_isLightMask)
        {
            Out.vColor = finalColor;
        }
        
        else if (false == g_isLightMask)
        {
            float alpha = smoothstep(g_fMaskControl.x, g_fMaskControl.x + g_fMaskControl.y, value + (1.0 - g_fMaskControl.y));
            
            Out.vColor.rgb = finalColor.rgb;
            Out.vColor.a *= alpha;
        }

    }
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Blend
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}