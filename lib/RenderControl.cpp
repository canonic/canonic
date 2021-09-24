#include "../include/RenderControl.hpp"

RenderControl::RenderControl(QWindow *w) : m_window(w)
{
}

QWindow *RenderControl::renderWindow(QPoint *offset)
{
    if (offset)
        *offset = QPoint(0, 0);
    return m_window;
}
