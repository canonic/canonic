#pragma once

#include <QOffscreenSurface>
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QWindow>


class CanonicRenderer
{
public:
    const static int TEXTURE_001_ATTRIB{0};
    const static int TEXTURE_002_ATTRIB{1};

    CanonicRenderer(QOpenGLContext *share, QOffscreenSurface *offscreenSurface);
    ~CanonicRenderer();

    void render(QWindow *w,
                QRectF normalisedViewportGeometry,
                uint texture,
                uint texture2);

private:
    void init(QWindow *w);
    void setupVertexAttribs();

    QOpenGLContext *m_share;
    QOffscreenSurface *m_offscreenSurface;
    QOpenGLContext *m_context;
    QOpenGLShaderProgram *m_program;
    QOpenGLBuffer *m_vbo;
    QOpenGLVertexArrayObject *m_vao;
};
