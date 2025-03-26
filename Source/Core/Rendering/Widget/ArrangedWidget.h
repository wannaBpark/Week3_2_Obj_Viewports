// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "memory"
#include "Core/Math/Geometry.h"

class SWidget;

/**
 * 한 쌍: 위젯과 그에 해당하는 Geometry.
 * 위젯은 자식 위젯을 배치할 때, WidgetGeometries 리스트를 채웁니다.
 * 자세한 내용은 SWidget::ArrangeChildren을 참조하십시오.
 */
class FArrangedWidget
{
public:

	FArrangedWidget(std::shared_ptr<SWidget> InWidget, const FGeometry& InGeometry)
		: Geometry(InGeometry)
		, Widget(InWidget)
	{
	}

	static const FArrangedWidget& GetNullWidget();

public:

	/** 배치되는 위젯입니다. */
	SWidget* GetWidgetPtr() const
	{
		return Widget.get();
	}

	///**
	// * 위젯과 해당 Geometry의 문자열 표현을 가져옵니다.
	// *
	// * @return 문자열 표현.
	// */
	//FString ToString() const;

public:

	/**
	 * 이 위젯 배치를 다른 배치와 비교하여 동일한지 여부를 반환합니다.
	 *
	 * @param Other 비교할 다른 배치.
	 * @return 두 배치가 동일하면 true, 그렇지 않으면 false.
	 */
	bool operator==(const FArrangedWidget& Other) const
	{
		return Widget == Other.Widget;
	}

public:

	/** 위젯의 Geometry입니다. */
	FGeometry Geometry;

	/** 배치되는 위젯입니다. */
	std::shared_ptr<SWidget> Widget;
};
