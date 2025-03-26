#include "Spliter.h"
#include "Reply.h"

void Spliter::Tick(const FGeometry& ParentGeometry, float DeltaTime)
{
    SPanel::Tick(ParentGeometry, DeltaTime);
}

void Spliter::AddChild(const std::shared_ptr<SWidget>& Child, const float SizeRatio)
{
    SWidget::AddChild(Child);
    // SizeRatio는 0 이상의 값이어야 합니다.
    ChildInfos.push_back({ Child, SizeRatio });
}

void Spliter::OnArrangeChildren(const FGeometry& AllottedGeometry, TArray<FArrangedWidget>& ArrangedChildren) const
{
    // 부모 위젯으로부터 할당받은 전체 영역의 크기를 가져옵니다.
    FVector2D AllottedSize = AllottedGeometry.GetLocalSize();
    if (ChildInfos.empty())
    {
        return;
    }

    // 내부 ChildInfos 벡터에 저장된 모든 자식의 비율 합을 계산합니다.
    const float TotalRatio = GetTotalRatio();
    if (TotalRatio <= 0.0f)
    {
        return;
    }

    // 분할 방향에 따라 자식들의 배치 로직을 분기 처리합니다.
    if (Orientation == ESplitterOrientation::Horizontal)
    {
        // 수평 분할: 전체 너비를 자식들의 비율에 따라 나누어 배치합니다.
        float CurrentX = 0.0f;
        for (const auto& Info : ChildInfos)
        {
            float ChildWidth = AllottedSize.X * (Info.Ratio / TotalRatio);
            FVector2D ChildSize(ChildWidth, AllottedSize.Y);
            FVector2D ChildPosition(CurrentX, 0.0f);
            FSlateLayoutTransform ChildLayoutTransform(1.0f, ChildPosition);
            FGeometry ChildGeometry = AllottedGeometry.MakeChild(ChildSize, ChildLayoutTransform, FSlateRenderTransform(), FVector2D(0.5f, 0.5f));
            Info.Widget->SetLocalPosition(ChildPosition);
            Info.Widget->SetLocalSize(ChildSize);
            ArrangedChildren.Add(FArrangedWidget(Info.Widget, ChildGeometry));
            CurrentX += ChildWidth;
        }
    }
    else // Orientation == ESplitterOrientation::Vertical
    {
        // 수직 분할: 전체 높이를 자식들의 비율에 따라 나누어 배치합니다.
        float CurrentY = 0.0f;
        for (const auto& Info : ChildInfos)
        {
            float ChildHeight = AllottedSize.Y * (Info.Ratio / TotalRatio);
            FVector2D ChildSize(AllottedSize.X, ChildHeight);
            FVector2D ChildPosition(0.0f, CurrentY);
            FSlateLayoutTransform ChildLayoutTransform(1.0f, ChildPosition);
            FGeometry ChildGeometry = AllottedGeometry.MakeChild(ChildSize, ChildLayoutTransform, FSlateRenderTransform(), FVector2D(0.5f, 0.5f));
            Info.Widget->SetLocalPosition(ChildPosition);
            Info.Widget->SetLocalSize(ChildSize);
            ArrangedChildren.Add(FArrangedWidget(Info.Widget, ChildGeometry));
            CurrentY += ChildHeight;
        }
    }
}

