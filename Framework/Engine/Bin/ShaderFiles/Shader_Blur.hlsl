#include "Engine_Shader_Defines.hlsli"

/* �������� : ���̴� �ܺο� �ִ� �����͸� ���̴� ������ �޾ƿ´�. */
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

/* ���� ���̴� */
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
#define RADIUS 2.0 // Ŀ���� ������
float Gaussian(float x, float sigma)
{
    return exp(-0.5 * (x * x) / (sigma * sigma));
}
//static const float Total = 6.2108;
static const float Total = 1.2108;

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 result = float4(0.0, 0.0, 0.0, 0.0); // ��� �÷� �ʱ�ȭ
    float totalWeight = 0.0; // �� ����ġ �ʱ�ȭ
    float2 t = In.vTexcoord;
    float2 uv = 0;
    float tu = 1.f / 800.f;
    for (int i = -KERNEL_SIZE / 2; i <= KERNEL_SIZE / 2; i++)
    {
        for (int j = -KERNEL_SIZE / 2; j <= KERNEL_SIZE / 2; j++)
        {
			// ���� �ȼ� ��ġ ���
            uv = t + float2(tu * i, tu * j);
            float weight = Gaussian(length(float2(i, j)), SIGMA);
            result += g_Texture.Sample(LinearSampler_Clamp, uv) * weight; // ����ġ ����
            totalWeight += weight; // �� ����ġ ������Ʈ
        }
    }
    result /= totalWeight;


    float4 result2 = float4(0.0, 0.0, 0.0, 0.0); // ��� �÷� �ʱ�ȭ
    float totalWeight2 = 0.0; // �� ����ġ �ʱ�ȭ
    float2 t2 = In.vTexcoord;
    float2 uv2 = 0;
    float tu2 = 1.f / (600.f * 0.5f);

    for (int i = -KERNEL_SIZE / 2; i <= KERNEL_SIZE / 2; i++)
    {
        for (int j = -KERNEL_SIZE / 2; j <= KERNEL_SIZE / 2; j++)
        {
			// ���� �ȼ� ��ġ ���
            uv2 = t2 + float2(tu2 * j, tu2 * i);
            float weight2 = Gaussian(length(float2(j, i)), SIGMA);
            result2 += g_Texture.Sample(LinearSampler_Clamp, uv2) * weight2; // ����ġ ����
            totalWeight2 += weight2; // �� ����ġ ������Ʈ
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
	
	
    float radius = 1.f / 640.f; // ���ø� �ݰ��� �����մϴ�. �� ���� ������ ������ �� �ֽ��ϴ�.

    int sampleCount = 140; // ���ø��� ���� ���Դϴ�. -6���� 6���� �� 13���Դϴ�.
    float angleStep = 2 * 3.14159265 / sampleCount; // �� ���ø� �������� ���� �����Դϴ�.
	

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

	
    float radius = 1.f / (640.f); // ���ø� �ݰ��� �����մϴ�. �� ���� ������ ������ �� �ֽ��ϴ�.

    int sampleCount = 35; // ���ø��� ���� ���Դϴ�. -6���� 6���� �� 13���Դϴ�.
    float angleStep = 2 * 3.14159265 / sampleCount; // �� ���ø� �������� ���� �����Դϴ�.
	
	
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
    
    //// Distortion ���� �����Ͽ� �ؽ�ó ��ǥ ����
    //float2 distortedTexCoords = In.vTexcoord + (distortion * g_DistortionAmount); // �ְ� ���� ����
    
    //// ������ �ؽ�ó ��ǥ�� ����Ͽ� ���� ���� ����
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

	
    //float radius = 1.f / (360); // ���ø� �ݰ��� �����մϴ�. �� ���� ������ ������ �� �ֽ��ϴ�.

    //int sampleCount = 35; // ���ø��� ���� ���Դϴ�. -6���� 6���� �� 13���Դϴ�.
    //float angleStep = 2 * 3.14159265 / sampleCount; // �� ���ø� �������� ���� �����Դϴ�.
	
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
    
    // Distortion ���� �����Ͽ� �ؽ�ó ��ǥ ����
    float2 distortedTexCoords = In.vTexcoord + (distortion * g_DistortionAmount); // �ְ� ���� ����
    
    // ������ �ؽ�ó ��ǥ�� ����Ͽ� ���� ���� ����
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

    float radius = 1.f / 320.f; // ���ø� �ݰ��� �����մϴ�. �� ���� ������ ������ �� �ֽ��ϴ�.

    int sampleCount = 13; // ���ø��� ���� ���Դϴ�. -6���� 6���� �� 13���Դϴ�.
    float angleStep = 2 * 3.14159265 / sampleCount; // �� ���ø� �������� ���� �����Դϴ�.

    for (int i = -6; i <= 6; ++i)
    {
        float angle = angleStep * i; // ���� ���ø� ���� ������ ����մϴ�.
		// ���ο� ���� ���� ���, ������ ���� ���ø� ��ġ�� ����մϴ�.
		//uv = t + radius * float2(cos(angle), sin(angle));
		
        vOut += Weight[6 + i] * g_Texture.Sample(LinearSampler_Clamp, uv);
    }
    vOut /= Total; // ����ġ�� �������� ������ ����� ���մϴ�.


    radius = 1.f / 640.f;
    for (int i = -6; i <= 6; ++i)
    {
        float angle = angleStep * i; // ���� ���ø� ���� ������ ����մϴ�.
		// ���ο� ���� ���� ���, ������ ���� ���ø� ��ġ�� ����մϴ�.
        uv = t + radius * float2(cos(angle), sin(angle));
        vOut2 += Weight[6 + i] * g_Texture.Sample(LinearSampler_Clamp, uv);
    }
    vOut2 /= Total * 2.f; // ����ġ�� �������� ������ ����� ���մϴ�.


    radius = 1.f / 880.f;
    for (int i = -6; i <= 6; ++i)
    {
        float angle = angleStep * i; // ���� ���ø� ���� ������ ����մϴ�.
		// ���ο� ���� ���� ���, ������ ���� ���ø� ��ġ�� ����մϴ�.
        uv = t + radius * float2(cos(angle), sin(angle));
        vOut3 += Weight[6 + i] * g_Texture.Sample(LinearSampler_Clamp, uv);
    }
    vOut3 /= Total * 4.f; // ����ġ�� �������� ������ ����� ���մϴ�.

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

		// ���� ������ ���� ���� ���� ����
		// ���� ��� ��, BS_AlphaBlend�� ������ ���� ���·� �����ϰ� �ʱ�ȭ�ؾ� ��
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