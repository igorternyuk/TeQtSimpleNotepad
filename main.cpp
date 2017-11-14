#include "mainwindow.h"
#include <QApplication>
#include <QString>
#include <QTextDocument>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle("fusion");
    app.setApplicationName("TeQtSimpleNotepad");
    app.setApplicationVersion("1.0");
    app.setObjectName("TernyukCorporation");
    app.setOrganizationDomain("www.igorternyuk.com");
    MainWindow w;
    w.show();

    return app.exec();
}
