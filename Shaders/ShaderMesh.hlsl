// ShaderMesh.hlsl

Texture2D g_DiffuseMap : register(t0);

SamplerState g_sampler0 : register(s0);

struct LightDesc
{
    float4 Ambient;     
    float4 Diffuse;     
    float4 Specular;    
    float3 Emissive;    
    float Padding1;     
    float3 Direction;   
    float Padding;      
};

struct MaterialDesc
{
    float4 Ambient;     
    float4 Diffuse;     
    float4 Specular;    
    float3 Emissive;    
    float Roughness;    
};

cbuffer Transform_buffer : register(b0)
{
    Matrix InverseTranspose;
    matrix Model;
    matrix View;
    matrix Projection;
}

cbuffer LightBuffer : register(b1)
{
    LightDesc GlobalLight;
};

cbuffer CameraPositionBuffer : register(b2)
{
    float3 CameraPosition;
    float Padding;
};

cbuffer MaterialBuffer : register(b3)
{
    MaterialDesc Material;
};

cbuffer PickingBuffer : register(b4)
{
    float4 indexColor;
    uint bIsPicked;
}

struct VS_INPUT
{
    float3 position : POSITION; // Input position from vertex buffer
    float3 normal : NORMAL; // NORMAL0 -> NORMAL
    float3 tangent : TANGENT; // TANGENT0 -> TANGENT
    float4 color : COLOR; // COLOR0 -> COLOR
    float2 texcoord : TEXCOORD; // TEXCOORD0 -> TEXCOORD
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // Input position from vertex buffer
    float3 normal : NORMAL; // NORMAL0 -> NORMAL
    float3 tangent : TANGENT; // TANGENT0 -> TANGENT
    float4 color : COLOR; // COLOR0 -> COLOR
    float2 texcoord : TEXCOORD0; // TEXCOORD0 -> TEXCOORD
    float3 worldPos : TEXCOORD1;
};

struct PS_OUTPUT
{
    float4 color : SV_TARGET0;
    float4 UUID : SV_TARGET1;
};

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    float4 position = float4(input.position.xyz, 1.0f);

    position = mul(position, Model);
    output.worldPos = position.xyz;
    position = mul(position, View);
    output.position = mul(position, Projection);
    
    output.color = input.color;
    output.texcoord = input.texcoord;
    
    if (length(input.normal) < 0.001f)
        output.normal = input.normal;
    else
    {
        output.normal = mul(float4(input.normal, 0.f), InverseTranspose).xyz;
        output.normal = normalize(output.normal);
    }
    
    output.tangent = input.tangent;
    
    return output;
}

float4 ComputeLight(float3 normal, float2 uv, float3 worldPosition)
{
    // 각 광원 요소에 대한 색상을 초기화
    float4 ambientColor = 0;
    float4 diffuseColor = 0;
    float4 specularColor = 0;
    float4 emissiveColor = 0;
    
    // Ambient 처리
    {
        float4 color = GlobalLight.Ambient * Material.Ambient;
        ambientColor = g_DiffuseMap.Sample(g_sampler0, float2(uv.x, uv.y)) * color;
    }
    
    // Diffuse 처리
    {
        float4 color = g_DiffuseMap.Sample(g_sampler0, float2(uv.x, uv.y));
        float value = saturate(dot(-GlobalLight.Direction, normalize(normal))); // 음수 값 제거
        
        float diffuseBoost = lerp(1.0f, 1.3f, 1 - Material.Roughness);
        
        diffuseColor = color * value * GlobalLight.Diffuse * Material.Diffuse * diffuseBoost;
    }
    
    // Specular 처리
    {
        // 반사 벡터
        float3 reflectionLight = GlobalLight.Direction - (2 * normal * dot(GlobalLight.Direction, normal));
        reflectionLight = normalize(reflectionLight);
        
        // 카메라 위치에서 세계 좌표까지의 방향 벡터
        float3 cameraPosition = CameraPosition - worldPosition;
        cameraPosition = normalize(cameraPosition);
        
        float value = saturate(dot(reflectionLight, cameraPosition));
        float shininess = lerp(64.0f, 1.0f, 1 - saturate(Material.Roughness));
        float specular = pow(value, shininess);
        
        specular *= lerp(1.0f, 0.1f, pow(1 - Material.Roughness, 2));
        
        specularColor = GlobalLight.Specular * Material.Specular * specular;
    }
    
    //Emissive 처리
    {
        float3 cameraPosition = normalize(CameraPosition - worldPosition);
        
        float value = saturate(dot(cameraPosition, normal));
        float emissive = 1.f - value;

        emissive = smoothstep(0.0f, 1.0f, emissive);
        emissive = pow(emissive, 2);
        
        emissiveColor = float4(GlobalLight.Emissive * Material.Emissive * emissive, 1.0f);
    }
    
    return ambientColor + diffuseColor + specularColor + emissiveColor;
}

PS_OUTPUT mainPS(PS_INPUT input)
{
    PS_OUTPUT output;
    float4 color;

    if (length(input.normal) <= 0.1f)
        color = float4(g_DiffuseMap.Sample(g_sampler0, float2(input.texcoord.x, input.texcoord.y)).xyz, 1.0f);
    else
        color = ComputeLight(input.normal, input.texcoord, input.worldPos);
    
    if (bIsPicked)
    {
        float3 toEye = normalize(CameraPosition - input.worldPos.xyz);
        float rim = (1.0 - max(0.0, dot(normalize(input.normal), toEye)));
        rim = smoothstep(0.0, 1.0, rim);
        rim = pow(abs(rim), 10.0f);
        color += rim * float4(1.0f, 1.0f, 1.0f, 0.5f) * 0.3f;
        //color *= 0.5;
    }
    
    output.color = float4(color.xyz, 1.f);
    output.UUID = indexColor; // UUID 초기화
    
    return output;
}

