#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QStringList>
#include <QStandardPaths>
#include <QFile>
#include <QFileInfo>
#include <QDir>
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
    ui->textArea->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
    ui->textArea->addAction(ui->action_copy);
    ui->textArea->addAction(ui->action_cut);
    ui->textArea->addAction(ui->action_paste);
    ui->textArea->addAction(ui->action_select_all);
    ui->textArea->addAction(ui->action_find);
    ui->textArea->addAction(ui->action_find_and_replace);
    ui->textArea->addAction(ui->action_next_search_result);
    ui->textArea->addAction(ui->action_previous_search_result);
    ui->textArea->document()->setDefaultFont(QFont("times", 14));
    QLabel *label = new QLabel(QString("Opened files"), this);
    ui->statusBar->addPermanentWidget(label);
    mFileSelector = new QComboBox(this);
    mFileSelector->setMinimumWidth(500);

    //QStringList list;
    //list << "First" << "Second" << "Third";
    //mFileSelector->addItems(list);
    ui->statusBar->addPermanentWidget(mFileSelector);

    mLblTexyInfo = new QLabel(this);
    ui->statusBar->addPermanentWidget(mLblTexyInfo);
    connect(ui->textArea, &QTextEdit::cursorPositionChanged, this,
            &MainWindow::update_cursor_info);

    connect(ui->textArea, &QTextEdit::textChanged, [this](){
        mFilesData[mCurrentFileIndex].isSaved = false;
    });

    auto signal = static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged);
    connect(mFileSelector, signal, this, &MainWindow::update_file_view);

    ui->tabWidget->setTabsClosable(true);
}

MainWindow::~MainWindow()
{
    delete ui;
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
        open_file(path);
}

void MainWindow::on_action_save_triggered()
{
    this->save_file(mCurrentFileIndex);
}

void MainWindow::on_action_save_as_triggered()
{
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
            stream << ui->textArea->toPlainText();
            file.flush();
            file.close();
        }
    }
}

void MainWindow::update_cursor_info()
{
    QTextCursor cursor = ui->textArea->textCursor();
    auto cursorColumn = ui->textArea->textCursor().columnNumber();
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

void MainWindow::update_file_view(int index)
{
    qDebug() << "index = " << index;
    if(index >= 0 && index < mFilesData.size())
    {
        mFilesData[mCurrentFileIndex].text = ui->textArea->toPlainText();
        mCurrentFileIndex = index;
        auto data = mFilesData.at(mCurrentFileIndex);
        QString text = data.text;
        ui->textArea->setText(text);
    }
}

bool MainWindow::open_file(const QString &path)
{
    QFile file(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!file.isOpen())
        return false;
    QTextStream stream(&file);
    QString text = stream.readAll();
    FileData data{path, text, true};
    mFilesData.append(data);
    file.flush();
    file.close();
    syncronize_combobox_and_file_list();
    return true;
}

bool MainWindow::save_file(int index)
{
    if(index >= 0 && index < mFilesData.size())
    {
        auto path = mFilesData.at(index).fullPath;
        QFile file(path);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return false;
        QTextStream stream(&file);
        stream << ui->textArea->toPlainText();
        file.flush();
        file.close();
        return true;
    }
    return false;
}

void MainWindow::close_file(int index)
{
    if(index >= 0 && index < mFilesData.size())
    {
        if(!mFilesData.at(index).isSaved)
        {
            auto answer = QMessageBox::question(this, "Save file?", "Would you like"
                                                " to save current file?",
                                                QMessageBox::Yes | QMessageBox::No);
            if(answer == QMessageBox::Yes)
            {
                this->save_file(index);
            }
        }
        //numOfFiles = mFilesData.size();
        mFilesData.removeAt(index);
        syncronize_combobox_and_file_list();
        if(mFileSelector->count() > 0)
            mFileSelector->setCurrentIndex(mFileSelector->count() - 1);
    }
}

void MainWindow::syncronize_combobox_and_file_list()
{
    mFileSelector->clear();
    for(const auto &data: mFilesData)
    {
        mFileSelector->addItem(data.fullPath);
    }
    if(mFileSelector->count() > 0)
        mFileSelector->setCurrentIndex(mFileSelector->count() - 1);
}

void MainWindow::on_action_close_triggered()
{
    close_file(mFileSelector->currentIndex());
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
    ui->textArea->copy();
}

void MainWindow::on_action_cut_triggered()
{
    ui->textArea->cut();
}

void MainWindow::on_action_paste_triggered()
{
    ui->textArea->paste();
}

void MainWindow::on_action_select_all_triggered()
{
    ui->textArea->selectAll();
}

void MainWindow::on_action_clear_Selection_triggered()
{
    QTextCursor c = ui->textArea->textCursor();
    c.setPosition(0);
    c.setPosition(0, QTextCursor::KeepAnchor);
    ui->textArea->setTextCursor(c);
}

void MainWindow::on_action_new_file_triggered()
{
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::AnyFile);
    if (dialog.exec() == QDialog::Accepted)
    {
        auto filePath = dialog.selectedFiles().first();
        open_file(filePath);
    }
}

