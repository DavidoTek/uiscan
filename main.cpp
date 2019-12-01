#include "mainwindow.h"
#include "avscanner.h"

#include <QApplication>
#include <QTranslator>
#include <QSysInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator appTranslator;
    if (appTranslator.load("translations/uiscan_" + QLocale::system().name()))
        a.installTranslator(&appTranslator);

    AVScanner::getInstance()->setArguments(a.arguments());
    AVScanner::getInstance()->setPlatform(QSysInfo::productType());

    MainWindow w;
    w.setWindowIcon(QIcon("uiscanico.ico"));
    w.setFixedSize(480,320);
    w.show();
    return a.exec();
}
