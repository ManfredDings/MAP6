// Copyright 2016 Manfred Dings

#ifndef AUFGABENLISTE_H
#define AUFGABENLISTE_H
#include <QObject>
#include <QStringList>
#include <QMediaPlayer>

class Aufgabe;

class Aufgabenliste : public QObject
{
    Q_OBJECT
public:
    explicit Aufgabenliste(QObject *parent = 0, QString filename = "");

    QString get_currentfilename()const {return mfilename;}
    QString set_currentfilename(QString filename);
    // gibt zugleich Pfad zur aktuellen Aufgabe zurück
    QString set_index(int index);
    int get_index()const {return mindex;}
    int count()const {return mdataList.count();}
    void addAufgabe(QString filename);

    void loadFile(QString s);
    void saveFile();
    void deleteItem(int index);
    void move_item(int idex, int direc);
    void NewFile(QString filename);

    QString getMcurrentpath() const;
    void setMcurrentpath(const QString &value);

    QStringList getMdataList() const;
    void setMdataList(const QStringList &value);

signals:

    void af_filenamechanged(QString s);
    void af_listchanged(QStringList titelListe);
    void af_listindexchanged(int index);


private:
    QString mfilename, mcurrentpath;
    QStringList mdataList, mtitelliste;
    // aktuell gewählter Index
    int mindex;
};

#endif  // AUFGABENLISTE_H
