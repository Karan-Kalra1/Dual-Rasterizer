Texture2D gDiffuseMap;

SamplerState Sampler_Linear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4x4 gWorldViewProj;

struct VS_INPUT
{
    float3 Position : POSITION;
    float2 UV : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD;
};

VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT o;
    o.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
    o.UV = input.UV;
    return o;
}

float4 PS_Main(VS_OUTPUT input) : SV_Target
{
    return gDiffuseMap.Sample(Sampler_Linear, input.UV);
}

RasterizerState DisableCulling
{
    CullMode = NONE;
    FillMode = SOLID;
};

BlendState AlphaBlend
{
    BlendEnable[0] = TRUE;

    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;

    SrcBlendAlpha = ONE;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;

    RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState ReadOnlyDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ZERO;
    DepthFunc = LESS;
};

technique11 FireTech
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Main()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Main()));

        SetRasterizerState(DisableCulling);
        SetBlendState(AlphaBlend, float4(0, 0, 0, 0), 0xFFFFFFFF);
        SetDepthStencilState(ReadOnlyDepth, 0);
    }
}
