// Copyright 2016 Manfred Dings

#include "aboutdlg.h"
#include "ui_aboutdlg.h"
#include "mapmainwindow.h"

AboutDlg::AboutDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDlg)
{
    ui->setupUi(this);
    ui->label->clear();
    ui->label->setText(CVERSIONSTEXT);
        setWindowFlags(Qt::SplashScreen);
}

AboutDlg::~AboutDlg()
{
    delete ui;
}
