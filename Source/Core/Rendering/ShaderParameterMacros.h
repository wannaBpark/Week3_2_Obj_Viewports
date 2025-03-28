#pragma once

#include "Core/Math/Matrix.h"
#include "Core/Math/Vector.h"

#define BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(StructType) \
    class alignas(16) StructType \
    { \
    public:
#define SHADER_PARAMETER(Type, Name) \
    Type Name;

#define END_GLOBAL_SHADER_PARAMETER_STRUCT() \
    };

struct FPosColor
{
    float X, Y, Z;    // Position
    float R, G, B, A; // Color
};

struct FPosColorNormal
{
    float X, Y, Z;    // Position
    float R, G, B, A; // Color 
    float NX, NY, NZ; // Normal 
};

struct FPosNormalTex
{
    float X, Y, Z;    // Position
    float NX, NY, NZ; // Normal 
    float U, V;       // Texcoord
};

struct FPosColorNormalTex
{
    float X, Y, Z;    // Position
    float R, G, B, A; // Color
    float NX, NY, NZ; // Normal 
    float U, V;       // Texcoord
};

struct FMatrix3
{
    FMatrix Model;
    FMatrix View;
    FMatrix Projection;
};

enum class InputLayoutType
{
    POSCOLOR,
    POSCOLORNORMAL,
    POSNORMALTEX,
    POSCOLORNORMALTEX,

    POSNORMALTANGENTCOLORTEX,
};

// Constant Buffer 구조체 정의

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FConstants)
    SHADER_PARAMETER(FMatrix, MVP)
    SHADER_PARAMETER(FVector4, Color)
    SHADER_PARAMETER(uint32, bUseVertexColor)
    SHADER_PARAMETER(FVector, eyeWorldPos)
    SHADER_PARAMETER(FVector4, indexColor)
    SHADER_PARAMETER(uint32, bIsPicked)
    SHADER_PARAMETER(FVector, Padding)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FPickingConstants)
    SHADER_PARAMETER(FVector4, UUIDColor)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FDepthConstants)
    SHADER_PARAMETER(unsigned int, DepthOffset)
    SHADER_PARAMETER(int, nearPlane)
    SHADER_PARAMETER(int, farPlane)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(ConstantUpdateInfo)
    SHADER_PARAMETER(const FMatrix&, WorldPosition)
    SHADER_PARAMETER(const FVector4&, Color)
    SHADER_PARAMETER(uint32, bUseVertexColor)
    SHADER_PARAMETER(FVector, eyeWorldPos)
    SHADER_PARAMETER(FVector4, indexColor)
 END_GLOBAL_SHADER_PARAMETER_STRUCT()

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FAtlasConstants)
    SHADER_PARAMETER(FMatrix, WorldPosition)
    SHADER_PARAMETER(FVector4, AtlasSzOffset)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

struct FLightConstants 
{
    FMatrix Model;              
    FMatrix Views[4];           
    FMatrix Projection;         
    FMatrix InvTranspose;       
    FVector4 Color;             
    uint32  bUseVertexColor;    
    FVector eyeWorldPos;        
    FVector4 indexColor;        
    uint32  bIsPicked;          
    FVector Padding;            
    uint32 ViewportIndex;       
    FVector Padding2;           
};

struct FLineConstants
{
    FMatrix Model;
    FMatrix Views[4];
    FMatrix Projection;
    uint32 bIsPicked;
    FVector Padding1;
    FVector4 CustomColor;
    uint32 bUseVertexColor;
    FVector Padding2;
    uint32 ViewportIndex;
    FVector Padding3;
};


BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FStaticMeshVertexConstant)
    SHADER_PARAMETER(FMatrix, InverseTranspose)
    SHADER_PARAMETER(FMatrix, Model)
    SHADER_PARAMETER(FMatrix, View)
    SHADER_PARAMETER(FMatrix, Projection)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FGlobalLightConstant)
    SHADER_PARAMETER(FVector4, Ambient)
    SHADER_PARAMETER(FVector4, Diffuse)
    SHADER_PARAMETER(FVector4, Specular)
    SHADER_PARAMETER(FVector, Emissive)
    SHADER_PARAMETER(float, Padding1)
    SHADER_PARAMETER(FVector, Direction)
    SHADER_PARAMETER(float, Padding)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FMaterialConstant)
    SHADER_PARAMETER(FVector4, Ambient)
    SHADER_PARAMETER(FVector4, Diffuse)
    SHADER_PARAMETER(FVector4, Specular)
    SHADER_PARAMETER(FVector, Emissive)
    SHADER_PARAMETER(float, Roughness)
    //SHADER_PARAMETER(float, Metalic)
    //SHADER_PARAMETER(FVector2D, Padding)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FCameraPositionConstant)
    SHADER_PARAMETER(FVector, CameraPosition)
    SHADER_PARAMETER(float, Padding)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FStaticPickingConstants)
    SHADER_PARAMETER(FVector4, UUIDColor)
    SHADER_PARAMETER(uint32, bIsPicked)
END_GLOBAL_SHADER_PARAMETER_STRUCT()


//struct FLightConstants {
//    FMatrix Model;              // 오프셋: 0,    크기: 64바이트
//    FMatrix Views[4];           // 오프셋: 64,   크기: 256바이트
//    FMatrix Projection;         // 오프셋: 320,  크기: 64바이트
//    FMatrix InvTranspose;       // 오프셋: 384,  크기: 64바이트
//    FVector4 Color;             // 오프셋: 448,  크기: 16바이트
//    uint32 bUseVertexColor;     // 오프셋: 464,  크기: 4바이트
//    FVector Padding1;          // 패딩,         크기: 12바이트
//    FVector eyeWorldPos;       // 오프셋: 480,  크기: 12바이트
//    float Padding2;             // 패딩,         크기: 4바이트
//    FVector4 indexColor;        // 오프셋: 496,  크기: 16바이트
//    uint32 bIsPicked;           // 오프셋: 512,  크기: 4바이트
//    FVector Padding3;          // 패딩,         크기: 12바이트
//    uint32 ViewportIndex;       // 오프셋: 528,  크기: 4바이트
//    FVector Padding4;          // 패딩,         크기: 12바이트
//};

//BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FLightConstants)
//    SHADER_PARAMETER(FMatrix, Model)
//    SHADER_PARAMETER(FMatrix, Views[4])
//    SHADER_PARAMETER(FMatrix, Projection)
//    SHADER_PARAMETER(FMatrix, InvTranspose)
//    SHADER_PARAMETER(FVector4, Color)
//    SHADER_PARAMETER(uint32, bUseVertexColor)
//    SHADER_PARAMETER(FVector, eyeWorldPos)
//    SHADER_PARAMETER(FVector4, indexColor)
//    SHADER_PARAMETER(uint32, bIsPicked)
//    SHADER_PARAMETER(FVector, Padding)
//    SHADER_PARAMETER(uint32, ViewportIndex)
//    SHADER_PARAMETER(FVector, Padding2)
//END_GLOBAL_SHADER_PARAMETER_STRUCT()
