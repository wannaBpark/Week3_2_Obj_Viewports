#pragma once
#include <memory>
#include "Core/Input/PlayerInput.h"
#include "Core/Math/IntPoint.h"

class SWidget;
class SWindow;

class FReply
{
public:

	FReply& CaptureMouse(std::shared_ptr<SWidget> InMouseCaptor)
	{
		this->MouseCaptor = InMouseCaptor;
		return *this;
	}

	FReply& SetMousePos(const FIntPoint& NewMousePos);

	FReply& SetUserFocus(std::shared_ptr<SWidget> GiveMeFocus);

	FReply& ClearUserFocus();

	FReply& CancelFocusRequest();

	FReply& LockMouseToWidget(std::shared_ptr<SWidget> InWidget)
	{
		this->MouseLockWidget = InWidget;
		this->bShouldReleaseMouseLock = false;
		return  *this;
	}

	FReply& ReleaseMouseLock()
	{
		this->bShouldReleaseMouseLock = true;
		MouseLockWidget.reset();
		return *this;
	}

	FReply& ReleaseMouseCapture()
	{
		this->MouseCaptor.reset();
		this->bReleaseMouseCapture = true;
		return *this;
	}

	/**
	 * Ask Slate to detect if a user started dragging in this widget.
	 * If a drag is detected, Slate will send an OnDragDetected event.
	 *
	 * @param DetectDragInMe  Detect dragging in this widget
	 * @param MouseButton     This button should be pressed to detect the drag
	 */
	FReply& DetectDrag(const std::shared_ptr<SWidget>& DetectDragInMe, EMouseButton MouseButton)
	{
		this->DetectDragForWidget = DetectDragInMe;
		this->DetectDragForMouseButton = MouseButton;
		return *this;
	}

	/**
	 * An event should return FReply::Handled().BeginDragDrop( Content ) to initiate a drag and drop operation.
	 *
	 * @param InDragDropContent  The content that is being dragged. This could be a widget, or some arbitrary data
	 *
	 * @return reference back to the FReply so that this call can be chained.
	 */
	 //FReply& BeginDragDrop(TSharedRef<FDragDropOperation> InDragDropContent)
	 //{
	 //	this->DragDropContent = InDragDropContent;
	 //	return Me();
	 //}

	 /** An event should return FReply::Handled().EndDragDrop() to request that the current drag/drop operation be terminated. */
	FReply& EndDragDrop()
	{
		this->bEndDragDrop = true;
		return *this;
	}

	/** True if this reply indicated that we should release mouse capture as a result of the event being handled */
	bool ShouldReleaseMouse() const { return bReleaseMouseCapture; }

	/** true if this reply indicated that we should set focus as a result of the event being handled */
	bool ShouldSetUserFocus() const { return bSetUserFocus; }

	/** true if this reply indicated that we should release focus as a result of the event being handled */
	bool ShouldReleaseUserFocus() const { return bReleaseUserFocus; }

	/** True if the reply indicated that we should release mouse lock */
	bool ShouldReleaseMouseLock() const { return bShouldReleaseMouseLock; }

	/** Returns the widget that the mouse should be locked to (if any) */
	std::shared_ptr<SWidget> GetMouseLockWidget() const { return MouseLockWidget.lock(); }

	/** When not nullptr, user focus has been requested to be set on the FocusRecipient. */
	std::shared_ptr<SWidget> GetUserFocusRecepient() const { return FocusRecipient.lock(); }

	/** If the event replied with a request to capture the mouse, this returns the desired mouse captor. Otherwise returns an invalid pointer. */
	std::shared_ptr<SWidget> GetMouseCaptor() const { return MouseCaptor.lock(); }

	///** @return the Content that we should use for the Drag and Drop operation; Invalid SharedPtr if a drag and drop operation is not requested*/
	//const std::shared_ptr<FDragDropOperation>& GetDragDropContent() const { return DragDropContent; }

	/** @return true if the user has asked us to terminate the ongoing drag/drop operation */
	bool ShouldEndDragDrop() const { return bEndDragDrop; }

	/** @return a widget for which to detect a drag; Invalid SharedPtr if no drag detection requested */
	std::shared_ptr<SWidget> GetDetectDragRequest() const { return DetectDragForWidget.lock(); }

	/** @return The coordinates of the requested mouse position */
	const FIntPoint& GetRequestedMousePos() const { return RequestedMousePos; }


private:
	std::weak_ptr<SWidget> EventHandler;
	std::weak_ptr<SWidget> MouseCaptor;
	std::weak_ptr<SWidget> FocusRecipient;
	std::weak_ptr<SWidget> MouseLockWidget;
	std::weak_ptr<SWidget> DetectDragForWidget;
	EKeyCode KeyCode;
	EMouseButton MouseButton;
	EMouseButton DetectDragForMouseButton;
	//FDragDropOperation??
	FIntPoint RequestedMousePos;
	bool bReleaseMouseCapture;
	bool bSetUserFocus;
	bool bReleaseUserFocus;
	bool bShouldReleaseMouseLock;
	bool bEndDragDrop;
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