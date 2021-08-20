#pragma once

#include "MainWindow.hpp"
#include <QQuickView>

class QQuickMainWindow: public MainWindow
{
    Q_INTERFACES(MainWindow)

  public:
    QQuickMainWindow();
    ~QQuickMainWindow();

    const int width() const;
    const int height() const;
    void resize(int w, int h);
    void setGeometry(int x, int y, int w, int h);
    void show();

    QQmlEngine* getQmlEngine() const;
    void refresh();

  private:
    QQuickView *m_qQuickView;

  public slots:
    void handleActiveViewChanged(View *view);
};
