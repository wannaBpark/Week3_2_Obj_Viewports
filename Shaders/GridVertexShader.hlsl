cbuffer constants : register(b0)
{
    matrix Model;
    matrix Views[4];
    matrix Projection;
    matrix InvTranspose;
    float4 CustomColor;
    uint bUseVertexColor;
    float3 eyeWorldPos;
    float4 indexColor;
    uint bIsPicked;
    float3 Padding;
    uint ViewportIndex;
    float3 Padding2;
}

struct VS_INPUT
{
    float3 position : POSITION;   // 격자 선분의 위치
    float4 color : COLOR;         // 선분의 색상
};

struct PS_INPUT
{
    float4 position : SV_POSITION;   // 변환된 화면 좌표
    float4 color : COLOR;            // 색상
};


PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    float4 position = mul(float4(input.position, 1.0f), Model);
    matrix View;
    if (ViewportIndex == 0)
    {
        View = Views[0];
    }
    else if (ViewportIndex == 1)
    {
        View = Views[1];
    }
    else if (ViewportIndex == 2)
    {
        View = Views[2];
    }
    else if (ViewportIndex == 3)
    {
        View = Views[3];
    }
    position = mul(position, View);
    output.position = mul(position, Projection);

    output.color = input.color; // 색상 그대로 전달

    return output;
}