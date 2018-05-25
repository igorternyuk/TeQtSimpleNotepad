#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "findreplacedialog.h"
#include <QMessageBox>
#include <QStringList>
#include <QStandardPaths>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextEdit>
#include <QTextStream>
#include <QFontDialog>
#include <QPalette>
#include <QColorDialog>
#include <QFileDialog>
#include <QTextCursor>
#include <QTextDocumentFragment>
#include <QTextBlock>
#include <QRegExp>
#include <QLabel>
#include <QComboBox>
#include <QPrinter>
#include <QVBoxLayout>
#include <QSettings>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#ifdef DEBUG
#include <QDebug>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("TeSimpleNotepad");
    QLabel *label = new QLabel(QString("Opened files"), this);
    ui->statusBar->addPermanentWidget(label);
    mLblTextInfo = new QLabel(this);
    ui->statusBar->addPermanentWidget(mLblTextInfo);
    ui->tabWidget->setTabsClosable(true);
    mFindReplaceDialog = new FindReplaceDialog(this);
    connect(ui->tabWidget, &QTabWidget::currentChanged, [this](auto index){
        if(index == -1) return;
        mFindReplaceDialog->setTextEdit(mListOfTextEdits.at(index));
    });
    load_settings();
    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    save_settings();
    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    auto data = event->mimeData();
    auto urls = data->urls();

    for(auto url: urls)
    {
        QString filePath = url.toLocalFile();
        open_file(filePath);
    }
}

void MainWindow::closeEvent(QCloseEvent *)
{
    QMainWindow::close();
}

void MainWindow::mark_unsaved_text_changes_on_tab()
{
    QTextEdit* sender = static_cast<QTextEdit*>(QObject::sender());
    auto index = ui->tabWidget->indexOf(sender);
    if(index == -1) return;
    ui->tabWidget->tabBar()->setTabTextColor(index, Qt::red);
}

void MainWindow::on_action_about_program_triggered()
{
    const QString msg = "Autor: Ternyuk Igor\n" \
            "Date: 11/11/2017\nCompany: TernyukCorporation";
    QMessageBox::about(this, "About TeNotepad", msg);
}

void MainWindow::on_action_about_Qt_triggered()
{
    QMessageBox::aboutQt(this, "About Qt");
}

void MainWindow::on_action_open_triggered()
{
    QString startLocation = QStandardPaths::standardLocations(
                            QStandardPaths::DocumentsLocation)
                            .value(0, QDir::homePath());
    QString filter = QString::fromStdString("Text documents (*.txt);;"
                                            "All files (*.*)");
    auto listOfFiles = QFileDialog::getOpenFileNames(this,
                                                     "Choose text files",
                                                     startLocation,
                                                     filter);

    open_files(listOfFiles);
    /*for(const auto &path: listOfFiles)
    {
        QFile file(path);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        if(!file.isOpen())
            continue;
        QTextStream stream(&file);
        QString text = stream.readAll();
        QFileInfo fileInfo(file);
        create_new_tab(fileInfo.fileName(), path, text);
        file.flush();
        file.close();
    }*/
}

void MainWindow::on_action_save_triggered()
{
    if(!save_file(ui->tabWidget->currentIndex()))
    {
        QMessageBox::critical(this, "Error", "Could not save file");
    }
}

void MainWindow::on_action_save_as_triggered()
{
    auto index = ui->tabWidget->currentIndex();
    if(index == -1) return;

    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::AnyFile);
    if (dialog.exec() == QDialog::Accepted) {
        const QString filePath = dialog.selectedFiles().first();
        QFile file(filePath);
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        if (file.isOpen())
        {
            QTextStream stream(&file);
            stream << mListOfTextEdits.at(index)->toPlainText();
            QFileInfo fileInfo(file);
            ui->tabWidget->setTabText(index, fileInfo.fileName());
            ui->tabWidget->setTabToolTip(index, filePath);
            file.flush();
            file.close();
        }           
    }
}

