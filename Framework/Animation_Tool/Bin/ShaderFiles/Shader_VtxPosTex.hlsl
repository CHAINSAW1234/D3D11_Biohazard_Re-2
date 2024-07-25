
#include "Engine_Shader_Defines.hlsli"

/* 전역변수 : 쉐이더 외부에 있는 데이터를 쉐이더 안으로 받아온다. */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_Texture;
texture2D g_MaskTexture;
texture2D g_MaskSub_Texture;
texture2D g_DepthTexture;

bool g_SelectColor, g_GreenColor;
float fDepthValue = 1000.f;

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

// Client // 
float4 g_vLightMask_Color;
bool g_isLightMask;

// Light
bool g_isLight;
float2 g_LightPosition;
float g_LightSize;

bool g_isBlackChange;

float2 g_maskCenter = float2(0.5, 0.5); 
float g_maskRadius = 1; 
bool g_maskCircle = false;

// 거리 계산 함수
float calculateDistance(float4 A, float4 target)
{
    return sqrt(
        pow(A.x - target.x, 2) +
        pow(A.y - target.y, 2) +
        pow(A.z - target.z, 2) +
        pow(A.w - target.w, 2)
    );
}
float AdjustAlpha(float4 color)
{
    float4 target = { 0.0f, 0.0f, 0.0f, 1.0f };
    float distance = calculateDistance(color, target);
    return exp(distance); // distance가 0에 가까워질수록 B는 1에 가까워짐
}

float4 MaskLight(float2 vTexcoord, float4 color, float blendFactor)
{
    float d = length(vTexcoord);
    
    d = abs(d);
    smoothstep(0, 1.f, d);
    
    float alphaA = AdjustAlpha(color);
    float4 lightColor = float4(d, d, d, alphaA);
    
    return lerp(lightColor, color, blendFactor);
}

float4 Light(float2 vTexcoord, float4 color)
{
    float2 uv = (vTexcoord - float2(g_LightPosition.x, g_LightPosition.y));
    float d = length(uv);
    d -= sin(d * 1.f) / g_LightSize;
    d = abs(d);
    smoothstep(0, 1.f, d);
    
    float alphaA = AdjustAlpha(color);
    return float4(d, d, d, alphaA);
}

float4 ModifyAlpha(float4 color, float3 lightColor, float2 texcoord)
{
    color *= Light(texcoord, color);

    float Intensity = length(color.rgb);
    
    float smoothAlpha = smoothstep(0.0, 1.0, Intensity);
    color.a *= smoothAlpha;

    return color;
}

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

