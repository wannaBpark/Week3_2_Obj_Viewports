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

void HandleMouseInput(HWND hWnd, LPARAM lParam, bool isDown, bool isRight)
{
    POINTS Pts = MAKEPOINTS(lParam);
    FVector WH = GetWndWH(hWnd);
    if (isDown)
    {
        APlayerInput::Get().MouseKeyDown(FVector(Pts.x , Pts.y , 0), FVector(WH.X , WH.Y , 0), isRight);
    }else
    {
        APlayerInput::Get().MouseKeyUp(FVector(Pts.x , Pts.y , 0), FVector(WH.X , WH.Y , 0), isRight);
    }
}

APlayerInput::APlayerInput()
{
    Keyboard = std::make_unique<DirectX::Keyboard>();
    Mouse = std::make_unique<DirectX::Mouse>();
    GamePad = std::make_unique<DirectX::GamePad>();
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

void APlayerInput::UpdateInput()
{
    auto Kb = Keyboard->GetState();
    KeyboardTracker.Update(Kb);

    // 마우스 상태 가져오기
    auto MouseState = Mouse->GetState();
    MouseTracker.Update(MouseState);

    PrevMouseState = CurrentMouseState;

    CurrentMouseState.LeftDown = MouseState.leftButton;
    CurrentMouseState.RightDown = MouseState.rightButton;
    CurrentMouseState.MiddleDown = MouseState.middleButton;
    CurrentMouseState.Wheel = MouseState.scrollWheelValue;
    CurrentMouseState.X = MouseState.x; // 윈도우에 상대적
    CurrentMouseState.Y = MouseState.y; // 윈도우에 상대적

    POINT p;
    GetCursorPos(&p);
    CurrentMouseState.ScreenX = p.x;
    CurrentMouseState.ScreenY = p.y;
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

std::vector<EKeyCode> APlayerInput::GetPressedKeys() {
    std::vector<EKeyCode> ret;

    for (int i = 0; i < 256; i++) {
        if (_keys[ i ]) {
            ret.push_back(static_cast< EKeyCode >( i ));
        }
    }

    return ret;
}
void APlayerInput::MouseKeyDown(FVector MouseDownPoint, FVector WindowSize, int isRight) {
    mouse[isRight] = true;
    onceMouse[isRight] = true;
    MouseKeyDownPos[isRight] = MouseDownPoint;
    MouseKeyDownNDCPos[isRight] = CalNDCPos(MouseKeyDownPos[isRight], WindowSize);
} //MouseKeyDownPos 설정

void APlayerInput::MouseKeyUp(FVector MouseUpPoint, FVector WindowSize, int isRight) {
    mouse[isRight] = false;
    onceMouse[isRight] = false;
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

void APlayerInput::HandleMouseInput(HWND hWnd, LPARAM lParam, bool isDown, bool isRight)
{
    POINTS Pts = MAKEPOINTS(lParam);
    FVector WH = GetWndWH(hWnd);
    if (isDown)
    {
        MouseKeyDown(FVector(Pts.x , Pts.y , 0), FVector(WH.X , WH.Y , 0), isRight);
    }else
    {
        MouseKeyUp(FVector(Pts.x , Pts.y , 0), FVector(WH.X , WH.Y , 0), isRight);
    }
}

