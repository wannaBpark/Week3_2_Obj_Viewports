// ShaderMesh.hlsl
Texture2D g_texture0 : register(t0);

SamplerState sampler0 : register(s0);

cbuffer constants : register(b0)
{
    matrix Model;
    matrix View;
    matrix Projection;
}

struct VS_INPUT
{
    float3 position : POSITION; // Input position from vertex buffer
    float3 normal : NORMAL; // NORMAL0 -> NORMAL
    float3 tangent : TANGENT; // TANGENT0 -> TANGENT
    float4 color : COLOR; // COLOR0 -> COLOR
    float2 texcoord : TEXCOORD; // TEXCOORD0 -> TEXCOORD
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // Input position from vertex buffer
    float3 normal : NORMAL; // NORMAL0 -> NORMAL
    float3 tangent : TANGENT; // TANGENT0 -> TANGENT
    float4 color : COLOR; // COLOR0 -> COLOR
    float2 texcoord : TEXCOORD; // TEXCOORD0 -> TEXCOORD
};

struct PS_OUTPUT
{
    float4 color : SV_TARGET0;
    float4 UUID : SV_TARGET1;
};

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    float4 position = float4(input.position.xyz, 1.f);

    position = mul(position, Model);
    position = mul(position, View);
    output.position = mul(position, Projection);
    
    output.color = input.color;
    output.texcoord = input.texcoord;
    output.normal = input.normal;
    output.tangent = input.tangent;
    
    return output;
}

PS_OUTPUT mainPS(PS_INPUT input)
{
    PS_OUTPUT output;

    float3 color = g_texture0.Sample(sampler0, float2(input.texcoord.x, input.texcoord.y)).rgb;

    output.color = float4(color.xyz, 1.0f);
    output.UUID = float4(0.0f, 0.0f, 0.0f, 0.0f); // UUID 초기화
    
    return output;
}