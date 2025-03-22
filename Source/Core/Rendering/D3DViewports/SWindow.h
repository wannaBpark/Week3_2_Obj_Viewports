#pragma once
#include "Core/Rendering/D3DViewports/SWidget.h"
#include "Core/Math/Vector.h"
#include <iostream>
#include <algorithm>

struct FPoint
{
	uint32 X, Y;
	FPoint(uint32 InX = 0, uint32 InY = 0) : X(InX), Y(InY) { }
};
struct FRect
{
	uint32 X, Y, Width, Height;
};
class SWindow : public SWidget
{
protected:
	FRect Rect;

	// TODO : 해당 WIndow에 마우스를 올려놓고 있는지 판단 가능해야 함
	bool IsHover(FPoint coord) const
	{
		return true;
	}
	
	virtual void Render() = 0; // 스플리터 창을 렌더합니다
};

