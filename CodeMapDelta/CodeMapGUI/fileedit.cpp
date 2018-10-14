#include "fileedit.h"

#include <QMenu>
#include <QContextMenuEvent>

#include <QPainter>
#include <QTextBlock>
#include <QtConcurrent>

#include "mainwindow.h"
#include "codeparser.h"
#include "linenumberarea.h"
#include "fileview.h"

FileEdit::FileEdit(QWidget* parent) : QPlainTextEdit(parent)
{
	lineNumberArea = new LineNumberArea(this);
	m_regionFolder = new FoldableTextArea(this, "ASD");

	document()->documentLayout()->registerHandler(m_regionFolder->type(), m_regionFolder);

    this->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
    this->setFont(QFont("Consolas"));
	
	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateLineNumberArea(QRect, int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

	connect(&foldWatcher, SIGNAL(finished()), this, SLOT(foldDefinesFinished()));

	updateLineNumberAreaWidth(0);
	highlightCurrentLine();
}

void FileEdit::fold() {
	auto c = textCursor();
	c.setPosition(10);
	c.setPosition(20, QTextCursor::KeepAnchor);
	m_regionFolder->fold(c);
}

void FileEdit::unfold() {
	auto c = textCursor();
	c.setPosition(11);
	m_regionFolder->unfold(c);
}

void FileEdit::contextMenuEvent(QContextMenuEvent* event)
{
	// TODO: put check to centext menu disable action if not available
	// TODO: write help in context menu, show what is missing
	// in case action is not available
    std::unique_ptr<QMenu> menu(createStandardContextMenu());
    menu->addAction(tr("Fold defines"), this, &FileEdit::foldDefines);
    menu->exec(event->globalPos());
}

void FileEdit::foldDefines()
{
    if(m_FilePath == "")
        return;
	auto mw = MainWindow::instance();
    const auto& terminal = mw->getTerminalView();
    terminal->showMessage(tr("Folding defines for %1").arg(m_FilePath));

	QString name = tr("Folded defines for: %1").arg(m_FilePath);
	m_FoldedFileView = MainWindow::instance()->getDocumentManager()->openStringFileView(name, "Loading...");

	auto foldFuture = QtConcurrent::run(this, &FileEdit::foldDefinesForFile, m_FilePath);
	foldWatcher.setFuture(foldFuture);
}

void FileEdit::foldDefinesFinished()
{
	QString result = foldWatcher.future().result();
	m_FoldedFileView->setText(result);
}

QString FileEdit::foldDefinesForFile(const QString& filePath) const
{
	auto mw = MainWindow::instance();
	const auto& terminal = mw->getTerminalView();
	auto& appState = mw->getAppState();

	std::vector<QString> includes;
	for(auto& i : appState.settings().globalIncludes)
	{
		includes.emplace_back(i);
	}
	auto processed = cm::CodeParser().getPreprocessedCodeFromPath(filePath, includes);

	if(processed.hasErrors())
	{
		terminal->showMessage(tr("Had %1 errors:").arg(processed.errors.size()));
		for(auto& e : processed.errors)
		{
			terminal->showMessage(e);
		}
	}
	return processed.code.content;
}

void FileEdit::setFilePath(const QString& path)
{
	m_FilePath = path;
}

// Line numbering things

void FileEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
	QPainter painter(lineNumberArea);
	painter.fillRect(event->rect(), Qt::lightGray);

	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int)blockBoundingRect(block).height();
	
	const int rightMargin = 2;

	while(block.isValid() && top <= event->rect().bottom()) {
		if(block.isVisible() && bottom >= event->rect().top()) {
			QString number = QString::number(blockNumber + 1);
			painter.setPen(Qt::black);
			painter.drawText(0, top, lineNumberArea->width() - rightMargin, fontMetrics().height(),
				Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + (int)blockBoundingRect(block).height();
		++blockNumber;
	}
}

int FileEdit::lineNumberAreaWidth()
{
	int digits = 1;
	int max = qMax(1, blockCount());
	while(max >= 10) {
		max /= 10;
		++digits;
	}

	int space = 3 + fontMetrics().width(QLatin1Char('9'))*digits;

	return space;
}

void FileEdit::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void FileEdit::updateLineNumberAreaWidth(int newBlockCount)
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void FileEdit::highlightCurrentLine()
{
	QList<QTextEdit::ExtraSelection> extraSelections;

	if(!isReadOnly()) {
		QTextEdit::ExtraSelection selection;

		QColor lineColor = QColor(Qt::green).darker(150);

		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
	}

	setExtraSelections(extraSelections);
}

void FileEdit::updateLineNumberArea(const QRect &rect, int dy)
{
	if(dy)
		lineNumberArea->scroll(0, dy);
	else
		lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

	if(rect.contains(viewport()->rect()))
		updateLineNumberAreaWidth(0);
}
