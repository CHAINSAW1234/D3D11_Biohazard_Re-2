#include "Engine_Shader_Defines.hlsli"

/* 전역변수 : 쉐이더 외부에 있는 데이터를 쉐이더 안으로 받아온다. */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_Texture;
texture2D g_BlackTexture;
texture2D g_DistortionTexture;
float g_DistortionAmount;


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

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

//struct PS_OUT
//{
//	float4		vColor : SV_TARGET0;
//};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

static const float Weight[13] =
{
    0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
};

#define KERNEL_SIZE 32
#define KERNEL_SIZE_EMISSIVE 13
#define SIGMA 100.0
#define SIGMA_EMISSIVE 200.0
#define RADIUS 2.0 // 커널의 반지름
float Gaussian(float x, float sigma)
{
    return exp(-0.5 * (x * x) / (sigma * sigma));
}
//static const float Total = 6.2108;
static const float Total = 1.2108;

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 result = float4(0.0, 0.0, 0.0, 0.0); // 결과 컬러 초기화
    float totalWeight = 0.0; // 총 가중치 초기화
    float2 t = In.vTexcoord;
    float2 uv = 0;
    float tu = 1.f / 800.f;
    for (int i = -KERNEL_SIZE / 2; i <= KERNEL_SIZE / 2; i++)
    {
        for (int j = -KERNEL_SIZE / 2; j <= KERNEL_SIZE / 2; j++)
        {
			// 현재 픽셀 위치 계산
            uv = t + float2(tu * i, tu * j);
            float weight = Gaussian(length(float2(i, j)), SIGMA);
            result += g_Texture.Sample(LinearSampler_Clamp, uv) * weight; // 가중치 적용
            totalWeight += weight; // 총 가중치 업데이트
        }
    }
    result /= totalWeight;


    float4 result2 = float4(0.0, 0.0, 0.0, 0.0); // 결과 컬러 초기화
    float totalWeight2 = 0.0; // 총 가중치 초기화
    float2 t2 = In.vTexcoord;
    float2 uv2 = 0;
    float tu2 = 1.f / (600.f * 0.5f);

    for (int i = -KERNEL_SIZE / 2; i <= KERNEL_SIZE / 2; i++)
    {
        for (int j = -KERNEL_SIZE / 2; j <= KERNEL_SIZE / 2; j++)
        {
			// 현재 픽셀 위치 계산
            uv2 = t2 + float2(tu2 * j, tu2 * i);
            float weight2 = Gaussian(length(float2(j, i)), SIGMA);
            result2 += g_Texture.Sample(LinearSampler_Clamp, uv2) * weight2; // 가중치 적용
            totalWeight2 += weight2; // 총 가중치 업데이트
        }
    }
    result2 /= totalWeight2;
    Out.vColor = result + result2;

    return Out;
}

PS_OUT PS_MAIN_X_BLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float4 vOut = 0;
    float2 t = In.vTexcoord;
    float2 uv = 0;
    float TotalWeight = 0;
    float tu = 1.f / 640.f;

    for (int i = -35; i <= 35; ++i)
    {
        uv = t + float2(tu * i, 0);
        float weight = Gaussian(float(i), 400);
        vOut += weight * g_Texture.Sample(LinearSampler_Clamp, uv);
        TotalWeight += weight;
    }
    TotalWeight *= 0.6f;
    vOut /= TotalWeight;
    Out.vColor = vOut;

    return Out;
}

PS_OUT PS_MAIN_Y_BLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float4 vOut = 0;
    float2 t = In.vTexcoord;
    float2 uv = 0;
    float TotalWeight = 0;
    float tu = 1.f / (640.f * 0.7f);

    for (int i = -35; i <= 35; ++i)
    {
        uv = t + float2(0, tu * i);
        float weight = Gaussian(float(i), 400);
        vOut += weight * g_Texture.Sample(LinearSampler_Clamp, uv);
        TotalWeight += weight;
    }

    vOut /= TotalWeight;
	
    Out.vColor = vOut;
    return Out;
}

