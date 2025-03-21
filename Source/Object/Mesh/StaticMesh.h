#pragma once
#include "Core/Container/Array.h"
#include "Core/Math/Vector.h"
#include "Core/Rendering/ShaderParameterMacros.h"

struct FNormalVertex
{
    FVector pos;
    FVector normal;
    FVector4 color;
    FVector2D tex;
};

/*
* 가공된 메시 [실제] 데이터 구조체
* [Preload]타임에 사용될 실제, 그러나 사용되기 좋도록 가공된 데이터
* @Vertices : <정점의 위치, 노말, 색상, 텍스쳐 좌표>
* @Indices : 정점의 인덱스 정보
* @TODO: StaticMesh가 어느 정보를 얼마나 담을지 고민 필요
*/
struct FStaticMesh
{
    std::string PathFileName;

    TArray<FNormalVertex> Vertices;
    TArray<uint32> Indices;

};
