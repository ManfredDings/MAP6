
//  Copyright 2016 Manfred Dings

#ifndef MDLISTWIDGETWITHDROP_H
#define MDLISTWIDGETWITHDROP_H

#include <QObject>
#include <QListWidget>
#include <QDragEnterEvent>
#include <QMouseEvent>

class MDListWidgetWithDrop : public QListWidget
{
    Q_OBJECT

public:
    explicit MDListWidgetWithDrop(QWidget *parent = 0);
    ~MDListWidgetWithDrop();


// Q_SIGNALS:e
    void MDListWidgetWithDrop_dropped(QListWidget *sender);


protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    void performDrag();
    QPoint startPos;
};

#endif  // MDLISTWIDGETWITHDROP_H
