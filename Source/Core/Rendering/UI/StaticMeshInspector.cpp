#include "StaticMeshInspector.h"
#include "ImGui/imgui.h"
#include "Object/StaticMeshComponent/StaticMeshComponent.h"
#include "Core/Container/ObjectIterator.h"
#include "Object/Material/Material.h"

void FStaticMeshInspector::Init(UStaticMeshComponent* Comp)
{
	if (Comp == nullptr)
		return;

	Component = Comp;

	StaticMeshes.Empty();
	StaticMeshIndex = 0;

	int Count = 0;
	// 이 스태틱메시 몇 번째인지
	for (TObjectIterator<UStaticMesh> It(UEngine::Get().GObjects.begin(), UEngine::Get().GObjects.end()); It; ++It)
	{
		if (*It == nullptr)
			continue;

		UStaticMesh* Mesh = *It;
		StaticMeshes.Add(Mesh);
		if (Mesh == Comp->StaticMesh)
		{
			StaticMeshIndex = Count;
		};
		Count++;
	}

	Materials.Empty();
	AllMaterials.Empty();

	Materials = Comp->GetMaterials();
	AllMaterials;
	// 모든 머티리얼들 가져오기
	for (TObjectIterator<UMaterial> It(UEngine::Get().GObjects.begin(), UEngine::Get().GObjects.end()); It; ++It)
	{
		UMaterial* Material = *It;
		AllMaterials.Add(Material);
	}
}

void FStaticMeshInspector::Update()
{
	if (Component == nullptr)
		return;

	ImGui::Begin("Static Mesh Inspector");
	UpdateStaticMeshCombo();
	UpdateMaterialCombo();
	ImGui::End();
}

void FStaticMeshInspector::UpdateStaticMeshCombo()
{
	if (StaticMeshes.Num() == 0)
		return;
	if (ImGui::BeginCombo("Static Mesh", StaticMeshes[StaticMeshIndex]->GetStaticMeshAsset()->PathFileName.c_char()))
	{
		for (int i = 0; i < StaticMeshes.Num(); i++)
		{
			bool IsSelected = StaticMeshIndex == i;
			if (ImGui::Selectable(StaticMeshes[i]->GetStaticMeshAsset()->PathFileName.c_char(), IsSelected))
			{
				StaticMeshIndex = i;
				Component->SetStaicMesh(StaticMeshes[i]->GetStaticMeshAsset()->PathFileName);
			}
			if (IsSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

void FStaticMeshInspector::UpdateMaterialCombo()
{
	for (int i = 0; i < Materials.Num(); i++)
	{
		MaterialNameStr = Materials[i]->GetMaterialName().ToString();
		const char* MaterialName = MaterialNameStr.c_char();
		if (ImGui::BeginCombo(("Material" + std::to_string(i)).c_str(), MaterialName))
		{
			for (int j = 0; j < AllMaterials.Num(); j++)
			{
				bool IsSelected = Materials[i] == AllMaterials[j];
				MaterialNameStr = AllMaterials[j]->GetMaterialName().ToString();
				if (ImGui::Selectable(MaterialNameStr.c_char(), IsSelected))
				{
					// 여기서 머티리얼을 교체함
					Component->SetMaterial(i, AllMaterials[j]);
					// 머티리얼 배열 Refresh
					Materials = Component->GetMaterials();
				}
				if (IsSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}
}

