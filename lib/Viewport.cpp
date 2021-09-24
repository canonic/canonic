#include "../include/Viewport.hpp"
#include "../include/MainWindow.hpp"
#include "../include/RenderControl.hpp"
#include "../include/Window.hpp"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QQmlContext>
#include <QQuickGraphicsDevice>
#include <QQuickRenderTarget>

Viewport::Viewport(MainWindow *mainWindow, RenderControl* renderControl):
    QQuickWindow(renderControl),
    m_mainWindow{mainWindow},
    m_qmlComponent{nullptr},
    m_qmlEngine{new QQmlEngine},
    m_renderControl{renderControl},
    m_rootItem{nullptr},
    m_source{QUrl()},
    m_status{Viewport::Status::Null},
    m_textureId{0}
{
    // Set the incubation controller to the QQuickWindows incubation controller
    // if no incubation controller is already set.
    if (!this->m_qmlEngine->incubationController())
        this->m_qmlEngine->setIncubationController(this->incubationController());

    this->m_qmlEngine->setNetworkAccessManagerFactory(mainWindow->m_networkAccessManagerFactory);

    // Set context properties
    this->m_qmlEngine->rootContext()->setContextProperty("window", m_mainWindow->m_window);

    // Create connections
    connect(this, &QQuickWindow::sceneGraphInitialized, this, &Viewport::createTexture);
    connect(this, &QQuickWindow::sceneGraphInvalidated, this, &Viewport::destroyTexture);
}

Viewport::~Viewport()
{
    // Make the context current during cleanup.
    QOpenGLContext *context = this->m_mainWindow->m_context;
    context->makeCurrent(this->m_mainWindow->m_offscreenSurface);

    delete this->m_rootItem;

    if (this->m_qmlComponent != nullptr)
    {
        delete this->m_qmlComponent;
    }

    if (this->m_textureId)
    {
        this->destroyTexture();
    }
    context->doneCurrent();
}

QQmlEngine* Viewport::getQmlEngine() const
{
    return this->m_qmlEngine;
}

RenderControl *Viewport::getRenderControl() const
{
    return m_renderControl;
}

QUrl Viewport::getSource() const
{
    return this->m_source;
}

void Viewport::setSource(QUrl source)
{
    this->setStatus(Viewport::Status::Loading);
    this->m_source = source;

    this->m_qmlComponent = new QQmlComponent(this->m_qmlEngine, source);
    if (this->m_qmlComponent->isLoading())
    {
        connect(this->m_qmlComponent, &QQmlComponent::statusChanged,
                this, &Viewport::handleComponentStatusChange);
    }
    else
    {
        this->handleComponentStatusChange();
    }
}

Viewport::Status Viewport::getStatus() const
{
    return this->m_status;
}

void Viewport::setStatus(Viewport::Status status)
{
    this->m_status = status;
    emit this->statusChanged(status);
}

uint Viewport::getTextureId() const
{
    return this->m_textureId;
}

void Viewport::resizeTexture(QSize newSize)
{
    // Only resize if the texture has already been created
    if(!this->m_textureId)
        return;

    // Only resize if the new size is actually different
    if(this->m_textureSize == newSize)
        return;

    // If the scene graph has not yet been initialized then do not proceed
    if(!this->isSceneGraphInitialized())
        return;

    QOpenGLContext *context = this->m_mainWindow->m_context;

    // Makes the opengl context current
    if (!context->makeCurrent(this->m_mainWindow->m_offscreenSurface))
        return;

    // Destroy the old texture
    this->destroyTexture();

    // Create the new resized texture
    this->createTexture();

    // Release the opengl context
    context->doneCurrent();

    // Finally update the sizes of the window and root item to be the new size
    updateSizes();
}

void Viewport::updateSizes()
{
    QSize newSize = this->m_mainWindow->size() * this->m_mainWindow->devicePixelRatio();

    // Make sure the texture has a non zero size otherwise QQuickRenderTarget
    // will raise warnings
    if (!this->m_textureSize.isValid())
    {
        this->m_textureSize = QSize(1,1);
    }

    if (m_rootItem != nullptr){
        this->m_rootItem->setWidth(newSize.width());
        this->m_rootItem->setHeight(newSize.height());
    }
    this->setGeometry(0, 0, newSize.width(), newSize.height());
}

void Viewport::createTexture()
{
    // If the scene graph has not yet been initialized then do not proceed
    if(!this->isSceneGraphInitialized())
        return;

    // Now that the scene graph has been initialized create the texture and
    // associate it with the QQuickWindow.
    this->m_textureSize = this->m_mainWindow->size() * this->m_mainWindow->devicePixelRatio();

    // Make sure the texture has a non zero size otherwise QQuickRenderTarget
    // will raise warnings
    if (!this->m_textureSize.isValid())
    {
        this->m_textureSize = QSize(1,1);
    }

    QOpenGLFunctions *f = this->m_mainWindow->m_context->functions();
    f->glGenTextures(1, &m_textureId);
    f->glBindTexture(GL_TEXTURE_2D, m_textureId);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    f->glTexImage2D(GL_TEXTURE_2D,                  // target
                    0,                              // level
                    GL_RGBA,                        // internalformat
                    this->m_textureSize.width(),    // width
                    this->m_textureSize.height(),   // height
                    0,                              // border
                    GL_RGBA,                        // format
                    GL_UNSIGNED_BYTE,               // type
                    nullptr);                       // pixels

    this->setRenderTarget(QQuickRenderTarget::fromOpenGLTexture(m_textureId,
                                                                m_textureSize));
}

void Viewport::destroyTexture()
{
    this->m_mainWindow->m_context->functions()->glDeleteTextures(1, &m_textureId);
    m_textureId = 0;
}

void Viewport::handleComponentStatusChange()
{
    disconnect(this->m_qmlComponent, &QQmlComponent::statusChanged, this, &Viewport::handleComponentStatusChange);

    if (this->m_qmlComponent->isError()) {
        const QList<QQmlError> errorList = this->m_qmlComponent->errors();
        for (const QQmlError &error : errorList)
            qWarning() << error.url() << error.line() << error;

        this->setStatus(Viewport::Status::ComponentError);
        return;
    }

    QObject *rootObject = this->m_qmlComponent->create();
    if (this->m_qmlComponent->isError()) {
        const QList<QQmlError> errorList = this->m_qmlComponent->errors();
        for (const QQmlError &error : errorList)
            qWarning() << error.url() << error.line() << error;

        this->setStatus(Viewport::Status::ObjectCreationError);
        return;
    }

    this->m_rootItem = qobject_cast<QQuickItem *>(rootObject);
    if (!this->m_rootItem) {
        qWarning("run: Not a QQuickItem");
        delete rootObject;

        this->setStatus(Viewport::Status::NotAnItemError);
        return;
    }

    // The root item is ready. Associate it with the window.
    this->m_rootItem->setParentItem(this->contentItem());

    // Update item and rendering related geometries.
    this->updateSizes();

    // Initialize the render control and our OpenGL resources.
    QOpenGLContext *context = this->m_mainWindow->m_context;
    context->makeCurrent(this->m_mainWindow->m_offscreenSurface);
    this->setGraphicsDevice(QQuickGraphicsDevice::fromOpenGLContext(context));
    this->m_renderControl->initialize();

    this->setStatus(Viewport::Status::Ready);
}
