#pragma once
#include "Core/Rendering/D3DViewports/SWidget.h"
#include "Core/Math/Vector.h"
#include "Debug/DebugConsole.h"
#include <d3d11.h>
#include <iostream>
#include <algorithm>

struct FPoint
{
	uint32 X, Y;
	explicit FPoint(uint32 InX = 0, uint32 InY = 0) : X(InX), Y(InY) { }
};

struct FRect
{
	uint32 X;
	uint32 Y;
	uint32 W;
	uint32 H;
};

// 사각형 내부 여부 검사
static bool IsInRect(const FPoint& pt, const FRect& rect)
{
	return (pt.X >= rect.X && pt.X <= rect.X + rect.W &&
		pt.Y >= rect.Y && pt.Y <= rect.Y + rect.H);
}


class SWindow /*: public SWidget*/
{
protected:
	FRect Rect;
	bool bIsDragging = false;
	bool bRenderable = true;
public:
	SWindow() = default;
	virtual ~SWindow() = default;

	void SetNeedsRender() { bRenderable = true; }
	void SetRect(const FRect& InRect) { Rect = InRect; SetNeedsRender(); }
	FRect GetRect() const { return Rect; }

	bool IsHover(FPoint coord) const
	{
		bool ret = (coord.X >= Rect.X && coord.X <= Rect.X + Rect.W
			&& coord.Y >= Rect.Y && coord.Y <= Rect.Y + Rect.H);
		return ret;
	}

	virtual void Render()
	{
		if (bRenderable) 
		{
			UE_LOG("Base Window Render");
			bRenderable = false;
		}
	}

	// 기본 마우스 이벤트
	virtual void OnMouseMove(const FPoint& MousePos)
	{
		if (IsHover(MousePos)) { UE_LOG("SWindow: Mouse Hover"); }
	}
	virtual void OnMouseDown(const FPoint& MousePos) {
		if (IsHover(MousePos)) { bIsDragging = true; }
	}
	virtual void OnMouseUp(const FPoint& MousePos) { bIsDragging = false; }
};

