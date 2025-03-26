#include "StaticMeshInspector.h"
#include "ImGui/imgui.h"
#include "Object/StaticMeshComponent/StaticMeshComponent.h"
#include "Object/UObjectIterator.h"
#include "Object/Material/Material.h"
#include <Object/Mesh/ObjManager.h>

void FStaticMeshInspector::Init(UStaticMeshComponent* Comp)
{
	if (Comp == nullptr || Comp->StaticMesh == nullptr)
		return;

	Component = Comp;

	StaticMeshes.Empty();
	StaticMeshIndex = 0;

	int Count = 0;

	TMap<FString, FStaticMesh*> StaticMeshMap = FObjManager::GetObjStaticMeshMap();
	for (auto& kvp : StaticMeshMap)
	{
		FStaticMesh* StaticMesh = kvp.second;
		StaticMeshes.Add(StaticMesh);
		if (StaticMesh->Name == Comp->StaticMesh->GetAssetPathFileName())
		{
			StaticMeshIndex = Count;
		}
		Count++;
	}

	ComponentMaterials.Empty();
	AllMaterials.Empty();

	ComponentMaterials = Comp->GetMaterials();
	// 모든 머티리얼들 가져오기
	TMap<FName, FObjMaterialInfo> GlobalMaterialMap = FObjManager::GetMaterialMap();
	for (auto& kvp : GlobalMaterialMap)
	{
		AllMaterials.Add(kvp.second);
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

void FStaticMeshInspector::OnNewStaticMeshImported(FStaticMesh* NewMesh)
{
	if (NewMesh == nullptr)
		return;
	if(StaticMeshes.Find(NewMesh) == -1)
		StaticMeshes.Add(NewMesh);
}

void FStaticMeshInspector::UpdateStaticMeshCombo()
{
	if (StaticMeshes.Num() == 0)
		return;
	if (ImGui::BeginCombo("Static Mesh", StaticMeshes[StaticMeshIndex]->Name.c_char()))
	{
		for (int i = 0; i < StaticMeshes.Num(); i++)
		{
			bool IsSelected = StaticMeshIndex == i;
			if (ImGui::Selectable(StaticMeshes[i]->Name.c_char(), IsSelected))
			{
				StaticMeshIndex = i;
				Component->SetStaticMesh(StaticMeshes[i]->Name);
				// 메시바뀌면 머티리얼 갱신해줘야 함
				ComponentMaterials = Component->GetMaterials();
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
	for (int i = 0; i < ComponentMaterials.Num(); i++)
	{
		MaterialNameStr = ComponentMaterials[i]->GetMaterialName().ToString();
		const char* MaterialName = MaterialNameStr.c_char();
		if (ImGui::BeginCombo(("Material" + std::to_string(i)).c_str(), MaterialName))
		{
			for(const auto& GlobalMat : AllMaterials)
			{
				bool IsSelected = ComponentMaterials[i]->MaterialName == GlobalMat.MaterialName;
				MaterialNameStr = GlobalMat.MaterialName.ToString();
				if (ImGui::Selectable(MaterialNameStr.c_char(), IsSelected))
				{
					// 여기서 머티리얼을 교체함
					UMaterial* MatInstance = FObjManager::LoadMaterial(GlobalMat.MaterialName);
					Component->SetMaterial(i, MatInstance);
					// 머티리얼 배열 Refresh
					ComponentMaterials = Component->GetMaterials();
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

