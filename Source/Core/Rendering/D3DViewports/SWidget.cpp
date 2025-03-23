#include "SWidget.h"

SWidget::SWidget()
{
}

SWidget::~SWidget()
{
}

FReply& FReply::SetMousePos(const FPoint& NewMousePos)
{
	this->bSetUserFocus = true;
	this->bReleaseUserFocus = false;

	return *this;
}

FReply& FReply::SetUserFocus(std::shared_ptr<SWidget> GiveMeFocus)
{
	this->FocusRecipient = GiveMeFocus;
}

FReply& FReply::ClearUserFocus()
{
	bSetUserFocus = false;
	return *this;
}

FReply& FReply::CancelFocusRequest()
{
	this->bSetUserFocus = false;
	this->FocusRecipient.reset();
	this->bReleaseUserFocus = false;

	return *this;
}
