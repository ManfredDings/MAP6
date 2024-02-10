// Copyright 2016 Manfred Dings

#include "dialog.h"
#include "ui_dialog.h"

const int STANDARDH = 180;
const int STANDARDW = 850;
const int CBIGMOVE = 20;


Dialog::Dialog(QWidget *parent, QSettings *settings) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::ToolTip);
    msettings = settings;
    mparent = parent;

    setWindowTitle(" ");

    restoreGeometry(msettings->value("zweitfenster/geometry").toByteArray());
    oldwidth = msettings->value("zweitfenster/Stoppuhrw",
                                geometry().width()).toInt();
    oldheight = msettings->value("zweitfenster/Stoppuhrh",
                                 geometry().height()).toInt();
    move(msettings->value("pos", pos() ).toPoint());
    textbrowser = ui->textBrowser;
    lbltime = ui->lblTime;
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::closeEvent(QCloseEvent *event)
{
    if (mstoppuhrsize)
    {
        this->setGeometry(geometry().x(), geometry().y(), oldwidth, oldheight);
    }
    msettings->setValue("zweitfenster/geometry", saveGeometry());
    msettings->setValue("zweitfenster/Stoppuhrw", oldwidth);
    msettings->setValue("zweitfenster/Stoppuhrh", oldheight);
    QDialog::closeEvent(event);
}

void Dialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() != Qt::Key_Escape)
        QDialog::keyPressEvent(event);
    else {};// minimize
    mparent->activateWindow();
}

void Dialog::vergroessern(int neux, int neuy, QRect &rect)
{
    const int INCX = 10;
    const int INCY = 5;

    QRect arect = geometry();
    arect.setWidth(arect.width()+INCX*neux);
    arect.setHeight(arect.height()+INCY*neuy);
    setGeometry(arect);
    arect = geometry();
    if (isVisible())
    {
        rect.setWidth(ui->textBrowser->width());
        rect.setHeight(ui->textBrowser->height());
    }
    else
    {
        const int MAGISCHEZAHLDIEICHNICHTVERSTEHE  = 28;
        const int MAGISCHEZAHLDIEICHNICHTVERSTEHE2 = 22;
        rect.setWidth(arect.width() -
                      MAGISCHEZAHLDIEICHNICHTVERSTEHE-lbltime->width());
        rect.setHeight(arect.height()-MAGISCHEZAHLDIEICHNICHTVERSTEHE2);
    }
    if (!mstoppuhrsize)
    {
        oldwidth = geometry().width();
        oldheight = geometry().height();
    }
}

void Dialog::standardgroesse()
{
    QRect rect = this->geometry();
    rect.setWidth(STANDARDW);
    rect.setHeight(STANDARDH);
    this->setGeometry(rect);
}

void Dialog::NurStoppuhr(bool stoppuhrsize)
{
    mstoppuhrsize = stoppuhrsize;
    QRect rect = this->geometry();
    if (mstoppuhrsize)
    {
        oldwidth = rect.width();
        oldheight = rect.height();
        rect.setWidth(ui->lblTime->width()+10);
        rect.setHeight(ui->lblTime->minimumHeight());
    }
    else
    {
        rect.setWidth(oldwidth);
        rect.setHeight(oldheight);
    }
    this->setGeometry(rect);
}

void Dialog::verschieben(int x, int y, bool big)
{
    int fac = 1;
    if (big) fac = CBIGMOVE;
    QRect rect = this->geometry();
    rect.setLeft(rect.left()+x*fac);
    rect.setRight(rect.right()+x*fac);
    rect.setTop(rect.top()+y*fac);
    rect.setBottom(rect.bottom()+y*fac);
    this->setGeometry(rect);
}

void Dialog::textloeschen()
{
    ui->textBrowser->setHtml("");
    setMnotext(true);
}
bool Dialog::getMnotext() const
{
    return mnotext;
}

void Dialog::setMnotext(bool value)
{
    mnotext = value;
}

void Dialog::zurueckholen()
{
    QPoint p = this->pos();
    p.setX(0);
    p.setY(0);
    this->move(p);
}

void Dialog::setStyles(QString style, QString styleTB)
{
    setStyleSheet(style);
    this->ui->textBrowser->setStyleSheet(styleTB);
    emit zwStyleChanged();
}
