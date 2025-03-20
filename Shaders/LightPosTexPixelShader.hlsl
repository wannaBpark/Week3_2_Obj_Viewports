Texture2D g_texture0 : register(t0);
SamplerState g_sampler : register(s0);

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

struct PS_INPUT
{
    float4 position : SV_POSITION; // Transformed position to pass to the pixel shader
    float4 posWorld : POSITION;
    float4 color : COLOR; // Color to pass to the pixel shader
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

struct PS_OUTPUT
{
    float4 color : SV_TARGET0;
    float4 UUID : SV_TARGET1;
};

PS_OUTPUT mainPS(PS_INPUT input)
{
    PS_OUTPUT output;
    
    float3 normalWorld = normalize(input.normal);
    float3 toEye = normalize(eyeWorldPos - input.posWorld.xyz);    
    float3 color = bUseVertexColor ? g_texture0.Sample(g_sampler, float2(input.texcoord.x, 1 - input.texcoord.y)).rgb : CustomColor;
    
    float avg = (color.r + color.g + color.b) / 3.0f;
    //clip(avg < 0.1f ? -1 : 1); // png 투명 이미지 밝기 작은 값들 제거 
    if (bIsPicked)
    {
        float rim = (1.0 - max(0.0, dot(normalWorld, toEye)));
        rim = smoothstep(0.0, 1.0, rim);
        rim = pow(abs(rim), 2.0f);
        color += rim * float4(1.0f, 1.0, 1.0, 1.0) * 1.f;
        //color *= 0.5;
    }
    output.color = float4(color, 1.0f);
    
    output.UUID = indexColor;

    return output;
}