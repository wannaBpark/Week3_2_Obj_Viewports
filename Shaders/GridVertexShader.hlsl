cbuffer LineConstants : register(b0)
{
    matrix Model;
    matrix Views[4];
    matrix Projection;
    uint bIsPicked;
    float3 CamPos;
    float4 CustomColor; // 3개 x,y,z축 컬러에 사용
    uint bUseVertexColor;
    float3 Padding3;
    uint ViewportIndex;
    float3 Padding2;
    
}
 
struct VS_INPUT
{
    float3 position : POSITION;   // 격자, 바운딩 박스 선분의 위치
    float4 color : COLOR;         // 선분의 색상
};

struct PS_INPUT
{
    float4 position : SV_POSITION;   // 변환된 화면 좌표
    float4 color : COLOR;            // 색상
    float3 worldPos : TEXCOORD0;
};

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    float4 position;

    position = mul(float4(input.position, 1.0f), Model);
    matrix View = Views[0];
    output.worldPos = position;
    position = mul(position, View);
    output.position = mul(position, Projection); // TODO
    output.color = bUseVertexColor == 1 ? input.color : CustomColor;
    
    return output;
}