#ifndef FINDREPLACEDIALOG_H
#define FINDREPLACEDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QMap>
#include <QTextCursor>

namespace Ui
{
    class FindReplaceDialog;
}

class QTextEdit;
class FindReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindReplaceDialog(QWidget *parent = nullptr);
    ~FindReplaceDialog();
    void setTextEdit(QTextEdit *textEdit);

public slots:
    void find();
    void find(bool down);
    void findNext();
    void findPrev();
    void replace();
    void replaceAll();

protected:
    void closeEvent(QCloseEvent*) override;

private:
    Ui::FindReplaceDialog *ui;
    QTextEdit *mTextEdit;
    QTextCursor mTextCursor;
    enum class SettingsKey
    {
        TEXT_TO_FIND,
        TEXT_TO_REPLACE,
        DOWN_RADIO,
        CASE_CHECK,
        WHOLE_WORD_CHECK,
        REGEX_CHECK
    };
    QMap<SettingsKey, QString> mKeys
    {
        { SettingsKey::TEXT_TO_FIND, "textToFind" },
        { SettingsKey::TEXT_TO_REPLACE, "textToReplace" },
        { SettingsKey::DOWN_RADIO, "downRadio "},
        { SettingsKey::CASE_CHECK, "caseCheck" },
        { SettingsKey::WHOLE_WORD_CHECK, "wholeWordCheck" },
        { SettingsKey::REGEX_CHECK, "regexCheck "}
    };
    const QString mSettingsGroup {"FindReplaceDialog"};
    void writeSettings();
    void readSettings();
private slots:
    void textToFindChanged();
    void validateRegExp(const QString &regExp);
    void showInfo(const QString &msg, bool isError = false);
};

#endif // FINDREPLACEDIALOG_H
