#pragma once
#include "ArrangedWidget.h"
#include "Core/Rendering/Widget/SPanel.h"

namespace ESplitterResizeMode
{
	enum Type
	{
		/** Resize the selected slot. If space is needed, then resize the next resizable slot. */
		FixedPosition,
		/** Resize the selected slot. If space is needed, then resize the last resizable slot. */
		FixedSize,
		/** Resize the selected slot by redistributing the available space with the following resizable slots. */
		Fill,
	};
}

/** 분할 방향을 나타내는 열거형 */
enum class ESplitterOrientation
{
	Horizontal, // 수평 분할: 자식 위젯들을 왼쪽에서 오른쪽으로 배열
	Vertical    // 수직 분할: 자식 위젯들을 위에서 아래로 배열
};

class FLayoutGeometry;

class Spliter : public SPanel
{
	using Super = SPanel;	
public:
	void Tick(const FGeometry& ParentGeometry, float DeltaTime) override;
	Spliter(ESplitterOrientation InOrientation)	: Orientation(InOrientation) {}
	virtual ~Spliter();

	/**
	* 자식 위젯과 그에 해당하는 크기 비율을 추가합니다.
	*
	* @param Child         추가할 자식 위젯.
	* @param SizeRatio     자식에게 할당할 너비의 비율 (예: 0.5는 전체 너비의 50%).
	*/
	void AddChild(const std::shared_ptr<SWidget>& Child, float SizeRatio = 0.5f);

	/**
	 * 부모로부터 할당받은 영역(AllottedGeometry)을 기반으로, 각 자식 위젯의 Geometry를 계산하여 ArrangedChildren에 추가합니다.
	 *
	 * @param AllottedGeometry    부모가 할당한 전체 영역 정보.
	 * @param ArrangedChildren    계산된 자식 위젯의 배치 정보를 담을 배열.
	 */
	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, TArray<FArrangedWidget>& ArrangedChildren) const override;

	// 입력 이벤트 오버라이드 (드래그 처리를 위해)
	virtual FReply OnMouseButtonDown(EMouseButton InMouseButton, const FPointer& InPointer) override;
	virtual FReply OnMouseMove(const FPointer& Pointer) override;
	virtual FReply OnMouseButtonUp(EMouseButton InMouseButton, const FPointer& InPointer) override;

private:
	// 각 자식 위젯과 해당 위젯에 할당할 너비 비율을 저장합니다.
	struct FChildInfo
	{
	public:
		std::shared_ptr<SWidget> Widget;
		float Ratio;
	};

	std::vector<FChildInfo> ChildInfos;

	/** 분할 방향 (수평 또는 수직) */
	ESplitterOrientation Orientation = ESplitterOrientation::Horizontal;

	bool bIsDragging = false;
	int DraggedHandleIndex = -1;    // 드래그 중인 경계 인덱스 (ChildInfos[DraggedHandleIndex]와 [DraggedHandleIndex+1] 사이)
	float InitialDragPosition = 0.0f; // 드래그 시작 시의 마우스 좌표 (X 또는 Y)
	float InitialLeftRatio = 0.0f;    // 드래그 시작 시 왼쪽(또는 위쪽) 자식의 비율
	float InitialRightRatio = 0.0f;   // 드래그 시작 시 오른쪽(또는 아래쪽) 자식의 비율
	
	// 헬퍼: 전체 비율 합 계산
	float GetTotalRatio() const;
};

