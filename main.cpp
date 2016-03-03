#include <iostream>
#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(application);

    QApplication app(argc, argv);
    app.setOrganizationName("Mingfei SUN");
    app.setApplicationName("COMP 5421 Project");
    MainWindow mainWin;
    mainWin.show();
    return app.exec();
}
