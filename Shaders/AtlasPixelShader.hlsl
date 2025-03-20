Texture2D atlasTexture : register(t0);
SamplerState atlasSampler : register(s0);

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 localPosition : POSITION;
    float2 texcoord : TEXCOORD0; 
};

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    float2 uv = input.texcoord;
    float3 color = atlasTexture.Sample(atlasSampler, uv).rgb;
    float avg = (color.r + color.g + color.b) / 3.0f;
    clip(avg < 0.5f ? -1 : 1);
    return float4(color, 1.0f);
}

float4 mainPSNoClip(PS_INPUT input) : SV_TARGET
{
    float2 uv = input.texcoord;
    float3 color = atlasTexture.Sample(atlasSampler, uv).rgb;
    return float4(color, 1.0f);
}