void MainWindow::update_cursor_info()
{
    auto index = ui->tabWidget->currentIndex();
    if(index == -1) return;

    QTextCursor cursor = mListOfTextEdits.at(index)->textCursor();
    auto cursorColumn = cursor.columnNumber();
    cursor.movePosition(QTextCursor::StartOfLine);
    auto cursorLine = 0u;
    while(cursor.positionInBlock() > 0)
    {
        cursor.movePosition(QTextCursor::Up);
        ++cursorLine;
    }

    auto block = cursor.block().previous();
    while(block.isValid())
    {
        cursorLine += block.lineCount();
        block = block.previous();
    }
    ++cursorLine;
    auto text = QString("Cursor: line %4 column %5")
            .arg(cursorLine)
            .arg(cursorColumn);
    this->mLblTextInfo->setText(text);
}

int MainWindow::create_new_tab(const QString &title, const QString &pathToFile,
                                const QString &text)
{
    QTextEdit *textEdit = new QTextEdit(ui->tabWidget);

    textEdit->document()->setDefaultFont(QFont("times", 14));
    textEdit->setText(text);
    textEdit->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
    textEdit->addAction(ui->action_copy);
    textEdit->addAction(ui->action_cut);
    textEdit->addAction(ui->action_paste);
    textEdit->addAction(ui->action_select_all);
    textEdit->addAction(ui->action_clear_Selection);
    textEdit->addAction(ui->action_save);
    textEdit->addAction(ui->actionExport_to_pdf);
    textEdit->addAction(ui->action_save_all_files);
    textEdit->addAction(ui->action_close);
    textEdit->addAction(ui->action_close_all_files);
    textEdit->addAction(ui->action_text_properties);
    connect(textEdit, &QTextEdit::cursorPositionChanged,
            this, &MainWindow::update_cursor_info);
    connect(textEdit, &QTextEdit::textChanged, this,
            &MainWindow::mark_unsaved_text_changes_on_tab);
    mListOfTextEdits.append(textEdit);
    ui->tabWidget->addTab(textEdit, title);
    auto index = ui->tabWidget->indexOf(textEdit);
    ui->tabWidget->setTabToolTip(index, pathToFile);

    if(!pathToFile.contains(QRegExp("(lastSession)")))
    {
        ui->tabWidget->setTabToolTip(index, pathToFile);
    }
    else
    {
        ui->tabWidget->tabBar()->setTabTextColor(index, Qt::red);
    }
    if((text.isEmpty() && pathToFile.isEmpty()))
        ui->tabWidget->tabBar()->setTabTextColor(index, Qt::red);
    ui->tabWidget->setCurrentIndex(index);

    return index;
}

bool MainWindow::open_files(const QStringList &listOfFiles)
{
    bool isAtLeastOneFile { false };
    for(const auto &path: listOfFiles)
        if(open_file(path))
            isAtLeastOneFile = true;
    return isAtLeastOneFile;
}

bool MainWindow::open_file(const QString &path)
{
    QFile file(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!file.isOpen())
        return false;
    QTextStream stream(&file);
    QString text = stream.readAll();
    QFileInfo fileInfo(file);
    if(path.contains(QRegExp("(lastSession)")))
    {
        create_new_tab(fileInfo.fileName(), "", text);
    }
    else
    {
        create_new_tab(fileInfo.fileName(), path, text);
    }
    file.flush();
    file.close();
    return true;
}

bool MainWindow::save_file(int index)
{
    if(index == -1) return false;

    QString currPath = ui->tabWidget->tabBar()->tabToolTip(index);

    if(currPath.isEmpty())
    {
        if(save_file_as(index))
        {
            ui->tabWidget->tabBar()->setTabTextColor(index, Qt::black);
            return true;
        }
    }
    else
    {
        auto currText = mListOfTextEdits.at(index)->toPlainText();
        if(save_text_to_file(currPath, currText))
        {
            ui->tabWidget->tabBar()->setTabTextColor(index, Qt::black);
            return true;
        }
    }
    return false;
}

