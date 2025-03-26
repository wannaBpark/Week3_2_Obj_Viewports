#pragma once
#include "Core/Rendering/D3DViewports/SWindow.h"

class SSplitter : public SWindow
{
    using Super = SWindow;
protected:
    std::shared_ptr<SWindow> SideLT; // Left 또는 Top
    std::shared_ptr<SWindow> SideRB; // Right 또는 Bottom
    float SplitRatio = 0.5f;
  
public:
    explicit SSplitter(std::shared_ptr<SWindow> LT, std::shared_ptr<SWindow> RB) : SideLT(LT), SideRB(RB) { }
public:
    float GetSplitRatio() const { return SplitRatio; }
    std::shared_ptr<SWindow> GetSideLT() { return SideLT; }
    std::shared_ptr<SWindow> GetSideRB() { return SideRB; }

    virtual void OnDrag(float Delta)
    {
        SplitRatio += Delta;
        SplitRatio = std::clamp(SplitRatio, 0.1f, 0.9f);
        UpdateLayout();
        //UE_LOG("SSplitterH: Dragging, SplitRatio = %.2f", SplitRatio);
    }

    virtual void Render() override // 자신과 자신의 자식(Swindow or Splitter)을 렌더
    {
        Super::Render();
        if (SideLT) SideLT->Render();
        if (SideRB) SideRB->Render();
    }

    // Layout 갱신 후, 자식들이 render 가능한 상태로 전환되어야 Render()에서 렌더됨
    virtual void UpdateLayout()
    {
        if (SideLT && SideRB)
        {
            SideLT->SetNeedsRender();
            SideRB->SetNeedsRender();
        }
    }

    // 마우스 움직임, 키 입력이 자식들에게 전달되며 알맞은 행동을 취합니다
    virtual void OnMouseMove(const FPoint& MousePos) override
    {
        if (SideLT) SideLT->OnMouseMove(MousePos);
        if (SideRB) SideRB->OnMouseMove(MousePos);
    }

    virtual void OnMouseUp(const FPoint& MousePos) override
    {
        bIsDragging = false;
        ::SetCursor(LoadCursor(nullptr, IDC_ARROW));
        if (SideLT) SideLT->OnMouseUp(MousePos);
        if (SideRB) SideRB->OnMouseUp(MousePos);
        //UE_LOG("SSplitterH: Mouse Up, stop dragging");
    }

    virtual void OnMouseDown(const FPoint& MousePos) override
    {
        if (SideLT) SideLT->OnMouseDown(MousePos);
        if (SideRB) SideRB->OnMouseDown(MousePos);
    }

    virtual void OnKeyDown(EKeyCode Key) override 
    {
        Super::OnKeyDown(Key);
        if (SideLT) SideLT->OnKeyDown(Key);
        if (SideRB) SideRB->OnKeyDown(Key);
    }   
};


class SSplitterV final : public SSplitter
{
    using Super = SSplitter;
private:
    const uint32 BorderThickness = 8;
    FPoint LastMousePos;
    bool bVerticalDragging = false; // vertical 축 드래그 플래그 변수 (추가)
public:
    explicit SSplitterV(std::shared_ptr<SWindow> Left, std::shared_ptr<SWindow> Right) : SSplitter(Left, Right) {}
    void SetSplitRatio(float ratio) { SplitRatio = ratio; }

    bool IsOverBorder(const FPoint& MousePos) const
    {
        uint32 leftWidth = static_cast<uint32>(Rect.W * SplitRatio);
        FRect BorderRect = {
            .X = Rect.X + leftWidth - BorderThickness / 2,
            .Y = Rect.Y,
            .W = BorderThickness,
            .H = Rect.H,
        };
        return IsInRect(MousePos, BorderRect);
    }

    virtual void OnMouseDown(const FPoint& MousePos) override
    {
        Super::OnMouseDown(MousePos);
        // 경계 영역에서만 드래그 시작하도록
        if (IsOverBorder(MousePos))
        {
            bIsDragging = true;
            LastMousePos = MousePos;
        }
    }

