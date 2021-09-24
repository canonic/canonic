#pragma once

#include <QQuickItem>
#include <QQuickWindow>

#include "RenderControl.fwd.hpp"
#include "MainWindow.fwd.hpp"

class Viewport: public QQuickWindow
{
    Q_OBJECT

  signals:
    void initalised();

  public:
    explicit Viewport(MainWindow *mainWindow, RenderControl* renderControl);
    ~Viewport();

    bool isInitialised() const;
    QQmlEngine* getQmlEngine() const;
    RenderControl * getRenderControl() const;
    QUrl getSource() const;
    void setSource(QUrl source);
    uint getTextureId() const;
    void resizeTexture(QSize newSize);

  protected:
    bool m_initialized;
    MainWindow *m_mainWindow;
    QQmlComponent *m_qmlComponent;
    QQmlEngine *m_qmlEngine;
    RenderControl *m_renderControl;
    QQuickItem *m_rootItem;
    QUrl m_source;

  private:
    uint m_textureId;
    QSize m_textureSize;

    void updateSizes();

  private slots:
    void createTexture();
    void destroyTexture();
    void handleComponentStatusChange();
};
