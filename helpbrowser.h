// Copyright 2016 Manfred Dings

#ifndef HELPBROWSER_H
#define HELPBROWSER_H

#include <QtWidgets>

class HelpBrowser : public QDialog
{
    Q_OBJECT
public:
    explicit HelpBrowser(QSettings *settings,
                         QWidget *parent = 0);
    ~HelpBrowser();

    static QDir directoryOf(const QString &subdir);
    void showPage(const QString &page);

    void closeEvent(QCloseEvent *event);

signals:

public slots:

private slots:
    void updateWindowTitle();


private:
    QTextBrowser *textBrowser;
    QPushButton *homeButton;
    QPushButton *backButton;
    QPushButton *closeButton;
    QSettings *msettings;
    QString mpath;
};

#endif  // HELPBROWSER_H