/* 정점 쉐이더 */
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

        distortedTexcoord.x += cos(gTime * time_factor) * g_Speed; // X축 울렁임
        distortedTexcoord.y += sin(gTime * time_factor) * g_Speed; // Y축 울렁임

        Out.vTexcoord = distortedTexcoord;
        Out.vTexcoord = In.vTexcoord;
    }

    if (g_isPush)
    {
        if (!g_Wave)
        {
            Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
            Out.vTexcoord = In.vTexcoord;
         // 기존 UV 좌표
            float2 uv = In.vTexcoord;

         // 회전 각도 (라디안 단위)
            float angle = g_UVPush_Timer * g_RotationSpeed;

         // 회전 행렬 계산
            float cosTheta = cos(angle);
            float sinTheta = sin(angle);
            float2x2 rotationMatrix = float2x2(cosTheta, -sinTheta, sinTheta, cosTheta);

         // UV 좌표 회전 변환
            uv = mul(uv, rotationMatrix);
         
            uv.r += g_UVPush_Timer * g_UVSpeed.x;
            uv.g += g_UVPush_Timer * g_UVSpeed.y;
         
            Out.vTexcoord.r = uv.r;
            Out.vTexcoord.g = uv.g;
        }
        else if (g_Wave)
        {
            float2 uv = Out.vTexcoord;
         // 회전 각도 (라디안 단위)
            float angle = g_UVPush_Timer * g_RotationSpeed;

         // 회전 행렬 계산
            float cosTheta = cos(angle);
            float sinTheta = sin(angle);
            float2x2 rotationMatrix = float2x2(cosTheta, -sinTheta, sinTheta, cosTheta);

         // UV 좌표 회전 변환
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
    
   // 선택
    if (g_SelectColor)
        Out.vColor = float4(1, 0, 0, 1);
    
    else if (g_ColorChange)
    {
        if (true == g_isBlackChange) /* Black 만 바꾸고 싶다면 */ 
        {
            float4 BlackColor = float4(0, 0, 0, 1);
            
            if (!(distance(Out.vColor, BlackColor) < 0.2f))
            {
                Out.vColor = lerp(Out.vColor, g_ColorValu, g_BlendingStrength);
                Out.vColor.a = lerp(Out.vColor.a, g_ColorValu.a, 0);
            }
        }
        
        else if (false == g_isBlackChange) 
        {
             if (g_Blending)
             {
                 if (Out.vColor.a <= 0.0f)
                     discard;
                 
                 Out.vColor = lerp(Out.vColor, g_ColorValu, g_BlendingStrength);
                 Out.vColor.a = lerp(Out.vColor.a, g_ColorValu.a, 0);
             }
             else
                 Out.vColor = g_ColorValu;
        }
    }
         
    if (g_isLight)
    {
        float blackRatio = (Out.vColor.a) / 1.f;
        
        Out.vColor = ModifyAlpha(Out.vColor, blackRatio, In.vTexcoord);

    }
    
    if (g_isMask)
    {
        if (true == g_isLightMask)
        {
            float2 MaskTexcoord = In.vTexcoord;
        
            MaskTexcoord.r += g_fMaskTime * g_MaskType.x;
            MaskTexcoord.g += g_fMaskTime * g_MaskType.y;
            
            float value = g_MaskTexture.Sample(LinearSampler, MaskTexcoord).r;
         
            /* Light Mask */
            float blendFactor = 1.0 - value;
            float4 LightColor = float4(min(g_vLightMask_Color.r, 1.0f), min(g_vLightMask_Color.g, 1.0f), min(g_vLightMask_Color.b, 1.0f), Out.vColor.a);
            float4 finalColor = lerp(Out.vColor, LightColor, blendFactor);
            
            finalColor *= MaskLight(In.vTexcoord, finalColor, blendFactor);
            Out.vColor = finalColor;
            
           
        }
        else if (false == g_isLightMask)
        {
            float2 MaskTexcoord = In.vTexcoord;
        
            MaskTexcoord.r += g_fMaskTime * g_MaskType.x;
            MaskTexcoord.g += g_fMaskTime * g_MaskType.y;

            float value = g_MaskTexture.Sample(LinearSampler, MaskTexcoord).r;
            float alpha = smoothstep(g_fMaskControl.x, g_fMaskControl.x + g_fMaskControl.y, value + (1.0 - g_fMaskControl.y));
            Out.vColor *= float4(Out.vColor.rgb, alpha);
        }
    }
    
    if (true == g_maskCircle)
    {
        float2 MaskTexcoord = In.vTexcoord - g_maskCenter;
        
        float distance = length(MaskTexcoord);
        
        float angle = atan2(MaskTexcoord.y, MaskTexcoord.x); 

        float maskRadius = g_maskRadius + g_fMaskTime * 0.5f;

        float Radvalue = saturate((maskRadius - distance) / maskRadius);
        
        Radvalue *= saturate(sin(angle * g_MaskType.y - g_fMaskTime * g_MaskType.x + 3.14 / 2.0f) * 0.5 + 0.5); // 위쪽 센터부터 시작하도록 각도 조정

        float alpha = Out.vColor.a;

        float3 baseColor = Out.vColor.rgb;

        float3 finalColor = lerp(baseColor, float3(0.8, 0.8, 0.8), Radvalue * (1.0 - (MaskTexcoord.y / maskRadius)));
        
        if (Out.vColor.r == 1.f && Out.vColor.g == 1.f && Out.vColor.b == 1.f && Out.vColor.a == 1.f)
            Out.vColor = float4(1.f, 1.f, 1.f, 1.f);
        else
            Out.vColor = float4(finalColor, alpha);
    }
    
     return Out;
}

technique11 DefaultTechnique
{
    pass Default
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