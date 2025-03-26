#include "SSpliter.h"
#include "Core/EngineConfig.h"
#include "Core/Engine.h"

// SSplitter
SSplitter::SSplitter(std::shared_ptr<SWindow> LT, std::shared_ptr<SWindow> RB)
    : SideLT(LT), SideRB(RB), SplitRatio(0.5f)
{
}

void SSplitter::SetSplitRatio(float InSplitRatio)
{
    SplitRatio = InSplitRatio;
}

float SSplitter::GetSplitRatio() const
{
    return SplitRatio;
}

std::shared_ptr<SWindow> SSplitter::GetSideLT()
{
    return SideLT;
}

std::shared_ptr<SWindow> SSplitter::GetSideRB()
{
    return SideRB;
}

void SSplitter::OnDrag(float Delta)
{
    SplitRatio += Delta;
    SplitRatio = std::clamp(SplitRatio, 0.1f, 0.9f);
    UpdateLayout();
    //UE_LOG("SSplitterH: Dragging, SplitRatio = %.2f", SplitRatio);
}

void SSplitter::Render()
{
    Super::Render();
    if (SideLT) SideLT->Render();
    if (SideRB) SideRB->Render();
}

void SSplitter::UpdateLayout()
{
    if (SideLT && SideRB)
    {
        SideLT->SetNeedsRender();
        SideRB->SetNeedsRender();
    }
}

void SSplitter::OnMouseMove(const FPoint& MousePos)
{
    if (SideLT) SideLT->OnMouseMove(MousePos);
    if (SideRB) SideRB->OnMouseMove(MousePos);
}

void SSplitter::OnMouseUp(const FPoint& MousePos)
{
    bIsDragging = false;
    ::SetCursor(LoadCursor(nullptr, IDC_ARROW));
    if (SideLT) SideLT->OnMouseUp(MousePos);
    if (SideRB) SideRB->OnMouseUp(MousePos);
    //UE_LOG("SSplitterH: Mouse Up, stop dragging");
}

void SSplitter::OnMouseDown(const FPoint& MousePos)
{
    if (SideLT) SideLT->OnMouseDown(MousePos);
    if (SideRB) SideRB->OnMouseDown(MousePos);
}

void SSplitter::OnKeyDown(EKeyCode Key)
{
    Super::OnKeyDown(Key);
    if (SideLT) SideLT->OnKeyDown(Key);
    if (SideRB) SideRB->OnKeyDown(Key);
}

// SSplitterV
SSplitterV::SSplitterV(std::shared_ptr<SWindow> Left, std::shared_ptr<SWindow> Right)
    : SSplitter(Left, Right)
{
}

void SSplitterV::SetSplitRatio(float ratio)
{
    SplitRatio = ratio;
}

bool SSplitterV::IsOverBorder(const FPoint& MousePos) const
{
    uint32 leftWidth = static_cast<uint32>(Rect.W * SplitRatio);
    FRect BorderRect = {
        Rect.X + leftWidth - BorderThickness / 2,
        Rect.Y,
        BorderThickness,
        Rect.H,
    };
    return IsInRect(MousePos, BorderRect);
}

void SSplitterV::OnMouseDown(const FPoint& MousePos)
{
    SSplitter::OnMouseDown(MousePos);
    // 경계 영역에서만 드래그 시작하도록
    if (IsOverBorder(MousePos))
    {
        bIsDragging = true;
        LastMousePos = MousePos;
    }
}

void SSplitterV::OnMouseMove(const FPoint& MousePos)
{
    SSplitter::OnMouseMove(MousePos);
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

void SSplitterV::UpdateLayout()
{
    SSplitter::UpdateLayout();
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

// SSplitterH
SSplitterH::SSplitterH(std::shared_ptr<SSplitterV> Top, std::shared_ptr<SSplitterV> Bottom)
    : SSplitter(Top, Bottom), TopSplitterV(Top), BottomSplitterV(Bottom)
{
}

bool SSplitterH::IsOverBorder(const FPoint& MousePos) const
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

bool SSplitterH::IsOverCenter(const FPoint& MousePos) const
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

bool SSplitterH::IsOverVerticalBorder(const FPoint& MousePos) const
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

void SSplitterH::OnMouseDown(const FPoint& MousePos)
{
    if (TopSplitterV) { TopSplitterV->OnMouseDown(MousePos); }
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

void SSplitterH::OnMouseMove(const FPoint& MousePos)
{
    SSplitter::OnMouseMove(MousePos);
    if (!bIsDragging) // 중앙, 수직, 단일 수평 축에 대한 아이콘 변경 (hover시)
    {
        if (IsOverCenter(MousePos)) { ::SetCursor(LoadCursor(nullptr, IDC_SIZEALL)); }
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
        else { OnDrag(normalizedDeltaY); }
        LastMousePos = MousePos;
    }
}

void SSplitterH::OnMouseUp(const FPoint& MousePos)
{
    bIsCenterDrag = bIsVerticalDrag = false;
    SSplitter::OnMouseUp(MousePos);
}

void SSplitterH::UpdateLayout()
{
    SSplitter::UpdateLayout();
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

void SSplitterH::OnKeyDown(EKeyCode Key)
{
    if (TopSplitterV) TopSplitterV->OnKeyDown(Key);
    if (BottomSplitterV) BottomSplitterV->OnKeyDown(Key);
}

void SSplitterH::SaveSplitterInfo()
{
    FEngineConfig* EngineConfig = UEngine::Get().GetEngineConfig();
    // Horizontal Splitter
    EngineConfig->SaveEngineConfig<float>(EEngineConfigValueType::EEC_HorizontalSplitRatio, GetSplitRatio());
    // Vertical Splitters
    if (TopSplitterV && BottomSplitterV)
    {
        EngineConfig->SaveEngineConfig<float>(EEngineConfigValueType::EEC_TopVerticalSplitRatio, TopSplitterV->GetSplitRatio());
        EngineConfig->SaveEngineConfig<float>(EEngineConfigValueType::EEC_BottomVerticalSplitRatio, BottomSplitterV->GetSplitRatio());
    }
    UE_LOG("Saved Split Value : %.2f %,2f %.2f", 
        GetSplitRatio(), TopSplitterV->GetSplitRatio(), BottomSplitterV->GetSplitRatio());
}