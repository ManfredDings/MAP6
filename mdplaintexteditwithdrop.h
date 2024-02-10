
//  Copyright 2016 Manfred Dings

#ifndef MDPLAINTEXTEDITWITHDROP_H
#define MDPLAINTEXTEDITWITHDROP_H

#include <QObject>
#include <QWidget>
#include <QPlainTextEdit>

class MDPlainTextEditWithDrop : public QPlainTextEdit
{
    Q_OBJECT

    bool DragEventHasMP3File(QDragMoveEvent *event);
    const QString csmp3 = "mp3";
public:
    explicit MDPlainTextEditWithDrop(QWidget *parent = 0);
    ~MDPlainTextEditWithDrop();
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
};

#endif  // MDPLAINTEXTEDITWITHDROP_H
