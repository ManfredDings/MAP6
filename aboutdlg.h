// Copyright 2016 Manfred Dings

#ifndef ABOUTDLG_H
#define ABOUTDLG_H

#include <QDialog>

namespace Ui {
class AboutDlg;
}

class AboutDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDlg(QWidget *parent = 0);
    ~AboutDlg();

private slots:

private:
    Ui::AboutDlg *ui;
};

#endif  // ABOUTDLG_H
