#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QMessageBox>
#include <QFontDialog>
#include <QColorDialog>
#include <QFileDialog>
#include <QTextCursor>
#include <QLabel>
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

/*void MainWindow::on_actionAbrir_triggered()
{
    QFileDialog dialogo(this);
    dialogo.setAcceptMode(QFileDialog::AcceptOpen);
    dialogo.setFileMode(QFileDialog::AnyFile);
    if (dialogo.exec() == QDialog::Rejected) {
        return;
    }
    const QString nombreArchivo = dialogo.selectedFiles().first();
    QFile archivo(nombreArchivo);
    archivo.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!archivo.isOpen()) {
        return;
    }
    QTextStream str(&archivo);
    QString contenido;
    contenido = str.readAll();
    archivo.flush();
    archivo.close();
    ui->textEdit->setText(contenido);
}

void MainWindow::on_actionNuevo_triggered()
{
    ui->textEdit->clear();
}

void MainWindow::on_actionGuardar_como_triggered()
{
    QFileDialog dialogo(this);
    dialogo.setAcceptMode(QFileDialog::AcceptSave);
    dialogo.setFileMode(QFileDialog::AnyFile);
    if (dialogo.exec() == QDialog::Rejected) {
        return;
    }
    const QString nombreArchivo = dialogo.selectedFiles().first();
    QFile archivo(nombreArchivo);
    archivo.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!archivo.isOpen()) { return; }
    QTextStream str(&archivo);
    const QString contenido = ui->textEdit->toPlainText();
    str << contenido;
    archivo.flush();
    archivo.close();
}*/
void MainWindow::on_action_open_triggered()
{
    /*QString startLocation = QStandardPaths::standardLocations(
                QStandardPaths::MusicLocation).value(0, QDir::homePath());
    QString filter = QString::fromStdString("Playlists (*.tpls);;All files (*.*)");
    QString fileName = QFileDialog::getOpenFileName(
                this,
                QString::fromStdString("Open playlist"),
                startLocation,
                filter);
    loadPlaylistFromFile(fileName);*/
}

void MainWindow::on_action_save_triggered()
{

}

void MainWindow::on_action_close_triggered()
{

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
    qDebug() << "ui->textArea->textCursor().position() = " << ui->textArea->textCursor().position();
    qDebug() << "ui->textArea->textCursor().position() = " << ui->textArea->textCursor().position();
    qDebug() << "Num of symbols = " << ui->textArea->toPlainText().size();
    qDebug() << "ui->textArea->depth() = " << ui->textArea->depth();
}

void MainWindow::on_action_new_file_triggered()
{

}

void MainWindow::on_action_choose_font_triggered()
{
    /*bool ok = true;
    QFont fuente = QFontDialog::getFont(&ok, this);
    if (!ok) {
        return;
    }
    ui->textEdit->setCurrentFont(fuente);*/


}

void MainWindow::on_action_choose_font_color_triggered()
{
    /*
    QColor color = QColorDialog::getColor(Qt::white, this);
    ui->textEdit->setTextColor(color);*/
}

void MainWindow::on_action_choose_background_color_triggered()
{
    /* QPalette paleta;
    QColorDialog dialogo(this);
    if (dialogo.exec() == QDialog::Rejected) {
        return;
    }
    QColor color = dialogo.selectedColor();
    paleta.setColor(QPalette::Base, color);
    ui->textEdit->setPalette(paleta);*/
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

void MainWindow::on_action_undo_triggered()
{
    ui->textArea->undo();
}

void MainWindow::on_action_redo_triggered()
{
    ui->textArea->redo();
}

void MainWindow::on_action_previous_search_result_triggered()
{

}