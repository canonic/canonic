#pragma once

#include <QQuickItem>
#include <QQuickWindow>

#include "RenderControl.fwd.hpp"
#include "MainWindow.fwd.hpp"

class Viewport: public QQuickWindow
{
    Q_OBJECT

  public:
    enum class Status {
        Null = 0,
        Loading = 1,
        Ready = 2,
        ComponentError,
        ObjectCreationError,
        NotAnItemError,
    };

    explicit Viewport(MainWindow *mainWindow, RenderControl* renderControl);
    ~Viewport();

    QQmlEngine* getQmlEngine() const;
    RenderControl * getRenderControl() const;

    QUrl getSource() const;
    void setSource(QUrl source);
    void setData(const QByteArray &data, QUrl source);
    QString getErrorString() const;

    Viewport::Status getStatus() const;
    uint getTextureId() const;
    void resizeTexture(QSize newSize);

  signals:
    void statusChanged(Viewport::Status status);
    void errorStringChanged(QString errorString);

  protected:
    MainWindow *m_mainWindow;
    QQmlComponent *m_qmlComponent;
    QQmlEngine *m_qmlEngine;
    RenderControl *m_renderControl;
    QQuickItem *m_rootItem;

    QUrl m_source;
    Viewport::Status m_status;

    void setStatus(Viewport::Status status);
    virtual void updateItemSizes(QSize newSize) = 0;
    void setErrorString(QString errorString);

  private:
    uint m_textureId;
    QSize m_textureSize;
    QString m_errorString;
    void updateSizes();

  private slots:
    void createTexture();
    void destroyTexture();
    void handleComponentStatusChange();
};
