#pragma once
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Object/Mesh/UStaticMesh.h"
#include "SimpleJSON/Json.hpp"
#include "Object/ObjectMacro.h"

/*
* 3D 메시를 렌더링하고 관리하는 컴포넌트
* 머티리얼, 충돌, 트랜스폼을 관리
*/
class UMeshComponent : public UPrimitiveComponent
{
    DECLARE_CLASS(UMeshComponent, UPrimitiveComponent);
	using Super = UPrimitiveComponent;
public:
    UMeshComponent() = default;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Render() override;								// [TODO] Render시 Preload된 UStaticMesh 인스턴스에 대한 GPU 메모리[Vertex Buffer, IL] 생성

	void SetMeshName(FName InMeshName) { MeshName = InMeshName; }
	FName GetMeshName() const { return MeshName; }

	//virtual void Serialize(bool bIsLoading, json::JSON Handle) = 0;

private:
	FName MeshName;

};

