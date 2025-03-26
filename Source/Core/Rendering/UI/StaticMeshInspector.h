#pragma once
#include "Core/Container/Array.h"
#include "Core/Container/String.h"
#include "Object/Mesh/StaticMesh.h"
#include "MaterialEditor.h"
class UStaticMeshComponent;

class FStaticMeshInspector
{
public:
	FStaticMeshInspector() = default;
	~FStaticMeshInspector() = default;

	void Init(UStaticMeshComponent* Comp);
	void Update();

	UStaticMeshComponent* GetCurrentStaticMeshComponent() const { return Component; }

private:
	void UpdateStaticMeshCombo();
	void UpdateMaterialCombo();
	UStaticMeshComponent* Component = nullptr;

	TArray<class FStaticMesh*> StaticMeshes;
	// Static Mesh Combo에서 몇 번째 인덱스의 스태틱 메시를 사용중인지에 대한 인덱스
	UINT StaticMeshIndex = 0;

	// Material
	FString MaterialNameStr;

	TArray<class UMaterial*> ComponentMaterials;
	TArray<FObjMaterialInfo> AllMaterials;
	MaterialEditor MaterialEditor;

	bool isMaterialEditorUpdated = false;
};

