#pragma once
#include <memory>
#include "Core/Input/PlayerInput.h"
#include "Core/Math/IntPoint.h"

class SWidget;
class SWindow;

class FReply
{
public:
    // 마우스 캡처를 요청합니다.
    FReply& CaptureMouse(std::shared_ptr<SWidget> InMouseCaptor)
    {
        this->MouseCaptor = InMouseCaptor;
        return *this;
    }

    // 마우스 위치를 설정합니다.
    FReply& SetMousePos(const FIntPoint& NewMousePos);

    // 사용자 포커스를 요청합니다.
    FReply& SetUserFocus(const std::shared_ptr<SWidget>& GiveMeFocus);

    // 사용자 포커스를 해제합니다.
    FReply& ClearUserFocus();

    // 포커스 요청을 취소합니다.
    FReply& CancelFocusRequest();

    // 위젯에 대해 마우스 캡처를 요청한 상태로 마우스를 고정합니다.
    FReply& LockMouseToWidget(std::shared_ptr<SWidget> InWidget)
    {
        this->MouseLockWidget = InWidget;
        this->bShouldReleaseMouseLock = false;
        return *this;
    }

    // 마우스 고정을 해제합니다.
    FReply& ReleaseMouseLock()
    {
        this->bShouldReleaseMouseLock = true;
        MouseLockWidget.reset();
        return *this;
    }

    // 이벤트 처리 결과로 마우스 캡처를 해제하도록 요청합니다.
    FReply& ReleaseMouseCapture()
    {
        this->MouseCaptor.reset();
        this->bReleaseMouseCapture = true;
        return *this;
    }

    /**
     * 위젯에서 사용자가 드래그를 시작했는지 감지하도록 요청합니다.
     * 드래그가 감지되면, Slate는 OnDragDetected 이벤트를 보냅니다.
     *
     * @param DetectDragInMe  이 위젯에서 드래그 감지를 수행합니다.
     * @param MouseButton     드래그 감지를 위해 눌러야 하는 마우스 버튼입니다.
     */
    FReply& DetectDrag(const std::shared_ptr<SWidget>& DetectDragInMe, EMouseButton MouseButton)
    {
        this->DetectDragForWidget = DetectDragInMe;
        this->DetectDragForMouseButton = MouseButton;
        return *this;
    }

    /**
     * 드래그 앤 드롭 이벤트에서, 이 함수는 현재 진행 중인 드래그/드롭 작업을 종료하도록 요청합니다.
     *
     * @return 체이닝을 위한 FReply 참조.
     */
    FReply& EndDragDrop()
    {
        this->bEndDragDrop = true;
        return *this;
    }

    /** 이 FReply가 이벤트 처리 결과로 마우스 캡처 해제를 요청했는지 여부를 반환합니다. */
    bool ShouldReleaseMouse() const { return bReleaseMouseCapture; }

    /** 이 FReply가 이벤트 처리 결과로 사용자 포커스 설정을 요청했는지 여부를 반환합니다. */
    bool ShouldSetUserFocus() const { return bSetUserFocus; }

    /** 이 FReply가 이벤트 처리 결과로 포커스 해제를 요청했는지 여부를 반환합니다. */
    bool ShouldReleaseUserFocus() const { return bReleaseUserFocus; }

    /** 이 FReply가 마우스 고정 해제를 요청했는지 여부를 반환합니다. */
    bool ShouldReleaseMouseLock() const { return bShouldReleaseMouseLock; }

    /** 마우스가 고정되어야 할 위젯(있는 경우)을 반환합니다. */
    std::shared_ptr<SWidget> GetMouseLockWidget() const { return MouseLockWidget.lock(); }

    /** FocusRecipient가 nullptr가 아니라면, 해당 위젯에 사용자 포커스 설정 요청이 된 상태입니다. */
    std::shared_ptr<SWidget> GetUserFocusRecepient() const { return FocusRecipient.lock(); }

    /** 이벤트 처리 결과로 마우스 캡처 요청이 있었다면, 해당 위젯을 반환합니다. (없으면 유효하지 않은 포인터 반환) */
    std::shared_ptr<SWidget> GetMouseCaptor() const { return MouseCaptor.lock(); }

    /** 드래그/드롭 작업을 종료하도록 요청했는지 여부를 반환합니다. */
    bool ShouldEndDragDrop() const { return bEndDragDrop; }

    /** 드래그 감지 요청이 있는 위젯을 반환합니다. (요청이 없으면 Invalid SharedPtr 반환) */
    std::shared_ptr<SWidget> GetDetectDragRequest() const { return DetectDragForWidget.lock(); }

    /** 요청된 마우스 위치 좌표를 반환합니다. */
    const FIntPoint& GetRequestedMousePos() const { return RequestedMousePos; }

private:
    std::weak_ptr<SWidget> EventHandler;
    std::weak_ptr<SWidget> MouseCaptor;
    std::weak_ptr<SWidget> FocusRecipient;
    std::weak_ptr<SWidget> MouseLockWidget;
    std::weak_ptr<SWidget> DetectDragForWidget;
    EKeyCode KeyCode = EKeyCode::None;
    EMouseButton MouseButton = EMouseButton::Max;
    EMouseButton DetectDragForMouseButton = EMouseButton::Max;
    FIntPoint RequestedMousePos = FIntPoint(0, 0);
    bool bReleaseMouseCapture = false;
    bool bSetUserFocus = false;
    bool bReleaseUserFocus = false;
    bool bShouldReleaseMouseLock = false;
    bool bEndDragDrop = false;
};


class FCursorReply
{
public:
	///**
	//* Respond with a specific cursor.
	//* This cursor will be used and no other widgets will be asked.
	//*/
	static FCursorReply Cursor(EMouseCursor InCursor)
	{
		return FCursorReply(InCursor);
	}
	/** @return The window to render the Cursor Widget in. */
	std::shared_ptr<SWindow> GetCursorWindow() const { return CursorWindow; }

	/** @return The custom Cursor Widget to render if set and the event was handled. */
	std::shared_ptr<SWidget> GetCursorWidget() const { return CursorWidget; }

	/** @return The requested MouseCursor if no custom widget is set and the event was handled. */
	//EMouseCursor::Type GetCursorType() const { return MouseCursor; }

	/** Set the Cursor Widget, used by slate application to set the cursor widget if the MapCursor returns a widget. */
	void SetCursorWidget(std::shared_ptr<SWindow> InCursorWindow, std::shared_ptr<SWidget> InCursorWidget) { CursorWindow = InCursorWindow; CursorWidget = InCursorWidget; }

private:
	/** Window to render for cursor */
	std::shared_ptr<SWindow> CursorWindow;

	/** Custom widget to render for cursor */
	std::shared_ptr<SWidget> CursorWidget;

	/** The cursor type must be set is CursorWidget is invalid */
	EMouseCursor MouseCursor;

	FCursorReply()
		: MouseCursor(EMouseCursor::Default)
	{
	}

	FCursorReply(EMouseCursor InCursorType)
		: MouseCursor(InCursorType)
	{
	}
};