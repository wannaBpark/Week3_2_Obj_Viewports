cbuffer constants : register(b0)
{
    matrix MVP;
    float2 atlasSize;
    float2 atlasOffset;
}

struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 localPosition : POSITION;
    float2 texcoord : TEXCOORD0;
};

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    float2 uv = input.texcoord;
    uv.x = uv.x * atlasSize.x + atlasOffset.x;
    uv.y = uv.y * atlasSize.y + atlasOffset.y;
    output.texcoord = uv;
    
    output.position = mul(float4(input.position, 1.0f), MVP);
    
    
    return output;
}