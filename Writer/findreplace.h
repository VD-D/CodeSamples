#ifndef FINDREPLACE_H
#define FINDREPLACE_H

#include <QDialog>

class MainWindow;

namespace Ui {
class FindReplace;
}

class FindReplace : public QDialog
{
    Q_OBJECT

public:
    explicit FindReplace(QWidget *parent = nullptr);
    ~FindReplace();

private:
    /**
     * Replaces next instance of matched substring.
     */
    void OnReplaceNextClicked();

    /**
     * Replaces all instances of matched substring.
     */
    void OnReplaceAllClicked();

    /**
     * @return Returns main window if is parent.
     */
    MainWindow *GetMainWindow() const;

    /**
     * @return Search case sensitivity based on check status.
     */
    Qt::CaseSensitivity GetCaseSensitivity() const;

    /**
     * @return True if we want to match by whole word only.
     */
    bool GetMatchWholeWord() const;

    /**
     * @param Original String to test.
     * @param Substring Substring for occurances.
     * @return How many times Substring occurs in Original.
     */
    int OccurancesOfSubstring(const QString& Original, const QString& Substring, bool WholeWordOnly = false) const;

    /* Pointer to current UI. */
    Ui::FindReplace *ui;
};

#endif // FINDREPLACE_H
