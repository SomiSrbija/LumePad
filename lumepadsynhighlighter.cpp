#include "lumepadsynhighlighter.h"
#include <QFont>

LumePadSyntaxHighlighter::LumePadSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    // Keyword patterns (a simple C++ list)
    QStringList keywordPatterns = {
        "\\bchar\\b", "\\bclass\\b", "\\bconst\\b", "\\bdouble\\b", "\\benum\\b",
        "\\bexplicit\\b", "\\bfriend\\b", "\\binline\\b", "\\bint\\b", "\\blong\\b",
        "\\bnamespace\\b", "\\boperator\\b", "\\bprivate\\b", "\\bprotected\\b",
        "\\bpublic\\b", "\\bshort\\b", "\\bsignals\\b", "\\bsigned\\b", "\\bslots\\b",
        "\\bstatic\\b", "\\bstruct\\b", "\\btemplate\\b", "\\btypedef\\b", "\\btypename\\b",
        "\\bunion\\b", "\\bunsigned\\b", "\\bvirtual\\b", "\\bvoid\\b", "\\bvolatile\\b"
    };

    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);
    for (const QString &pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // Single-line comments (//...)
    singleLineCommentFormat.setForeground(Qt::darkGreen);
    HighlightingRule rule;
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    // Quoted strings ("...")
    quotationFormat.setForeground(Qt::darkRed);
    rule.pattern = QRegularExpression("\".*\""); // a simple string pattern
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    // Function names (simplified: any word followed by an opening parenthesis)
    functionFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    // Multi-line comments (/* ... */)
    multiLineCommentFormat.setForeground(Qt::darkGreen);
    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");
}

void LumePadSyntaxHighlighter::highlightBlock(const QString &text)
{
    // Apply all single-line highlighting rules
    for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // Handle multi-line comments
    setCurrentBlockState(0);
    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch endMatch;
        int endIndex = text.indexOf(commentEndExpression, startIndex, &endMatch);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + endMatch.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
