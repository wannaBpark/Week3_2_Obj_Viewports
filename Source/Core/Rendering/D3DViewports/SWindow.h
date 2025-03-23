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

class SWindow : public SWidget
{
protected:
	FRect Rect;
	bool bIsDragging = false;
	bool bRenderable = true;
public:
	virtual ~SWindow() = default;
	virtual void Render() 
	{
		if (bRenderable) {
			UE_LOG("Base Window Render");
			bRenderable = false;
		}
	}

	bool IsHover(FPoint coord) const
	{
		bool ret = (coord.X >= Rect.X && coord.X <= Rect.X + Rect.W
			&& coord.Y >= Rect.Y && coord.Y <= Rect.Y + Rect.H);
		return ret;
	}
	void SetNeedsRender() { bRenderable = true; }
	void SetRect(const FRect& InRect) { Rect = InRect; SetNeedsRender(); }
	FRect GetRect() const { return Rect; }
};

