#pragma once
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Object/Mesh/UStaticMesh.h"
#include "SimpleJSON/Json.hpp"

/*
* 3D 메시를 렌더링하고 관리하는 컴포넌트
* 머티리얼, 충돌, 트랜스폼을 관리
*/
class JSON;
class UMeshComponent : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
public:
    UMeshComponent() = default;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Render() override;								// [TODO] Render시 Preload된 UStaticMesh 인스턴스에 대한 GPU 메모리[Vertex Buffer, IL] 생성

	void SetMeshName(FName InMeshName) { MeshName = InMeshName; }
	FName GetMeshName() const { return MeshName; }

	virtual void Serialize(bool bIsLoading, JSON Handle) = 0;

private:
	FName MeshName;

};

/*
* 특정 Actor에 부착되어 메시 렌더링을 담당하는 컴포넌트
* @UStaticMesh : 에셋 참조 - FObjManager를 통해 에셋 로드
*/
class UStaticMeshComp : public UMeshComponent
{
    UStaticMesh* StaticMesh;

    using Super = UMeshComponent;
    void Serialize(bool bIsLoading, JSON Handle)
    {
        Super::Serialize(bIsLoading, Handle);

        if (bIsLoading)
        {
            FString assetName;
            Handle << "ObjStaticMeshAsset" << assetName;
            //StaticMesh = FObjManager::LoadObjStaticMesh(assetName);
        }
        else
        {
            FString assetName;
            //FString assetName = StaticMesh->GetAssetPathFileName();
            Handle << "ObjStaticMeshAsset" << assetName;
        }
    }
};