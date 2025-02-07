#include "codeeditor.h"
#include <QPainter>
#include <QTextBlock>
#include <QResizeEvent>
#include <algorithm>

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, &QPlainTextEdit::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

int CodeEditor::lineNumberAreaWidth()
{
    if (!m_showLineNumbers)
        return 0;
    int digits = 1;
    int max = std::max(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int)
{
    if(m_showLineNumbers)
        setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
    else
        setViewportMargins(0, 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    if (!m_showLineNumbers)
        return;
    QPainter painter(lineNumberArea);

    // Use the editor's base background color for the margin so it blends in.
    QColor backgroundColor = this->palette().color(QPalette::Base);
    painter.fillRect(event->rect(), backgroundColor);

    // Draw a subtle vertical separator line on the right edge.
    QPen separatorPen(Qt::lightGray);
    painter.setPen(separatorPen);
    painter.drawLine(event->rect().topRight(), event->rect().bottomRight());

    // Set the pen for drawing line numbers (using the editor's text color).
    QColor textColor = this->palette().color(QPalette::Text);
    painter.setPen(textColor);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = static_cast<int>(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + static_cast<int>(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.drawText(0, top, lineNumberArea->width()-4, fontMetrics().height(),
                             Qt::AlignRight, number);
        }
        block = block.next();
        top = bottom;
        bottom = top + static_cast<int>(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        // Use a semi-transparent light blue for the current line background.
        QColor lineColor = QColor(232, 232, 255, 100);
        selection.format.setBackground(lineColor);
        // Notice we do NOT use FullWidthSelection so that only the text’s background is tinted.
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }
    setExtraSelections(extraSelections);
}

void CodeEditor::setLineNumbersVisible(bool visible)
{
    m_showLineNumbers = visible;
    updateLineNumberAreaWidth(0);
    lineNumberArea->update();
}

bool CodeEditor::lineNumbersVisible() const
{
    return m_showLineNumbers;
}
