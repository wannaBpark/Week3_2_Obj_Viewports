
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

cbuffer ViewportConstants : register(b1)
{
    uint VPIndex;
    float3 padding;
}

struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
};
struct PS_INPUT
{
    float4 position : SV_POSITION; // Transformed position to pass to the pixel shader
    float4 posWorld : POSITION;
    float4 color : COLOR; // Color to pass to the pixel shader
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
};
PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    float4 position;

    position = mul(float4(input.position, 1.0f), Model);
    output.posWorld = position;
    
    //matrix View = Views[ViewportIndex]; 
    matrix View = Views[VPIndex];
    position = mul(position, View);
    output.position = mul(position, Projection);

    // 수정된 부분 (float3 변환)
    output.normal = normalize(mul(float4(input.normal, 0.0f), InvTranspose).xyz);
    
    // TEXCOORD0 전달
    output.texcoord = input.texcoord;

    // 색상 처리
    output.color = bUseVertexColor == 1 ? input.color : CustomColor;

    return output;
}