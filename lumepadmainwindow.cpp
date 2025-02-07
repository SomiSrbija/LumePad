#include "lumepadmainwindow.h"
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QTextStream>
#include <QInputDialog>
#include <QFileInfo>
#include <QFontDialog>
#include "lumepadsynhighlighter.h" // Ensure the filename matches exactly
#include <QIcon>

LumePadMainWindow::LumePadMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Create a tab widget for multiple documents
    tabWidget = new QTabWidget;
    tabWidget->setTabsClosable(true);
    setCentralWidget(tabWidget);
    setWindowIcon(QIcon(":/icons/lumepad_logo.png"));

    // When a tab’s close button is clicked, try to save changes
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, [=](int index) {
        CodeEditor *editor = qobject_cast<CodeEditor*>(tabWidget->widget(index));
        if (editor) {
            if (maybeSave(editor)) {
                tabWidget->removeTab(index);
                editor->deleteLater();
            }
        }
    });

    createActions();
    createMenus();
    createStatusBar();

    newFile(); // Start with one new file open

    setWindowTitle("LumePad");
    resize(800, 600);

}

LumePadMainWindow::~LumePadMainWindow()
{
}

void LumePadMainWindow::createActions()
{
    newAct = new QAction("&New", this);
    newAct->setShortcuts(QKeySequence::New);
    connect(newAct, &QAction::triggered, this, &LumePadMainWindow::newFile);

    openAct = new QAction("&Open...", this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, &QAction::triggered, this, &LumePadMainWindow::openFile);

    saveAct = new QAction("&Save", this);
    saveAct->setShortcuts(QKeySequence::Save);
    connect(saveAct, &QAction::triggered, this, &LumePadMainWindow::saveFile);

    saveAsAct = new QAction("Save &As...", this);
    connect(saveAsAct, &QAction::triggered, this, &LumePadMainWindow::saveFileAs);

    closeTabAct = new QAction("&Close Tab", this);
    connect(closeTabAct, &QAction::triggered, this, &LumePadMainWindow::closeCurrentTab);

    exitAct = new QAction("E&xit", this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    undoAct = new QAction("&Undo", this);
    undoAct->setShortcuts(QKeySequence::Undo);
    connect(undoAct, &QAction::triggered, [=](){
        if (currentEditor()) currentEditor()->undo();
    });

    redoAct = new QAction("&Redo", this);
    redoAct->setShortcuts(QKeySequence::Redo);
    connect(redoAct, &QAction::triggered, [=](){
        if (currentEditor()) currentEditor()->redo();
    });

    cutAct = new QAction("Cu&t", this);
    cutAct->setShortcuts(QKeySequence::Cut);
    connect(cutAct, &QAction::triggered, [=](){
        if (currentEditor()) currentEditor()->cut();
    });

    copyAct = new QAction("&Copy", this);
    copyAct->setShortcuts(QKeySequence::Copy);
    connect(copyAct, &QAction::triggered, [=](){
        if (currentEditor()) currentEditor()->copy();
    });

    pasteAct = new QAction("&Paste", this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    connect(pasteAct, &QAction::triggered, [=](){
        if (currentEditor()) currentEditor()->paste();
    });

    findAct = new QAction("&Find", this);
    findAct->setShortcuts(QKeySequence::Find);
    connect(findAct, &QAction::triggered, this, &LumePadMainWindow::findText);

    replaceAct = new QAction("&Replace", this);
    replaceAct->setShortcuts(QKeySequence::Replace);
    connect(replaceAct, &QAction::triggered, this, &LumePadMainWindow::replaceText);

    selectAllAct = new QAction("Select &All", this);
    selectAllAct->setShortcuts(QKeySequence::SelectAll);
    connect(selectAllAct, &QAction::triggered, [=](){
        if (currentEditor()) currentEditor()->selectAll();
    });

    aboutAct = new QAction("&About", this);
    connect(aboutAct, &QAction::triggered, this, &LumePadMainWindow::about);

    // New actions for additional features
    goToLineAct = new QAction("Go to &Line...", this);
    connect(goToLineAct, &QAction::triggered, this, &LumePadMainWindow::goToLine);

    zoomInAct = new QAction("Zoom &In", this);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    connect(zoomInAct, &QAction::triggered, this, &LumePadMainWindow::zoomIn);

    zoomOutAct = new QAction("Zoom &Out", this);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    connect(zoomOutAct, &QAction::triggered, this, &LumePadMainWindow::zoomOut);

    toggleWrapAct = new QAction("Toggle &Word Wrap", this);
    connect(toggleWrapAct, &QAction::triggered, this, &LumePadMainWindow::toggleWordWrap);

    changeFontAct = new QAction("Change &Font...", this);
    connect(changeFontAct, &QAction::triggered, this, &LumePadMainWindow::changeFont);

    toggleLineNumbersAct = new QAction("Toggle &Line Numbers", this);
    connect(toggleLineNumbersAct, &QAction::triggered, this, &LumePadMainWindow::toggleLineNumbers);

    toggleFullScreenAct = new QAction("Toggle &Full Screen", this);
    connect(toggleFullScreenAct, &QAction::triggered, this, &LumePadMainWindow::toggleFullScreen);
}

void LumePadMainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(closeTabAct);
    fileMenu->addSeparator();
    // Recent Files submenu
    recentFilesMenu = fileMenu->addMenu("Recent Files");
    updateRecentFilesMenu();
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu("&Edit");
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addSeparator();
    editMenu->addAction(findAct);
    editMenu->addAction(replaceAct);
    editMenu->addSeparator();
    editMenu->addAction(selectAllAct);
    editMenu->addSeparator();
    editMenu->addAction(goToLineAct);

    viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(toggleWrapAct);
    viewMenu->addAction(changeFontAct);
    viewMenu->addAction(toggleLineNumbersAct);
    viewMenu->addAction(toggleFullScreenAct);

    helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(aboutAct);
}

void LumePadMainWindow::createStatusBar()
{
    statusBar()->showMessage("Ready");
}

CodeEditor* LumePadMainWindow::currentEditor()
{
    return qobject_cast<CodeEditor*>(tabWidget->currentWidget());
}

void LumePadMainWindow::newFile()
{
    CodeEditor *editor = new CodeEditor;
    new LumePadSyntaxHighlighter(editor->document());
    int index = tabWidget->addTab(editor, "Untitled");
    tabWidget->setCurrentIndex(index);
    connect(editor, &CodeEditor::textChanged, this, &LumePadMainWindow::documentModified);
}

void LumePadMainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "",
                                                    "Text Files (*.txt *.cpp *.h *.py);;All Files (*)");
    if (!fileName.isEmpty()) {
        loadFile(fileName);
    }
}

void LumePadMainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.exists()) {
        QMessageBox::warning(this, "LumePad", tr("File does not exist: %1").arg(fileName));
        return;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "LumePad",
                             tr("Cannot read file %1:\n%2.").arg(fileName, file.errorString()));
        return;
    }

    // Read the file as UTF-8 text
    QString text = QString::fromUtf8(file.readAll());
    file.close();

    CodeEditor *editor = new CodeEditor;
    // Attach syntax highlighter to the document.
    new LumePadSyntaxHighlighter(editor->document());
    // Set the loaded text into the editor.
    editor->setPlainText(text);
    int index = tabWidget->addTab(editor, QFileInfo(fileName).fileName());
    tabWidget->setCurrentIndex(index);
    // Store the file path as a property of the editor.
    editor->setProperty("filePath", fileName);
    connect(editor, &CodeEditor::textChanged, this, &LumePadMainWindow::documentModified);
    statusBar()->showMessage("File loaded", 2000);

    // Update the Recent Files list.
    m_recentFiles.removeAll(fileName);
    m_recentFiles.prepend(fileName);
    while (m_recentFiles.size() > 5)
        m_recentFiles.removeLast();
    updateRecentFilesMenu();
}

void LumePadMainWindow::saveFile()
{
    CodeEditor *editor = currentEditor();
    if (!editor)
        return;
    QVariant filePathVar = editor->property("filePath");
    QString fileName = filePathVar.toString();
    if (fileName.isEmpty()) {
        saveFileAs();
    } else {
        if (saveFileAsHelper(editor, fileName))
            statusBar()->showMessage("File saved", 2000);
    }
}

bool LumePadMainWindow::saveFileAsHelper(CodeEditor *editor, const QString &fileName)
{
    QString name = fileName;
    if (name.isEmpty()) {
        name = QFileDialog::getSaveFileName(this, "Save File As", "",
                                            "Text Files (*.txt *.cpp *.h *.py);;All Files (*)");
        if (name.isEmpty())
            return false;
    }
    QFile file(name);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "LumePad",
                             tr("Cannot write file %1:\n%2.").arg(name, file.errorString()));
        return false;
    }
    QTextStream out(&file);
    out << editor->toPlainText();
    editor->setProperty("filePath", name);
    tabWidget->setTabText(tabWidget->currentIndex(), QFileInfo(name).fileName());
    return true;
}

void LumePadMainWindow::saveFileAs()
{
    CodeEditor *editor = currentEditor();
    if (editor)
        saveFileAsHelper(editor, QString());
}

void LumePadMainWindow::closeCurrentTab()
{
    int currentIndex = tabWidget->currentIndex();
    if (currentIndex != -1) {
        CodeEditor *editor = qobject_cast<CodeEditor*>(tabWidget->widget(currentIndex));
        if (editor) {
            if (maybeSave(editor)) {
                tabWidget->removeTab(currentIndex);
                editor->deleteLater();
            }
        }
    }
}

