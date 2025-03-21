Texture2D g_texture0 : register(t0);
SamplerState g_sampler : register(s0);

cbuffer constants : register(b0)
{
    matrix MVP;
    float4 CustomColor;
    uint bUseVertexColor;  // 텍스쳐 사용할지 결정하는 변수
    float3 eyeWorldPos;
    float4 indexColor;
    uint bIsPicked;
    float3 Padding;
    // 선택 됐는지 여부 확인하는 변수 추가
}

struct PS_INPUT
{
    float4 position : SV_POSITION; // Transformed position to pass to the pixel shader
    float4 color : COLOR; // Color to pass to the pixel shader
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 color : SV_TARGET0;
    float4 UUID : SV_TARGET1;
};

PS_OUTPUT mainPS(PS_INPUT input)
{
    // output.depthPosition = output.position;
    
    PS_OUTPUT output;
    
    //output.normal = input.normal;
    //output.texcoord = input.texcoord;
    float3 color = bUseVertexColor ? g_texture0.Sample(g_sampler, float2(input.texcoord.x, 1 - input.texcoord.y)).rgb : CustomColor;
    
    float avg = (color.r + color.g + color.b) / 3.0f; 
    clip(avg < 0.1f ? -1 : 1);                          // png 투명 이미지 밝기 작은 값들 제거 
    if(bIsPicked)
    {
        color *= 0.5;
    }
    output.color = float4(color, 1.0f);
    output.UUID = indexColor;

    return output;
}