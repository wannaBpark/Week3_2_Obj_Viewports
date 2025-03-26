#pragma once
#include "iostream"
#include "Object/Material/Material.h"

class MaterialEditor
{
public:
	MaterialEditor() = default;
	~MaterialEditor() = default;

public:
	void Open(UMaterial* InitData);
	void Close();

	void Update();
	bool IsOpen() { return isOpen; }

	void Init(UMaterial* InitData);
	void Reset();
	void Apply();

private:
	UMaterial* TargetMaterial;
	UMaterial ResetData;

	bool isOpen = false;
};

