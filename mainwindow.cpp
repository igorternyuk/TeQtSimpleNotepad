#include "mainwindow.h"
#include "ui_mainwindow.h"
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
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("TeSimpleNotepad");
    QLabel *label = new QLabel(QString("Opened files"), this);
    ui->statusBar->addPermanentWidget(label);
    mLblTexyInfo = new QLabel(this);
    ui->statusBar->addPermanentWidget(mLblTexyInfo);
    ui->tabWidget->setTabsClosable(true);    
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mark_unsaved_test_changes_on_tab()
{
    QTextEdit* sender = static_cast<QTextEdit*>(QObject::sender());
    auto index = ui->tabWidget->indexOf(sender->parentWidget());
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
    auto listOfFiles = QFileDialog::getOpenFileNames(this, "Choose text files", startLocation,
                                                     filter);
    for(const auto &path: listOfFiles)
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
    }
}

void MainWindow::on_action_save_triggered()
{
    auto index = ui->tabWidget->currentIndex();
    if(index == -1) return;
    this->save_file(index);
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
    this->mLblTexyInfo->setText(text);
}

void MainWindow::create_new_tab(const QString &title, const QString &pathToFile,
                                const QString &text)
{
    QWidget *widget = new QWidget(ui->tabWidget);
    QTextEdit *textEdit = new QTextEdit(widget);

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
            &MainWindow::mark_unsaved_test_changes_on_tab);
     mListOfTextEdits.append(textEdit);
    QVBoxLayout *l = new QVBoxLayout(widget);
    l->addWidget(textEdit);
    widget->setLayout(l);
    ui->tabWidget->addTab(widget, title);
    auto index = ui->tabWidget->indexOf(widget);
    ui->tabWidget->setTabToolTip(index, pathToFile);
    if(title.contains(QRegExp("Untitled")))
        ui->tabWidget->tabBar()->setTabTextColor(index, Qt::red);
}

bool MainWindow::save_file(int index)
{
    if(index == -1) return false;
    QString tapCaption = ui->tabWidget->tabText(index);
    if(tapCaption.contains(QRegExp("(Untitled)")))
    {
        if(save_file_as(index))
        {
            ui->tabWidget->tabBar()->setTabTextColor(index, Qt::black);
            return true;
        }
    }
    else
    {
        auto path = ui->tabWidget->tabToolTip(index);
        auto text = mListOfTextEdits.at(index)->toPlainText();
        if(save_text_to_file(path, text))
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
    dialog.setWindowTitle(QString("Specify path for ") + ui->tabWidget->tabText(index));
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
    qDebug() << "On action close triggered";
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
        this->close();
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
    static int counter = 0;
    create_new_tab(QString("Untitled%1").arg(++counter));
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

void MainWindow::on_action_find_triggered()
{

}

void MainWindow::on_action_find_and_replace_triggered()
{

}

void MainWindow::on_action_next_search_result_triggered()
{

}

void MainWindow::on_action_previous_search_result_triggered()
{

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
    QString fileName = QFileDialog::getSaveFileName(this, "Choose a file path", startLocation,
                                                    filter);
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    mListOfTextEdits.at(index)->document()->print(&printer);
}

void MainWindow::on_action_save_all_files_triggered()
{
    for(size_t i{0u}; i < ui->tabWidget->count(); ++i)
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
        auto answer = QMessageBox::question(this, "Save file?", "Would you like"
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
