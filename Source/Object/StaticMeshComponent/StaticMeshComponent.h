#pragma once
#include "MeshComponent.h"
#include "Object/ObjectMacro.h"
#include "Core/Container/Array.h"

/*
* 특정 Actor에 부착되어 메시 렌더링을 담당하는 컴포넌트
* @UStaticMesh : 에셋 참조 - FObjManager를 통해 에셋 로드
*/

class UMaterial;

class UStaticMeshComponent :
    public UMeshComponent
{
	DECLARE_CLASS(UStaticMeshComponent, UMeshComponent)

	virtual void Tick(float DeltaTime) override;
	virtual void Render() override;
	virtual EPrimitiveType GetType() override;

public:
	void SetStaticMesh(const FString& staticMeshPath);
private:
	void CreateVertexBuffer();
	void CreateIndexBuffer();

public:
	UStaticMesh* StaticMesh;
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;

	const TArray<UMaterial*>& GetMaterials() { return Materials; }
	const UMaterial* GetMaterial(uint32 Index);
	void SetMaterial(uint32 Index, UMaterial* InMaterial);

public:
	void LoadAndConstruct(const FActorComponentInfo Info) override;
	FActorComponentInfo GetActorComponentInfo() override;

private:
	TArray<UMaterial*> Materials;
};

