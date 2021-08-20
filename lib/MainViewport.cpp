#include "../include/MainViewport.hpp"
#include <QCoreApplication>

MainViewport::MainViewport(LayoutViewport *unhandledEventHandler): m_unhandledEventHandler{unhandledEventHandler}
{
}

/*
void MainViewport::mouseDoubleClickEvent(QMouseEvent *e)
{
    this->handleMouseEvent(e);
}

void MainViewport::mouseMoveEvent(QMouseEvent *e)
{
    this->handleMouseEvent(e);
}

void MainViewport::mousePressEvent(QMouseEvent *e)
{
    this->handleMouseEvent(e);
}

void MainViewport::mouseReleaseEvent(QMouseEvent *e)
{
    this->handleMouseEvent(e);
}

void MainViewport::tabletEvent(QTabletEvent *e)
{
    if(!this->isMouseOver())
    {
        this->m_unhandledEventHandler->tabletEvent(e);
    }
}


void MainViewport::wheelEvent(QWheelEvent *e)
{
    if(!this->isMouseOver())
    {
        this->m_unhandledEventHandler->wheelEvent(e);
    }
}
*/

