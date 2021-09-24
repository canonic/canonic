#include "../include/CanonicRenderer.hpp"

#include <QOpenGLFunctions>

CanonicRenderer::CanonicRenderer(QOpenGLContext *share, QOffscreenSurface *offscreenSurface)
    : m_share(share),
      m_offscreenSurface(offscreenSurface),
      m_context(nullptr),
      m_program(nullptr),
      m_vbo(nullptr),
      m_vao(nullptr)
{
}

CanonicRenderer::~CanonicRenderer()
{
    // Use a temporary offscreen surface to do the cleanup.
    // There may not be a native window surface available anymore at this stage.
    m_context->makeCurrent(m_offscreenSurface);
    delete m_program;
    delete m_vbo;
    delete m_vao;
    m_context->doneCurrent();
    delete m_context;
}

void CanonicRenderer::init(QWindow *w)
{
    m_context = new QOpenGLContext;
    m_context->setShareContext(m_share);
    m_context->setFormat(w->requestedFormat());
    m_context->create();
    if (!m_context->makeCurrent(w))
        return;

    QOpenGLFunctions *f = m_context->functions();
    f->glViewport(0, 0, w->width() * w->devicePixelRatio(), w->height() * w->devicePixelRatio());

    static const char *vertexShaderSource =
            "#version 100\n"
            "attribute vec2 vertexIn;\n"
            "attribute vec2 vertTexCoord;\n"
            "varying highp vec2 fragTexCoord;\n"
            "void main(void)\n"
            "{\n"
            "    gl_Position = vec4(vertexIn, 0.0, 1.0);\n"
            "    fragTexCoord = vertTexCoord;\n"
            "}\n";

#ifdef Q_OS_WASM
    static const char *fragmentShaderSource =
            "#version 100\n"
            "#ifdef GL_ES\n"
            "   precision mediump float;\n"
            "#endif\n"
            "uniform lowp sampler2D tex;\n"
            "uniform lowp sampler2D tex2;\n"
            "varying vec2 fragTexCoord;\n"
//            "uniform lowp float viewportWidth;\n"
//            "uniform lowp float viewportHeight;\n"
            "void main(void)\n"
            "{\n"
//            "    vec2 viewportTexCoord = vec2(fragTexCoord.x / viewportWidth, fragTexCoord.y / viewportHeight);\n"
            "    vec4 a = texture2D(tex, fragTexCoord);\n"
            "    vec4 b = texture2D(tex2, fragTexCoord);\n"
            "    gl_FragColor = (a * (1.0 - b.a)) + (b * b.a);\n"
            "    gl_FragColor.a = 1.0;\n"
            "}\n";
#else
    static const char *fragmentShaderSource =
            "uniform lowp sampler2D tex;\n"
            "uniform lowp sampler2D tex2;\n"
            "varying vec2 fragTexCoord;\n"
//            "uniform lowp float viewportWidth;\n"
//            "uniform lowp float viewportHeight;\n"
            "void main(void)\n"
            "{\n"
//            "    vec2 viewportTexCoord = vec2(fragTexCoord.x / viewportWidth, fragTexCoord.y / viewportHeight);\n"
            "    vec4 a = texture2D(tex, fragTexCoord);\n"
            "    vec4 b = texture2D(tex2, fragTexCoord);\n"
            "    gl_FragColor = (a * (1.0 - b.a)) + (b * b.a);\n"
            "    gl_FragColor.a = 1.0;\n"
            "}\n";
#endif

    m_program = new QOpenGLShaderProgram;
    m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->bindAttributeLocation("vertexIn", CanonicRenderer::TEXTURE_001_ATTRIB);
    m_program->bindAttributeLocation("vertTexCoord", CanonicRenderer::TEXTURE_002_ATTRIB);
    m_program->link();

    f->glUseProgram(m_program->programId());

    m_program->setUniformValue("tex", 0);
    m_program->setUniformValue("tex2", 1);

    int t1 = f->glGetUniformLocation(m_program->programId(), "tex");
    int t2 = f->glGetUniformLocation(m_program->programId(), "tex2");

    f->glUniform1i(t1, CanonicRenderer::TEXTURE_001_ATTRIB);
    f->glUniform1i(t2, CanonicRenderer::TEXTURE_002_ATTRIB);

    m_vao = new QOpenGLVertexArrayObject;
    m_vao->create();
    QOpenGLVertexArrayObject::Binder vaoBinder(m_vao);

    m_vbo = new QOpenGLBuffer;
    m_vbo->create();
    m_vbo->bind();

    GLfloat v[] = {-1.0,-1.0, 1.0,-1.0, 1.0,1.0,
                   1.0,1.0, -1.0,1.0, -1.0,-1.0};

    GLfloat texCoords[] = {0.0,0.0, 1.0,0.0, 1.0,1.0,
                           1.0,1.0, 0.0,1.0, 0.0,0.0};

    const int vertexCount = 6;
    m_vbo->allocate(sizeof(GLfloat) * vertexCount * 4);
    m_vbo->write(0, v, sizeof(GLfloat) * vertexCount * 2);
    m_vbo->write(sizeof(GLfloat) * vertexCount * 2, texCoords, sizeof(GLfloat) * vertexCount * 2);
    m_vbo->release();

    if (m_vao->isCreated())
        setupVertexAttribs();
}

void CanonicRenderer::setupVertexAttribs()
{
    m_vbo->bind();
    m_program->enableAttributeArray(0);
    m_program->enableAttributeArray(1);
    m_context->functions()->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    m_context->functions()->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0,
                                                  (const void *)(6 * 2 * sizeof(GLfloat)));
    m_vbo->release();
}

void CanonicRenderer::render(QWindow *w,
                             QRectF normalisedViewportGeometry,
                             uint texture,
                             uint texture2)
{
    if (!m_context)
        init(w);

    if (!m_context->makeCurrent(w))
        return;

    if (texture and texture2) {
        QOpenGLFunctions *f = m_context->functions();
        f->glViewport(0, 0, w->width() * w->devicePixelRatio(), w->height() * w->devicePixelRatio());
        f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        f->glEnable(GL_CULL_FACE);
        f->glEnable(GL_DEPTH_TEST);

        m_program->bind();

        //m_program->setUniformValue("viewportWidth", (float) normalisedViewportGeometry.width());
        //m_program->setUniformValue("viewportHeight", (float) normalisedViewportGeometry.height());

        QOpenGLVertexArrayObject::Binder vaoBinder(m_vao);

        // If VAOs are not supported, set the vertex attributes every time.
        if (!m_vao->isCreated())
            setupVertexAttribs();

        f->glActiveTexture(GL_TEXTURE0 + 0);
        f->glBindTexture(GL_TEXTURE_2D, texture);
        f->glActiveTexture(GL_TEXTURE0 + 1);
        f->glBindTexture(GL_TEXTURE_2D, texture2);

        // Draw the verticies.
        f->glDrawArrays(GL_TRIANGLES, 0, 6);

        f->glBindTexture(GL_TEXTURE_2D, 0);

        m_context->swapBuffers(w);
    }
}
