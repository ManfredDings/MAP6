// Copyright 2016 Manfred Dings

#include "editierbareaufgabe.h"
#include <QFile>
#include <QTextStream>
#include <QFileDialog>

EditierbareAufgabe::EditierbareAufgabe()
{
}

EditierbareAufgabe::~EditierbareAufgabe()
{
}

QString EditierbareAufgabe::getMstring() const
{
    return mstring;
}

void EditierbareAufgabe::setMstring(const QString &value)
{
    mstring = value;
}
QString EditierbareAufgabe::getFilename() const
{
    return filename;
}

void EditierbareAufgabe::setFilename(const QString &value)
{
    filename = value;
    setHasChanged(false);
}

void EditierbareAufgabe::saveToFile(QString stringToBeSaved, QString afilename)
{
    setFilename(afilename);
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    QTextStream stream(&file);
    stream << stringToBeSaved;
    file.close();
    setHasChanged(false);
}

bool EditierbareAufgabe::getHasChanged() const
{
    return hasChanged;
}

void EditierbareAufgabe::setHasChanged(bool value)
{
    hasChanged = value;
}

QString EditierbareAufgabe::title()
{
    QString s = getFilename();
    if (getHasChanged()) s = s+"*";
    return s;
}
