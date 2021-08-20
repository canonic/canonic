#pragma once

#include <QMainWindow>

#include "LayoutViewport.hpp"
#include "MainViewport.hpp"
#include "MainWindow.hpp"

class QWidgetMainWindow: public MainWindow
{
    Q_INTERFACES(MainWindow)

  public:
    QWidgetMainWindow();
    ~QWidgetMainWindow();

    const int width() const;
    const int height() const;
    void resize(int w, int h);
    void setGeometry(int x, int y, int w, int h);
    void show();

    void setActiveViewIndex(int activeViewIndex);

    LayoutViewport* getLayoutViewport() const;
    MainViewport* getMainViewport() const;
    QQmlEngine* getQmlEngine() const;

    void handleMouseDoubleClickEvent(QPointF localPos,
        int button,
        int buttons,
        int modifiers,
        int source);

    void handlePositionChangedEvent(QPointF localPos,
        int button,
        int buttons,
        int modifiers,
        int source);

    void handlePressEvent(QPointF localPos,
        int button,
        int buttons,
        int modifiers,
        int source);

    void handleReleaseEvent(QPointF localPos,
        int button,
        int buttons,
        int modifiers,
        int source);

    void handleWheelEvent(QPointF localPos,
        QPoint pixelDelta,
        QPoint angleDelta,
        int buttons,
        int modifiers,
        bool inverted,
        int source);

  private:
    void handleMouseEvent(QEvent::Type type,
        QPointF localPos,
        int button,
        int buttons,
        int modifiers,
        int source);

    QMainWindow *m_qMainWindow;
    LayoutViewport *m_layoutViewport;
    MainViewport *m_mainViewport;

  public slots:
    void handleActiveViewIndexChanged(int activeViewIndex);
    void handleLayoutViewStatusChanged(QQuickWidget::Status status);
};
