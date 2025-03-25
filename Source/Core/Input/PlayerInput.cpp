#include "PlayerInput.h"

FVector GetWndWH(HWND hWnd)
{
    RECT Rect;
    int Width , Height;
    if (GetClientRect(hWnd , &Rect)) {
        Width = Rect.right - Rect.left;
        Height = Rect.bottom - Rect.top;
    }

    return FVector(Width, Height, 0.0f);
}

void HandleMouseInput(HWND hWnd, LPARAM lParam, bool isDown, EMouseButton InMouseButton)
{
    POINTS Pts = MAKEPOINTS(lParam);
    FVector WH = GetWndWH(hWnd);
    if (isDown)
    {
        APlayerInput::Get().MouseKeyDown(FVector(Pts.x , Pts.y , 0), FVector(WH.X , WH.Y , 0), InMouseButton);
    }
    else
    {
        APlayerInput::Get().MouseKeyUp(FVector(Pts.x , Pts.y , 0), FVector(WH.X , WH.Y , 0), InMouseButton);
    }
}

APlayerInput::APlayerInput()
{
    for (bool& key : _keys)
    {
        key = false;
    }

    for (bool& m : mouse)
    {
        m=false;
    }

    for (bool& ok : _onceKeys)
    {
        ok=false;
    }

    for (bool& om : onceMouse)
    {
        om=false;
    }
}

bool APlayerInput::IsPressedKey(EKeyCode key) const
{
    return _keys[static_cast<uint8_t>(key)];
}

void APlayerInput::KeyDown(EKeyCode key)
{
    _keys[static_cast<uint8_t>(key)] = true;
    _onceKeys[static_cast<uint8_t>(key)] = true;
}

void APlayerInput::KeyOnceUp(EKeyCode key)
{
    _onceKeys[static_cast<uint8_t>(key)] = false;
}

void APlayerInput::KeyUp(EKeyCode key)
{
    _keys[static_cast<uint8_t>(key)] = false;
    _onceKeys[static_cast<uint8_t>(key)] = false;
}

std::vector<EKeyCode> APlayerInput::GetPressedKeys()
{
    std::vector<EKeyCode> ret;

    for (int i = 0; i < 256; i++) {
        if (_keys[ i ]) {
            ret.push_back(static_cast< EKeyCode >( i ));
        }
    }

    return ret;
}
void APlayerInput::MouseKeyDown(FVector MouseDownPoint, FVector WindowSize, EMouseButton InMouseButton)
{
    mouse[static_cast<uint32>(InMouseButton)] = true;
    onceMouse[static_cast<uint32>(InMouseButton)] = true;
    MouseKeyDownPos[static_cast<uint32>(InMouseButton)] = MouseDownPoint;
    MouseKeyDownNDCPos[static_cast<uint32>(InMouseButton)] = CalNDCPos(MouseKeyDownPos[static_cast<uint32>(InMouseButton)], WindowSize);
} //MouseKeyDownPos 설정

void APlayerInput::MouseKeyUp(FVector MouseUpPoint, FVector WindowSize, EMouseButton InMouseButton)
{
    mouse[static_cast<uint32>(InMouseButton)] = false;
    onceMouse[static_cast<uint32>(InMouseButton)] = false;
}

void APlayerInput::PreProcessInput()
{
    ExpireOnce();
}

void APlayerInput::TickPlayerInput()
{
    SetMousePos();
}

void APlayerInput::SetMousePos()
{
    POINT Pts;
    if (GetCursorPos(&Pts))
    {
        FVector PtV = FVector(Pts.x , Pts.y, 0);
        SetMousePos(PtV);
    }
}

void APlayerInput::ExpireOnce()
{
    for (bool& i : onceMouse)
    {
        i = false;
    }

    for (bool& m : _onceKeys)
    {
        m=false;
    }
}

FVector APlayerInput::CalNDCPos(FVector MousePos, FVector WindowSize)
{
    return {( MousePos.X / ( WindowSize.X / 2 ) ) - 1, ( MousePos.Y / ( WindowSize.Y / 2 ) ) - 1, 0};
}

void APlayerInput::SetPointer(FVector2D position)
{
    Pointer.LastScreenSpacePosition = Pointer.ScreenSpacePosition;
    Pointer.ScreenSpacePosition = position;
    Pointer.CursorDelta = Pointer.ScreenSpacePosition - Pointer.LastScreenSpacePosition;
}

void APlayerInput::HandleMouseInput(HWND hWnd, LPARAM lParam, bool isDown, EMouseButton InMouseButton)
{
    POINTS Pts = MAKEPOINTS(lParam);
    FVector WH = GetWndWH(hWnd);
    if (isDown)
    {
        MouseKeyDown(FVector(Pts.x , Pts.y , 0), FVector(WH.X , WH.Y , 0), InMouseButton);
    }else
    {
        MouseKeyUp(FVector(Pts.x , Pts.y , 0), FVector(WH.X , WH.Y , 0), InMouseButton);
    }
}

