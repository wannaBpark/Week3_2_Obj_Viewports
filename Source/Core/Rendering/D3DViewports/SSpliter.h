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
};

class SSplitterH final : public SSplitter {
private:
    std::shared_ptr<SSplitterV> TopSplitterV;
    std::shared_ptr<SSplitterV> BottomSplitterV;

public:
    SSplitterH(std::shared_ptr<SSplitterV> Top, std::shared_ptr<SSplitterV> Bottom)
        : SSplitter(Top, Bottom), TopSplitterV(Top), BottomSplitterV(Bottom) {
    }

    void OnDrag(float Delta) override {
        SplitRatio += Delta;
        SplitRatio = std::clamp(SplitRatio, 0.2f, 0.8f);
        UpdateLayout();
    }

    void UpdateLayout() override {
        SSplitter::UpdateLayout();

        FRect topRect = Rect;
        topRect.H = static_cast<uint32_t>(Rect.H * SplitRatio);
        TopSplitterV->SetRect(topRect);

        FRect rightRect = Rect;
        rightRect.Y = Rect.Y + topRect.H;
        rightRect.H = Rect.H - topRect.H;
        BottomSplitterV->SetRect(rightRect);

        TopSplitterV->UpdateLayout();
        BottomSplitterV->UpdateLayout();
    }
};

class SSplitterV final : public SSplitter
{
    using Super = SSplitter;
public:
    explicit SSplitterV(std::shared_ptr<SWindow> Left, std::shared_ptr<SWindow> Right) : SSplitter(Left, Right) { }

    void OnDrag(float Delta) override
    {
        SplitRatio += Delta;
        SplitRatio = std::clamp(SplitRatio, 0.2f, 0.8f);
        UpdateLayout();
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