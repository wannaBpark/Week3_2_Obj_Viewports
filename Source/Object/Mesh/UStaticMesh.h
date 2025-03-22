#pragma once
#include "Object/UObject.h"
#include "Object/Mesh/StaticMesh.h"

/*
* UStaticMeshComp에게 소유될 UObject
* [런타임]에서 사용될 정적 메시 에셋의 인스턴스 입니다
* 가공된 메시 구조체[FStaticMesh]를 참조함
*/
class UStaticMesh : public UObject
{
public:
    FStaticMesh* StaticMeshAsset;

public:
    const FString& GetAssetPathFileName()
    {
        return StaticMeshAsset->PathFileName;
    }

    void SetStaticMeshAsset(FStaticMesh* InStaticMesh)
    {
        StaticMeshAsset = InStaticMesh;
    }
};
