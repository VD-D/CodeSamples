#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTextEdit;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * Constructor/destructor.
     */
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    /**
     * @return Text edit widget with all text.
     */
    QTextEdit* GetTextEditWidget() const;

private:
    /**
     * Creates new file.
     */
    void OnActionNew();

    /**
     * Opens a text file.
     */
    void OnActionOpen();

    /**
     * Saves file.
     */
    void OnActionSave();

    /**
     * Saves file with location dialogue.
     */
    void OnActionSaveAs();

    /**
     * Cuts highlighted text
     */
    void OnActionCut();

    /**
     * Copies highlighted text
     */
    void OnActionCopy();

    /**
     * Pastes copied text.
     */
    void OnActionPaste();

    /**
     * Undoes last text edit.
     */
    void OnActionUndo();

    /**
     * Redos undone text edit
     */
    void OnActionRedo();

    /**
     * Opens find and replace dialog
     */
    void OnActionFindReplace();

private:
    /**
     * Internal function for saving.
     * @param myFilePath Path to save to.
     */
    void SaveInternal(const QString &myFilePath);

    /* Pointer to UI. */
    Ui::MainWindow *ui;

    /* Current file path */
    QString filePath;
};
#endif // MAINWINDOW_H