PS_OUT PS_MAIN_X_BLUR_EMISSIVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float4 vOut = 0;
    float2 t = In.vTexcoord;
    float2 uv = 0;
    float TotalWeight = 0;
    float tu = 1.f / 400.f;
	
	
    float radius = 1.f / 640.f; // 샘플링 반경을 설정합니다. 이 값은 적절히 조정할 수 있습니다.

    int sampleCount = 140; // 샘플링할 점의 수입니다. -6부터 6까지 총 13개입니다.
    float angleStep = 2 * 3.14159265 / sampleCount; // 각 샘플링 점까지의 각도 스텝입니다.
	

    //Original Version
	
    for (int i = -70; i <= 70; ++i)
    {
        //uv = t + float2(tu * i, 0);
		
        float angle = angleStep * i;
        uv = t + radius * float2(cos(angle), sin(angle));
        float weight = Gaussian(float(i), 20);
        vOut += weight * g_Texture.Sample(LinearSampler_Clamp, uv);
        TotalWeight += weight;
    }
    vOut /= TotalWeight * 0.3f;
    Out.vColor = vOut;
    


    return Out;
}

PS_OUT PS_MAIN_Y_BLUR_EMISSIVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float4 vOut = 0;
    float2 t = In.vTexcoord;
    float2 uv = 0;
    float TotalWeight = 0;
    float tu = 1.f / (400.f * 0.7f);

	
    float radius = 1.f / (640.f); // 샘플링 반경을 설정합니다. 이 값은 적절히 조정할 수 있습니다.

    int sampleCount = 35; // 샘플링할 점의 수입니다. -6부터 6까지 총 13개입니다.
    float angleStep = 2 * 3.14159265 / sampleCount; // 각 샘플링 점까지의 각도 스텝입니다.
	
	
    //Original Version

    for (int i = -70; i <= 70; ++i)
    {
        //uv = t + float2(0, tu * i);
        float angle = angleStep * (i);
        uv = t + (radius) * float2(cos(angle), sin(angle));
        float weight = Gaussian(float(i), 20);
        vOut += weight * g_Texture.Sample(LinearSampler_Clamp, uv);
        TotalWeight += weight;
    }
    vOut /= TotalWeight * 0.3f;
    Out.vColor = vOut;
	

    return Out;
}

PS_OUT PS_MAIN_X_BLUR_EMISSIVE_2(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vOut = 0;
    float2 t = In.vTexcoord;
    float2 uv = 0;
    float TotalWeight = 0;
    float tu = 1.f / 1280.f;

    for (int i = -70; i <= 70; ++i)
    {
        uv = t + float2(tu * i, 0);
        float weight = Gaussian(float(i), 4);
        vOut += weight * g_Texture.Sample(LinearSampler_Clamp, uv);
        TotalWeight += weight;
    }

    vOut /= TotalWeight * 0.5f;
    Out.vColor = vOut;
    
    
    //float2 distortion = g_DistortionTexture.Sample(LinearSampler_Clamp, In.vTexcoord).rg;
    
    //// Distortion 값을 조정하여 텍스처 좌표 변형
    //float2 distortedTexCoords = In.vTexcoord + (distortion * g_DistortionAmount); // 왜곡 정도 조절
    
    //// 변형된 텍스처 좌표를 사용하여 최종 색상 결정
    //float4 color = g_Texture.Sample(LinearSampler_Clamp, distortedTexCoords);
    //Out.vColor += color;

    return Out;
}

