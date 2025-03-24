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
    SSplitter(std::shared_ptr<SWindow> LT, std::shared_ptr<SWindow> RB) : SideLT(LT), SideRB(RB) { }

    virtual void OnDrag(float Delta) = 0; // [순수 가상 함수 오버라이딩 필요] : 마우스 드래그에 따른 창 크기를 조절할 함수입니다

    virtual void Render() override
    {
        Super::Render();
        if (SideLT) SideLT->Render();
        if (SideRB) SideRB->Render();
    }

    virtual void UpdateLayout()
    {
        if (SideLT && SideRB)
        {
            SideLT->SetNeedsRender();
            SideRB->SetNeedsRender();
        }
    }

    float GetSplitRatio() const { return SplitRatio; }
    std::shared_ptr<SWindow> GetSideLT() { return SideLT; }
    std::shared_ptr<SWindow> GetSideRB() { return SideRB; }
};


class SSplitterV final : public SSplitter
{
    using Super = SSplitter;
private:
    const uint32 BorderThickness = 4;
    FPoint LastMousePos;
public:
    explicit SSplitterV(std::shared_ptr<SWindow> Left, std::shared_ptr<SWindow> Right) : SSplitter(Left, Right) {}

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
        // 경계 영역에서만 드래그 시작하도록
        if (IsOverBorder(MousePos))
        {
            bIsDragging = true;
            LastMousePos = MousePos;
            UE_LOG("SSplitterV: Border Mouse Down, start dragging");
        }
    }

    virtual void OnMouseMove(const FPoint& MousePos) override
    {
        if (bIsDragging)
        {
            // X축 이동량 기준으로 delta 계산
            int deltaX = static_cast<int>(MousePos.X) - static_cast<int>(LastMousePos.X);
            float normalizedDelta = static_cast<float>(deltaX) / static_cast<float>(Rect.W);
            OnDrag(normalizedDelta);
            LastMousePos = MousePos;
        }
        else if (IsOverBorder(MousePos))
        {
            UE_LOG("SSplitterV: Mouse Hovering over border");
        }
    }
    
    virtual void OnMouseUp(const FPoint& MousePos) override
    {
        bIsDragging = false;
        UE_LOG("SSpliterV : Mouse Up, stop dragging");
    }

    void OnDrag(float Delta) override
    {
        SplitRatio += Delta;
        SplitRatio = std::clamp(SplitRatio, 0.2f, 0.8f);
        UpdateLayout();
        UE_LOG("SSplitterV: Dragging, SplitRatio = %.2f", SplitRatio);
    }

    void UpdateLayout() override
    {
        Super::UpdateLayout();

        // 왼쪽의 width를 반영
        FRect leftRect = Rect;
        leftRect.W = static_cast<uint32_t>(Rect.W * SplitRatio);
        SideLT->SetRect(leftRect);

        // 오른쪽의 width와 시작점 X를 반영
        FRect rightRect = Rect;
        rightRect.X = Rect.X + leftRect.W;
        rightRect.W = Rect.W - leftRect.W;
        SideRB->SetRect(rightRect);
    }
};

class SSplitterH final : public SSplitter {
private:
    const uint32 BorderThickness = 4;
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
        uint32_t topHeight = static_cast<uint32_t>(Rect.H * SplitRatio);
        FRect borderRect;
        borderRect.X = Rect.X;
        borderRect.Y = Rect.Y + topHeight - BorderThickness / 2;
        borderRect.W = Rect.W;
        borderRect.H = BorderThickness;
        return IsInRect(MousePos, borderRect);
    }

    virtual void OnMouseDown(const FPoint& MousePos) override
    {
        if (IsOverBorder(MousePos))
        {
            bIsDragging = true;
            LastMousePos = MousePos;
            UE_LOG("SSplitterH: Border Mouse Down, start dragging");
        }
    }

    virtual void OnMouseMove(const FPoint& MousePos) override
    {
        if (TopSplitterV) TopSplitterV->OnMouseMove(MousePos);
        if (BottomSplitterV) BottomSplitterV->OnMouseMove(MousePos);
        if (bIsDragging)
        {
            // Y축 이동량 기준 delta 계산
            int deltaY = static_cast<int>(MousePos.Y) - static_cast<int>(LastMousePos.Y);
            float normalizedDelta = static_cast<float>(deltaY) / static_cast<float>(Rect.H);
            OnDrag(normalizedDelta);
            LastMousePos = MousePos;
        }
        else if (IsOverBorder(MousePos))
        {
            UE_LOG("SSplitterH: Mouse Hovering over border");
        }
    }

    void OnDrag(float Delta) override {
        SplitRatio += Delta;
        SplitRatio = std::clamp(SplitRatio, 0.2f, 0.8f);
        UpdateLayout();
        UE_LOG("SSplitterH: Dragging, SplitRatio = %.2f", SplitRatio);
    }

    void UpdateLayout() override {
        SSplitter::UpdateLayout();

        FRect topRect = Rect;
        topRect.H = static_cast<uint32_t>(Rect.H * SplitRatio);
        TopSplitterV->SetRect(topRect);

        FRect bottomRect = Rect;
        bottomRect.Y = Rect.Y + topRect.H;
        bottomRect.H = Rect.H - topRect.H;
        BottomSplitterV->SetRect(bottomRect);

        TopSplitterV->UpdateLayout();
        BottomSplitterV->UpdateLayout();
    }
};