bool MainWindow::save_file_as(int index)
{
    if(index == -1) return false;

    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::AnyFile);
   // dialog.setFilter("Text documents (*.txt);;All files (*.*));");
    dialog.setWindowTitle(QString("Specify path for ")
                          + ui->tabWidget->tabText(index));
    if (dialog.exec() == QDialog::Accepted) {
        const QString filePath = dialog.selectedFiles().first();
        QFile file(filePath);
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        if (file.isOpen())
        {
            QTextStream stream(&file);
            stream << mListOfTextEdits.at(index)->toPlainText();
            QFileInfo fileInfo(file);
            ui->tabWidget->setTabText(index, fileInfo.fileName());
            ui->tabWidget->setTabToolTip(index, filePath);
            file.flush();
            file.close();
            return true;
        }
    }
    return false;
}

bool MainWindow::save_text_to_file(const QString &filePath, const QString &text)
{
    QFile file(filePath);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    if (file.isOpen())
    {
        QTextStream stream(&file);
        stream << text;
        file.flush();
        file.close();
        return true;
    }
    return false;
}

void MainWindow::on_action_close_triggered()
{
    auto index = ui->tabWidget->currentIndex();
    if(index == -1) return;
    on_tabWidget_tabCloseRequested(index);
}

void MainWindow::on_action_quit_triggered()
{
    auto reply = QMessageBox::question(this, "Confirm exit",
                                       "Do you really want to quit application?",
                                       QMessageBox::Yes | QMessageBox::No);
    if(reply == QMessageBox::Yes)
    {
        this->close();
    }
}

void MainWindow::on_action_copy_triggered()
{
    auto index = ui->tabWidget->currentIndex();
    if(index == -1) return;
    mListOfTextEdits.at(index)->copy();
}

void MainWindow::on_action_cut_triggered()
{
    auto index = ui->tabWidget->currentIndex();
    if(index == -1) return;
    mListOfTextEdits.at(index)->cut();
}

void MainWindow::on_action_paste_triggered()
{
    auto index = ui->tabWidget->currentIndex();
    if(index == -1) return;
    mListOfTextEdits.at(index)->paste();
}

void MainWindow::on_action_select_all_triggered()
{
    auto index = ui->tabWidget->currentIndex();
    if(index == -1) return;
    mListOfTextEdits.at(index)->selectAll();
}

void MainWindow::on_action_clear_Selection_triggered()
{
    auto index = ui->tabWidget->currentIndex();
    if(index == -1) return;
    QTextCursor c = mListOfTextEdits.at(index)->textCursor();
    c.setPosition(0);
    c.setPosition(0, QTextCursor::KeepAnchor);
    mListOfTextEdits.at(index)->setTextCursor(c);
}

void MainWindow::on_action_new_file_triggered()
{
    create_new_tab(QString("Untitled%1").arg(++mUntitledTabIndexMax));
}

void MainWindow::on_action_choose_font_triggered()
{
    auto index = ui->tabWidget->currentIndex();
    if(index == -1) return;
    bool ok { true };
    QFont font = QFontDialog::getFont(&ok, QFont("times", 14), this,
                                      QString("Choose a font for text"));
    if(ok) mListOfTextEdits.at(index)->setCurrentFont(font);
}

void MainWindow::on_action_choose_font_color_triggered()
{
    auto index = ui->tabWidget->currentIndex();
    if(index == -1) return;
    QColorDialog dialog(Qt::black, this);
    if(dialog.exec() == QDialog::Accepted)
    {
        mListOfTextEdits.at(index)->setTextColor(dialog.selectedColor());
    }
}

