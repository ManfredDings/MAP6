#include "about.h"
#include "ui_about.h"
#include "mapmainwindow.h"

About::About(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::About)
{
    ui->setupUi(this);
    ui->label->clear();
    ui->label->setText(CVERSIONSTEXT);
    ui->groupBox->setStyleSheet("background-image: url(:/splash.png)");
 //   ui->label->setStyleSheet("background-color: rgba(0,0,0,0%)");
}

About::~About()
{
    delete ui;
}
