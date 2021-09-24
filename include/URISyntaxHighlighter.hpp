#pragma once

#include <QObject>
#include <QTextDocument>
#include <QTextCharFormat>
#include <QSyntaxHighlighter>
#include <QQuickTextDocument>

/**
 * Based on a mashup of code from:
 * https://code.qt.io/cgit/qt/qtbase.git/tree/examples/widgets/richtext/syntaxhighlighter?h=5.15
 * https://github.com/stephenquan/QtSyntaxHighlighterApp
 **/
class URISyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

    Q_PROPERTY(QQuickTextDocument* textDocument READ textDocument WRITE setTextDocument NOTIFY textDocumentChanged)
    Q_PROPERTY(bool isFocused READ isFocused WRITE setIsFocused NOTIFY isFocusedChanged)

    Q_PROPERTY(QColor defaultColor READ getDefaultColor WRITE setDefaultColor NOTIFY defaultColorChanged REQUIRED)
    Q_PROPERTY(QColor highlightForgroundColor READ getHighlightForgroundColor WRITE setHighlightForgroundColor NOTIFY highlightForgroundColorChanged REQUIRED)

  public:
    URISyntaxHighlighter(QObject* parent = nullptr);

    bool isFocused() const;
    void setIsFocused(bool isFocused);

    QColor getDefaultColor() const;
    void setDefaultColor(QColor defaultColor);
    QColor getHighlightForgroundColor() const;
    void setHighlightForgroundColor(QColor highlightForgroundColor);

  signals:
    void textDocumentChanged();
    void isFocusedChanged();

    void defaultColorChanged();
    void highlightForgroundColorChanged();

  protected:
    QQuickTextDocument* m_TextDocument;
    QQuickTextDocument* textDocument() const;
    void setTextDocument(QQuickTextDocument* textDocument);

    virtual void highlightBlock(const QString &text);


  private:
    bool m_isFocused{false};

    QTextCharFormat defaultFormat;
    QTextCharFormat highlightFormat;
    QTextCharFormat metaSchemeFormat;

    // focused formats
    QTextCharFormat metaMethodFormatFocused;
    QTextCharFormat metaSubMethodFormatFocused;
    QTextCharFormat metaCategoryFormatFocused;

    // un-focused formats
    QTextCharFormat metaMethodFormat;
    QTextCharFormat metaSubMethodFormat;
    QTextCharFormat metaCategoryFormat;
};
