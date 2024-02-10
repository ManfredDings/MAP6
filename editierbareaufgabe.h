// Copyright 2016 Manfred Dings

#ifndef EDITIERBAREAUFGABE_H
#define EDITIERBAREAUFGABE_H
#include <QString>
#include <QWidget>


class EditierbareAufgabe
{
public:
    EditierbareAufgabe();
    ~EditierbareAufgabe();


    QString getMstring() const;
    void setMstring(const QString &value);

    QString getFilename() const;
    void setFilename(const QString &value);

    void saveToFile(QString stringToBeSaved, QString afilename);

    bool getHasChanged() const;
    void setHasChanged(bool value);

    QString title();

private:
    QString mstring;
    QString filename;
    bool hasChanged;
};

#endif  //  EDITIERBAREAUFGABE_H