    virtual void OnMouseMove(const FPoint& MousePos) override
    {
        Super::OnMouseMove(MousePos);

        // 경계 영역에 걸침 -> 드래그 아이콘으로 변경
        if (!bIsDragging && IsOverBorder(MousePos)) { ::SetCursor(LoadCursor(nullptr, IDC_SIZEWE)); }

        if (bIsDragging)
        {
            int deltaX = static_cast<int>(MousePos.X) - static_cast<int>(LastMousePos.X);
            float normalizedDelta = static_cast<float>(deltaX) / static_cast<float>(Rect.W);
            OnDrag(normalizedDelta);
            LastMousePos = MousePos;
        }
    }

    void UpdateLayout() override
    {
        Super::UpdateLayout();

        // 전체 width에서 border 영역을 빼면 사용 가능한 너비
        uint32 availableWidth = Rect.W - BorderThickness;
        uint32 leftWidth = static_cast<uint32>(availableWidth * SplitRatio);

        // 왼쪽 창의 rect (border 제외)
        FRect leftRect = Rect;
        leftRect.W = leftWidth;
        SideLT->SetRect(leftRect);

        // 오른쪽 창의 rect: border를 포함하여 X 위치를 조정
        FRect rightRect = Rect;
        rightRect.X = Rect.X + leftWidth + BorderThickness;
        rightRect.W = Rect.W - leftWidth - BorderThickness;
        SideRB->SetRect(rightRect);
    }
};

class SSplitterH final : public SSplitter {
    using Super = SSplitter;
private:
    const uint32 BorderThickness = 8;
    bool bIsCenterDrag = false;
    bool bIsVerticalDrag = false;
    FPoint LastMousePos;
    std::shared_ptr<SSplitterV> TopSplitterV;
    std::shared_ptr<SSplitterV> BottomSplitterV;

public:
    SSplitterH(std::shared_ptr<SSplitterV> Top, std::shared_ptr<SSplitterV> Bottom)
        : SSplitter(Top, Bottom), TopSplitterV(Top), BottomSplitterV(Bottom) {
    }

    // 마우스 좌표가 수평 경계 영역에 있는지 검사
    bool IsOverBorder(const FPoint& MousePos) const
    {
        uint32 topHeight = static_cast<uint32>(Rect.H * SplitRatio);
        FRect borderRect = {
            Rect.X,
            Rect.Y + topHeight - BorderThickness / 2,
            Rect.W,
            BorderThickness
        };
        return IsInRect(MousePos, borderRect);
    }

    // 중앙 영역 (vertical border와 horizontal border의 교차 영역) 검사
    bool IsOverCenter(const FPoint& MousePos) const
    {
        // vertical border (위쪽 splitter 기준; top과 bottom은 동일한 분할을 가짐)
        uint32 AvailableWidth = Rect.W - BorderThickness;
        uint32 leftWidth = static_cast<uint32>(AvailableWidth * TopSplitterV->GetSplitRatio());

        // horizontal border (자신의 border)
        uint32 AvailableHeight = Rect.H - BorderThickness;
        uint32 topHeight = static_cast<uint32>(AvailableHeight * SplitRatio);

        FRect CenterRect = {
            Rect.X + leftWidth,
            Rect.Y + topHeight,
            BorderThickness,
            BorderThickness
        };
        return IsInRect(MousePos, CenterRect);
    }

    bool IsOverVerticalBorder(const FPoint& MousePos) const
    {
        // top splitter의 vertical border 영역을 기준으로 계산 (bottom도 동일하다고 가정)
        FRect topRect = TopSplitterV->GetRect();
        uint32 leftWidth = static_cast<uint32>(topRect.W * TopSplitterV->GetSplitRatio());
        FRect BorderRect = {
            topRect.X + leftWidth - BorderThickness / 2,
            topRect.Y,
            BorderThickness,
            this->Rect.H                // [수정] : 아랫 수직축도 드래그 가능하도록 함
        };  
        return IsInRect(MousePos, BorderRect);
    }