PS_OUT PS_MAIN_Y_BLUR_EMISSIVE_2(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    //Circle Form
    
    //float4 vOut = 0;
    //float2 t = In.vTexcoord;
    //float2 uv = 0;
    //float TotalWeight = 0;
    //float tu = 1.f / (400.f * 0.7f);

	
    //float radius = 1.f / (360); // 샘플링 반경을 설정합니다. 이 값은 적절히 조정할 수 있습니다.

    //int sampleCount = 35; // 샘플링할 점의 수입니다. -6부터 6까지 총 13개입니다.
    //float angleStep = 2 * 3.14159265 / sampleCount; // 각 샘플링 점까지의 각도 스텝입니다.
	
    //for (int i = -140; i <= 140; ++i)
    //{
    //    //uv = t + float2(0, tu * i);
    //    float angle = angleStep * (i);
    //    uv = t + (radius) * float2(cos(angle), sin(angle));
    //    float weight = Gaussian(float(i), 20);
    //    vOut += weight * g_Texture.Sample(LinearSampler_Clamp, uv);
    //    TotalWeight += weight;
    //}
    //vOut /= TotalWeight * 0.3f;
    //Out.vColor = vOut;
    
    
    
    float4 vOut = 0;
    float2 t = In.vTexcoord;
    float2 uv = 0;
    float TotalWeight = 0;
    float tu = 1.f / (1280.f * 0.7f * 0.5f);

    for (int i = -70; i <= 70; ++i)
    {
        uv = t + float2(0, tu * i);
        float weight = Gaussian(float(i), 4);
        vOut += weight * g_Texture.Sample(LinearSampler_Clamp, uv);
        TotalWeight += weight;
    }

    vOut /= TotalWeight * 0.5f;
    Out.vColor = vOut;
   
    return Out;
}

PS_OUT PS_MAIN_X_BLUR_EMISSIVE_3(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    float4 vOut = 0;
    float2 t = In.vTexcoord;
    float2 uv = 0;
    float TotalWeight = 0;
    float tu = 1.f / 1280.f;

    for (int i = -70; i <= 70; ++i)
    {
        uv = t + float2(tu * i, 0);
        float weight = Gaussian(float(i), 400);
        vOut += weight * g_Texture.Sample(LinearSampler_Clamp, uv);
        TotalWeight += weight;
    }

    vOut /= TotalWeight * 0.6f;
    Out.vColor = vOut;

    return Out;
}

PS_OUT PS_MAIN_Y_BLUR_EMISSIVE_3(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    float4 vOut = 0;
    float2 t = In.vTexcoord;
    float2 uv = 0;
    float TotalWeight = 0;
    float tu = 1.f / (1280.f * 0.7f * 0.5f);

    for (int i = -70; i <= 70; ++i)
    {
        uv = t + float2(0, tu * i);
        float weight = Gaussian(float(i), 400);
        vOut += weight * g_Texture.Sample(LinearSampler_Clamp, uv);
        TotalWeight += weight;
    }

    vOut /= TotalWeight * 0.6f;
    Out.vColor = vOut;

    return Out;
}

PS_OUT PS_MAIN_X_BLUR_EMISSIVE_4(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    float4 vOut = 0;
    float2 t = In.vTexcoord;
    float2 uv = 0;
    float TotalWeight = 0;
    float tu = 1.f / 1280.f;

    for (int i = -70; i <= 70; ++i)
    {
        uv = t + float2(tu * i, 0);
        float weight = Gaussian(float(i), 40);
        vOut += weight * g_Texture.Sample(LinearSampler_Clamp, uv);
        TotalWeight += weight;
    }

    vOut /= TotalWeight * 0.35f;
    Out.vColor = vOut;

    return Out;
}

PS_OUT PS_MAIN_Y_BLUR_EMISSIVE_4(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    float4 vOut = 0;
    float2 t = In.vTexcoord;
    float2 uv = 0;
    float TotalWeight = 0;
    float tu = 1.f / (1280.f * 0.7f * 0.5f);

    for (int i = -70; i <= 70; ++i)
    {
        uv = t + float2(0, tu * i);
        float weight = Gaussian(float(i), 40);
        vOut += weight * g_Texture.Sample(LinearSampler_Clamp, uv);
        TotalWeight += weight;
    }

    vOut /= TotalWeight * 0.35f;
    Out.vColor = vOut;

    return Out;
}

PS_OUT PS_MAIN_DISTORTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 distortion = g_DistortionTexture.Sample(LinearSampler_Clamp, In.vTexcoord).rg;
    
    // Distortion 값을 조정하여 텍스처 좌표 변형
    float2 distortedTexCoords = In.vTexcoord + (distortion * g_DistortionAmount); // 왜곡 정도 조절
    
    // 변형된 텍스처 좌표를 사용하여 최종 색상 결정
    float4 color = g_Texture.Sample(LinearSampler_Clamp, distortedTexCoords);
    Out.vColor += color;
    return Out;
}

