// Copyright 2016 Manfred Dings

#include <QtCore>
#include "aufgabe.h"
#include <QFileInfo>
#include <QFile>

Aufgabe::Aufgabe(QObject *parent) :
    QObject(parent)
{
    QLocale loc;

    mDecimalPoint = loc.decimalPoint();
}

void Aufgabe::set_currentfilename(QString filename)
{
    mfilename = filename;
    emit auf_aufgabechanged(mfilename);
}

QString Aufgabe::getMcurrentdir() const
{
    return mcurrentdir;
}

void Aufgabe::setMcurrentdir(const QString &value)
{
    mcurrentdir = value;
}
QString Aufgabe::getMaudiofilename() const
{
    return maudiofilename;
}

void Aufgabe::setMaudiofilename(const QString &value)
{
    maudiofilename = value;
}
QString Aufgabe::getMtitle() const
{
    return mtitle;
}

void Aufgabe::setMtitle(const QString &value)
{
    mtitle = value;
}

QString Aufgabe::adjust_slashes(QString path)
{
  return path.replace("\\", "/");
}
QStringList Aufgabe::getMfulllist() const
{
    return mfulllist;
}

void Aufgabe::setMfulllist(const QStringList &value)
{
    mfulllist = value;
}
QStringList Aufgabe::getMkommentarlist() const
{
    return mkommentarlist;
}

void Aufgabe::setMkommentarlist(const QStringList &value)
{
    mkommentarlist = value;
}

QString Aufgabe::loadAufgabe()
{
    QString fn = mcurrentdir+"/"+mfilename;
    QFileInfo fi;
    if (fi.exists(fn)&&(mfilename != ""))
    {
        mliste.clear();
        mfulllist.clear();
        mtimelist.clear();
        mtext1.clear();
        mtext2.clear();
        msteuerfeld.clear();
        mdisplaylist.clear();
        mkommentarlist.clear();
        QFile file(fn);

        file.open(QIODevice::ReadOnly);
        QTextStream stream(&file);
        QString s;
        s = stream.readLine();
        s = s.simplified();
        mfulllist.append(s);
        mliste.append(s.split("#"));
        mtitle = mliste.value(0);
        maudiofilename = adjust_slashes(mcurrentdir+"/"+mliste.value(1));
        mliste.clear();
        while (!stream.atEnd())
        {
            s = stream.readLine();
            mliste.append(s);
            mfulllist.append(s);
        }
        file.close();
        splitAufgaben();
        return maudiofilename;
    }
    return "";
}

QStringList Aufgabe::getMliste() const
{
    return mliste;
}

void Aufgabe::setMliste(const QStringList &value)
{
    mliste = value;
}

void Aufgabe::splitAufgaben()
{
    mtimelist.clear();
    mtext1.clear();
    mtext2.clear();
    msteuerfeld.clear();
    mkommentarlist.clear();
    QStringList l;
    for (int i = 0; i < mliste.count(); ++i)
    {
        l.clear();
        l = mliste.value(i).split("%%");
        mkommentarlist.append(l.count() > 1 ? l.value(1) : "");
        l = l[0].split("#");
        mtimelist.append("");
        mtext1.append("");
        mtext2.append("");
        msteuerfeld.append("");
        for (int j=0; j < l.count(); j++)
        {
            switch (j) {
            case 0:
                mtimelist.replace(i, l.value(j));
                break;
            case 1:
                mtext1.replace(i, l.value(j));
                break;
            case 2:
                mtext2.replace(i, l.value(j));
                break;
            case 3:
                msteuerfeld.replace(i, l.value(j));
                break;
            }
        }
    }
    fillMilliseclist();
    fillDisplaylist();
}

void Aufgabe::fillDisplaylist()
{
    mdisplaylist.clear();
    for (int i = 0; i < mliste.count(); i++)
    {
        QTime time = QTime::fromMSecsSinceStartOfDay(mmilliseclist.value(i));
        QString s = time.toString("mm:ss"+mDecimalPoint+"zzz")+" ";
        QString s1 = mliste.value(i);
        int index = s1.indexOf("#");
        s1 = s1.mid(index);
        mdisplaylist.append(s+s1);
    }
}

void Aufgabe::fillMilliseclist()
{
    mmilliseclist.clear();
    for (int i = 0; i < mtimelist.count(); i++)
    {
        QString s = mtimelist.value(i);
        quint64 u = getMilliSeconds(s);
        mmilliseclist.append(u);
    }
}

double Aufgabe::getMilliSeconds(QString s)
{
    QLocale germ;
    int min = 0;
    double sec = 0.0;
    QStringList l = s.split(":");
    if (l.count() > 1)
    {
        min = l.value(0).toInt();
        // Unterscheiden, ob . oder , als Dezimaltrenner
        // notwendig wegen Abwärtskompatitiblität der
        // auf-Dateien (Aufgabendateien),
        // die womöglich "." als Trenner enthalten oder auch ","
        if (l.value(1).indexOf(".") < 0)
        sec = germ.toDouble(l.value(1));
        else
            sec = l.value(1).toFloat();
        return min*60*1000 + sec*1000.0;
    }
    else
        // Unterscheiden, ob . oder , als Dezimaltrenner
    {   if (s.indexOf(".") < 0)
        return germ.toDouble(l.value(0))*1000.0;
        else return l.value(0).toFloat()*1000.0;
    }
}

QString Aufgabe::output(quint64 millisec)
{
    QString  s = "";
    for (int i = 0; i < mmilliseclist.count(); i++)
    {
        if (millisec >= mmilliseclist.value(i))
        {
            s = compose(i);
        }
        else break;
    }
    return s;
}
QStringList Aufgabe::getMtimelist() const
{
    return mtimelist;
}

void Aufgabe::setMtimelist(const QStringList &value)
{
    mtimelist = value;
}
QList<quint64> Aufgabe::getMmilliseclist() const
{
    return mmilliseclist;
}

void Aufgabe::setMmilliseclist(const QList<quint64> &value)
{
    mmilliseclist = value;
}
QStringList Aufgabe::getMdisplaylist() const
{
    return mdisplaylist;
}

void Aufgabe::setMdisplaylist(const QStringList &value)
{
    mdisplaylist = value;
}

QString Aufgabe::compose(int index)
{
    const QString head = "<h";
    const QString para = "<p>";
    QString s = mtext1.value(index)+mtext2.value(index);
    bool isHTML = (s.indexOf(head) >= 0) || (s.indexOf(para) >= 0);
    if (!isHTML) s = "<h2>"+mtext1.value(index)+"</h2><p>"+mtext2.value(index);
    emit auf_aufgabepositionchanges(index);
    return s;
}
