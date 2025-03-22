// ShaderMesh.hlsl
cbuffer constants : register(b0)
{
    matrix Model;
    matrix View;
    matrix Projection;
}

struct VS_INPUT
{
    float3 position : POSITION; // Input position from vertex buffer
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float4 color : COLOR; // Input color from vertex buffer
    float2 texcoord : TEXCOORD0;
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // Transformed position to pass to the pixel shader
    float4 color : COLOR; // Color to pass to the pixel shader
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
    return output;
}


PS_OUTPUT mainPS(PS_INPUT input)
{
    PS_OUTPUT output;

    // 색상 설정 (예: 흰색)
    // output.color = input.color;
    output.color = float4(1, 1, 1, 1);
    
    return output;
}