PS_OUT PS_MAIN_EMISSIVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vOut = 0;
    float4 vOut2 = 0;
    float4 vOut3 = 0;
    float2 t = In.vTexcoord;
    float2 uv = 0;

    float radius = 1.f / 320.f; // 샘플링 반경을 설정합니다. 이 값은 적절히 조정할 수 있습니다.

    int sampleCount = 13; // 샘플링할 점의 수입니다. -6부터 6까지 총 13개입니다.
    float angleStep = 2 * 3.14159265 / sampleCount; // 각 샘플링 점까지의 각도 스텝입니다.

    for (int i = -6; i <= 6; ++i)
    {
        float angle = angleStep * i; // 현재 샘플링 점의 각도를 계산합니다.
		// 가로와 세로 방향 대신, 각도에 따라 샘플링 위치를 계산합니다.
		//uv = t + radius * float2(cos(angle), sin(angle));
		
        vOut += Weight[6 + i] * g_Texture.Sample(LinearSampler_Clamp, uv);
    }
    vOut /= Total; // 가중치의 총합으로 나누어 평균을 구합니다.


    radius = 1.f / 640.f;
    for (int i = -6; i <= 6; ++i)
    {
        float angle = angleStep * i; // 현재 샘플링 점의 각도를 계산합니다.
		// 가로와 세로 방향 대신, 각도에 따라 샘플링 위치를 계산합니다.
        uv = t + radius * float2(cos(angle), sin(angle));
        vOut2 += Weight[6 + i] * g_Texture.Sample(LinearSampler_Clamp, uv);
    }
    vOut2 /= Total * 2.f; // 가중치의 총합으로 나누어 평균을 구합니다.


    radius = 1.f / 880.f;
    for (int i = -6; i <= 6; ++i)
    {
        float angle = angleStep * i; // 현재 샘플링 점의 각도를 계산합니다.
		// 가로와 세로 방향 대신, 각도에 따라 샘플링 위치를 계산합니다.
        uv = t + radius * float2(cos(angle), sin(angle));
        vOut3 += Weight[6 + i] * g_Texture.Sample(LinearSampler_Clamp, uv);
    }
    vOut3 /= Total * 4.f; // 가중치의 총합으로 나누어 평균을 구합니다.

    Out.vColor = vOut + vOut2 + vOut3;
    return Out;
}

PS_OUT PS_MAIN_AFTER_IMAGE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler_Clamp, In.vTexcoord);
    
    return Out;
}


technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Emissive_Blur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EMISSIVE();
    }

    pass AlphaBlendPass
    {
        SetRasterizerState(RS_Sky);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);

		// 알파 블렌딩을 위한 블렌드 상태 설정
		// 실제 사용 시, BS_AlphaBlend를 적절한 블렌드 상태로 설정하고 초기화해야 함
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass X_Blur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_X_BLUR();
    }

    pass Y_Blur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Y_BLUR();
    }

    pass X_Blur_Emissive
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_X_BLUR_EMISSIVE();
    }

    pass Y_Blur_Emissive
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Y_BLUR_EMISSIVE();
    }

    pass X_Blur_Emissive_2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_X_BLUR_EMISSIVE_2();
    }

    pass Y_Blur_Emissive_2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Y_BLUR_EMISSIVE_2();
    }
    
    pass Distortion 
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
    }

    pass X_Blur_Emissive_3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_X_BLUR_EMISSIVE_3();
    }

    pass Y_Blur_Emissive_3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Y_BLUR_EMISSIVE_3();
    }

    pass AfterImage
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_AFTER_IMAGE();
    }

    pass X_Blur_Emissive_4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_X_BLUR_EMISSIVE_4();
    }

    pass Y_Blur_Emissive_4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Y_BLUR_EMISSIVE_4();
    }

}