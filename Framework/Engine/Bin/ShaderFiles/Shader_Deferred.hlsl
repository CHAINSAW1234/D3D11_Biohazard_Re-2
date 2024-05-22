#include "Engine_Shader_Defines.hlsli"

/* 전역변수 : 쉐이더 외부에 있는 데이터를 쉐이더 안으로 받아온다. */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
//matrix g_LightViewMatrix, g_LightProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

texture2D g_Texture;
texture2D g_NormalTexture;
texture2D g_DiffuseTexture;
texture2D g_ShadeTexture;
texture2D g_DepthTexture;
texture2D g_MaterialTexture;

texture2D g_LightDepthTexture;

texture2D g_SpecularTexture;
texture2D g_LightResultTexture;

texture2D g_AmbientDiffuseTexture;
texture2D g_AmbientShadeTexture;

texture2D g_DistortionTexture;
texture2D g_EmissiveTexture;

texture2D g_AdditionalLightTexture;

texture2D g_PostprocessingDiffuseTexture;
texture2D g_PostprocessingShadeTexture;

bool g_isRadialBlurActive = { false };
float2 g_vRadialBlurUV;
float g_fRadialAmount;

//예은 추가
float		g_fCutOff;
float		g_fOutCutOff;
//

// 현진 추가
TextureCubeArray g_CubeTexture;
matrix g_LightViewMatrix[6];
matrix g_LightProjMatrix;
//

float4 g_vLightDir;
float4 g_vLightPos;
float g_fLightRange;

float4 g_vLightDiffuse;
float4 g_vLightAmbient;
float4 g_vLightSpecular;

float4 g_vWaterColor;

float4 g_vMtrlAmbient = float4(1.f, 1.f, 1.f, 1.f);
float4 g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);

float4 g_vCamPosition;

float g_fLightDepthFar;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD;
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
    float2 vTexcoord : TEXCOORD;
};

struct PS_SHADOW_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 FragPos : FragPos;
    uint RTIndex : SV_RenderTargetArrayIndex;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

struct PS_OUT_EMISSIVE
{
    float4 vEmissive : SV_TARGET0;
};

struct PS_OUT_PRE_POST
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vMaterial : SV_TARGET3;
};

struct PS_OUT_POST_PROCESSING
{
    float4 vDiffuse : SV_TARGET0;
    float4 vShade : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

PS_OUT PS_MAIN_CUBE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float3 vTex = normalize(float3(1.0, In.vTexcoord.y * -2 + 1.f, In.vTexcoord.x * -2.f + 1.f));
    //float3 vTex = normalize(float3(In.vTexcoord.x * 2 - 1.f, In.vTexcoord.y * -2.f + 1.f, 1.f));
    //float3 vTex = normalize(float3(-1.0, In.vTexcoord.y * -2 + 1.f, In.vTexcoord.x * -2.f + 1.f));
    
    Out.vColor = g_CubeTexture.Sample(LinearSampler, float4(vTex, 0));

    return Out;
}

struct PS_OUT_LIGHT
{
    float4 vShade : SV_TARGET0;
    float4 vSpecular : SV_TARGET1;
};

/* 빛 하나당 480000 수행되는 쉐이더. */

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.0f;
    
    vector vMaterialDesc = g_MaterialTexture.Sample(PointSampler, In.vTexcoord);
    float fMaterialMetalic = vMaterialDesc.x;
    float fMaterialRoughness = vMaterialDesc.y;
    float fMaterialSpecular = vMaterialDesc.z;
	
    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + g_vLightAmbient * g_vMtrlAmbient);

    float4 vWorldPos;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / View.z */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
    vWorldPos *= fViewZ;

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 로컬위치 * 월드행렬 */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    float4 vLook = vWorldPos - g_vCamPosition;
    float4 vReflect = reflect(normalize(g_vLightDir), vNormal);

    float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 3.f);

    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.0f;

    float4 vWorldPos;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / View.z */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
    vWorldPos *= fViewZ;

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 로컬위치 * 월드행렬 */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    vector vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);

    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);

    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + g_vLightAmbient * g_vMtrlAmbient);
    Out.vShade *= fAtt;
	
    float4 vLook = vWorldPos - g_vCamPosition;
    float4 vReflect = reflect(normalize(vLightDir), vNormal);

    float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 3.f);

    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
    Out.vSpecular *= fAtt;

    return Out;
}

