#include "../include/CxxSyntaxHighlighter.hpp"

#include <QColor>

CxxSyntaxHighlighter::CxxSyntaxHighlighter( QObject* parent ) :
                                                        QSyntaxHighlighter(parent),
                                                        m_TextDocument(nullptr)
{
    HighlightingRule rule;

    keywordFormat.setForeground(QColor("#fe1c82"));
    keywordFormat.setFontWeight(QFont::Bold);
    const QString keywordPatterns[] = {
        QStringLiteral("\\bconst\\b"),
        QStringLiteral("\\breturn\\b"),
        QStringLiteral("\\bthis\\b"),
        QStringLiteral("\\bexplicit\\b"),
        QStringLiteral("\\bfriend\\b"), QStringLiteral("\\binline\\b"),
        QStringLiteral("\\boperator\\b"),
        QStringLiteral("\\bprivate\\b"), QStringLiteral("\\bprotected\\b"), QStringLiteral("\\bpublic\\b"),
        QStringLiteral("\\bsignals\\b"), QStringLiteral("\\bsigned\\b"),
        QStringLiteral("\\bslots\\b"), QStringLiteral("\\bstatic\\b"),
        QStringLiteral("\\btemplate\\b"), QStringLiteral("\\btypedef\\b"), QStringLiteral("\\btypename\\b"),
        QStringLiteral("\\bunsigned\\b"), QStringLiteral("\\bvirtual\\b"),
        QStringLiteral("\\bvolatile\\b")
    };
    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    typeFormat.setForeground(QColor("#1cf3fe"));
    typeFormat.setFontWeight(QFont::Bold);
    const QString typePatterns[] = {
        QStringLiteral("\\bchar\\b"), QStringLiteral("\\bclass\\b"),
        QStringLiteral("\\bdouble\\b"), QStringLiteral("\\benum\\b"),
        QStringLiteral("\\bint\\b"),
        QStringLiteral("\\blong\\b"), QStringLiteral("\\bnamespace\\b"),
        QStringLiteral("\\bshort\\b"),
        QStringLiteral("\\bstruct\\b"),
        QStringLiteral("\\bunion\\b"),
        QStringLiteral("\\bvoid\\b"), QStringLiteral("\\bbool\\b")
    };
    for (const QString &pattern : typePatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = typeFormat;
        highlightingRules.append(rule);
    }

    // class
    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(QColor("#fe1c82"));
    rule.pattern = QRegularExpression(QStringLiteral("\\bQ[A-Za-z]+\\b"));
    rule.format = classFormat;
    highlightingRules.append(rule);

    // single line comment
    singleLineCommentFormat.setForeground(QColor("#666666"));
    rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    // multiline
    multiLineCommentFormat.setForeground(QColor("#666666"));

    // string
    quotationFormat.setForeground(QColor("#E0DF73"));
    rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    // function
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(QColor("#1cfe98"));
    rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
    rule.format = functionFormat;
    highlightingRules.append(rule);

    // comment start / end
    commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
}

void CxxSyntaxHighlighter::highlightBlock( const QString &text )
{

    for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}

QQuickTextDocument* CxxSyntaxHighlighter::textDocument() const
{
    return m_TextDocument;
}

void CxxSyntaxHighlighter::setTextDocument( QQuickTextDocument* textDocument )
{
    if (textDocument == m_TextDocument)
    {
        return;
    }

    m_TextDocument = textDocument;

    QTextDocument* doc = m_TextDocument->textDocument();
    setDocument(doc);

    emit textDocumentChanged();
}
