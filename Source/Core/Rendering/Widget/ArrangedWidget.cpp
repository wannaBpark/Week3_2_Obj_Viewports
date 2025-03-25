#include "ArrangedWidget.h"

const FArrangedWidget& FArrangedWidget::GetNullWidget()
{
    return FArrangedWidget(nullptr, FGeometry());
}
