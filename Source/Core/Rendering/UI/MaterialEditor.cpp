#include "MaterialEditor.h"
#include "Object/Mesh/ObjManager.h"

#include <directxtk/DDSTextureLoader.h> // Create DDS Texture Loader
#include <directxtk/WICTextureLoader.h>

void MaterialEditor::Open(UMaterial* InitData)
{
	Init(InitData);
	isOpen = true;
}

void MaterialEditor::Close()
{
	Reset();
	isOpen = false;
}

void MaterialEditor::Update()
{
	if (!isOpen || TargetMaterial == nullptr) 
		return;

	ImGui::Begin("Material Editor", &isOpen);

	ImGui::Text("Material %s", TargetMaterial->MaterialName.ToString().c_char());
	
	// Ambient, Diffuse, Specular, Emissive
	ImGui::ColorEdit4("Ambient", (float*)&TargetMaterial->Ambient);
	ImGui::ColorEdit4("Diffuse", (float*)&TargetMaterial->Diffuse);
	ImGui::ColorEdit4("Specular", (float*)&TargetMaterial->Specular);
	ImGui::ColorEdit3("Emissive", (float*)&TargetMaterial->Emissive);
	ImGui::Separator();

	// Rouchness
	ImGui::SliderFloat("Roughness", &TargetMaterial->Shininess, 0.0f, 250);

	ImGui::Separator();

	// Texture
	static int CurrentTextureIndex = TargetMaterial->TextureMapIndex;
	const char* PreviewLabel = TargetMaterial->TextureName.ToString().c_char();

	//if (ImGui::BeginCombo("Texture", PreviewLabel))
	//{
	//	TArray<FName> TextureNames = FObjManager::GetTextureList();
	// 
	//	for (int i = 0; i < TextureNames.Num(); ++i)
	//	{
	//		bool selected = (CurrentTextureIndex == i);
	//		const char* TexName = TextureNames[i].ToString().c_str();

	//		if (ImGui::Selectable(TexName, selected))
	//		{
	//			CurrentTextureIndex = i;
	//			TargetMaterial->TextureName = TextureNames[i];
	//			TargetMaterial->TextureMapIndex = i;
	//		}
	//		if (selected)
	//			ImGui::SetItemDefaultFocus();
	//	}
	// 
	//	ImGui::EndCombo();
	//}	

	ImGui::Text("Texture %s", TargetMaterial->TextureName.ToString().c_char());


	//ID3D11ShaderResourceView* SRV;
	//ID3D11Texture2D Texture;

	//auto Device = UEngine::Get().GetRenderer()->GetDevice();
	//auto DeviceContext = UEngine::Get().GetRenderer()->GetDeviceContext();
	//auto string = TargetMaterial->TextureName
	//
	//auto hr = DirectX::CreateWICTextureFromFile(Device, DeviceContext, filename, (ID3D11Resource**)Texture.GetAddressOf(), SRV.GetAddressOf());

	//ImGui::Image((ImTextureID)SRV, ImVec2(64, 64));

	ImGui::Separator();
	if (ImGui::Button("Apply"))
		Apply();
	ImGui::SameLine();
	if (ImGui::Button("Reset"))
		Reset();

	ImGui::End();

	if (!isOpen)
		Close();
}

void MaterialEditor::Init(UMaterial* InitData)
{
	TargetMaterial = InitData;
	ResetData = *InitData;
}

void MaterialEditor::Apply()
{
	if (TargetMaterial == nullptr)
		return;

	ResetData = *TargetMaterial;
}

void MaterialEditor::Reset()
{
	if (TargetMaterial == nullptr)
		return;

	*TargetMaterial = ResetData;
}