void MainWindow::on_action_choose_font_triggered()
{
    bool ok { true };
    QFont font = QFontDialog::getFont(&ok, QFont("times", 14), this,
                                      QString("Choose a font for text"));
    if(ok)
    {
        ui->textArea->setCurrentFont(font);
    }
}

void MainWindow::on_action_choose_font_color_triggered()
{
    QColorDialog dialog(Qt::black, this);
    if(dialog.exec() == QDialog::Accepted)
    {
        ui->textArea->setTextColor(dialog.selectedColor());
    }
}

void MainWindow::on_action_choose_background_color_triggered()
{
    QColorDialog dialog(Qt::white, this);
    if(dialog.exec() == QDialog::Accepted)
    {
        QPalette pal;
        pal.setColor(QPalette::Base, dialog.selectedColor());
        ui->textArea->setPalette(pal);
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
    ui->textArea->undo();
}

void MainWindow::on_action_redo_triggered()
{
    ui->textArea->redo();
}

void MainWindow::on_action_text_properties_triggered()
{
    //Whole text
    auto lineCount = ui->textArea->document()->lineCount();
    auto wordCount = ui->textArea->toPlainText()
            .split(QRegExp("(\\s|\\n|\\r)+"), QString::SkipEmptyParts).count();
    auto characterCount = ui->textArea->document()->characterCount();
    auto specialSymbolsCount = ui->textArea->toPlainText()
            .count(QRegExp("(\\s|\\n|\\r|\\t)"));

    QTextCursor cursor = ui->textArea->textCursor();

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

    /////////////PDF//////////////////


    /*
     * Exporting a document to PDF
    Found it very easy to convert a QTextDocument to PDF in Qt. And useful in scenarios like creating reports etc.

    For example, Consider the following snippet,

           QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
        "untitled",tr("PDF Document (*.pdf)"));
        QPrinter printer;
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        doc->print(&printer); // doc is QTextDocument *
    We can use the printer class to print the document to a file : a pdf file.

    */
}

void MainWindow::on_action_save_all_files_triggered()
{
    for(size_t i{0u}; i < mFilesData.size(); ++i)
        this->save_file(i);
}

void MainWindow::on_action_close_all_files_triggered()
{
    for(size_t i{0u}; i < mFilesData.size(); ++i)
        this->close_file(i);
    ui->textArea->setText("");
    mFileSelector->clear();
}

void MainWindow::on_actionAdd_tab_triggered()
{
    static int counter = 0;
    QWidget *widget = new QWidget(this);
    QTextEdit *textEdit = new QTextEdit(widget);
    QVBoxLayout *l = new QVBoxLayout(widget);
    l->addWidget(textEdit);
    widget->setLayout(l);
    ui->tabWidget->addTab(widget, QString("New tab %1").arg(++counter));
}

/// Tab close event
void MainWindow::on_tabWidget_tabCloseRequested(int index)
{

}
