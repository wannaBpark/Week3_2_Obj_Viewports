#pragma once
#include "SWidget.h"

struct FArrangedWidget;

class SPanel : public SWidget
{
    using Super = SWidget;
public:
    SPanel();
    virtual ~SPanel();
    void Tick(const FGeometry& ParentGeometry, float DeltaTime) override;
    /**
    * 자식 위젯들의 Geometry를 계산하여 ArrangedChildren 리스트에 추가합니다.
    *  이 예제에서는 부모 위젯에게 할당된 영역을 자식 수만큼 균등하게 분할하여,
    * 각 자식을 수평으로 배열합니다.
    *
    * @param AllottedGeometry    부모로부터 할당받은 전체 영역 정보.
    * @param ArrangedChildren    계산된 자식 위젯들의 배치 정보를 담을 배열.
    */
    virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, TArray<FArrangedWidget>& ArrangedChildren) const override = 0;

private:
    // 자식 위젯들을 저장하는 벡터
    std::vector<std::shared_ptr<SWidget>> Children;
};

