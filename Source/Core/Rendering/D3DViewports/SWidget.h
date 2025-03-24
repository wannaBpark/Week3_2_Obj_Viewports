#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include "memory"

class SWidget : public std::enable_shared_from_this<SWidget>
{
	friend class SWindow;
public:
	enum class EState
	{
		Disable,
		Active,
		Destroy,
	};

	SWidget();
	virtual ~SWidget();

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void OnGUI() = 0;
	virtual void Run() = 0;
	virtual void OnEnable() = 0;
	virtual void OnDisable() = 0;
	virtual void OnDestroy() = 0;

private:
	ImGuiWindowFlags Flags;
	EState State;
	ImVec2 Size;
};

