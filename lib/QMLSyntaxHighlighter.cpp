#include "../include/QMLSyntaxHighlighter.hpp"

#include <QColor>


QMLSyntaxHighlighter::QMLSyntaxHighlighter( QObject* parent ) : QSyntaxHighlighter(parent),
    m_TextDocument(nullptr)
{
    HighlightingRule rule;

    keywordFormat.setForeground(QColor("#1c9bfe"));
    keywordFormat.setFontWeight(QFont::Bold);

    const QString keywordPatterns[] = {
        QStringLiteral("\\balias\\b"),
        QStringLiteral("\\barguments\\b"),
        QStringLiteral("\\bawait\\b"),
        QStringLiteral("\\bbreak\\b"),
        QStringLiteral("\\bcase\\b"),
        QStringLiteral("\\bclass\\b"),
        QStringLiteral("\\bconst\\b"),
        QStringLiteral("\\bcontinue\\b"),
        QStringLiteral("\\bdebugger\\b"),
        QStringLiteral("\\bdefault\\b"),
        QStringLiteral("\\bdelete\\b"),
        QStringLiteral("\\bdo\\b"),
        QStringLiteral("\\belse\\b"),
        QStringLiteral("\\beval\\b"),
        QStringLiteral("\\bexport\\b"),
        QStringLiteral("\\bextends\\b"),
        QStringLiteral("\\bextends\\b"),
        QStringLiteral("\\bfinally\\b"),
        QStringLiteral("\\bfor\\b"),
        QStringLiteral("\\bfunction\\b"),
        QStringLiteral("\\bif\\b"),
        QStringLiteral("\\bimplements\\b"),
        QStringLiteral("\\bimport\\b"),
        QStringLiteral("\\bin\\b"),
        QStringLiteral("\\binstanceof\\b"),
        QStringLiteral("\\binterface\\b"),
        QStringLiteral("\\blet\\b"),
        QStringLiteral("\\bnew\\b"),
        QStringLiteral("\\bpackage\\b"),
        QStringLiteral("\\bprivate\\b"),
        QStringLiteral("\\bprotected\\b"),
        QStringLiteral("\\bpublic\\b"),
        QStringLiteral("\\bproperty\\b"),
        QStringLiteral("\\breadonly\\b"),
        QStringLiteral("\\breturn\\b"),
        QStringLiteral("\\brequired\\b"),
        QStringLiteral("\\bsignal\\b"),
        QStringLiteral("\\bslots\\b"),
        QStringLiteral("\\bstatic\\b"),
        QStringLiteral("\\bsuper\\b"),
        QStringLiteral("\\bswitch\\b"),
        QStringLiteral("\\bsynchronized\\b"),
        QStringLiteral("\\bthis\\b"),
        QStringLiteral("\\bthrow\\b"),
        QStringLiteral("\\btry\\b"),
        QStringLiteral("\\btypeof\\b"),
        QStringLiteral("\\bvar\\b"),
        QStringLiteral("\\bvoid\\b"),
        QStringLiteral("\\bwhile\\b"),
        QStringLiteral("\\bwith\\b"),
        QStringLiteral("\\byield\\b"),
    };

    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    valueFormat.setForeground(QColor("#bd93f9"));
    valueFormat.setFontWeight(QFont::Bold);

    const QString valuePatterns[] = {
        QStringLiteral("\\bfalse\\b"),
        QStringLiteral("\\btrue\\b"),
        QStringLiteral("\\bNaN\\b"),
        QStringLiteral("\\bnull\\b"),
        QStringLiteral("\\bInfinity\\b"),
        QStringLiteral("\\bundefined\\b"),
        QStringLiteral("^[+-]?(\\d*\\.)?\\d+$"),
    };

    for (const QString &pattern : valuePatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = valueFormat;
        highlightingRules.append(rule);
    }

    typeFormat.setForeground(QColor("#1cf3fe"));
    typeFormat.setFontWeight(QFont::Bold);
    const QString typePatterns[] = {
        QStringLiteral("\\bbool\\b"),
        QStringLiteral("\\bdouble\\b"),
        QStringLiteral("\\benum\\b"),
        QStringLiteral("\\bint\\b"),
        QStringLiteral("\\blist\\b"),
        QStringLiteral("\\breal\\b"),
        QStringLiteral("\\bstring\\b"),
        QStringLiteral("\\burl\\b"),

        QStringLiteral("\\bdate\\b"),
        QStringLiteral("\\bpoint\\b"),
        QStringLiteral("\\brect\\b"),
        QStringLiteral("\\bsize\\b"),
    };
    for (const QString &pattern : typePatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = typeFormat;
        highlightingRules.append(rule);
    }

    // Component
    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(QColor("#fe1c82"));
    //rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z]+\\b"));
    rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Z].*?\\b"));
    rule.format = classFormat;
    highlightingRules.append(rule);

    // property
    propertyFormat.setForeground(QColor("#1cfe98"));
    rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_\.]+:"));
    rule.format = propertyFormat;
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
    rule.pattern = QRegularExpression(QStringLiteral("\".*\"|'.*'"));
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    // function
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(QColor("#febb1c"));
    rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
    rule.format = functionFormat;
    highlightingRules.append(rule);

    // comment start / end
    commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
}

void QMLSyntaxHighlighter::highlightBlock( const QString &text )
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

QQuickTextDocument* QMLSyntaxHighlighter::textDocument() const
{
    return m_TextDocument;
}


void QMLSyntaxHighlighter::setTextDocument( QQuickTextDocument* textDocument )
{
    if (textDocument == this->m_TextDocument)
    {
        return;
    }

    this->m_TextDocument = textDocument;

    QTextDocument* doc = this->m_TextDocument->textDocument();
    setDocument(doc);

    emit textDocumentChanged();
}

