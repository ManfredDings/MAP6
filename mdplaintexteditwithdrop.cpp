// Copyright 2016 Manfred Dings

#include <QMimeData>
#include <QFileInfo>
#include "mdplaintexteditwithdrop.h"
#include "mdlistwidgetwithdrop.h"

MDPlainTextEditWithDrop::MDPlainTextEditWithDrop
(QWidget *parent) : QPlainTextEdit(parent)
{
    setAcceptDrops(true);
}

MDPlainTextEditWithDrop::~MDPlainTextEditWithDrop()
{
}

void MDPlainTextEditWithDrop::dragEnterEvent(QDragEnterEvent *event)
{
    MDListWidgetWithDrop *source =
            qobject_cast<MDListWidgetWithDrop *>(event->source());
    if (source )
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        if (DragEventHasMP3File(event)) event->accept();
    }
}

void MDPlainTextEditWithDrop::dragMoveEvent(QDragMoveEvent *event)
{
    MDListWidgetWithDrop*source =
            qobject_cast<MDListWidgetWithDrop *>(event->source());
    if (source)
    {   event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        if (DragEventHasMP3File(event)) event->accept();
    }
}

bool MDPlainTextEditWithDrop::DragEventHasMP3File(QDragMoveEvent *event)
{
    if (event->mimeData()->hasText()) {
        const QMimeData *mime = event->mimeData();
        QString dropfilename = mime->text();

        QUrl u(dropfilename);
        dropfilename = u.toLocalFile();

        QFileInfo inf(dropfilename);
        if ((inf.completeSuffix() == csmp3))
        {
            return true;
        }
    } return false;
}

void MDPlainTextEditWithDrop::dropEvent(QDropEvent *event)
{
    MDListWidgetWithDrop *source =
            qobject_cast<MDListWidgetWithDrop *>(event->source());
    if (source)
    {
        appendPlainText(event->mimeData()->text());
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else

    {
        if (event->mimeData()->hasText()) {
            const QMimeData *mime = event->mimeData();
            QString dropfilename = mime->text();

            QUrl u(dropfilename);
            dropfilename = u.toLocalFile();

            QFileInfo inf(dropfilename);
            if ((inf.completeSuffix() == csmp3))
            {
                appendPlainText(event->mimeData()->text());
            }
            event->accept();
        }
    }
}