void MainWindow::on_action_choose_background_color_triggered()
{
    auto index = ui->tabWidget->currentIndex();
    if(index == -1) return;
    QColorDialog dialog(Qt::white, this);
    if(dialog.exec() == QDialog::Accepted)
    {
        QPalette pal;
        pal.setColor(QPalette::Base, dialog.selectedColor());        
        mListOfTextEdits.at(index)->setPalette(pal);
    }
}

void MainWindow::on_action_find_and_replace_triggered()
{
    if(this->mFindReplaceDialog->isHidden())
        this->mFindReplaceDialog->show();
    else
        this->mFindReplaceDialog->hide();
}

void MainWindow::on_action_next_search_result_triggered()
{
    mFindReplaceDialog->findNext();
}

void MainWindow::on_action_previous_search_result_triggered()
{
    mFindReplaceDialog->findPrev();
}

void MainWindow::on_action_undo_triggered()
{
    auto index = ui->tabWidget->currentIndex();
    if(index == -1) return;
    mListOfTextEdits.at(index)->undo();
}

void MainWindow::on_action_redo_triggered()
{
    auto index = ui->tabWidget->currentIndex();
    if(index == -1) return;
    mListOfTextEdits.at(index)->redo();
}

void MainWindow::on_action_text_properties_triggered()
{
    auto index = ui->tabWidget->currentIndex();
    if(index == -1) return;
    auto currTextEdit = mListOfTextEdits.at(index);

    //Whole text

    auto lineCount = currTextEdit->document()->lineCount();
    auto wordCount = currTextEdit->toPlainText()
            .split(QRegExp("(\\s|\\n|\\r)+"), QString::SkipEmptyParts).count();
    auto characterCount = currTextEdit->document()->characterCount();
    auto specialSymbolsCount = currTextEdit->toPlainText()
            .count(QRegExp("(\\s|\\n|\\r|\\t)"));

    QTextCursor cursor = currTextEdit->textCursor();

    //Selected text

    auto selectedText = cursor.selectedText();
    auto selectedTextWordCount = selectedText.split(QRegExp("(\\s|\\n|\\r)+"),
                                        QString::SkipEmptyParts).count();
    auto selectedTextCompleteCharacterCount = selectedText.size();
    auto selectedTextCharacterCount = selectedTextCompleteCharacterCount -
            selectedText.count(QRegExp("(\\s|\\n|\\r|\\t)"));
    auto textInfo = QString("Whole text:\n"
                        "lines %1 words %2 characters %3\n"
                        "characters without spaces %4\n"
                        "Selected text:\n"
                        "words %5 characters %6\n"
                        "characters without spaces %7")
            .arg(lineCount + 1)
            .arg(wordCount)
            .arg(characterCount)
            .arg(characterCount - specialSymbolsCount)
            .arg(selectedTextWordCount)
            .arg(selectedTextCompleteCharacterCount)
            .arg(selectedTextCharacterCount);
    QMessageBox::information(this, "Text properties", textInfo);
}

void MainWindow::on_actionExport_to_pdf_triggered()
{
    auto index = ui->tabWidget->currentIndex();
    if(index == -1) return;
    QString startLocation = QStandardPaths::standardLocations(
                            QStandardPaths::DocumentsLocation)
                            .value(0, QDir::homePath());
    QString filter = QString::fromStdString("PDF documents (*.pdf);;"
                                            "All files (*.*)");
    QString fileName = QFileDialog::getSaveFileName(this, "Choose a file path",
                                                    startLocation,
                                                    filter);
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    mListOfTextEdits.at(index)->document()->print(&printer);
}

void MainWindow::on_action_save_all_files_triggered()
{
    for(int i{0}; i < ui->tabWidget->count(); ++i)
        this->save_file(i);
}

void MainWindow::on_action_close_all_files_triggered()
{
    while(ui->tabWidget->count())
        on_tabWidget_tabCloseRequested(ui->tabWidget->count() - 1);
}

