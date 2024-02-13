// Copyright 2016 Manfred Dings

#include <QSplashScreen>
#include <QTranslator>
#include <QLibraryInfo>
#include <QGuiApplication>
#include "mapmainwindow.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(mapressources);

    // Set Organization etc. for the "ini"-file

    QCoreApplication::setOrganizationName("MAP");
    QCoreApplication::setOrganizationDomain("musikundtheorie.de");
    QCoreApplication::setApplicationName("MAP5");

    QApplication a(argc, argv);

    QPixmap Pixmap(":/splash.png");
    QSplashScreen splash(Pixmap);

    splash.show();

    Qt::Alignment al = Qt::AlignHCenter | Qt::AlignTop;

    splash.showMessage(CVERSIONSTEXT, al, (0xee7000));


    // Übersetzung der Standarddialog, Buttons etc.
  //  QTranslator qtTranslator;
  //  qtTranslator.load("qt_" + QLocale::system().name(),
  //                    QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  //  a.installTranslator(&qtTranslator);

    // wait for WaitN Millisecs to show the splash-screen
    MapMainWindow w;
    QGuiApplication::setWindowIcon(QIcon(":MainIcon2.icns"));
   splash.finish(&w);
    w.show();
return a.exec();
}
