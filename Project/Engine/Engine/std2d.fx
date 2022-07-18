#ifndef _STD2D
#define _STD2D

#include "value.fx"


struct VTX_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VTX_OUT
{
    float4 vPosition : SV_Position;
    float3 vWorldPos : POSITION;
    float2 vUV : TEXCOORD;
};


// =========================
// Std2D Shader
// g_tex_0 : Output Texture
// Rasterizer : CULL_NONE
// BlendState : Default
// DepthStencilState : LESS
// DOMAIN : Masked
// =========================
VTX_OUT VS_Std2D(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld).xyz;
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_Std2D(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
    
    // Animation 정보가 있는 경우
    if (g_useAnim2D)
    {
        float2 vUV = _in.vUV * g_vSlice; // * g_vBackgroundSize;
        vUV = vUV + g_vLT;//        -(g_vBackgroundSize - g_vSlice) / 2.f + g_vLT - g_vOffset;
        
        if (vUV.x < g_vLT.x || g_vLT.x + g_vSlice.x < vUV.x
            || vUV.y < g_vLT.y || g_vLT.y + g_vSlice.y < vUV.y)
        {            
            discard;
        }
                
        vOutColor = g_Atlas.Sample(g_sam_1, vUV);     
    }
    else
    {
        //if(IsBind(g_tex_0))
        if(g_btex_0)
        {
            float2 vUV = _in.vUV * s_vSize;
            vUV = vUV + s_vPos;
            
            if (vUV.x < s_vPos.x || s_vPos.x + s_vSize.x < vUV.x
            || vUV.y < s_vPos.y || s_vPos.y + s_vSize.y < vUV.y)
            {
                discard;
            }
            
            vOutColor = g_tex_0.Sample(g_sam_0, vUV);
        }
        else
        {
            vOutColor = float4(1.f, 0.f, 1.f, 1.f);
        }        
    }
    
    if (vOutColor.a <= 0.f)
    {
        discard;
    }        
      
    // 광원에 따른 빛 계산    
    float3 vLightColor = CalculateLight2D(_in.vWorldPos);
    
    // 빛 적용
    vOutColor.rgb *= vLightColor.rgb;
    
    return vOutColor;
}

// ========================
// Std2DAlphaBlend
// BlendState           : Alpha Blend
// DepthStencilState    : No_Write
// DOMAIN               : OPAQUE
//=========================
VTX_OUT VS_Std2DAlpha(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld).xyz;
    
    return output;
}

float4 PS_Std2DAlpha(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
    
    // Animation 정보가 있는 경우
    if (g_useAnim2D)
    {
        float2 vUV = _in.vUV * g_vSlice; // * g_vBackgroundSize;
        vUV = vUV + g_vLT; //        -(g_vBackgroundSize - g_vSlice) / 2.f + g_vLT - g_vOffset;
        
        if (vUV.x < g_vLT.x || g_vLT.x + g_vSlice.x < vUV.x
            || vUV.y < g_vLT.y || g_vLT.y + g_vSlice.y < vUV.y)
        {
            discard;
        }
                
        vOutColor = g_Atlas.Sample(g_sam_1, vUV);
    }
    else
    {
        if (g_btex_0)
        {
            float2 vUV = _in.vUV * s_vSize;
            vUV = vUV + s_vPos;
            
            if (vUV.x < s_vPos.x || s_vPos.x + s_vSize.x < vUV.x
            || vUV.y < s_vPos.y || s_vPos.y + s_vSize.y < vUV.y)
            {
                discard;
            }
            
            vOutColor = g_tex_0.Sample(g_sam_0, vUV);
        }
        else
        {
            vOutColor = float4(1.f, 0.f, 1.f, 1.f);
        }
    }
    
    if (vOutColor.a <= 0.f)
    {
        discard;
    }
      
    // 광원에 따른 빛 계산    
    float3 vLightColor = CalculateLight2D(_in.vWorldPos);
    
    // 빛 적용
    vOutColor.rgb *= vLightColor.rgb;
    
    return vOutColor;
}


// ==================
// Collider2D Shader
// g_int_0 : Collision
// ==================
VTX_OUT VS_Collider2D(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f; 
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld).xyz;
    output.vUV = _in.vUV;
    
    return output;
}


float4 PS_Collider2D(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
       
    if(g_int_0)
    {
        vOutColor = float4(1.f, 0.f, 0.f, 1.f);
    }
    else
    {
        vOutColor = float4(0.f, 1.f, 0.f, 1.f);
    }
        
    return vOutColor;
}







#endif
