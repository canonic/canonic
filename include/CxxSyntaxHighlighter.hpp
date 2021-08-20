#pragma once

#include <QObject>
#include <QTextDocument>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QQuickTextDocument>

/**
 * Based on a mashup of code from:
 * https://code.qt.io/cgit/qt/qtbase.git/tree/examples/widgets/richtext/syntaxhighlighter?h=5.15
 * https://github.com/stephenquan/QtSyntaxHighlighterApp
 **/

class CxxSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

    Q_PROPERTY(QQuickTextDocument* textDocument READ textDocument WRITE setTextDocument NOTIFY textDocumentChanged)

  public:
    CxxSyntaxHighlighter(QObject* parent = nullptr);

  signals:
    void textDocumentChanged();

  protected:
    QQuickTextDocument* m_TextDocument;
    QQuickTextDocument* textDocument() const;
    void setTextDocument(QQuickTextDocument* textDocument);

    virtual void highlightBlock(const QString &text);


  private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat typeFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};