PS_OUT_LIGHT PS_MAIN_SPOT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.0f;
    float4 vWorldPos;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / View.z */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
    vWorldPos *= fViewZ;

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 로컬위치 * 월드행렬 */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    vector vLightDir = vWorldPos - g_vLightPos;
	
    vector vSpotDir = g_vLightDir;
	
    float fResult = acos(dot(normalize(vLightDir), normalize(vSpotDir)));
	
 
    if (g_fOutCutOff >= fResult) // 빛이 번질 범위 안에 있을 때
    {
        float fDistance = length(vLightDir);

        float fIntensity = (fResult - g_fOutCutOff) / (g_fCutOff - g_fOutCutOff);
		
        float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange) * fIntensity; //범위 줘서 끝 범위에서는 연해지게 

        Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + g_vLightAmbient * g_vMtrlAmbient);
        Out.vShade *= fAtt;
	
        float4 vLook = vWorldPos - g_vCamPosition;
        float4 vReflect = reflect(normalize(vLightDir), vNormal);

        float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

        Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
        Out.vSpecular *= fAtt;

    }
		
    return Out;
}

/* 최종적으로 480000 수행되는 쉐이더. */
PS_OUT_PRE_POST PS_MAIN_LIGHT_RESULT(PS_IN In)
{
    PS_OUT_PRE_POST Out = (PS_OUT_PRE_POST) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (0.0f == vDiffuse.a)
        discard;
    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vDepth = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormal = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vMaterial = g_MaterialTexture.Sample(PointSampler, In.vTexcoord);

    float fMetalness = vMaterial.r;
    
    vSpecular = lerp(0.04, vSpecular, fMetalness);
    
    Out.vDiffuse = vDiffuse * vShade + vSpecular;
    Out.vNormal = vNormal;
    Out.vDepth = vDepth;
    Out.vMaterial = vMaterial;
    
    
    /* Shadow */
    /* ProjPos.w == View.Z */
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.0f;

    float4 vWorldPos;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / View.z */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
    vWorldPos *= fViewZ;

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 로컬위치 * 월드행렬 */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    float3 vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);
    vLightDir = normalize(vLightDir);
    
    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);
    
    vector vLightDepthDesc = g_CubeTexture.Sample(PointSampler, float4(vLightDir, 0));
    //Out.vDiffuse = vLightDepthDesc * 2000;
	/* vPosition.w : 현재 내가 그릴려고 했던 픽셀의 광원기준의 깊이. */
	/* vLightDepthDesc.x * 2000.f : 현재 픽셀을 광원기준으로  그릴려고 했던 위치에 이미 그려져있떤 광원 기준의 깊이.  */
    
    float minDepth = 0;
    for (int i = 0; i < 6; ++i)
    {
        vector vPosition = mul(vWorldPos, g_LightViewMatrix[i]);
        vPosition = mul(vPosition, g_LightProjMatrix);
        minDepth = max(minDepth, vPosition.w);

    }
    
    //float shadow = fDistance < vLightDepthDesc.r * 1000 ? 1.0 : 0.0;
    

    if (minDepth -0.1f > vLightDepthDesc.x * 1000)
        Out.vDiffuse = float4(0.f,0.f,0.f,1.f);
        
        //lerp((g_vLightAmbient * g_vMtrlAmbient), float4(1, 1, 1, 1), );
        
    

    return Out;
}

PS_OUT_EMISSIVE PS_MAIN_EMISSIVE(PS_IN In)
{
    PS_OUT_EMISSIVE Out = (PS_OUT_EMISSIVE) 0;
    
    vector MaterialDesc = g_MaterialTexture.Sample(PointSampler, In.vTexcoord);
    vector DiffuseDesc = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    
    Out.vEmissive = DiffuseDesc * MaterialDesc.a;
    
    return Out;
}

PS_OUT_POST_PROCESSING PS_MAIN_POSTPROCESSING(PS_IN In)
{
    PS_OUT_POST_PROCESSING Out = (PS_OUT_POST_PROCESSING) 0;
    
    vector vAmbientDiffuseDesc = g_AmbientDiffuseTexture.Sample(PointSampler, In.vTexcoord);
    vector vAmbientShadeDesc = g_AmbientShadeTexture.Sample(PointSampler, In.vTexcoord);
    
    Out.vDiffuse = vAmbientDiffuseDesc;
    Out.vShade = vAmbientShadeDesc;
    
    return Out;
}

