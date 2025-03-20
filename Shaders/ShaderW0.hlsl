// ShaderW0.hlsl
cbuffer constants : register(b0)
{
    matrix MVP;
    float4 CustomColor;
    uint bUseVertexColor;
    float3 eyeWorldPos;
    float4 indexColor;
}

struct VS_INPUT
{
    float4 position : POSITION; // Input position from vertex buffer
    float4 color : COLOR;       // Input color from vertex buffer
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // Transformed position to pass to the pixel shader
    float4 color : COLOR;          // Color to pass to the pixel shader

};

struct PS_OUTPUT
{
    float4 color : SV_TARGET0;
    float4 UUID : SV_TARGET1;
};

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;

    output.position = mul(input.position, MVP);
    output.color = bUseVertexColor == 1 ? input.color : CustomColor;
    return output;
}


PS_OUTPUT mainPS(PS_INPUT input)
{
    PS_OUTPUT output;

    // 색상 설정 (예: 흰색)
    output.color = input.color;
    output.UUID = indexColor;
    
    return output;
}