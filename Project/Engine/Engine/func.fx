#ifndef _FUNC
#define _FUNC

#include "value.fx"

static float GaussianFilter[5][5] =
{
    0.003f , 0.0133f, 0.0219f, 0.0133f, 0.003f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.0219f, 0.0983f, 0.1621f, 0.0983f, 0.0219f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.003f , 0.0133f, 0.0219f, 0.0133f, 0.003f,
};


int IsBind(in Texture2D _tex)
{
    uint width = 0;
    uint height = 0;
    _tex.GetDimensions(width, height);
    
    if(0 != width || 0 != height)
        return 1;
   
    return 0;
}

float4 GaussianSample(in Texture2D _noisetex, float2 _vUV)
{
    float4 vOutColor = (float4) 0.f;
    
    if(0.f < _vUV.x)
    {
        _vUV.x = frac(_vUV.x);
    }
    
    if (0.f < _vUV.y)
    {
        _vUV.y = frac(_vUV.y);
    }
    
    
    
    // NoiseTexture �ػ󵵸� �̿��ؼ� �ȼ� �ε���(����) �� �˾Ƴ���.
    int2 iPixelIdx = (int2) (vNoiseResolution * _vUV);
    iPixelIdx -= int2(2, 2);
    
    
    // �ش� �ȼ��� �߽����� ����þ� ���͸� �����ؼ� ������ ���ġ�� ����Ѵ�.
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            int2 idx = int2(iPixelIdx.x + j, iPixelIdx.y + i);
            vOutColor += _noisetex[idx] * GaussianFilter[j][i];
        }
    }
    
    return vOutColor;
}


float3 CalculateLight2D(float3 _vWolrdPos)
{
    // �� ���� �������� ����
    float3 vLightColor = (float3) 0.f;

    
    // ==============
    // | Light Type |
    // | 0 : Dir    |
    // | 1 : Point  |
    // | 2 : Spot   |
    // ==============

    for (int i = 0; i < iLight2DCount; ++i)
    {
        // Directional Light
        if (0 == g_Light2DBuffer[i].iLightType)
        {
            vLightColor += g_Light2DBuffer[i].color.vDiff.rgb;
        }
        
        // Point Light
        else if (1 == g_Light2DBuffer[i].iLightType)
        {
            // pixel worldpos <--> Light World Pos
            float fDistance = distance(g_Light2DBuffer[i].vWorldPos.xy, _vWolrdPos.xy);
            
            // �������κ��� �Ÿ��� ������ ���� �������� ���� ���� ������ ����ġ�� �ش�.
            float fRatio = 1.f - saturate(fDistance / g_Light2DBuffer[i].fRange);
            vLightColor += g_Light2DBuffer[i].color.vDiff.rgb * fRatio;
        }
        
        // Spot Light
        else if (2 == g_Light2DBuffer[i].iLightType)
        { 
            // pixel worldpos <--> Light World Pos
            float fDistance = distance(_vWolrdPos.xy, g_Light2DBuffer[i].vWorldPos.xy);

            // �������κ��� �Ÿ��� ������ ���� �������� ���Ͽ� ���� ����ġ�� ����Ѵ�. 
            float fRatio = 1.f - saturate(fDistance / g_Light2DBuffer[i].fRange);
            
            // ������ ���� ���� �ۿ� �ִ� �ȼ��� ������ 0���� �ش�
            //float fAngle = acos(normalize(lightToPixelVec));
            
            float2 lightToPixelVec = _vWolrdPos.xy - g_Light2DBuffer[i].vWorldPos.xy;
            float2 lightDirVec = g_Light2DBuffer[i].vLightDir.xy;
            float fAngle = acos(dot(normalize(lightToPixelVec), normalize(lightDirVec)));
            
            if (fAngle <= radians(g_Light2DBuffer[i].fAngle/2))
            {
                vLightColor += g_Light2DBuffer[i].color.vDiff.rgb * fRatio;
            }
            else
            {
                vLightColor = 0;
            }
        }
    }
    
    return vLightColor;
}

#endif