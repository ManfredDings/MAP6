// Copyright 2016 Manfred Dings

#include "mdlistwidgetwithdrop.h"
#include <QMimeData>
#include <QApplication>
#include <QDrag>

MDListWidgetWithDrop::MDListWidgetWithDrop(QWidget *parent)
    : QListWidget(parent)
{
}

MDListWidgetWithDrop::~MDListWidgetWithDrop()
{
}

void MDListWidgetWithDrop::dragEnterEvent(QDragEnterEvent *event)
{
    event->setDropAction(Qt::CopyAction);
    event->accept();
}


void MDListWidgetWithDrop::dragMoveEvent(QDragMoveEvent *event)
{
    event->setDropAction(Qt::CopyAction);
    event->accept();
}

void MDListWidgetWithDrop::dropEvent(QDropEvent *event)
{
    if (!(event->source() == this))
    QListWidget::dropEvent(event);
}

void MDListWidgetWithDrop::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        startPos = event->pos();
        QListWidget::mousePressEvent(event);
    }
}

void MDListWidgetWithDrop::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        int distance = (event->pos() - startPos).manhattanLength();
        if (distance >=QApplication::startDragDistance())
            performDrag();
    }
    QListWidget::mouseMoveEvent(event);
}


void MDListWidgetWithDrop::performDrag()
{
    QListWidgetItem *item = currentItem();
    if (item)
    {
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(item->text());
        QDrag *mdrag = new QDrag(this);
        mdrag->setMimeData(mimeData);
        mdrag->exec();
    }
}
