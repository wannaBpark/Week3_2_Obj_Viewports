#pragma once
#include "Object/UObject.h"
#include "Object/Mesh/StaticMesh.h"
#include "Object/ObjectMacro.h"

/*
* UStaticMeshComp에게 소유될 UObject
* [런타임]에서 사용될 정적 메시 에셋의 인스턴스 입니다
* 가공된 메시 구조체[FStaticMesh]를 참조함
*/
class UStaticMesh : public UObject
{
    DECLARE_CLASS(UStaticMesh, UObject)
private:
    FStaticMesh* StaticMeshAsset;
    TMap<FString, class UMaterial*> Materials;

public:
    const FString& GetAssetPathFileName()
    {
        return StaticMeshAsset->Name;
    }

    FStaticMesh* GetStaticMeshAsset() { return StaticMeshAsset; }
    const FStaticMesh* GetStaticMeshAsset() const { return StaticMeshAsset; }

    void SetStaticMeshAsset(FStaticMesh* InStaticMesh)
    {
        StaticMeshAsset = InStaticMesh;
    }
};
