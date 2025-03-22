cbuffer LineConstants : register(b0)
{
    matrix Model;
    matrix Views[4];
    matrix Projection;
    uint bIsPicked;
    uint ViewportIndex;
}

struct PS_INPUT
{
    float4 position : SV_POSITION; // 변환된 화면 좌표
    float4 color : COLOR; // 색상
};

float4 mainPS(PS_INPUT input) : SV_Target
{
    float4 color = input.color;   
    return color; // 흰색 그리드 또는 좌표축 라인 렌더링
}