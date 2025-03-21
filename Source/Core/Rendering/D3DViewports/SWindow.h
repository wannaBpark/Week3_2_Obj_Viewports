#pragma once
#include "Core/Rendering/D3DViewports/SWidget.h"
#include "Core/Math/Vector.h"
#include <iostream>
#include <algorithm>

struct FRect
{
	uint32 H;
	uint32 W;
	uint32 X;
	uint32 Y;
};
struct FPoint
{
	uint32 ID;
	FVector Position;
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

