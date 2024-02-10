// Copyright 2016 Manfred Dings

#ifndef DIALOG_H
#define DIALOG_H
#include <QSettings>
#include <QTextBrowser>
#include <QLabel>
#include <QKeyEvent>

#include <QDialog>

/*
Stylesheet für weißen Hintergrund der LaTeX-Präsentation:

background-color:rgb(255,254,248);\nborder-style: none
*/

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0, QSettings *settings = 0);
    ~Dialog();
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);


    QSettings *msettings;
    QTextBrowser *textbrowser;
    QLabel *lbltime;

    void vergroessern(int neux, int neuy, QRect &rect);
    void standardgroesse();
    void NurStoppuhr(bool stoppuhrsize);
    void verschieben(int x, int y, bool big);
    void textloeschen();

    bool getMnotext() const;
    void setMnotext(bool value);
    void zurueckholen();
    void setStyles(QString style, QString styleTB);

signals:
    void zwStyleChanged();

private:
    Ui::Dialog *ui;

    QWidget *mparent;

    bool mnotext, mstoppuhrsize;

    int oldwidth, oldheight;
};

#endif  // DIALOG_H
