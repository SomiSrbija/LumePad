#ifndef LUMEPADMAINWINDOW_H
#define LUMEPADMAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include "codeeditor.h"

class LumePadMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit LumePadMainWindow(QWidget *parent = nullptr);
    ~LumePadMainWindow();

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs(); // Slot with no parameters
    void closeCurrentTab();
    void documentModified();
    void findText();
    void replaceText();
    void about();

    // New feature slots:
    void goToLine();
    void zoomIn();
    void zoomOut();
    void toggleWordWrap();
    void changeFont();

    // Additional new features:
    void toggleLineNumbers();
    void toggleFullScreen();
    void openRecentFile();

private:
    void createActions();
    void createMenus();
    void createStatusBar();
    CodeEditor* currentEditor();
    void loadFile(const QString &fileName);
    bool saveFileAsHelper(CodeEditor *editor, const QString &fileName = QString());
    bool maybeSave(CodeEditor *editor);

    void updateRecentFilesMenu();

    QTabWidget *tabWidget;

    // Menus
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;
    QMenu *recentFilesMenu;

    // Actions
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *closeTabAct;
    QAction *exitAct;
    QAction *undoAct;
    QAction *redoAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *findAct;
    QAction *replaceAct;
    QAction *selectAllAct;
    QAction *aboutAct;

    // New actions for additional features
    QAction *goToLineAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *toggleWrapAct;
    QAction *changeFontAct;
    QAction *toggleLineNumbersAct;
    QAction *toggleFullScreenAct;

    // Recent Files list
    QList<QString> m_recentFiles;
};

#endif // LUMEPADMAINWINDOW_H
