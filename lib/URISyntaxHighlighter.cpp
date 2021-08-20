#include "../include/URISyntaxHighlighter.hpp"

#include <QColor>

URISyntaxHighlighter::URISyntaxHighlighter( QObject* parent ) :
                                                              QSyntaxHighlighter(parent),
                                                              m_TextDocument(nullptr)
{
    defaultFormat.setForeground(QColor("#c2c2c3"));
    highlightFormat.setForeground(QColor("#ffffff"));
    metaSchemeFormat.setForeground(QColor("#1cfe98"));

    metaMethodFormatFocused.setForeground(highlightFormat.foreground());
    metaSubMethodFormatFocused.setForeground(highlightFormat.foreground());
    metaCategoryFormatFocused.setForeground(highlightFormat.foreground());

    metaMethodFormat.setForeground(defaultFormat.foreground());
    metaSubMethodFormat.setForeground(defaultFormat.foreground());
    metaCategoryFormat.setForeground(highlightFormat.foreground());
}

bool URISyntaxHighlighter::isFocused() const
{
    return m_isFocused;
}

void URISyntaxHighlighter::setIsFocused(bool isFocused)
{
    m_isFocused = isFocused;
    emit this->isFocusedChanged();
}

QColor URISyntaxHighlighter::getDefaultColor() const
{
    return this->defaultFormat.foreground().color();
}

void URISyntaxHighlighter::setDefaultColor(QColor defaultColor)
{
    this->defaultFormat.setForeground(defaultColor);
    emit this->defaultColorChanged();
}

QColor URISyntaxHighlighter::getHighlightForgroundColor() const
{
    return this->defaultFormat.foreground().color();
}

void URISyntaxHighlighter::setHighlightForgroundColor(QColor highlightForgroundColor)
{
    this->highlightFormat.setForeground(highlightForgroundColor);
    emit this->highlightForgroundColorChanged();
}

void URISyntaxHighlighter::highlightBlock( const QString &text )
{
    setFormat(0, text.length(), defaultFormat);

    QString metaScheme = "meta:";
    int textLength = text.length();
    int currectPos = 0;
    if (text.startsWith(metaScheme))
    {
        setFormat(currectPos, metaScheme.length(), metaSchemeFormat);
        currectPos += metaScheme.length();

        if(textLength > metaScheme.length()){
            QStringRef subString(&text, currectPos, textLength - currectPos);

            QString method = "compare";
            int methodLength = method.length();
            if(subString.startsWith(method))
            {
                setFormat(currectPos,
                    methodLength,
                    this->isFocused()?metaMethodFormatFocused : metaMethodFormat);

                currectPos += methodLength;
                subString = QStringRef(&text, currectPos, textLength - currectPos);

                QString subMethod = " category";
                int subMethodLength = subMethod.length();

                if(subString.startsWith(subMethod))
                {
                    setFormat(currectPos,
                        subMethodLength,
                        this->isFocused()?metaSubMethodFormatFocused : metaSubMethodFormat);

                    currectPos += subMethodLength;
                    subString = QStringRef(&text, currectPos, textLength - currectPos);

                    if (subString.startsWith(" "))
                    {
                        setFormat(currectPos,
                            subString.length(),
                            this->isFocused()?metaCategoryFormatFocused : metaCategoryFormat);
                    }
                }
            }
        }
    }
    else
    {
        if(!this->isFocused())
        {
            QUrl url{text};
            QString authority = url.authority();

            if(!authority.isEmpty())
            {
                int start = text.indexOf(authority);
                if(start >= 0)
                {
                    setFormat(start, authority.length(), highlightFormat);
                    return;
                }
            }
        }

        setFormat(0, text.length(), highlightFormat);
    }
}

QQuickTextDocument* URISyntaxHighlighter::textDocument() const
{
    return m_TextDocument;
}

void URISyntaxHighlighter::setTextDocument( QQuickTextDocument* textDocument )
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
