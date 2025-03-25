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
    
    input.normal = input.position.xyz;

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
        float value = dot(-GlobalLight.Direction, normalize(normal));
        diffuseColor = color * value * GlobalLight.Diffuse * Material.Diffuse;
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
        float shininess = pow(1.f - Material.Roughness, 4.f) * 128.f;
        float specular = pow(value, shininess);
        specular = smoothstep(0.0f, 1.0f, specular);
        //float specular = pow(value, Material.Roughness);
        
        
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
    
    output.color = float4(color.xyz, 1.f);
    output.UUID = float4(0.0f, 0.0f, 0.0f, 0.0f); // UUID 초기화
    
    return output;
}

//float4 PS(VertexOutput input) : SV_TARGET
//{
//    float3 normal = normalize(input.WorldNormal);
//    float3 lightDir = normalize(LightPosition - input.WorldPos);
    
//    // Diffuse Lighting
//    float diffuseIntensity = saturate(dot(normal, lightDir));
//    float3 diffuse = diffuseIntensity * DiffuseColor;

//    // Specular Lighting (옵션)
//    float3 viewDir = normalize(CameraPosition - input.WorldPos);
//    float3 reflectDir = reflect(-lightDir, normal);
//    float specularIntensity = pow(saturate(dot(viewDir, reflectDir)), SpecularPower);
//    float3 specular = specularIntensity * SpecularColor;

//    // Texture
//    float4 texColor = DiffuseTexture.Sample(Sampler, input.TexCoord);

//    float3 finalColor = (diffuse + specular) * texColor.rgb;

//    return float4(finalColor, texColor.a);
//}

