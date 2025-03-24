cbuffer LineConstants : register(b0)
{
    matrix Model;
    matrix Views[4];
    matrix Projection;
    uint bIsPicked;
    float3 Padding1;
    float4 CustomColor; // 3개 x,y,z축 컬러에 사용
    uint bUseVertexColor;
    float3 Padding3;
    uint ViewportIndex;
    float3 Padding2;
}
struct PS_INPUT
{
    float4 position : SV_POSITION; // 변환된 화면 좌표
    float4 color : COLOR; // 색상
};

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    float4 color = input.color;   
    return color; // 흰색 그리드 또는 좌표축 라인 렌더링
}