    virtual void OnMouseDown(const FPoint& MousePos) override
    {
        if (TopSplitterV)    { TopSplitterV->OnMouseDown(MousePos); }
        if (BottomSplitterV) { BottomSplitterV->OnMouseDown(MousePos); }

        if (IsOverCenter(MousePos))
        {
            bIsCenterDrag = bIsDragging = true;
            ::SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
        }
        else if (IsOverVerticalBorder(MousePos))
        {
            bIsVerticalDrag = bIsDragging = true;
            ::SetCursor(LoadCursor(nullptr, IDC_SIZEWE));  // 좌우 아이콘
        }
        else if (IsOverBorder(MousePos))
        {
            bIsDragging = true;
            ::SetCursor(LoadCursor(nullptr, IDC_SIZENS));
        }
        LastMousePos = MousePos;

    }


    virtual void OnMouseMove(const FPoint& MousePos) override
    {
        Super::OnMouseMove(MousePos);

        if (!bIsDragging) // 중앙, 수직, 단일 수평 축에 대한 아이콘 변경 (hover시)
        {
            if (IsOverCenter(MousePos))      { ::SetCursor(LoadCursor(nullptr, IDC_SIZEALL)); }
            else if (IsOverVerticalBorder(MousePos)) { ::SetCursor(LoadCursor(nullptr, IDC_SIZEWE)); }
            else if (IsOverBorder(MousePos)) { ::SetCursor(LoadCursor(nullptr, IDC_SIZENS)); }
        }

        if (bIsDragging)
        {
            int deltaY = static_cast<int>(MousePos.Y) - static_cast<int>(LastMousePos.Y);
            float normalizedDeltaY = static_cast<float>(deltaY) / static_cast<float>(Rect.H - BorderThickness);
            int deltaX = static_cast<int>(MousePos.X) - static_cast<int>(LastMousePos.X);
            float normalizedDeltaX = static_cast<float>(deltaX) / static_cast<float>(Rect.W - BorderThickness);

            if (bIsVerticalDrag) // Vertical 축 드래그 : 좌우 delta만 적용
            {
                TopSplitterV->OnDrag(normalizedDeltaX);
                BottomSplitterV->OnDrag(normalizedDeltaX);
            }
            else if (bIsCenterDrag) // 중앙 드래그 시 상하좌우 조절
            {
                // 양쪽 vertical splitter의 SplitRatio를 동시에 조정
                TopSplitterV->OnDrag(normalizedDeltaX);
                BottomSplitterV->OnDrag(normalizedDeltaX);
                OnDrag(normalizedDeltaY); // horizontal 드래그
            }
            // 단순 horizontal 경계 드래그 (상하)
            else { OnDrag(normalizedDeltaY);  }
            LastMousePos = MousePos;
        }
    }

    void OnMouseUp(const FPoint& MousePos) override
    {
        bIsCenterDrag = bIsVerticalDrag = false;
        Super::OnMouseUp(MousePos);
    }

    void UpdateLayout() override {
        Super::UpdateLayout();

        uint32 availableHeight = Rect.H - BorderThickness;
        uint32 topHeight = static_cast<uint32>(availableHeight * SplitRatio);

        FRect topRect = Rect;
        topRect.H = topHeight;
        TopSplitterV->SetRect(topRect);

        FRect bottomRect = Rect;
        bottomRect.Y = Rect.Y + topHeight + BorderThickness;
        bottomRect.H = Rect.H - topHeight - BorderThickness;
        BottomSplitterV->SetRect(bottomRect);

        TopSplitterV->UpdateLayout();
        BottomSplitterV->UpdateLayout();
    }

    // 키 이벤트 전파: 자식 SSplitterV에게 전달
    virtual void OnKeyDown(EKeyCode Key) override 
    {
        if (TopSplitterV) TopSplitterV->OnKeyDown(Key);
        if (BottomSplitterV) BottomSplitterV->OnKeyDown(Key);
    }
};
