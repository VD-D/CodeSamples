#include "findreplace.h"
#include "ui_findreplace.h"
#include "mainwindow.h"

#include <regex>
#include <string>
#include <QRegularExpression>
#include <QtLogging>
#include <QTextEdit>

FindReplace::FindReplace(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FindReplace)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    // Connecting actions.
    connect(ui->replaceNextButton, &QPushButton::clicked, this, &FindReplace::OnReplaceNextClicked);
    connect(ui->replaceAllButton, &QPushButton::clicked, this, &FindReplace::OnReplaceAllClicked);
}

FindReplace::~FindReplace()
{
    delete ui;
}

void FindReplace::OnReplaceNextClicked()
{
    if (ui == nullptr)
    {
        qCritical("FindReplace::OnReplaceAllClicked - ui is invalid!");
        return;
    }

    if (MainWindow* mainWindow = GetMainWindow())
    {
        if (QTextEdit* textEdit = mainWindow->GetTextEditWidget())
        {
            QString text = textEdit->toPlainText();
            QString findString = ui->lineEditFind->text();

            if (OccurancesOfSubstring(text, ui->lineEditFind->text(), GetMatchWholeWord()) > 0)
            {
                ui->searchKeyReplaced->setText(QString("Search key replaced"));
            }
            else
            {
                ui->searchKeyReplaced->setText(QString("Search key not found"));
            }
            text.replace(text.indexOf(findString, GetCaseSensitivity()), findString.size(), ui->lineEditReplace->text());
            textEdit->setText(text);
        }
    }
}

void FindReplace::OnReplaceAllClicked()
{
    if (ui == nullptr)
    {
        qCritical("FindReplace::OnReplaceAllClicked - ui is invalid!");
        return;
    }

    if (MainWindow* mainWindow = GetMainWindow())
    {
        if (QTextEdit* textEdit = mainWindow->GetTextEditWidget())
        {
            QString text = textEdit->toPlainText();
            ui->searchKeyReplaced->setText(QString("Search key replaced ") + QString::number(OccurancesOfSubstring(text, ui->lineEditFind->text(), GetMatchWholeWord())) + QString(" times"));

            if (GetMatchWholeWord())
            {
                QRegularExpression regexp;
                regexp.setPattern(QString("\\b") + ui->lineEditFind->text() + QString("\\b"));
                text.replace(regexp, ui->lineEditReplace->text());
            }
            else
            {
                text.replace(ui->lineEditFind->text(), ui->lineEditReplace->text(), GetCaseSensitivity());
            }

            textEdit->setText(text);
        }
    }
}

MainWindow *FindReplace::GetMainWindow() const
{
    return qobject_cast<MainWindow*>(parentWidget());
}

Qt::CaseSensitivity FindReplace::GetCaseSensitivity() const
{
    if (ui == nullptr)
    {
        qCritical("FindReplace::GetCaseSensitivity - ui is invalid!");
    }

    if (QCheckBox *CheckBox = ui->caseSensitiveCheck)
    {
        return CheckBox->isChecked() ? Qt::CaseSensitivity::CaseSensitive : Qt::CaseSensitivity::CaseInsensitive;
    }

    return Qt::CaseSensitivity::CaseInsensitive;
}

bool FindReplace::GetMatchWholeWord() const
{
    if (ui == nullptr)
    {
        qCritical("FindReplace::GetMatchWholeWord - ui is invalid!");
    }

    if (QCheckBox *CheckBox = ui->wholeWordCheck)
    {
        return CheckBox->isChecked();
    }

    return false;
}

int FindReplace::OccurancesOfSubstring(const QString &Original, const QString &Substring, bool WholeWordOnly) const
{
    std::string stringOriginal = Original.toStdString();
    std::string stringSubstring = Substring.toStdString();

    int count = 0;
    if (WholeWordOnly)
    {
        std::regex regexp("\\b" + stringSubstring + "\\b");
        std::smatch match;

        std::string::const_iterator searchStart(stringOriginal.cbegin());
        while(std::regex_search(searchStart, stringOriginal.cend(), match, regexp))
        {
            count += 1;
            searchStart = match.suffix().first;
        }
    }
    else
    {
        size_t nPos = stringOriginal.find(stringSubstring, 0);
        while (nPos != std::string::npos)
        {
            count += 1;
            nPos = stringOriginal.find(stringSubstring, nPos + 1);
        }
    }

    return count;
}
