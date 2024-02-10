// Copyright 2016 Manfred Dings

#ifndef AUFGABE_H
#define AUFGABE_H
#include <QString>
#include <QObject>
#include <QStringList>

class Aufgabe : public QObject
{
    Q_OBJECT
public:
    explicit Aufgabe(QObject *parent = 0);

    QString get_currentfilename()const {return mfilename;}
    void set_currentfilename(QString filename);

    QString getMcurrentdir() const;
    void setMcurrentdir(const QString &value);

    QString getMaudiofilename() const;
    void setMaudiofilename(const QString &value);

    QString getMtitle() const;
    void setMtitle(const QString &value);

    QString loadAufgabe();

    void setMliste(const QStringList &value);
    QStringList getMliste() const;

    QString output(quint64 millisec);

    QStringList getMtimelist() const;
    void setMtimelist(const QStringList &value);

    QList<quint64> getMmilliseclist() const;
    void setMmilliseclist(const QList<quint64> &value);

    QStringList getMdisplaylist() const;
    void setMdisplaylist(const QStringList &value);

    static double getMilliSeconds(QString s);

    static QString adjust_slashes(QString path);
    QStringList getMfulllist() const;
    void setMfulllist(const QStringList &value);

    QStringList getMkommentarlist() const;
    void setMkommentarlist(const QStringList &value);

public slots:

signals:

    void auf_aufgabechanged(QString s);
    void auf_aufgabepositionchanges(int i);


private:
    QString mcurrentdir, mfilename, mtitle, maudiofilename;

    QStringList mfulllist,  // gesamter Aufgabentext
                mliste,  // Text der Aufgabendetails
                mtimelist,  // Liste mit millisecs der Aufgabendetails
                // Aufgabendetails, separiert nach #-Einheiten
                mtext1, mtext2, msteuerfeld,
                mdisplaylist,  // formatierte Aufgabendetails für Listenamzeige
                mkommentarlist;  // Kommentare zu Aufgaben
    QList <quint64> mmilliseclist;

    QString mDecimalPoint;

    void splitAufgaben();
    void fillMilliseclist();
    QString compose(int index);
    void fillDisplaylist();
};

#endif  // AUFGABE_H
