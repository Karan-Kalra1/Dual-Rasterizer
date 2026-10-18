//---------------------------------------------------------------
// Input/Output Structs
//---------------------------------------------------------------

struct VS_INPUT
{

	float3 Position : POSITION;
    float2 uv : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
	float3 Color : COLOR;


};


struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 worldPos : POSITION1;
	float3 Color : COLOR;

};



//---------------------------------------------------------------
// Texture2D
//---------------------------------------------------------------

Texture2D gDiffuseMap : DiffuseMap;
Texture2D gNormalMap : NormalMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossMap : GlossMap;


//---------------------------------------------------------------
// Light AND Camera
//---------------------------------------------------------------

float3 gLightDirection = float3(0.577f, -0.577f, 0.577f);
float3 gCameraPos : CAMERA;


const float3 ambient = float3(0.25f, 0.25f, 0.25f);
static const float gLightIntensity = 7.0f;
static const float gShininess = 25.0f;
static const float PI = 3.14159f;


//---------------------------------------------------------------
// World View Projection Matrix
//---------------------------------------------------------------

float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gWorld : WORLD;



//---------------------------------------------------------------
// Vertex Shader
//---------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{

	VS_OUTPUT output = (VS_OUTPUT)0;
    
    output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
    output.uv = input.uv;
	output.Color = input.Color;
    float3 worldNormal = mul(input.Normal, (float3x3) gWorld);
    float3 worldTangent = mul(input.Tangent, (float3x3) gWorld);
    
    output.worldPos = mul(float4(input.Position, 1.f), gWorld).xyz;
    output.Normal = normalize(worldNormal);
    output.Tangent = normalize(worldTangent);
	return output;

}


//---------------------------------------------------------------
// Samplers
//---------------------------------------------------------------

SamplerState Sampler_Point
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState Sampler_Linear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState Sampler_Aniso
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 16;
    AddressU = Wrap;
    AddressV = Wrap;
};

//---------------------------------------------------------------
// Shared Sample Function (to avoid duplicate code)
//---------------------------------------------------------------
float4 SampleTexture(float2 uv, SamplerState s)
{
    
    return gDiffuseMap.Sample(s, uv);
}


//---------------------------------------------------------------
// Pixel Shaders
//---------------------------------------------------------------

float4 PS_Point(VS_OUTPUT input) : SV_TARGET
{
    // ------------------------------
    // Build ONB (Tangent Space Basis)
    // -----------------------------
    float3 normalWS = normalize(input.Normal);
    float3 tangentWS = normalize(input.Tangent);

    // Correct handedness!
    float3 binormalWS = normalize(cross(tangentWS, normalWS));

    // ------------------------------
    // Sample & transform normal map
    // ------------------------------
    float3 normalSample = gNormalMap.Sample(Sampler_Point, input.uv).xyz;
    normalSample = normalSample * 2.f - 1.f; // 0..1 -> -1..1

    float3 n =
        normalSample.x * tangentWS +
        normalSample.y * binormalWS +
        normalSample.z * normalWS;

    n = normalize(n);

    // ------------------------------
    // Light & Lambert Diffuse
    // ------------------------------
    float3 L = normalize(-gLightDirection);

    float nDotL = saturate(dot(n, L));
    
    float3 observedArea = (nDotL, nDotL, nDotL);

    float3 albedo = gDiffuseMap.Sample(Sampler_Point, input.uv).rgb;

    float3 diffuse =
       saturate((albedo *
        gLightIntensity) / PI);

    // ------------------------------
    // Specular (Phong)
    // ------------------------------
    float3 viewDir = normalize(gCameraPos - input.worldPos);
    
    float3 reflectDir = reflect(-L, n);

    float gloss =
        gGlossMap.Sample(Sampler_Point, input.uv).r *
        gShininess;

    float specularStrength =
        pow(saturate(dot(reflectDir, viewDir)), gloss);

    float3 specularColor =
        saturate(gSpecularMap.Sample(Sampler_Point, input.uv).rgb *
        specularStrength);

    // ------------------------------
    // Final Output (CLAMP REQUIRED)
    // ------------------------------
    float3 finalColor = observedArea * (diffuse + specularColor + ambient);

    return float4(saturate(finalColor), 1.f);
}