FReply Spliter::OnMouseButtonDown(EMouseButton InMouseButton, const FPointer& InPointer)
{
    if (IsHovered() == false) return FReply();
    
    if (InMouseButton == EMouseButton::Left)
    {
        FVector2D localPos = MyGeometry.AbsoluteToLocal(InPointer.ScreenSpacePosition);
        const float DragThreshold = 5.0f; // 경계 감지 임계값 (픽셀)
        float TotalRatio = GetTotalRatio();
        if (Orientation == ESplitterOrientation::Horizontal)
        {
            float cumulative = 0.0f;
            // 경계는 ChildInfos[0]~ChildInfos[n-2]의 오른쪽 경계
            for (int i = 0; i < static_cast<int>(ChildInfos.size()) - 1; ++i)
            {
                float ChildWidth = MyGeometry.GetLocalSize().X * (ChildInfos[i].Ratio / TotalRatio);
                cumulative += ChildWidth;
                if (std::fabs(localPos.X - cumulative) < DragThreshold)
                {
                    bIsDragging = true;
                    DraggedHandleIndex = i;
                    InitialDragPosition = localPos.X;
                    InitialLeftRatio = ChildInfos[i].Ratio;
                    InitialRightRatio = ChildInfos[i + 1].Ratio;
                    bIsMouseCaptured = true;
                    return FReply().CaptureMouse(shared_from_this());
                }
            }
        }
        else // Vertical
        {
            float cumulative = 0.0f;
            for (int i = 0; i < static_cast<int>(ChildInfos.size()) - 1; ++i)
            {
                float ChildHeight = MyGeometry.GetLocalSize().Y * (ChildInfos[i].Ratio / TotalRatio);
                cumulative += ChildHeight;
                if (std::fabs(localPos.Y - cumulative) < DragThreshold)
                {
                    bIsDragging = true;
                    DraggedHandleIndex = i;
                    InitialDragPosition = localPos.Y;
                    InitialLeftRatio = ChildInfos[i].Ratio;
                    InitialRightRatio = ChildInfos[i + 1].Ratio;
                    return FReply().CaptureMouse(shared_from_this());
                }
            }
        }
    }
    else if (InMouseButton == EMouseButton::Right)
    {
        return Super::OnMouseButtonDown(InMouseButton, InPointer);
    }
    
    return FReply();
}

FReply Spliter::OnMouseMove(const FPointer& Pointer)
{
    Super::OnMouseMove(Pointer);

    if (IsHovered() == false) return FReply();
    
    if (bIsDragging)
    {
        const FVector2D localPos = MyGeometry.AbsoluteToLocal(Pointer.ScreenSpacePosition);
        const float TotalSize = (Orientation == ESplitterOrientation::Horizontal) ? MyGeometry.GetLocalSize().X : MyGeometry.GetLocalSize().Y;
        const float delta = (Orientation == ESplitterOrientation::Horizontal) ? (localPos.X - InitialDragPosition) : (localPos.Y - InitialDragPosition);
        const float totalAdjacent = InitialLeftRatio + InitialRightRatio;
        // 비율 변경량은 전체 크기에 대한 delta 비례로 조정
        float newLeftRatio = InitialLeftRatio + (delta / TotalSize) * totalAdjacent;
        const float MinRatio = 0.05f; // 최소 비율 제한
        if (newLeftRatio < MinRatio)
        {
            newLeftRatio = MinRatio;
        }
        if (newLeftRatio > totalAdjacent - MinRatio)
        {
            newLeftRatio = totalAdjacent - MinRatio;
        }
        float newRightRatio = totalAdjacent - newLeftRatio;
        // 드래그 중인 경계 양쪽 자식의 비율을 업데이트
        ChildInfos[DraggedHandleIndex].Ratio = newLeftRatio;
        ChildInfos[DraggedHandleIndex + 1].Ratio = newRightRatio;

        TArray<FArrangedWidget> arrangedWidget;
        for (int i = 0; i < ChildInfos.size(); ++i)
        {
            std::shared_ptr<SWidget> childWidget = ChildInfos[i].Widget;
            arrangedWidget.Add({childWidget, childWidget->GetGeometry()});
        }
        
        OnArrangeChildren(MyGeometry, arrangedWidget);
        return FReply();
    }
    return FReply();
}

FReply Spliter::OnMouseButtonUp(EMouseButton InMouseButton, const FPointer& InPointer)
{
    if (IsHovered() == false) return FReply();
    
    if (bIsDragging && InMouseButton == EMouseButton::Left)
    {
        bIsDragging = false;
        DraggedHandleIndex = -1;
        bIsMouseCaptured = false;
        return FReply().ReleaseMouseCapture();
    }
    else if (InMouseButton == EMouseButton::Right)
    {
        return Super::OnMouseButtonUp(InMouseButton, InPointer);
    }
    
    return FReply();
}

float Spliter::GetTotalRatio() const
{
    float Total = 0.0f;
    for (const auto& Info : ChildInfos)
    {
        Total += Info.Ratio;
    }
    return Total;
}
