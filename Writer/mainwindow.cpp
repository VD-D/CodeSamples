#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "findreplace.h"

#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QShortcut>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(GetTextEditWidget());
    setWindowIcon(QIcon(":/icon/png/coffee.png"));
    setWindowTitle("Writer");

    // Connecting actions.
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::OnActionNew);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::OnActionOpen);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::OnActionSave);
    connect(ui->actionSave_As, &QAction::triggered, this, &MainWindow::OnActionSaveAs);
    connect(ui->actionCut, &QAction::triggered, this, &MainWindow::OnActionCut);
    connect(ui->actionCopy, &QAction::triggered, this, &MainWindow::OnActionCopy);
    connect(ui->actionPaste, &QAction::triggered, this, &MainWindow::OnActionPaste);
    connect(ui->actionUndo, &QAction::triggered, this, &MainWindow::OnActionUndo);
    connect(ui->actionRedo, &QAction::triggered, this, &MainWindow::OnActionRedo);
    connect(ui->actionFind_Replace, &QAction::triggered, this, &MainWindow::OnActionFindReplace);

    // Shortcut actions
    QShortcut* saveShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_S), this);
    connect(saveShortcut, &QShortcut::activated, this, &MainWindow::OnActionSave);
    QShortcut* saveAsShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_X), this);
    connect(saveAsShortcut, &QShortcut::activated, this, &MainWindow::OnActionSaveAs);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QTextEdit* MainWindow::GetTextEditWidget() const
{
    return ui != nullptr ? ui->textEdit : nullptr;
}

void MainWindow::OnActionNew()
{
    OnActionSave();
    if (QTextEdit *TextEdit = GetTextEditWidget())
    {
        TextEdit->setText("");
    }
}

void MainWindow::OnActionOpen()
{
    QString newFilePath = QFileDialog::getOpenFileName(this, "Open File");
    filePath = newFilePath;

    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Writer", "File not opened");
        return;
    }

    if (GetTextEditWidget() != nullptr)
    {
        QTextStream stream(&file);
        GetTextEditWidget()->setText(stream.readAll());
    }

    QFileInfo fileInfo(file);
    setWindowTitle(QString("Writer - ") + fileInfo.fileName());

    file.close();
}

void MainWindow::OnActionSave()
{
    QFileInfo checkFile(filePath);
    if (checkFile.exists() && checkFile.isFile())
    {
        SaveInternal(filePath);
    }
    else
    {
        OnActionSaveAs();
    }
}

void MainWindow::OnActionSaveAs()
{
    SaveInternal(QFileDialog::getSaveFileName(this, "Save File"));
}

void MainWindow::OnActionCut()
{
    if (GetTextEditWidget() != nullptr) GetTextEditWidget()->cut();
}

void MainWindow::OnActionCopy()
{
    if (GetTextEditWidget() != nullptr) GetTextEditWidget()->copy();
}

void MainWindow::OnActionPaste()
{
    if (GetTextEditWidget() != nullptr) GetTextEditWidget()->paste();
}

void MainWindow::OnActionUndo()
{
    if (GetTextEditWidget() != nullptr) GetTextEditWidget()->undo();
}

void MainWindow::OnActionRedo()
{
    if (GetTextEditWidget() != nullptr) GetTextEditWidget()->redo();
}

void MainWindow::OnActionFindReplace()
{
    FindReplace *findReplaceDialog = new FindReplace(this);
    findReplaceDialog->show();
}

void MainWindow::SaveInternal(const QString &myFilePath)
{
    QFile file(myFilePath);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Writer", "File cannot be saved");
        return;
    }

    if (GetTextEditWidget() != nullptr)
    {
        QTextStream stream(&file);
        stream << GetTextEditWidget()->toPlainText();
    }

    QFileInfo fileInfo(file);
    setWindowTitle(QString("Writer - ") + fileInfo.fileName());

    file.flush();
    file.close();
}