PS_OUT PS_POST_PROCESSING_RESULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector DistortionDesc = g_DistortionTexture.Sample(PointSampler, In.vTexcoord);
    DistortionDesc.xyz *= 2.f;
    DistortionDesc.xyz -= 1.f;
    
    DistortionDesc.xy = normalize(DistortionDesc.xy);
    float2 vTexcoord = float2(In.vTexcoord.x + (DistortionDesc.x * DistortionDesc.w), In.vTexcoord.y + (DistortionDesc.y * DistortionDesc.w));

    vector DepthDescSrc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector DepthDescDst = g_DepthTexture.Sample(PointSampler, vTexcoord);
    
    //  현재 깊이가 원본위치에서의 깊이가 디스토션이적용된 깊이보다 멀다면
    if (DepthDescSrc.y >= DepthDescDst.y)
        vTexcoord = In.vTexcoord;
    
    vector AdditionalLightDesc = g_AdditionalLightTexture.Sample(PointSampler, In.vTexcoord);
    vector DiffuseDesc = g_DiffuseTexture.Sample(PointSamplerClamp, vTexcoord);
    vector PostprocessingDiffuseDesc = g_PostprocessingDiffuseTexture.Sample(PointSamplerClamp, vTexcoord);
    vector PostprocessingShadeDesc = g_PostprocessingShadeTexture.Sample(PointSamplerClamp, vTexcoord);
    
    float fSrcAlpha = PostprocessingDiffuseDesc.a;
    float fDstAlpha = 1.f - fSrcAlpha;
   
    
    
    //Out.vColor = DiffuseDesc * fDstAlpha + PostprocessingDiffuseDesc * fSrcAlpha;
    //Out.vColor *= PostprocessingShadeDesc;
    Out.vColor = DiffuseDesc;
    
    Out.vColor += AdditionalLightDesc;
    
    //  Out.vColor.xyz = pow(Out.vColor.xyz, 1.f / 2.2f);
    
    return Out;
}

PS_OUT PS_RADIALBLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (true == g_isRadialBlurActive)
    {
        float fWidth, fHeight;
        g_DiffuseTexture.GetDimensions(fWidth, fHeight);
        float2 vTexOffset = 1.0 / float2(fWidth, fHeight); // 텍스처 각 요소에 대한 오프셋 계산 
    
        float4 vColor = float4(0.f, 0.f, 0.f, 0.f);
        int iSampleCnt = 10;
        float2 vStep = (In.vTexcoord - g_vRadialBlurUV) * vTexOffset * g_fRadialAmount;

        for (int i = -iSampleCnt / 2; i <= iSampleCnt / 2; i++)
        {
            float2 vSamplePos = In.vTexcoord + vStep * i;
            vColor += g_DiffuseTexture.Sample(PointSamplerClamp, vSamplePos);
        }

        vColor /= iSampleCnt + 1;
        Out.vColor = vColor;
    }
    else
    {
        vector DiffuseDesc = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
        Out.vColor = DiffuseDesc;
        
        if (DiffuseDesc.a == 0)
            discard;

    }
    
    return Out;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

float Compute_Gaussian(float fX, float fSigma)
{
    float fResult = exp(-((fX * fX) / (2.0 * fSigma * fSigma))) / (fSigma * sqrt(2.0 * 3.14159));
    
    return fResult;
}

// 가우시안 블러를 적용할 픽셀 셰이더 함수
PS_OUT PS_MAIN_BLOOM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float fWidth, fHeight;
    g_Texture.GetDimensions(fWidth, fHeight);
    
    float4 vResult = float4(0, 0, 0, 0);
    float2 vTexOffset = 1.0 / float2(fWidth, fHeight); // 텍스처 각 요소에 대한 오프셋 계산 
    float fRadius = 15.f;

    // 주변 픽셀들을 반복하여 블러 처리
    for (int iY = (int) fRadius * -1.f; iY <= (int) fRadius; iY++)
    {
        for (int iX = (int) fRadius * -1.f; iX <= (int) fRadius; iX++)
        {
            float fDistance = sqrt(float(iX * iX + iY * iY));
            float fWeight = Compute_Gaussian(fDistance, 1.f); // 가우시안 함수를 이용해 가중치 계산
            // 가중치가 적용된 색상을 결과에 더함
            vResult += fWeight * g_Texture.Sample(LinearSampler, In.vTexcoord + float2(iX, iY) * vTexOffset);
        }
    }
    
    Out.vColor = vResult;

    return Out;
}
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

technique11 DefaultTechnique
{
    pass Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Light_Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Light_Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass Light_Result
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_RESULT();
    }

    pass Emissive //  4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EMISSIVE();
    }

    pass Bloom //  5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLOOM();
    }


    pass Post_Processing //  6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POSTPROCESSING();
    }

    pass Post_Processing_Result //  7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_POST_PROCESSING_RESULT();
    }

    pass RadialBlur // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_RADIALBLUR();
    }

    pass Debug_Cube // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CUBE();
    }

}