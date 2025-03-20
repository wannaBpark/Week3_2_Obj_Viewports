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

    // 변환된 위치를 MVP 행렬을 통해 클립 공간으로 변환
    output.position = mul(float4(input.position, 1.0f), MVP);

    output.color = input.color; // 색상 그대로 전달

    return output;
}