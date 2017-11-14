#include "findreplacedialog.h"
#include "ui_findreplacedialog.h"
#include <QTextEdit>
#include <QTextDocument>
#include <QPushButton>
#include <QRegExp>
#include <QDebug>
#include "settingsuntil.h"

FindReplaceDialog::FindReplaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindReplaceDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Find&Replace dialog");
    ui->lblSearchResults->setText("");
    connect(ui->txtFind, SIGNAL(textChanged(QString)), this,
            SLOT(textToFindChanged()));
    connect(ui->checkRegEx, &QCheckBox::clicked, [this](bool isSelected){
        this->validateRegExp(isSelected ? ui->txtFind->text() : "");
    });
    auto slot = static_cast<void(FindReplaceDialog::*)()>(&FindReplaceDialog::find);
    connect(ui->btnFind, &QPushButton::clicked, this, slot);
    connect(ui->btnFindNext, &QPushButton::clicked, this,
            &FindReplaceDialog::findNext);
    connect(ui->btnFindPrevoius, &QPushButton::clicked, this,
            &FindReplaceDialog::findPrev);
    connect(ui->btnReplace, &QPushButton::clicked, this,
            &FindReplaceDialog::replace);
    connect(ui->btnReplaceAll, &QPushButton::clicked, this,
            &FindReplaceDialog::replaceAll);
    readSettings();
}

FindReplaceDialog::~FindReplaceDialog()
{
    //qDebug() << "Find&Replace dialog destructor was called";
    writeSettings();
    delete ui;
}

void FindReplaceDialog::setTextEdit(QTextEdit *textEdit)
{
    this->mTextEdit = textEdit;
    connect(mTextEdit, &QTextEdit::copyAvailable, ui->btnReplace,
            &QPushButton::setEnabled);
    connect(mTextEdit, &QTextEdit::copyAvailable, ui->btnReplaceAll,
            &QPushButton::setEnabled);
}

void FindReplaceDialog::find()
{
    find(ui->radioDown->isChecked());
}

void FindReplaceDialog::find(bool down)
{
    if(!mTextEdit) return;
    const QString &toSearch = ui->txtFind->text();
    bool isMatch { false };
    QTextDocument::FindFlags flags;
    if(!down)
        flags |= QTextDocument::FindBackward;
    if(ui->checkCaseSensitive->isChecked())
        flags |= QTextDocument::FindCaseSensitively;
    if(ui->checkWholeWords->isChecked())
        flags |= QTextDocument::FindWholeWords;
    if(ui->checkRegEx->isChecked())
    {
        QRegExp re(toSearch, ui->checkRegEx->isChecked() ?
                   Qt::CaseSensitive : Qt::CaseInsensitive);
        mTextCursor = mTextEdit->document()->find(re, mTextCursor, flags);
        mTextEdit->setTextCursor(mTextCursor);
        isMatch = !mTextCursor.isNull();
    }
    else
    {
        isMatch = mTextEdit->find(toSearch, flags);
    }
    if(isMatch)
    {
        this->showInfo("");
    }
    else
    {
        this->showInfo("No match found", true);
        mTextEdit->textCursor().setPosition(0);
    }

}

void FindReplaceDialog::findNext()
{
    this->find(true);
}

void FindReplaceDialog::findPrev()
{
    this->find(false);
}

void FindReplaceDialog::replace()
{
    if(!mTextEdit->textCursor().hasSelection())
    {
        find();
    }
    else
    {
        mTextEdit->textCursor().insertText(ui->txtReplace->text());
        find();
    }
}

void FindReplaceDialog::replaceAll()
{
    size_t counter {0};
    while(mTextEdit->textCursor().hasSelection())
    {
        mTextEdit->textCursor().insertText(ui->txtReplace->text());
        find();
        ++counter;
    }
    this->showInfo(QString("%1 occurences were replaced").arg(counter));
}

void FindReplaceDialog::closeEvent(QCloseEvent *)
{
    this->hide();
}

void FindReplaceDialog::writeSettings()
{
    saveParameter(mKeys[SettingsKey::TEXT_TO_FIND], ui->txtFind->text(),
            mSettingsGroup);
    saveParameter(mKeys[SettingsKey::TEXT_TO_REPLACE], ui->txtReplace->text(),
            mSettingsGroup);
    saveParameter(mKeys[SettingsKey::DOWN_RADIO], ui->radioDown->isChecked(),
            mSettingsGroup);
    saveParameter(mKeys[SettingsKey::CASE_CHECK], ui->checkCaseSensitive->isChecked(),
            mSettingsGroup);
    saveParameter(mKeys[SettingsKey::WHOLE_WORD_CHECK], ui->checkWholeWords->isChecked(),
            mSettingsGroup);
    saveParameter(mKeys[SettingsKey::REGEX_CHECK], ui->checkRegEx->isChecked(),
            mSettingsGroup);
}

void FindReplaceDialog::readSettings()
{
    ui->txtFind->setText(loadParameter(
                             mKeys[SettingsKey::TEXT_TO_FIND],
                             mSettingsGroup,
                             QString()).toString());
    ui->txtReplace->setText(loadParameter(
                                mKeys[SettingsKey::TEXT_TO_REPLACE],
                                mSettingsGroup,
                                QString()).toString());
    ui->radioDown->setChecked(loadParameter(
                                  mKeys[SettingsKey::DOWN_RADIO],
                                  mSettingsGroup,
                                  false).toBool());
    ui->checkCaseSensitive->setChecked(loadParameter(
                                           mKeys[SettingsKey::CASE_CHECK],
                                           mSettingsGroup,
                                          false).toBool());
    ui->checkWholeWords->setChecked(loadParameter(
                                        mKeys[SettingsKey::WHOLE_WORD_CHECK],
                                        mSettingsGroup,
                                        false).toBool());
    ui->checkRegEx->setChecked(loadParameter(
                                   mKeys[SettingsKey::REGEX_CHECK],
                                   mSettingsGroup,
                                   false).toBool());
}

void FindReplaceDialog::textToFindChanged()
{
    bool enabled = ui->txtFind->text().size() > 0;
    ui->btnFind->setEnabled(enabled);
    ui->btnFindNext->setEnabled(enabled);
    ui->btnFindPrevoius->setEnabled(enabled);
}

void FindReplaceDialog::validateRegExp(const QString &regExp)
{
    if(!ui->checkRegEx->isChecked() || regExp.isEmpty())
    {
        ui->lblSearchResults->setText("");
        return;
    }
    QRegExp re(regExp, (ui->checkCaseSensitive->isChecked() ?
                        Qt::CaseSensitive : Qt::CaseInsensitive));
    if(re.isValid())
        ui->lblSearchResults->setText("");
    else
        this->showInfo(re.errorString(), true);
}

void FindReplaceDialog::showInfo(const QString &msg, bool isError)
{
    ui->lblSearchResults->setText("<span style =\" font-weight:600;" +
                                  (isError ? QString("color:red;") :
                                             QString("color:green;")) + " \">" +
                                  msg + "</span>");
}
