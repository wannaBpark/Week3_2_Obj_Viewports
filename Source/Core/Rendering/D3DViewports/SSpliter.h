#pragma once
#include "Core/Rendering/D3DViewports/SWindow.h"

class SSplitter : public SWindow
{
protected:
    std::shared_ptr<SWindow> SideLT; // Left 또는 Top
    std::shared_ptr<SWindow> SideRB; // Right 또는 Bottom
    float SplitRatio = 0.5f;

public:
    SSplitter(std::shared_ptr<SWindow> LT, std::shared_ptr<SWindow> RB) : SideLT(LT), SideRB(RB) { }

    virtual void OnDrag(float Delta) = 0; // [순수 가상 함수 오버라이딩 필요] : 마우스 드래그에 따른 창 크기를 조절할 함수입니다

    virtual void Render() override
    {
        //SideLT->Render();
        //SideRB->Render();
    }
};

class SSplitterH : public SSplitter
{
public:
    SSplitterH(std::shared_ptr<SWindow> Top, std::shared_ptr<SWindow> Bottom) : SSplitter(Top, Bottom) { }

    void OnDrag(float Delta) override
    {
        SplitRatio += Delta;
        SplitRatio = std::clamp(SplitRatio, 0.2f, 0.8f);
    }
};

class SSplitterV : public SSplitter
{
public:
    SSplitterV(std::shared_ptr<SWindow> Left, std::shared_ptr<SWindow> Right) : SSplitter(Left, Right) { }

    void OnDrag(float Delta) override
    {
        SplitRatio += Delta;
        SplitRatio = std::clamp(SplitRatio, 0.2f, 0.8f);
    }
};