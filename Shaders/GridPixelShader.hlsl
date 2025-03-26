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
struct PS_INPUT
{
    float4 position : SV_POSITION; // 변환된 화면 좌표
    float4 color : COLOR; // 색상
    float3 worldPos : TEXCOORD0;
};

float4 mainPS(PS_INPUT input) : SV_TARGET
{    
    float3 fogColor = float3(0.025f, 0.025f, 0.025f); // ClearColor(배경색)

    float3 worldPos = input.worldPos.xyz;
    float3 camPos = CamPos;
    camPos.z = 0;

    worldPos.z = 0;
    
    float dist = length(worldPos - camPos);

    // 지수 기반 페이드 아웃 - 더 자연스러운 안개 느낌
    float density = 0.08f; // 밀도 조절
    float fogFactor = saturate(1.0f - exp(-density * dist));

    // 원래 색과 안개색 보간
    float3 finalColor = lerp(input.color.rgb, fogColor, fogFactor);
    
    float4 color = float4(finalColor, 1.0f);
    
    return color; // 흰색 그리드 또는 좌표축 라인 렌더링
}