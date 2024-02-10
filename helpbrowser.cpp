// Copyright 2016 Manfred Dings

#include "helpbrowser.h"

HelpBrowser::HelpBrowser(QSettings *settings, QWidget *parent):
        QDialog(parent)
{
    msettings = settings;

    textBrowser = new QTextBrowser;

    homeButton = new QPushButton(tr("H&ome"));
    backButton = new QPushButton(tr("Zu&rück"));
    closeButton = new QPushButton(tr("&Schließen"));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(homeButton);
    buttonLayout->addWidget(backButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(textBrowser);
    setLayout(mainLayout);

    connect(homeButton, SIGNAL(clicked()), textBrowser, SLOT(home()));
    connect(backButton, SIGNAL(clicked()), textBrowser, SLOT(backward()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(textBrowser, SIGNAL(sourceChanged(QUrl)),
            this, SLOT(updateWindowTitle()));

    mpath = directoryOf("help").absolutePath();
    textBrowser->setSearchPaths(QStringList() << mpath<< ":/images");
    restoreGeometry(msettings->value("helpBrowser/geometry").toByteArray());
}


HelpBrowser::~HelpBrowser()
{
}

void HelpBrowser::showPage(const QString &page)
{
    textBrowser->setSource(page);
}

void HelpBrowser::closeEvent(QCloseEvent *event)
{
    msettings->setValue("helpBrowser/geometry", saveGeometry());
    QDialog::closeEvent(event);
}

void HelpBrowser::updateWindowTitle()
{
    setWindowTitle(tr("Hilfe: %1").arg(textBrowser->documentTitle()));
}

QDir HelpBrowser::directoryOf(const QString &subdir)
{
    QDir dir(QApplication::applicationDirPath());
    if (dir.dirName().toLower() == "debug" ||
            dir.dirName().toLower() == "release")
    {
        dir.cdUp();
        dir.cdUp();
    }
    dir.cd(subdir);
    return dir;
}
