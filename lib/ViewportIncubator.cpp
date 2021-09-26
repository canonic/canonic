#include "../include/ViewportIncubator.hpp"

void ViewportIncubator::statusChanged(QQmlIncubator::Status status)
{
    emit this->statusChangedSignal(status);
}
