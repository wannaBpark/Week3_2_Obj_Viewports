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
    explicit SSplitter(std::shared_ptr<SWindow> LT, std::shared_ptr<SWindow> RB);
public:
    virtual void SetSplitRatio(float InSplitRatio) override;
    virtual float GetSplitRatio() const override;
    std::shared_ptr<SWindow> GetSideLT();
    std::shared_ptr<SWindow> GetSideRB();

    virtual void OnDrag(float Delta);

    virtual void Render() override; // 자신과 자신의 자식(Swindow or Splitter)을 렌더

    // Layout 갱신 후, 자식들이 render 가능한 상태로 전환되어야 Render()에서 렌더됨
    virtual void UpdateLayout();

    // 마우스 움직임, 키 입력이 자식들에게 전달되며 알맞은 행동을 취합니다
    virtual void OnMouseMove(const FPoint& MousePos) override;

    virtual void OnMouseUp(const FPoint& MousePos) override;

    virtual void OnMouseDown(const FPoint& MousePos) override;

    virtual void OnKeyDown(EKeyCode Key) override;
};

class SSplitterV final : public SSplitter
{
    using Super = SSplitter;
private:
    const uint32 BorderThickness = 8;
    FPoint LastMousePos;
    bool bVerticalDragging = false; // vertical 축 드래그 플래그 변수 (추가)
public:
    explicit SSplitterV(std::shared_ptr<SWindow> Left, std::shared_ptr<SWindow> Right);
    void SetSplitRatio(float ratio);

    bool IsOverBorder(const FPoint& MousePos) const;

    virtual void OnMouseDown(const FPoint& MousePos) override;

    virtual void OnMouseMove(const FPoint& MousePos) override;

    virtual void UpdateLayout() override;
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
    SSplitterH(std::shared_ptr<SSplitterV> Top, std::shared_ptr<SSplitterV> Bottom);
    void SaveSplitterInfo();

    // 마우스 좌표가 수평 경계 영역에 있는지 검사
    bool IsOverBorder(const FPoint& MousePos) const;

    // 중앙 영역 (vertical border와 horizontal border의 교차 영역) 검사
    bool IsOverCenter(const FPoint& MousePos) const;

    bool IsOverVerticalBorder(const FPoint& MousePos) const;

    virtual void OnMouseDown(const FPoint& MousePos) override;

    virtual void OnMouseMove(const FPoint& MousePos) override;

    virtual void OnMouseUp(const FPoint& MousePos) override;

    virtual void UpdateLayout() override;

    // 키 이벤트 전파: 자식 SSplitterV에게 전달
    virtual void OnKeyDown(EKeyCode Key) override;
};