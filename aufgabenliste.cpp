// Copyright 2016 Manfred Dings

#include <QFileInfo>
#include <QStringList>
#include <QDir>
#include "aufgabe.h"
#include "aufgabenliste.h"

Aufgabenliste::Aufgabenliste(QObject *parent, QString filename) :
    QObject(parent)
{
    set_currentfilename(filename);
}

QString  Aufgabenliste::set_currentfilename(QString filename)
{
    mfilename = filename;
    QFileInfo info(mfilename);
    mcurrentpath = info.absolutePath();
    emit af_filenamechanged(mfilename);
    return mcurrentpath;
}

QString Aufgabenliste::set_index(int index)
{
    mindex = index;
    return mdataList.value(index);
}

void Aufgabenliste::addAufgabe(QString filename)
{
    QFileInfo inf(filename);
     if (inf.exists())
     {
         Aufgabe auf;
         QString fn = inf.fileName();
         if (!(inf.absoluteDir() == mcurrentpath)) return;

         QFileInfo in(mcurrentpath+"/"+fn);
         if (in.exists())
         {
             auf.setMcurrentdir(mcurrentpath);
             auf.set_currentfilename(fn);
             auf.loadAufgabe();
             mtitelliste.append(auf.getMtitle());
             mdataList.append(fn);
         }
     emit af_listchanged(mtitelliste);
     }
}

void Aufgabenliste::loadFile(QString s)
{
   QFileInfo inf;
    if (inf.exists(s))
    {
        set_currentfilename(s);
        QFile file(mfilename);
        file.open(QIODevice::ReadOnly);
        QTextStream stream(&file);
        mdataList.clear();
        mtitelliste.clear();
        Aufgabe auf;
        while (!stream.atEnd())
        {
            QString s = stream.readLine();
            auf.setMcurrentdir(mcurrentpath);
            auf.set_currentfilename(s);
            auf.loadAufgabe();
            mtitelliste.append(auf.getMtitle());
            mdataList.append(s);
        }
     file.close();
    emit af_listchanged(mtitelliste);
    }
}

void Aufgabenliste::saveFile()
{
    QFile file(mfilename);
    file.open(QIODevice::WriteOnly);
    QTextStream stream(&file);
    for (int i = 0; i < mdataList.count(); i++)
        stream << mdataList.value(i) << Qt::endl;
    file.close();
}

void Aufgabenliste::NewFile(QString filename)
{
    mdataList.clear();
    mtitelliste.clear();
    set_currentfilename(filename);
    saveFile();
    emit af_listchanged(mtitelliste);
}

void Aufgabenliste::deleteItem(int index)
{
    mdataList.removeAt(index);
    mtitelliste.removeAt(index);
    emit af_listchanged(mtitelliste);
}

void Aufgabenliste::move_item(int idex, int direc)
{
    int newIdex = idex+direc;
    newIdex = newIdex % mdataList.count();
    if (newIdex < 0) newIdex = newIdex + mdataList.count();
    mdataList.move(idex, newIdex);
    mtitelliste.move(idex, newIdex);
    emit af_listchanged(mtitelliste);
}
QString Aufgabenliste::getMcurrentpath() const
{
    return mcurrentpath;
}

void Aufgabenliste::setMcurrentpath(const QString &value)
{
    mcurrentpath = value;
}
QStringList Aufgabenliste::getMdataList() const
{
    return mdataList;
}

void Aufgabenliste::setMdataList(const QStringList &value)
{
    mdataList = value;
}
