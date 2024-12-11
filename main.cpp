#include "mainwindow.h"

#include <QApplication>
#include <QScreen>
#include <Windows.h>
#include <WinUser.h>
#include <wingdi.h>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
