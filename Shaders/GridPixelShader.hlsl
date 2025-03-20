struct PS_INPUT
{
    float4 position : SV_POSITION; // 최종 화면 위치
};

float4 mainPS(PS_INPUT input) : SV_Target
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f); // 흰색 그리드 라인 렌더링
}