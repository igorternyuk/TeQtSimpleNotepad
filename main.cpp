#include "mainwindow.h"
#include <QApplication>
#include <QString>
#include <QTextDocument>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle("fusion");

    MainWindow w;
    w.show();

    return app.exec();
}