/// Tab close event
void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    if(index == -1) return;
    auto color = ui->tabWidget->tabBar()->tabTextColor(index);
    if(color == Qt::red)
    {
        auto answer = QMessageBox::question(this, "Save file?",
                                            "Would you like"
                                            " to save current file?",
                                            QMessageBox::Yes | QMessageBox::No);
        if(answer == QMessageBox::Yes)
        {
            this->save_file(index);
        }
    }

    QWidget *widget = ui->tabWidget->widget(index);
    ui->tabWidget->removeTab(index);
    delete widget;
    mListOfTextEdits.removeAt(index);
}

void MainWindow::save_all_current_session_files()
{
    QFile file(SESSION_FILE_PATH);
    file.open(QIODevice::WriteOnly);
    if (file.isOpen())
    {
        if(!QDir(".lastSession").exists())
            QDir().mkdir(".lastSession");
        QTextStream stream(&file);
        for(int i = 0; i < ui->tabWidget->count(); ++i)
        {
            auto path = ui->tabWidget->tabBar()->tabToolTip(i);
            if(path.isEmpty())
            {
                path = QString(".lastSession/%1").arg(ui->tabWidget->tabText(i));
            }
            auto text = mListOfTextEdits.at(i)->toPlainText();
            save_text_to_file(path, text);
            stream << path << "\n";
        }
        file.flush();
        file.close();
    }
}

void MainWindow::load_all_last_session_files()
{
    QFile file(SESSION_FILE_PATH);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        bool isAtLeastOneFile = false;
        while(!stream.atEnd())
        {
            auto path = stream.readLine();
            if(!path.isEmpty())
                isAtLeastOneFile = true;
            open_file(path);
        }
        if(!isAtLeastOneFile)
            on_action_new_file_triggered();
        file.flush();
        file.close();
    }
    else
    {
        QMessageBox::critical(this, "Error", "Could not restore session");
    }
}

void MainWindow::load_settings()
{
    load_all_last_session_files();
    auto size = loadParameter(mKeys[SettingsKey::WINDOW_SIZE],
                              mSettingsGroups[SettingsGroup::MainWindowGeometry],
                              this->size()).value<QSize>();
    this->resize(size);
    auto pos = loadParameter(mKeys[SettingsKey::WINDOW_POS],
                             mSettingsGroups[SettingsGroup::MainWindowGeometry],
                             this->pos()).value<QPoint>();
    this->move(pos);
    bool ok { false };
    auto curr_tab = loadParameter(mKeys[SettingsKey::CURRENT_TAB],
                                  mSettingsGroups[SettingsGroup::TabWidget],
                                  0).toInt(&ok);
    if(ok)
        ui->tabWidget->setCurrentIndex(curr_tab);
    ok = false;
    auto untitledTabIndexMax = loadParameter(
            mKeys[SettingsKey::UNTITLED_TAB_INDEX_MAX],
            mSettingsGroups[SettingsGroup::TabWidget],
            0).toInt(&ok);
    if(ok)
        mUntitledTabIndexMax = untitledTabIndexMax;
}

void MainWindow::save_settings()
{
    save_all_current_session_files();
    saveParameter(mKeys[SettingsKey::WINDOW_SIZE], this->size(),
            mSettingsGroups[SettingsGroup::MainWindowGeometry]);
    saveParameter(mKeys[SettingsKey::WINDOW_POS], this->pos(),
            mSettingsGroups[SettingsGroup::MainWindowGeometry]);
    saveParameter(mKeys[SettingsKey::CURRENT_TAB],
                  ui->tabWidget->currentIndex(),
            mSettingsGroups[SettingsGroup::TabWidget]);
    saveParameter(mKeys[SettingsKey::UNTITLED_TAB_INDEX_MAX],
                  mUntitledTabIndexMax,
            mSettingsGroups[SettingsGroup::TabWidget]);
}