bool LumePadMainWindow::maybeSave(CodeEditor *editor)
{
    if (editor->document()->isModified()) {
        QMessageBox::StandardButton ret = QMessageBox::warning(this, "LumePad",
                                                               "The document has been modified.\nDo you want to save your changes?",
                                                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save) {
            return saveFileAsHelper(editor, editor->property("filePath").toString());
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

void LumePadMainWindow::documentModified()
{
    CodeEditor *editor = currentEditor();
    if (editor) {
        int index = tabWidget->currentIndex();
        QString title = tabWidget->tabText(index);
        if (!title.endsWith("*"))
            tabWidget->setTabText(index, title + "*");
    }
}

void LumePadMainWindow::findText()
{
    CodeEditor *editor = currentEditor();
    if (!editor)
        return;
    bool ok;
    QString text = QInputDialog::getText(this, "Find", "Find:", QLineEdit::Normal, "", &ok);
    if (ok && !text.isEmpty()) {
        if (!editor->find(text)) {
            editor->moveCursor(QTextCursor::Start);
            if (!editor->find(text))
                QMessageBox::information(this, "LumePad", "Text not found.");
        }
    }
}

void LumePadMainWindow::replaceText()
{
    CodeEditor *editor = currentEditor();
    if (!editor)
        return;
    bool ok;
    QString findStr = QInputDialog::getText(this, "Replace", "Find:", QLineEdit::Normal, "", &ok);
    if (!ok || findStr.isEmpty())
        return;
    QString replaceStr = QInputDialog::getText(this, "Replace", "Replace with:", QLineEdit::Normal, "", &ok);
    if (!ok)
        return;

    editor->moveCursor(QTextCursor::Start);
    int count = 0;
    while (editor->find(findStr)) {
        editor->textCursor().insertText(replaceStr);
        ++count;
    }
    statusBar()->showMessage(QString("Replaced %1 occurrence(s)").arg(count), 2000);
}

void LumePadMainWindow::about()
{
    QMessageBox::about(this, "About LumePad",
                       "<b>LumePad</b> is a simple but effective text editor.<br>"
                       "It supports multiple tabs, syntax highlighting, and more.<br>"
                       "Made by SomiSrbija");
}

void LumePadMainWindow::goToLine()
{
    CodeEditor *editor = currentEditor();
    if (!editor)
        return;
    bool ok;
    int lineNumber = QInputDialog::getInt(this, "Go To Line", "Line number:",
                                          1, 1, editor->document()->blockCount(), 1, &ok);
    if (ok) {
        QTextBlock block = editor->document()->findBlockByNumber(lineNumber - 1);
        if (block.isValid()) {
            QTextCursor cursor(block);
            editor->setTextCursor(cursor);
            editor->centerCursor();
        } else {
            QMessageBox::information(this, "Go To Line", "Line not found.");
        }
    }
}

void LumePadMainWindow::zoomIn()
{
    CodeEditor *editor = currentEditor();
    if (editor)
        editor->zoomIn(1);
}

void LumePadMainWindow::zoomOut()
{
    CodeEditor *editor = currentEditor();
    if (editor)
        editor->zoomOut(1);
}

void LumePadMainWindow::toggleWordWrap()
{
    CodeEditor *editor = currentEditor();
    if (editor) {
        if (editor->lineWrapMode() == QPlainTextEdit::NoWrap)
            editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        else
            editor->setLineWrapMode(QPlainTextEdit::NoWrap);
    }
}

void LumePadMainWindow::changeFont()
{
    CodeEditor *editor = currentEditor();
    if (editor) {
        bool ok;
        QFont font = QFontDialog::getFont(&ok, editor->font(), this, "Select Font");
        if (ok)
            editor->setFont(font);
    }
}

void LumePadMainWindow::toggleLineNumbers()
{
    // Toggle line numbers for all open editors.
    bool visible = true;
    CodeEditor *editor = currentEditor();
    if (editor) {
        visible = !editor->lineNumbersVisible();
    }
    for (int i = 0; i < tabWidget->count(); ++i) {
        CodeEditor *ed = qobject_cast<CodeEditor*>(tabWidget->widget(i));
        if (ed)
            ed->setLineNumbersVisible(visible);
    }
}

void LumePadMainWindow::toggleFullScreen()
{
    if (isFullScreen())
        showNormal();
    else
        showFullScreen();
}

void LumePadMainWindow::updateRecentFilesMenu()
{
    recentFilesMenu->clear();
    for (const QString &fileName : m_recentFiles) {
        QAction *action = new QAction(fileName, this);
        action->setData(fileName);
        connect(action, &QAction::triggered, this, &LumePadMainWindow::openRecentFile);
        recentFilesMenu->addAction(action);
    }
}

void LumePadMainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (action) {
        QString fileName = action->data().toString();
        loadFile(fileName);
    }
}