float4 PS_Linear(VS_OUTPUT input) : SV_TARGET
{
    
    // ------------------------------
    // Build ONB (Tangent Space Basis)
    // -----------------------------
    float3 normalWS = normalize(input.Normal);
    float3 tangentWS = normalize(input.Tangent);

    // Correct handedness!
    float3 binormalWS = normalize(cross(tangentWS, normalWS));

    // ------------------------------
    // Sample & transform normal map
    // ------------------------------
    float3 normalSample = gNormalMap.Sample(Sampler_Linear, input.uv).xyz;
    normalSample = normalSample * 2.f - 1.f; // 0..1 -> -1..1

    float3 n =
        normalSample.x * tangentWS +
        normalSample.y * binormalWS +
        normalSample.z * normalWS;

    n = normalize(n);

    // ------------------------------
    // Light & Lambert Diffuse
    // ------------------------------
    float3 L = normalize(-gLightDirection);

    float nDotL = saturate(dot(n, L));
    
    float3 observedArea = (nDotL, nDotL, nDotL);

    float3 albedo = gDiffuseMap.Sample(Sampler_Linear, input.uv).rgb;

    float3 diffuse =
       saturate((albedo *
        gLightIntensity) / PI);

    // ------------------------------
    // Specular (Phong)
    // ------------------------------
    float3 viewDir = normalize(gCameraPos - input.worldPos);
    
    float3 reflectDir = reflect(-L, n);

    float gloss =
        gGlossMap.Sample(Sampler_Linear, input.uv).r *
        gShininess;

    float specularStrength =
        pow(saturate(dot(reflectDir, viewDir)), gloss);

    float3 specularColor =
        saturate(gSpecularMap.Sample(Sampler_Linear, input.uv).rgb *
        specularStrength);

    // ------------------------------
    // Final Output (CLAMP REQUIRED)
    // ------------------------------
    float3 finalColor = observedArea * (diffuse + specularColor + ambient);

    return float4(saturate(finalColor), 1.f);
}

float4 PS_Aniso(VS_OUTPUT input) : SV_TARGET
{
    // ------------------------------
    // Build ONB (Tangent Space Basis)
    // -----------------------------
    float3 normalWS = normalize(input.Normal);
    float3 tangentWS = normalize(input.Tangent);

    // Correct handedness!
    float3 binormalWS = normalize(cross(tangentWS, normalWS));

    // ------------------------------
    // Sample & transform normal map
    // ------------------------------
    float3 normalSample = gNormalMap.Sample(Sampler_Aniso, input.uv).xyz;
    normalSample = normalSample * 2.f - 1.f; // 0..1 -> -1..1

    float3 n =
        normalSample.x * tangentWS +
        normalSample.y * binormalWS +
        normalSample.z * normalWS;

    n = normalize(n);

    // ------------------------------
    // Light & Lambert Diffuse
    // ------------------------------
    float3 L = normalize(-gLightDirection);

    float nDotL = saturate(dot(n, L));
    
    float3 observedArea = (nDotL, nDotL, nDotL);

    float3 albedo = gDiffuseMap.Sample(Sampler_Aniso, input.uv).rgb;

    float3 diffuse =
       saturate((albedo *
        gLightIntensity) / PI);

    // ------------------------------
    // Specular (Phong)
    // ------------------------------
    float3 viewDir = normalize(gCameraPos - input.worldPos);
    
    float3 reflectDir = reflect(-L, n);

    float gloss =
        gGlossMap.Sample(Sampler_Aniso, input.uv).r *
        gShininess;

    float specularStrength =
        pow(saturate(dot(reflectDir, viewDir)), gloss);

    float3 specularColor =
        saturate(gSpecularMap.Sample(Sampler_Aniso, input.uv).rgb *
        specularStrength);

    // ------------------------------
    // Final Output (CLAMP REQUIRED)
    // ------------------------------
    float3 finalColor = observedArea * (diffuse + specularColor + ambient);

    return float4(saturate(finalColor), 1.f);
}




//---------------------------------------------------------------
// Techniques
//---------------------------------------------------------------

RasterizerState SolidBackface
{
    CullMode = BACK;
    FillMode = SOLID;
};

BlendState OpaqueBlend
{
    BlendEnable[0] = FALSE;
};

DepthStencilState EnableDepthWrite
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL; // <-- IMPORTANT
    DepthFunc = LESS;
};


technique11 PointTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Point()));

        SetRasterizerState(SolidBackface);
        SetBlendState(OpaqueBlend, float4(0, 0, 0, 0), 0xFFFFFFFF);
        SetDepthStencilState(EnableDepthWrite, 0);
    }
}

technique11 LinearTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Linear()));

        SetRasterizerState(SolidBackface);
        SetBlendState(OpaqueBlend, float4(0, 0, 0, 0), 0xFFFFFFFF);
        SetDepthStencilState(EnableDepthWrite, 0);
    }
}

technique11 AnisoTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Aniso()));

        SetRasterizerState(SolidBackface);
        SetBlendState(OpaqueBlend, float4(0, 0, 0, 0), 0xFFFFFFFF);
        SetDepthStencilState(EnableDepthWrite, 0);
    }
}

