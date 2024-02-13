// Copyright 2016 Manfred Dings

#include <qfiledialog.h>
#include <QMediaPlayer>
#include <QDesktopServices>
#include <QMimeData>
#include <QMediaMetaData>
#include <QTime>
#include <QtMath>
#include <QMessageBox>
#include <QSettings>
#include <QtWidgets>
#include <QUrl>
#include "aufgabe.h"
#include "dialog.h"
#include "mdutils.h"
#include "about.h"
//#include "aboutdlg.h"
#include "mdlistwidgetwithdrop.h"
#include "mapmainwindow.h"
#include "ui_mapmainwindow.h"


// Max. Länge für die Pfade in der Statusleiste
const int CMAXPATHLENFORDISPLAY = 88;
// Auflösung des senkrechten Schiebers für die Lautstärke
const double cVolumeRange = 200.0;

MapMainWindow::MapMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MapMainWindow)
{
    // Programmeinstellungen

    QLocale loc;

    mWasMutedBevoreSliderMoving = false;

    mDecimalPoint = loc.decimalPoint();

    ui->setupUi(this);
    // Farbe der Uhrzeitanzeige setzen
    ui->labelTime->setStyleSheet("color: blue");
    ui->textBrowser_2->setVisible(false);
    ui->textBrowser_2->setEnabled(false);

    ui->lblKommentare->setVisible(false);

    ui->textBrowser->setAcceptDrops(false);
    setAcceptDrops(true);

    statuslabel = new QLabel(this);
    QFont font = statuslabel->font();
    font.setPointSize(8);
    statuslabel->setFont(font);

    statuslabel2 = new QLabel(this);
    statuslabel2->setFont(statuslabel->font());

    statuslabel3 = new QLabel(this);
    statuslabel3->setFont(statuslabel->font());
    ui->statusBar->addWidget(statuslabel3, 0);

    ui->statusBar->addWidget(statuslabel, 2);

    ui->statusBar->addWidget(statuslabel2, 2);

    separatorAction = ui->menuDatei->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i],
                SIGNAL(triggered()), this, SLOT(openRecentFile()));
        ui->menuDatei->addAction(recentFileActions[i]);
    }
    ui->btnMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    ui->stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui->pauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->btnPrevious->setIcon(style()
                             ->standardIcon(QStyle::SP_MediaSeekBackward));
    ui->btnNext->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    ui->btnFadeIn->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
    ui->btnFadeOut->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));
    ui->btnToStandardvolume->
            setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));

    mplayer = new QMediaPlayer(this);
    mAudioOutput = new QAudioOutput(this);
    mplayer->setAudioOutput(mAudioOutput);

    connect(mplayer, SIGNAL(positionChanged(qint64)),
            this, SLOT(on_mediapositionchanges()));
    connect(mplayer, SIGNAL(playbackStateChanged(QMediaPlayer::PlaybackState)),
            this, SLOT(onPlaybackstateChanged(QMediaPlayer::PlaybackState)));
    connect(mplayer, SIGNAL(durationChanged(qint64)),
            this, SLOT(on_mediadurationchanges()));
    connect(mAudioOutput, SIGNAL(volumeChanged(float)),
            this, SLOT(on_mediavolumechanges()));
    connect(mplayer, SIGNAL(playbackRateChanged(qreal)),
            this, SLOT(onPlaybackRateChanged(qreal)));
    connect(mplayer, SIGNAL(hasAudioChanged(bool)),
            this, SLOT(on_audioavailablechanges(bool)));
    connect(ui->pauseButton, SIGNAL(clicked()),
            this, SLOT(on_actionActionPause_triggered()));
    connect(ui->pushButtonClear_3, SIGNAL(clicked(bool)),
            this, SLOT(on_actionAufgabe_in_Editor_laden_triggered()));
    connect(ui->pushButtonClear_4, SIGNAL(clicked(bool)),
            this, SLOT(on_actionAufgabe_speichern_triggered()));
    connect(ui->pushButtonClear_5, SIGNAL(clicked(bool)),
            this, SLOT(on_actionAufgabe_speichern_unter_triggered()));
    connect(ui->pushButtonClear_6, SIGNAL(clicked(bool)),
            this, SLOT(on_actionAufgabe_schlie_en_triggered()));
    connect(ui->stopButton, SIGNAL(clicked()),
            this , SLOT(on_actionStop_triggered()));
    connect(ui->btnNext, SIGNAL(clicked()),
            this, SLOT(on_actionNext_triggered()));
    connect(ui->btnPrevious, SIGNAL(clicked()),
            this, SLOT(on_actionPrevious_triggered()));
    connect(ui->btnFadeIn, SIGNAL(clicked()),
            this, SLOT(on_actionFade_in_triggered()));
    connect(ui->btnFadeOut, SIGNAL(clicked()),
            this, SLOT(on_actionFade_out_triggered()));
    connect(ui->btnFadeStop, SIGNAL(clicked()),
            this, SLOT(on_actionFading_anhalten_triggered()));
    connect(ui->btnVolStandard, SIGNAL(clicked()),
            this, SLOT(on_actionStandardvolumen_festlegen_triggered()));
    connect(ui->actionStandardvolumen_anwenden, SIGNAL(triggered()),
            this, SLOT(on_btnToStandardvolume_clicked()));
    connect(ui->btnDeleteAufgabe, SIGNAL(clicked()),
            this, SLOT(on_actionAufgabe_l_schen_triggered()));
    connect(ui->btnAufgabenlistespeichern, SIGNAL(clicked()),
            this, SLOT(on_actionAufgabenliste_speichern_triggered()));
    connect(ui->btnAufgabenlisteNeuLaden, SIGNAL(clicked()),
            this, SLOT(on_actionAufgabenliste_neu_laden_triggered()));
    connect(ui->btnAufgabeUp, SIGNAL(clicked()),
            this, SLOT(on_actionAufgabe_nach_oben_triggered()));
    connect(ui->btnAufgabeDown, SIGNAL(clicked()),
            this, SLOT(on_actionAufgabe_nach_unten_triggered()));
    connect(ui->btnAufgabeHinzuf, SIGNAL(clicked()),
            this, SLOT(on_actionAufgabe_hinzuf_gen_triggered()));
    connect(ui->actionInternen_Hilfebrowser_verwenden,
            SIGNAL(triggered(bool)),
            SLOT(actionInternen_Hilfebrowser_verwenden_triggerd()));

    setupPopupMenus();

    mPlaybackRateText = " <0>";

    ui->vslider->setMaximum(cVolumeRange);

    mPopupMain = new QMenu(this);
    mPopupMain->addAction(ui->actionZum_Hauptfenster_bewegen);
    mPopupMain->addAction(ui->actionMen_leiste);

    mtimer = new QTimer(this);
    connect(mtimer, SIGNAL(timeout()), this, SLOT(onTimer()));
    mtimer->setInterval(100);  //  millisec
    mtimer->start();
    mFadeTimer = new QTimer(this);
    mFadeTimer->setInterval(CFADINGTIMERINTERVAL);
    connect(mFadeTimer, SIGNAL(timeout()), this, SLOT(onFadeTimer()));
    setIsFading(0);
    QString formatstrs = "00:00" % mDecimalPoint % "000 | 00:00 | 00:00 ";
    mlabeltext = formatstrs+mPlaybackRateText;
    // Nach setupUi (nicht davor) Einstellungen laden
    msettings = new QSettings();
    maufgabenliste = new Aufgabenliste(this,
                msettings->value("Aufgabenliste", "").toString());
    mcurrent_aufgabe = new Aufgabe(this);

    zweitfenster = new Dialog(this, msettings);
    zweitfenster->setModal(false);
    setShowZweitfenster(msettings->value("showZweitfenster", false).toBool());
    connect(zweitfenster, SIGNAL(zwStyleChanged()),
            this, SLOT(onZweitfensterStyleChanged()));
    setMsynchWindows(msettings->value("syncWindows", true).toBool());

    int i = msettings->value("zweitfenster/WindowStyle", 0).toInt();
    mZweitfensterStyles = static_cast<enStyles>(i);
    zweitfenster->setStyles(sStyles[i], sTextBrowserStyles[i]);

    restoreSettings();
    ui->actionBeenden->setShortcut(tr("Alt+F4"));

    connect(maufgabenliste, SIGNAL(af_filenamechanged(QString)),
            this, SLOT(onAufgabenlisteFilenamechanged(QString)));
    connect(maufgabenliste, SIGNAL(af_listchanged(QStringList)),
            this, SLOT(onAufgabenlisteChanged(QStringList)));
    connect(mcurrent_aufgabe, SIGNAL(auf_aufgabechanged(QString)),
            this, SLOT(onAufgabechanged()));
    connect(mcurrent_aufgabe, SIGNAL(auf_aufgabepositionchanges(int)),
            this, SLOT(on_aufgabepositionchanges(int)));
    connect(ui->actionMute, SIGNAL(triggered()),
            this, SLOT(on_btnMute_clicked()));

    // Actions zum Hauptfenster hinzufügen, falls Menü ausgeblendet
    this->addAction(ui->actionPrevious);
    this->addAction(ui->actionNext);
    this->addAction(ui->actionSchriftart);
    this->addAction(ui->actionAuf_Stoppuhr_verkleinern);
    this->addAction(ui->actionAufgabe_in_Editor_laden);
    this->addAction(ui->actionBei_Stop_Leinwand_l_schen);
    this->addAction(ui->actionKommentare_zeigen);
    this->addAction(ui->actionAufgabe_speichern);
    this->addAction(ui->actionAufgabe_speichern_unter);
    this->addAction(ui->actionAufgabe_schlie_en);
    this->addAction(ui->actionEditor_anzeigen);
    this->addAction(ui->actionListe_l_schen);
    this->addAction(ui->actionPickup);
    this->addAction(ui->actionAufgabendetails_zeigen);
    this->addAction(ui->actionN_chste_Teilaufgabe);
    this->addAction(ui->action_Vorherige_Teilaufgabe);
    this->addAction(ui->actionAufgabendateiinfos_zeigen);
    this->addAction(ui->actionDateiinformationen_zeigen);
    this->addAction(ui->actionStatusleiste);
    this->addAction(ui->actionZu_Cueingpoint_gehen);
    this->addAction(ui->actionGanztonTiefer);
    this->addAction(ui->actionHalbton_tiefer);
    this->addAction(ui->actionHaltbon_h_her);
    this->addAction(ui->actionGanzton_h_her);
    this->addAction(ui->actionNormales_Tempo);
    this->addAction(ui->actionZum_Hauptfenster_bewegen);
    this->addAction(ui->actionAufgabenliste_bearbeiten);
    this->addAction(ui->actionAufgabe_bearbeiten);
    this->addAction(ui->actionAufgabenliste_neu_laden);
    this->addAction(ui->actionAufgabe_hinzuf_gen);
    this->addAction(ui->actionAufgabe_nach_oben);
    this->addAction((ui->actionAufgabe_nach_unten));
    this->addAction(ui->actionAufgabenliste_speichern_als);
    this->addAction(ui->actionAufgabe_l_schen);
    this->addAction(ui->actionAufgabenliste_speichern);
    this->addAction(ui->actionBeenden);
    this->addAction(ui->actionLeinwand_l_schen);
    this->addAction(ui->actionMute);
    this->addAction(ui->actionZweitfenster);
    this->addAction(ui->actionMen_leiste);
    this->addAction(ui->actionActionPause);
    this->addAction(ui->actionStop);
    this->addAction(ui->actionAufgabenliste_laden);
    this->addAction(ui->actionLauter);
    this->addAction(ui->actionLeiser);
    this->addAction(ui->actionFade_in);
    this->addAction(ui->actionFade_out);
    this->addAction(ui->actionFading_anhalten);
    this->addAction(ui->actionStandardvolumen_festlegen);
    this->addAction(ui->actionStandardvolumen_anwenden);
    this->addAction(ui->actionNormalansicht);
    this->addAction(ui->actionLektionsansicht);
    this->addAction(ui->actionNur_Stoppuhr_2);
    this->addAction((ui->actionKeine_Stoppuhr));
    this->addAction(ui->actionGr_er);
    this->addAction(ui->actionKleiner);
    this->addAction(ui->actionEnger);
    this->addAction(ui->actionWeiter);
    this->addAction(ui->actionStandardgr_e);
    this->addAction(ui->actionNach_rechts);
    this->addAction(ui->actionNach_links);
    this->addAction(ui->actionNach_oben);
    this->addAction(ui->actionNach_untern);
    this->addAction(ui->actionSchnell_rechts);
    this->addAction(ui->actionSchnell_links);
    this->addAction(ui->actionSchnell_oben);
    this->addAction(ui->actionSchnell_unten);
    this->addAction(ui->actionInternen_Hilfebrowser_verwenden);

    mStoppuhronly = false;
    ui->actionNur_Stoppuhr_2->setChecked(mStoppuhronly);
    mlblTimeGeometry = ui->lblTime->geometry();
    recentFiles = msettings->value("recentFiles").toStringList();
    UpdateRecentFileActions();
    if (recentFiles.count() > 0) emit recentFileActions[0]->triggered();

    QString afl = msettings->value(
                "Aufgabenliste", "").toString();
    maufgabenliste->set_currentfilename(afl);

    QRegularExpression rx("(([0-5]?[0-9]:)?[0-5][0-9](["
               % mDecimalPoint % "][0-9]?[0-9]?[0-9]?)?)");
    QRegularExpressionValidator *valTime;
    valTime = new QRegularExpressionValidator(rx, this);
    ui->lineEditCurrentTime->setValidator(valTime);

    mtempoState = ts_norm;
    changePlaybackDisplay(1, false);
    ui->labelMediaPos->setText(mlabeltext);
    mhelpb = new HelpBrowser(msettings, this);

    mplayer->stop();

    QString sdummy;
    // Beim ersten Programmstart Beispieldatei laden
    if (first_start)
    {
      mDocPath = QStandardPaths::locate(QStandardPaths::DocumentsLocation, sdummy, QStandardPaths::LocateDirectory);
      mDocPath = mDocPath + SBeispielAufgabenliste;
      // Beispieldatei laden wird wohl nur unter Windows funktionieren.
      if (QFileInfo::exists(mDocPath))
      {
        AufgabenlisteLaden(mDocPath);
      }
      else
      {
          QMessageBox::warning(this, "MAP","Beispieldatei Dokumente/MAP/Beispieldateien/Sample.afl konnte nicht geladen werden");
      }
      // ... und nachfragen, ob das Zweitfenster dargestellt werden soll
      bool ZweitfensterVisible = getShowZweitfenster();
      if (!ZweitfensterVisible)
      {
          bool showPopup = (QMessageBox::question(this, "MAP",
                                                  "Möchten Sie das Zweitfenster einschalten? Dies geht auch später mit F3.", QMessageBox::Yes | QMessageBox::No)==QMessageBox::Yes);
          if (!ZweitfensterVisible && showPopup) setShowZweitfenster(true);
      }
    }
    // Auf jeden Fall den Play-Button enablen. Ist unschädlich, auch wenn keine Datei geladen wurde.
    // Sonst kann es bei leerer Registry (erster Programmstart) Probleme geben.
    on_audioavailablechanges(true);
}

// Läd eine Aufgabenliste
void MapMainWindow::AufgabenlisteLaden(QString s)
{
    mplayer->stop();
    maufgabenliste->loadFile(s);
    recentFiles.removeAll(s);
    recentFiles.prepend(s);
    UpdateRecentFileActions();
    statuslabel->clear();
    statuslabel2->clear();
    statuslabel->setText("");
    statuslabel2->setText("");
    setInfo();
}

QString MapMainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MapMainWindow::UpdateRecentFileActions()
{
    QMutableStringListIterator i(recentFiles);
    while (i.hasNext())
    {
        if (!QFile::exists(i.next()))
            i.remove();
    }
    for (int j = 0; j < MaxRecentFiles; ++j)
    {
        if (j < recentFiles.count())
        {
            QString text = tr("&%1 %2").arg(j+1)
                    .arg(recentFiles[j]);
            recentFileActions[j]->setText(text);
            recentFileActions[j]->setData((recentFiles[j]));
            recentFileActions[j]->setVisible(true);
        }
        else {
            recentFileActions[j]->setVisible(false);
        }
    }
    separatorAction->setVisible(!recentFiles.isEmpty());
}

void MapMainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        mplayer->stop();
        QString s = action->data().toString();
        if (QFile::exists(s))
        {
            AufgabenlisteLaden(s);
        }
    }
}

void MapMainWindow::on_aufgabepositionchanges(int index)
{
    ui->lwAufgabenDetails->setCurrentRow(index);
    ui->lblKommentare->
            setText(mcurrent_aufgabe->getMkommentarlist().value(index));
    ui->lblKommentare->setVisible(
                (mcurrent_aufgabe->getMkommentarlist().value
                 (index) != "") && getMkommentareZeigen());
}

void MapMainWindow::closeEvent(QCloseEvent *event)
{
    mplayer->stop();
    zweitfenster->closeEvent(event);
    saveSettings();
    QMainWindow::closeEvent(event);
}

bool MapMainWindow::set_mname_currentmp3file(QString s, bool open_file)
{
    QFileInfo info;
    if (info.exists(s))
    {
        mname_currentmp3file = s;
        if (open_file) openMp3File(mname_currentmp3file);
        return true;
    }
    return false;
}

void MapMainWindow::setBrowserSize(int x, int y)
{
    QRect rect;
    zweitfenster->vergroessern(x, y, rect);
    if (getMsynchWindows())
        ui->textBrowser->setFixedSize(rect.width(), rect.height());
}

void MapMainWindow::resizeEvent(QResizeEvent *event)
{
    {
        int l = ui->hSlider->geometry().left();
        ui->hSlider->setFixedWidth(this->width()-2 * l);


        const int spc = 5;
        int statusbarheight = 0;
  //      if (ui->statusBar->isVisible())
        {
            statusbarheight = ui->statusBar->height();
        };

        double fac = 1.0;
        if (ui->lwAufgabenDetails->isVisible()){
            fac = 1.45;
        }

        int aheight = (this->height()-ui->lwAufgaben->y()) / fac;

        ui->lwAufgaben->setFixedHeight(aheight - 2* statusbarheight);

        int diff = ui->listWidget->geometry().top() - ui->lwAufgaben->geometry().top();
        ui->listWidget->setGeometry(ui->listWidget->geometry().left(),ui->listWidget->geometry().top(),
                                    ui->listWidget->width(),ui->lwAufgaben->geometry().height()-diff);

        int ay = ui->lwAufgaben->geometry().bottom();// +  aheight +1;
        int ax = ui->lwAufgabenDetails->geometry().left();
        int aw = ui->lwAufgabenDetails->geometry().width();
        int ah = this->height() - statusbarheight - ay;
        ui->lwAufgabenDetails->setGeometry(ax, ay, aw, ah);

        l = ui->groupBox->geometry().left();
        int t = ui->groupBox->geometry().top();
        int h = this->height() - statusbarheight - ui->label_2->height() - t - spc ;
        ui->groupBox->setFixedWidth(this->width()-l-2);
        ui->groupBox->setFixedHeight(h);
        ui->lblKommentare->setFixedWidth(ui->groupBox->geometry().width());

        ui->lwAufgabenDetails->setGeometry(ax, ay, aw, ui->groupBox->geometry().bottom() - ay);

        QString s_html;
        if (showZweitfenster)
        {
            s_html = "<font color=#007015>Popup";
            QFont font = ui->lblTime->font();
            font.setItalic(false);
            ui->lblTime->setFont(font);
        }
        else
        {
            s_html = "<font color=#dd0000>Popup";
            QFont font = ui->lblTime->font();
            font.setItalic(true);
            ui->lblTime->setFont(font);
        }
        statuslabel->setTextFormat(Qt::RichText);
        statuslabel3->setText(s_html);
        if ( !getMsynchWindows() )
        {
            int l = ui->textBrowser->geometry().left();
            ui->textBrowser->setFixedWidth(this->width()-l-11);
        }
        else setBrowserSize(0, 0);
    }
    QMainWindow::resizeEvent(event);
}

void MapMainWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    // Zweimal, sonst falsche Darstellung beim Programmstart
    resizeEvent(0);
    resizeEvent(0);
}

bool MapMainWindow::checkWindowStandardHeight(
        int currentheight, int targetheight)
{
    const int RANGE = 5;
    int diff = qAbs(currentheight - targetheight);
    return diff < RANGE;
}

void MapMainWindow::restoreSettings()
{
    // Beim ersten Mal Fensterposition nicht auslesen
    first_start = msettings->value("first_start", true).toBool();
    if (!first_start)
    {
        // Fensterposition wiederherstellen
        restoreGeometry(msettings->value("geometry").toByteArray());
        restoreState(msettings->value("windowState").toByteArray());
        move(msettings->value("pos", pos() ).toPoint());
    }
    // In Variable speichern, weil das Menu nicht sofort sichtbar sein wird,
    // denn wir sind im Konstruktor
    bool menubarvisible = msettings->value("Menubarvisible", true).toBool();
    ui->menuBar->setVisible(menubarvisible);
    ui->actionMen_leiste->setChecked(menubarvisible);
    bool statusbarvisible = msettings->value("Statusbarvisible", true).toBool();
    ui->statusBar->setVisible(statusbarvisible);
    setuse_internal_browser(msettings->value(
                              "InternenHilfebrowserVerwenden", true).toBool());
    ui->actionStatusleiste->setChecked(statusbarvisible);
    set_mname_currentmp3file(msettings->value("currentmp3", "").toString());
    setVolume(msettings->value("volume", cVolumeRange/2).toInt());
    setVolumeStandard(msettings->value("volumeStandard", cVolumeRange/2).toInt());
    setStyles(static_cast<enStyles>(msettings->
                                    value("WindowStyle", 1).toInt()));
    setShowFileInformations(msettings->
                   value("showFileInformations", true).toBool());
    setShowAufgabenFileInformations(msettings->
                   value("showAufgabenFileInformations", true).toBool());
    setMaufgabendetailszeigen(msettings->value("showAufgabenDetials",
                                               true).toBool());
    setMeditorZeigen(msettings->value("Aufgabeneditor", true).toBool());
    setMkommentareZeigen(msettings->value("Kommentare", true).toBool());
    setClearSreenWhenStop(msettings->value("ClearScreenAtStop",
                                           false).toBool());
    setmStoppuhrsize(msettings->value("NurStoppuhr", false).toBool());
    setKeineStoppuhr(msettings->value("KeineStoppuhr", false).toBool());
    QFont font = ui->textBrowser->font();

    QString fontFamily = msettings->value(
                "font.family", font.family()).toString();
    int fontSize = msettings->value("font.size", font.pointSize()).toInt();
    bool fontIsBold = msettings->value("font.bold", font.bold()).toBool();
    bool fontIsItalic = msettings->value("font.italic", font.italic()).toBool();
    font.setFamily(fontFamily);
    font.setPointSize(fontSize);
    font.setBold(fontIsBold);
    font.setItalic(fontIsItalic);
    fontsetzen(font);
}
bool MapMainWindow::showAufgabenFileInformations() const
{
    return mShowAufgabenFileInformations;
}

void MapMainWindow::setShowAufgabenFileInformations
    (bool showAufgabenFileInformations)
{
    mShowAufgabenFileInformations = showAufgabenFileInformations;
    ui->actionAufgabendateiinfos_zeigen->
            setChecked(mShowAufgabenFileInformations);
    setInfo();
}

bool MapMainWindow::getMsynchWindows() const
{
    return msynchWindows;
}

void MapMainWindow::setMsynchWindows(bool value)
{
    msynchWindows = value;
    ui->actionMit_Zweitfenster_s_ynchronisieren->setChecked(msynchWindows);
    resizeEvent(0);
}

bool MapMainWindow::showFileInformations() const
{
    return mShowFileInformations;
}

void MapMainWindow::  setShowFileInformations(bool showFileInformations)
{
    mShowFileInformations = showFileInformations;
    ui->actionDateiinformationen_zeigen->setChecked(mShowFileInformations);
    setInfo();
}

int MapMainWindow::volumeStandard() const
{
    return mVolumeStandard;
}

void MapMainWindow::setVolumeStandard(int volumeStandard)
{
    mVolumeStandard = volumeStandard;
    int vlm = trunc(mVolume * 100.0 / cVolumeRange);
    QString s = QString::number(vlm);
    ui->btnVolStandard->setText(s);
}
bool MapMainWindow::stoppuhronly() const
{
    return mStoppuhronly;
}

void MapMainWindow::setStoppuhronly(bool stoppuhronly)
{
    mStoppuhronly = stoppuhronly;
}

void MapMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText() && (event->source() != ui->listWidget))
        event->acceptProposedAction();
    else event->ignore();
}

void MapMainWindow::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasText()) {
        const QMimeData *mime = event->mimeData();
        QString dropfilename = mime->text();

        QUrl u(dropfilename);
        dropfilename = u.toLocalFile();

        QFileInfo inf(dropfilename);
        if ((inf.completeSuffix() == "auf"))
        {
            maufgabenliste->addAufgabe(dropfilename);

            event->acceptProposedAction();
        }
        else if ((inf.completeSuffix() == "afl"))
        {
            if (QFile::exists(dropfilename))
            {
                mplayer->stop();
                maufgabenliste->loadFile(dropfilename);
                recentFiles.removeAll(dropfilename);
                recentFiles.prepend(dropfilename);
                UpdateRecentFileActions();
            }
            event->acceptProposedAction();
        }
    }
}
bool MapMainWindow::getmStoppuhrsize() const
{
    return mStoppuhrsize;
}

void MapMainWindow::setmStoppuhrsize(bool value)
{
    mStoppuhrsize = value;
    ui->actionAuf_Stoppuhr_verkleinern->setChecked(mStoppuhrsize);
    if (mStoppuhrsize) ui->actionAuf_Stoppuhr_verkleinern->
            setText("Stoppuhransicht abschalten");
    else ui->actionAuf_Stoppuhr_verkleinern->
            setText("Auf Stoppuhr verkleinern");
    ui->textBrowser_2->setVisible(mStoppuhrsize);
    ui->actionKleiner->setEnabled(!mStoppuhrsize);
    ui->actionGr_er->setEnabled(!mStoppuhrsize);
    ui->actionEnger->setEnabled(!mStoppuhrsize);
    ui->actionWeiter->setEnabled(!mStoppuhrsize);
    zweitfenster->NurStoppuhr(mStoppuhrsize);
    resizeEvent(0);
}


int MapMainWindow::isFading() const
{
    return mIsFading;
}

void MapMainWindow::setIsFading(int isFading)
{
    mIsFading = isFading;
    ui->btnFadeStop->setEnabled(mIsFading != 0);
    ui->actionFading_anhalten->setEnabled(ui->btnFadeStop->isEnabled());
}

int MapMainWindow::volume() const
{
    return mVolume;
}

void MapMainWindow::setVolume(int volume)
{
    mVolume = volume;
    ui->vslider->setValue(mVolume);
    applyVolume(mVolume);
    QLocale loc;
    QString s;
    double x = CurrentVolumeToDecibels();
    s = loc.toString(x, 'f', 1);
    if (x>=0) {s = "   " % s; }
    else {
        if (x >- 100) s = " " % s;
        if (x >- 10) s = " " % s;
    }

    int vlm = trunc(mVolume * 100.0 / cVolumeRange);
    QString vls = QString::number(vlm);
    if (cVolumeRange>999)
    {
        if (vlm < 1000) vls = " " % vls;
    }
    if (vlm < 100) vls = " " % vls;
    if (vlm < 10) vls = " " % vls;

    ui->lbldB->setText(s % " dB "% vls % "%");
    bool enable = volume < volumeStandard();
    ui->btnFadeIn->setEnabled(enable);
    ui->actionFade_in->setEnabled(enable);
    enable = volume < cVolumeRange;
    ui->actionLauter->setEnabled(enable);
    enable = volume > 0;  // > 0 funktioniert nicht
    ui->btnFadeOut->setEnabled(enable);
    ui->actionFade_out->setEnabled(enable);
    ui->actionLeiser->setEnabled(enable);
}

double MapMainWindow::CurrentVolumeToDecibels()
{
  return QAudio::convertVolume(mAudioOutput->volume(),
       QAudio::LogarithmicVolumeScale, QAudio::DecibelVolumeScale);
 }

void MapMainWindow::saveSettings()
{
    // Der erste Programmlauf ist rum
    first_start = false;
    msettings->setValue("first_start", first_start);
    // Fensterposition speichern
    msettings->setValue("geometry", saveGeometry());
    msettings->setValue("windowState", saveState());
    msettings->setValue("Menubarvisible", ui->menuBar->isVisible());
    msettings->setValue("Statusbarvisible", ui->statusBar->isVisible());
    msettings->setValue("InternenHilfebrowserVerwenden", use_internal_browser);
    msettings->setValue("currentmp3", mname_currentmp3file);
    msettings->setValue("Aufgabenliste", maufgabenliste->get_currentfilename());
    msettings->setValue("recentFiles", recentFiles);
    msettings->setValue("showZweitfenster", showZweitfenster);
    msettings->setValue("volumeStandard", volumeStandard());
    msettings->setValue("volume", mVolume);
    msettings->setValue("WindowStyle", mStyles);
    msettings->setValue("zweitfenster/WindowStyle", mZweitfensterStyles);
    msettings->setValue("showFileInformations", showFileInformations());
    msettings->setValue("showAufgabenFileInformations",
                        showAufgabenFileInformations());
    msettings->setValue("syncWindows", getMsynchWindows());
    msettings->setValue("showAufgabenDetials", getMaufgabendetailszeigen());
    msettings->setValue("Aufgabeneditor", getMeditorZeigen());
    msettings->setValue("Kommentare", getMkommentareZeigen());
    msettings->setValue("ClearScreenAtStop", clearSreenWhenStop());
    msettings->setValue("NurStoppuhr", getmStoppuhrsize());
    msettings->setValue("KeineStoppuhr", getKeineStoppuhr());
    QFont font = ui->textBrowser->font();
    msettings->setValue("font.family", font.family());
    msettings->setValue("font.size", font.pointSize());
    msettings->setValue("font.bold", font.bold());
    msettings->setValue("font.italic", font.italic());
}

MapMainWindow::~MapMainWindow()
{
    delete ui;
}

void MapMainWindow::openMp3File(QString filename)
{
    mplayer->setSource(QUrl::fromLocalFile(filename));
}

QString MapMainWindow::millisecs2QTime(quint64 msec, QString &withmillisec)
{
    QTime time = QTime::fromMSecsSinceStartOfDay(msec);
    withmillisec = time.toString("mm:ss" % mDecimalPoint % "zzz");
    return time.toString("mm:ss");
}
bool MapMainWindow::clearSreenWhenStop() const
{
    return mClearSreenWhenStop;
}

void MapMainWindow::setClearSreenWhenStop(bool clearSreenWhenStop)
{
    mClearSreenWhenStop = clearSreenWhenStop;
    ui->actionBei_Stop_Leinwand_l_schen->setChecked(mClearSreenWhenStop);
}

bool MapMainWindow::getMkommentareZeigen() const
{
    return mkommentareZeigen;
}

void MapMainWindow::setMkommentareZeigen(bool value)
{
    mkommentareZeigen = value;
    ui->actionKommentare_zeigen->setChecked(mkommentareZeigen);
}

bool MapMainWindow::getMeditorZeigen() const
{
    return mEditorZeigen;
}

void MapMainWindow::setMeditorZeigen(bool value)
{
    mEditorZeigen = value;
    ui->groupBox->setVisible(mEditorZeigen);
    ui->actionEditor_anzeigen->setChecked(mEditorZeigen);
    ui->actionAufgabe_in_Editor_laden->setEnabled(mEditorZeigen);
    ui->actionAufgabe_speichern->setEnabled(mEditorZeigen);
    ui->actionAufgabe_speichern_unter->setEnabled(mEditorZeigen);
    ui->actionAufgabe_schlie_en->setEnabled(mEditorZeigen);
}

bool MapMainWindow::getuse_internal_browser()
{
    return use_internal_browser;
}

void MapMainWindow::setuse_internal_browser(bool value)
{
    use_internal_browser = value;
    ui->actionInternen_Hilfebrowser_verwenden->setChecked(use_internal_browser);
}


bool MapMainWindow::getShowZweitfenster() const
{
    return showZweitfenster;
}

void MapMainWindow::setShowZweitfenster(bool value)
{
    showZweitfenster = value;
    if (showZweitfenster)
        zweitfenster->show();
    else zweitfenster->hide();
    ui->actionZweitfenster->setChecked(showZweitfenster);
    enableMenuesZweitfenster(showZweitfenster);
    resizeEvent(0);
}

bool MapMainWindow::getMaufgabendetailszeigen() const
{
    return maufgabendetailszeigen;
}

void MapMainWindow::setMaufgabendetailszeigen(bool value)
{
    maufgabendetailszeigen = value;
    ui->actionAufgabendetails_zeigen->setChecked(maufgabendetailszeigen);
    ui->lwAufgabenDetails->setEnabled(maufgabendetailszeigen);
    ui->lwAufgabenDetails->setVisible(maufgabendetailszeigen);
    enableNavigationTeilaufgaben();
    resizeEvent(0);
    resizeEvent(0);
}

enStyles MapMainWindow::styles() const
{
    return mStyles;
}

void MapMainWindow::setStyles(const enStyles &styles)
{
    mStyles = styles;
    int i = static_cast<int>(mStyles);
    setStyleSheet(sStyles[i]);
    ui->textBrowser->setStyleSheet(sTextBrowserStylesMainWin[i]);
    ui->textBrowser_2->setStyleSheet(sTextBrowserStylesMainWin2[i]);
    QString s = sMenuStyles[i];
    mPopupMenuLwAufgaben->setStyleSheet(s);
    mPopupMain->setStyleSheet(s);
    mPopupMenuLwAufgabenDetails->setStyleSheet(s);
    mPopupCuelist->setStyleSheet(s);
    ui->menuBar->setStyleSheet(s);
    ui->lineEditCurrentTime->setStyleSheet(sEditStyles[i]);
    ui->actionWindows_Style->setChecked(mStyles == styleNormal);
    ui->actionHfM_Style->setChecked(mStyles == styleHfM);
}


void MapMainWindow::on_mediapositionchanges()
{
    qint64 pos, duration;
    pos = mplayer->position();
    duration = mplayer->duration();
    if ((pos >= duration) && (duration > 0))
    {
        if (clearSreenWhenStop()) clearScreen();
        on_actionStop_triggered();
        pos = 0;
    }
    QString wm, s2;
    QString s = millisecs2QTime(pos, wm);
    if (getKeineStoppuhr()) s = "";
    ui->lblTime->setText(s);
    zweitfenster->lbltime->setText(s);
    // muss vorzeichenbehaftet sein, wg. Differenz
    // Auf Sekunden runden
    qint64 d = duration-pos;
    QString withmillisec = wm % " | " % millisecs2QTime(d, s2) % " | "
    % millisecs2QTime(duration, s2) % " " % mPlaybackRateText;
    if ((duration > 0) && (pos >= 0))
        ui->labelMediaPos->setText(withmillisec);
    ui->hSlider->setValue(pos);
}

void MapMainWindow::on_mediadurationchanges()
{
   on_mediavolumechanges();
    ui->hSlider->setRange(0, mplayer->duration());
}

// gibt die aktuelle Lautstärke (ein double) als int 0..cVolumeRange zurück
int MapMainWindow::volume2Faderposition()
{
    double rvolume = mAudioOutput->volume();
    double linearVolume = QAudio::convertVolume(rvolume,
                                                QAudio::LinearVolumeScale,
                                                QAudio::LogarithmicVolumeScale);
    return qRound(linearVolume*cVolumeRange);//runden
}

void MapMainWindow::on_mediavolumechanges()
{
    int vol = volume2Faderposition();
    setVolume(vol);
}

void MapMainWindow::setInfo()
{
    QString s=" ";

    if (showFileInformations()) s=s % " <font color=#0000aa>" %
            truncateFilename(mcurrent_aufgabe->getMaudiofilename(),
                             CMAXPATHLENFORDISPLAY+10);
    statuslabel->setText(s % "</p>");
    if (showAufgabenFileInformations())
    {
        statuslabel2->setText("<p><font color=#000000>" %
                              truncateFilename(maufgabefilename, CMAXPATHLENFORDISPLAY-10)
                              %  "</p>");
    }
    else statuslabel2->setText("");
}

void MapMainWindow::on_audioavailablechanges(bool available)
{
    ui->actionActionPause->setEnabled(available);
    ui->pauseButton->setEnabled(available);
    switch_PlayButton();
}

void MapMainWindow::switch_PlayButton()
{
    switch (mplayer->playbackState()) {
      case QMediaPlayer::StoppedState:
      case QMediaPlayer::PausedState:
            ui->pauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        ui->actionActionPause->setText(tr("&Play"));
        break;
      default:
        ui->pauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        ui->actionActionPause->setText(tr("&Pause"));
        break;
    }
}

void MapMainWindow::on_actionMen_leiste_triggered()
{
    ui->menuBar->setVisible(!ui->menuBar->isVisible());
    ui->actionMen_leiste->setChecked(ui->menuBar->isVisible());
    resizeEvent(0);
}

void MapMainWindow::on_hSlider_sliderMoved(int position)
{
    mplayer->setPosition(position);
}



void MapMainWindow::on_lineEditCurrentTime_textChanged()
{
    QColor clr;
    if (ui->lineEditCurrentTime->hasAcceptableInput())
    {
        clr = Qt::black;
        ui->pushButton->setEnabled(true);
        ui->pushButtonAdd2History->setEnabled(true);
        ui->actionZu_Cueingpoint_gehen->setEnabled(true);
    }
    else
    {
        clr = Qt::red;
        ui->pushButton->setEnabled(false);
        ui->pushButtonAdd2History->setEnabled(true);
        ui->actionZu_Cueingpoint_gehen->setEnabled(false);
    }
    mpalette.setColor(QPalette::Text, clr);
    ui->lineEditCurrentTime->setPalette(mpalette);
}

void MapMainWindow::on_actionActionPause_triggered()
{
    // Altes Aufgabenfile merken
    static QString oldAufgabenfile;
    // vergleichen, ob Aufgabenfile geändert wurde,
    // z. B. durch Laden einer neuen Liste
    QString currentAufgabenfile = mcurrent_aufgabe->getMaudiofilename();
    // Feststellen, ob Aufgabenfile gewechselt hat
    bool changed = oldAufgabenfile != mcurrent_aufgabe->getMaudiofilename();
    // falls ja, dann 1. hier die statische Merkvariable neu setzen und...(s.u.)
    if (changed) oldAufgabenfile = currentAufgabenfile;
    switch (mplayer->playbackState()) {
    case QMediaPlayer::PlayingState:
        mplayer->pause();
        ui->pauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        ui->actionActionPause->setText(tr("&Play"));
        break;
    case QMediaPlayer::StoppedState:
        // .. 2. in der Funktion mit changend (dann true) neues File laden
        // sonst - wenn changed false - dies eben
        // nicht tun, damit Fortschrittsbalken
        // nicht auf 0 springt.
        if (set_mname_currentmp3file(mcurrent_aufgabe->getMaudiofilename(),
                                      changed)){PauseState();};
        break;

    case QMediaPlayer::PausedState:
    {
        PauseState();
        break;
    }
    }
}

void MapMainWindow::PauseState()
{
    mplayer->play();
    ui->pauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    ui->actionActionPause->setText(tr("&Pause"));
}

// Wenn Stopptaste gedrückt wurde:
void MapMainWindow::on_actionStop_triggered()
{
    // Position zurücksetzen
    mplayer->setPosition(0);
    // Buttons anpassen
    switch (mplayer->playbackState()) {
    case QMediaPlayer::PlayingState:
    case QMediaPlayer::PausedState:
        mplayer->stop();
        ui->pauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        ui->actionActionPause->setText(tr("&Play"));
    default:{};
    }
}

void MapMainWindow::on_actionAufgabenliste_laden_triggered()
{
    QString s = QFileDialog::getOpenFileName(this,
                    tr("Aufgabenliste laden"),
                    maufgabenliste->get_currentfilename(),
                    tr("Aufgabenlisten (*.afl)"));
    if (s != "")
    {
        QFileInfo finf(s);
        if (finf.suffix() == "afl")
        {
            mplayer->stop();
            AufgabenlisteLaden(s);
        }
        else
        {
            QString s = "Falscher Dateityp";
            QMessageBox mb(QMessageBox::Warning,
                            "Bitte eine Aufgabenliste wählen.",
                            s, QMessageBox::Ok, this);
            mb.exec();
        }
    }
}

void MapMainWindow::onAufgabenlisteFilenamechanged(QString s)
{
    mcurrent_aufgabe->setMcurrentdir(maufgabenliste->getMcurrentpath());
    maufgabefilename = s;
    setInfo();
}

void MapMainWindow::onAufgabenlisteChanged(QStringList titelliste)
{
    ui->lwAufgaben->clear();
    ui->lwAufgabenDetails->clear();
    ui->lwAufgaben->addItems(titelliste);
    ui->lwAufgaben->setCurrentRow(0);
}

void MapMainWindow::onAufgabechanged()
{
    mcurrent_aufgabe->setMcurrentdir(maufgabenliste->getMcurrentpath());
    mcurrent_aufgabe->loadAufgabe();
    ui->textBrowser->clear();
    ui->textBrowser_2->clear();
    ui->lwAufgabenDetails->clear();
    ui->lwAufgabenDetails->addItems(mcurrent_aufgabe->getMdisplaylist());
    enableNavigationTeilaufgaben();
}

void MapMainWindow::AufgabeInEditorLaden()
{
    QString filename = mcurrent_aufgabe->get_currentfilename();
    // Falls keine Aufgabe geladen ist, eine neue erzeugen
    if (filename == "")
    {
        QString path = maufgabenliste->getMcurrentpath();
        filename = QFileDialog::getOpenFileName(this,
                           tr("Bitte einen Dateinamen vergeben"),
                           path, tr("Aufgaben (*.auf)"));

        if (filename != "")
        {
            ui->Aufgabeneditor->clear();
            if (QFile::exists(filename))
            {
                QFile file(filename);
                file.open(QIODevice::ReadOnly);
                QTextStream stream(&file);
                QString s = stream.readAll();
                file.close();
                ui->Aufgabeneditor->setPlainText(s);
                mEditierbareAufgabe.setFilename(filename);
                ui->label_2->setText(mEditierbareAufgabe.title());
            }
        }
    }
    else
        // aktuelle Aufgabe laden
    {
        ui->Aufgabeneditor->clear();
        foreach(QString s, mcurrent_aufgabe->getMfulllist())
        {
            ui->Aufgabeneditor->appendPlainText(s);
        }
        mEditierbareAufgabe.setFilename(
                    mcurrent_aufgabe->getMcurrentdir() %
                    "/" % mcurrent_aufgabe->get_currentfilename());
        ui->label_2->setText(mEditierbareAufgabe.title());
    }
}

void MapMainWindow::enableNavigationTeilaufgaben()
{
    bool enable = ui->lwAufgabenDetails->count() > 0;
    enable = enable && getMaufgabendetailszeigen();
    ui->actionN_chste_Teilaufgabe->setEnabled(enable);
    ui->action_Vorherige_Teilaufgabe->setEnabled(enable);
}


void MapMainWindow::on_lwAufgaben_currentRowChanged(int currentRow)
{
    maufgabenliste->set_index(currentRow);
    QString s = maufgabenliste->set_index(currentRow);
    mcurrent_aufgabe->set_currentfilename(s);
    setInfo();
    set_mname_currentmp3file(mcurrent_aufgabe->getMaudiofilename(), false);
    on_actionStop_triggered();
}

void MapMainWindow::on_lwAufgaben_doubleClicked()
{
    on_actionStop_triggered();
    on_actionActionPause_triggered();
}

void MapMainWindow::on_actionPrevious_triggered()
{
    changeAufgabe(-1);
}

void MapMainWindow::on_actionNext_triggered()
{
    changeAufgabe(1);
}

void MapMainWindow::onTimer()
{
    if (mplayer->playbackState() == QMediaPlayer::PlayingState)
    {
        QString s = mcurrent_aufgabe->output(mplayer->position());
        if (mStoppuhronly) s = " ";
        ui->textBrowser->setHtml(s);
        ui->textBrowser_2->setHtml(s);
        zweitfenster->textbrowser->setHtml(s);
    }
    ui->labelTime->setText(
                QDateTime::currentDateTime().
                toString("hh:mm:ss     ddd, dd.MM.yyyy "));
}

void MapMainWindow::changeAufgabe(int increment)
{
    int index = maufgabenliste->get_index();
    int a = maufgabenliste->count();
    index = (index+increment)% a;
    if (index <0) index = index + a;
    ui->lwAufgaben->setCurrentRow(index);
    on_lwAufgaben_currentRowChanged(index);
    on_lwAufgaben_doubleClicked();
}

void MapMainWindow::on_actionZweitfenster_triggered()
{
    setShowZweitfenster(!getShowZweitfenster());
}

void MapMainWindow::enableMenuesZweitfenster(bool enable)
{
    ui->actionStandardgr_e->setEnabled(enable);
    ui->actionNach_rechts->setEnabled(enable);
    ui->actionNach_links->setEnabled(enable);
    ui->actionNach_oben->setEnabled(enable);
    ui->actionNach_untern->setEnabled(enable);
    ui->actionSchnell_rechts->setEnabled(enable);
    ui->actionSchnell_links->setEnabled(enable);
    ui->actionSchnell_oben->setEnabled(enable);
    ui->actionSchnell_unten->setEnabled(enable);
    ui->actionZum_Hauptfenster_bewegen->setEnabled(enable);
    ui->actionHfM_Stil->setEnabled(enable);
    ui->actionHauptfensterStil->setEnabled(enable);
}

void MapMainWindow::on_action_ber_MAP_triggered()
{
    About *ab = new About(this);
    ab->setWindowFlag(Qt::Dialog);
    ab->setModal(true);
    ab->show();
   // AboutDlg ab(this);
   // ab.setModal(true);
   // ab.setStyleSheet("background-image: url(:/splash.png)");
   // ab.show();
}

void MapMainWindow::on_actionLauter_triggered()
{
    if (ui->vslider->value() == 0)
    {
        ui->vslider->setValue(ui->vslider->value()+1);
        if (ui->vslider->value() <1)
            ui->vslider->setValue(2);
    }
    else
        ui->vslider->setValue(ui->vslider->value()+1);
}

void MapMainWindow::on_actionLeiser_triggered()
{
    ui->vslider->setValue(ui->vslider->value()-1);
}

void MapMainWindow::on_actionFade_out_triggered()
{
    setIsFading(-1);
    mFadeTimer->start();
}

void MapMainWindow::on_actionFade_in_triggered()
{
    setIsFading(1);
    mFadeTimer->start();
}

void MapMainWindow::onFadeTimer()
{
    int v = volume2Faderposition();
    // mIsFading ist -1, 0 oder 1
    double a = 0;
    if (v <= 1.0) a = 1.0;
    int newV = v+(FADINGINTERVAL+a)*isFading();
    if (newV <= mVolumeStandard || isFading() == -1)
    {
        applyVolume(newV);
        v = volume2Faderposition();
        if (v <= 0.0 || v >=cVolumeRange)
        {
            mFadeTimer->stop();
            setIsFading(0);
        }
    }
    else
    {
        mFadeTimer->stop();
        setIsFading(0);
    }
}

void MapMainWindow::on_actionFading_anhalten_triggered()
{
    mFadeTimer->stop();
    setIsFading(0);
}


void MapMainWindow::on_vslider_valueChanged(int value)
{
    setVolume(value);
}

void MapMainWindow::on_actionStandardvolumen_festlegen_triggered()
{
    setVolumeStandard(volume());
}

void MapMainWindow::on_pushButton_clicked()
{
    if (ui->lineEditCurrentTime->hasAcceptableInput())
    {
        QString arg1 = ui->lineEditCurrentTime->text();
        double ms = Aufgabe::getMilliSeconds(arg1);
        if (ms < mplayer->duration())
        {
            mplayer->setPosition(ms);
            QTime tm = QTime::fromMSecsSinceStartOfDay(ms);
            QString txt = tm.toString("mm:ss" % mDecimalPoint % "zzz");
            insertToCueHistory(txt);
        }
    }
}

void MapMainWindow::insertToCueHistory(QString insertText)
{
    bool found = false;
    for (int i = 0; (i < ui->listWidget->count() && (!found)); i++)
    {
        found = (ui->listWidget->item(i)->text() == insertText);
    }

    if (!found)
    {
        ui->listWidget->addItem(insertText);
        ui->listWidget->sortItems();
    }
}

void MapMainWindow::on_pushButtonClear_clicked()
{
    ui->lineEditCurrentTime->setText("");
}

void MapMainWindow::on_pushButtonPickup_clicked()
{
    QTime tm = QTime::fromMSecsSinceStartOfDay(mplayer->position());
    QString txt = tm.toString("mm:ss" % mDecimalPoint % "zzz");
    ui->lineEditCurrentTime->setText(txt);
    insertToCueHistory(txt);
}

void MapMainWindow::on_btnMute_clicked()
{
    bool muted = mAudioOutput->isMuted();
    mAudioOutput->setMuted(!muted);
    muted = mAudioOutput->isMuted();
    ui->actionMute->setChecked(muted);
    if (muted) ui->btnMute->
            setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    else ui->btnMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
}

void MapMainWindow::on_btnToStandardvolume_clicked()
{
    applyVolume(volumeStandard());
    //mAudioOutput->setVolume(volumeStandard());
}

void MapMainWindow::on_actionNormalansicht_triggered()
{
    Fensterhoehe_setzen(HNORMALANSICHT + 20);
}

void MapMainWindow::Fensterhoehe_setzen(int h)
{
    if (windowState() != Qt::WindowMaximized)
    {
        int a = menuBar()->geometry().height();
        int b = 0;
        if (ui->statusBar->isVisible())b=ui->statusBar->geometry().height();
        QRect rect = this->geometry();
        rect.setHeight(h+a+b+ui->hSlider->geometry().height() + 10);
        this->setGeometry(rect);
    }
}

void MapMainWindow::on_actionLektionsansicht_triggered()
{
    Fensterhoehe_setzen(ui->lwAufgaben->geometry().top() - 30);
}

void MapMainWindow::on_actionEnger_triggered()
{
    setBrowserSize(-1, 0);
}

void MapMainWindow::on_actionWeiter_triggered()
{
    setBrowserSize(1, 0);
}

void MapMainWindow::on_actionKleiner_triggered()
{
    setBrowserSize(0, -1);
}

void MapMainWindow::on_actionGr_er_triggered()
{
    setBrowserSize(0, 1);
}

void MapMainWindow::on_actionStandardgr_e_triggered()
{
    zweitfenster->standardgroesse();
    resizeEvent(0);
}

void MapMainWindow::on_actionNach_rechts_triggered()
{
    zweitfenster->verschieben(1, 0, false);
}

void MapMainWindow::on_actionNach_links_triggered()
{
    zweitfenster->verschieben(-1, 0, false);
}

void MapMainWindow::on_actionNach_oben_triggered()
{
    zweitfenster->verschieben(0, -1, false);
}

void MapMainWindow::on_actionNach_untern_triggered()
{
    zweitfenster->verschieben(0, 1, false);
}

void MapMainWindow::on_actionSchnell_rechts_triggered()
{
    zweitfenster->verschieben(1, 0, true);
}

void MapMainWindow::on_actionSchnell_links_triggered()
{
    zweitfenster->verschieben(-1, 0, true);
}

void MapMainWindow::on_actionSchnell_oben_triggered()
{
    zweitfenster->verschieben(0, -1, true);
}

void MapMainWindow::on_actionSchnell_unten_triggered()
{
    zweitfenster->verschieben(0, 1, true);
}

void MapMainWindow::on_actionBeenden_triggered()
{
    close();
}

void MapMainWindow::on_actionLeinwand_l_schen_triggered()
{
    ui->textBrowser->setHtml("");
    ui->textBrowser_2->setHtml("");
    zweitfenster->textloeschen();
}

void MapMainWindow::on_actionNur_Stoppuhr_2_triggered()
{
    setStoppuhronly(!stoppuhronly());
    ui->actionNur_Stoppuhr_2->setChecked(mStoppuhronly);
}

void MapMainWindow::on_actionKeine_Stoppuhr_triggered()
{
    setKeineStoppuhr(!getKeineStoppuhr());
}

void MapMainWindow::on_actionAufgabe_l_schen_triggered()
{
    if (!ui->listWidget->hasFocus())
    {
        int sel = ui->lwAufgaben->currentRow();
        maufgabenliste->deleteItem(sel);
    }
    else
    {
        int i = ui->listWidget->currentRow();
        ui->listWidget->model()->removeRow(i);
    }
}


void MapMainWindow::on_actionAufgabenliste_speichern_triggered()
{
    maufgabenliste->saveFile();
}

void MapMainWindow::on_actionAufgabenliste_neu_laden_triggered()
{
    mplayer->stop();
    maufgabenliste->loadFile(maufgabenliste->get_currentfilename());
    statuslabel->setText("");
    statuslabel2->setText("");
}

void MapMainWindow::on_actionAufgabenliste_speichern_als_triggered()
{
    bool dirOK = false;
    QString s = "";
    QFileInfo fileinfo(maufgabenliste->get_currentfilename());
    QString oldDir = fileinfo.absoluteDir().absolutePath();
    do
    {
        s = QFileDialog::getSaveFileName(this, tr("Aufgabenliste speichern"),
                                         maufgabenliste->get_currentfilename(),
                                         tr("Aufgabenlisten (*.afl)"));
        if (s != "")
        {
            QFileInfo fi(s);
            QString newDir = fi.absoluteDir().absolutePath();
            dirOK = (newDir == oldDir);
            if (dirOK)
            {
                maufgabenliste->set_currentfilename(s);
                maufgabenliste->saveFile();
                recentFiles.removeAll(s);
                recentFiles.prepend(s);
                UpdateRecentFileActions();
            }
            else
            {
                QString s =
         "Bitte im selben Verzeichnis speichern wie die ursprüngliche Datei";
                QMessageBox mb(QMessageBox::Warning,
                               "Falsches Verzeichnis", s, QMessageBox::Ok |
                               QMessageBox::Cancel, this);
                int a = mb.exec();  // Wenn Nachricht escaped wird, Ende
                dirOK = (a == QMessageBox::Cancel);
            }
        }
    }while (!(dirOK || (s == "")));
}

void MapMainWindow::on_actionAufgabe_nach_oben_triggered()
{
    moveAufgabe(-1);
}

void MapMainWindow::on_actionAufgabe_nach_unten_triggered()
{
    moveAufgabe(1);
}

void MapMainWindow::moveAufgabe(int direc)
{
    int cr = ui->lwAufgaben->currentRow();
    maufgabenliste->move_item(ui->lwAufgaben->currentRow(), +direc);
    int cnt = ui->lwAufgaben->count();
    cr = (cr + direc) % cnt;
    if (cr <0 ) cr = cr + cnt;
    ui->lwAufgaben->setCurrentRow(cr);
}

void MapMainWindow::on_actionLeere_Aufgabenliste_triggered()
{
    QString s = QFileDialog::
            getSaveFileName(this, tr("Neue Aufgabenliste"),
            maufgabenliste->get_currentfilename(),
                            tr("Aufgabenlisten (*.afl)"));
    if (s != "")
    {
        maufgabenliste->NewFile(s);
        recentFiles.removeAll(s);
        recentFiles.prepend(s);
        UpdateRecentFileActions();
        ui->lwAufgabenDetails->clear();
        ui->Aufgabeneditor->clear();
    }
}

void MapMainWindow::on_actionAufgabe_hinzuf_gen_triggered()
{
    QFileInfo fi(maufgabenliste->get_currentfilename());
    QDir dir = fi.absoluteDir();
    QString s = dir.absolutePath();
    s = QFileDialog::getOpenFileName(this, tr("Aufgabe hinzufügen"),
                                     s, tr("Aufgaben (*.auf)"));
    if (s != "" && QFile::exists(s))
    {
        maufgabenliste->addAufgabe(s);
    }
}

void MapMainWindow::on_lwAufgaben_customContextMenuRequested(const QPoint &pos)
{
    mPopupMenuLwAufgaben->popup(ui->lwAufgaben->viewport()->mapToGlobal(pos));
}

void MapMainWindow::on_actionAufgabenliste_bearbeiten_triggered()
{
    raiseFileWithExternalSoftware(maufgabefilename);
}

void MapMainWindow::raiseFileWithExternalSoftware(QString s)
{
    if (QFile::exists(s))
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(s));
    }
}

void MapMainWindow::on_actionAufgabe_bearbeiten_triggered()
{
    int index = ui->lwAufgaben->currentRow();
    QString s = maufgabenliste->getMdataList().value(index);
    s = maufgabenliste->getMcurrentpath() % "/" % s;
    raiseFileWithExternalSoftware(s);
}

void MapMainWindow::on_actionIn_Explorer_zeigen_triggered()
{
    int index = ui->lwAufgaben->currentRow();
    QString s = maufgabenliste->getMdataList().value(index);
    s = maufgabenliste->getMcurrentpath();
    raiseFileWithExternalSoftware(s);
}

void MapMainWindow::on_actionZum_Hauptfenster_bewegen_triggered()
{
    zweitfenster->zurueckholen();
}

void MapMainWindow::on_MapMainWindow_customContextMenuRequested
(const QPoint &pos)
{
    mPopupMain->popup(mapToGlobal(pos));
}

void MapMainWindow::on_actionHalbton_tiefer_triggered()
{
    qreal rate = 17.0/18.0;
    mplayer->setPlaybackRate(rate);
    changePlaybackDisplay(rate);
}

void MapMainWindow::on_actionNormales_Tempo_triggered()
{
    qreal rate = 1.0;
    mplayer->setPlaybackRate(rate);
    changePlaybackDisplay(rate);
}

void MapMainWindow::on_actionHaltbon_h_her_triggered()
{
    qreal rate = 18.0/17.0;
    mplayer->setPlaybackRate(rate);
    changePlaybackDisplay(rate);
}

void MapMainWindow::on_actionGanzton_h_her_triggered()
{
    qreal rate = 9.0/8.0;
    mplayer->setPlaybackRate(rate);
    changePlaybackDisplay(rate);
}

void MapMainWindow::onPlaybackRateChanged(qreal rate)
{
    changePlaybackDisplay(rate);
}

void MapMainWindow::clearScreen()
{
    ui->textBrowser->clear();
    ui->textBrowser_2->clear();
    zweitfenster->textbrowser->clear();
    ui->lblKommentare->setVisible(false);
}

void MapMainWindow::onPlaybackstateChanged(QMediaPlayer::PlaybackState state)
{
    if (state == QMediaPlayer::StoppedState)
        if (mClearSreenWhenStop) clearScreen();
    switch_PlayButton();
}

entempoState MapMainWindow::getTempoState(qreal rate)
{
    if (rate >= (1.06))  // größer als 18/17
    {
        return ts_plus2;
    }
    else
        if (rate >= (1.01))  // größer als 1
            return ts_plus1;
        else
            if (rate >= (.98))  // größer als 17/18
            {
                return ts_norm;
            }
            else
                if (rate >= (.92))  // größer als 8/9
                    return ts_min1;
                else
                    return ts_min2;
}

void MapMainWindow::changePlaybackDisplay(qreal rate, bool setMediapos)
{
    QString style = "color: red";
    mtempoState = getTempoState(rate);
    mPlaybackRateText = sTempoStates[mtempoState];
    if (mtempoState == ts_norm) style = "color: black";
    ui->lblTime->setStyleSheet(style);
    // beim ersten Mal dies nicht aufrufen, wg. fehlender Initialisierungen
    if (setMediapos) on_mediapositionchanges();
    ui->actionGanztonTiefer->setChecked(mtempoState == ts_min2);
    ui->actionHalbton_tiefer->setChecked(mtempoState == ts_min1);
    ui->actionNormales_Tempo->setChecked(mtempoState == ts_norm);
    ui->actionGanzton_h_her->setChecked(mtempoState == ts_plus2);
    ui->actionHaltbon_h_her->setChecked(mtempoState == ts_plus1);
}

void MapMainWindow::on_actionGanztonTiefer_triggered()
{
    qreal rate = 8.0/9.0;
    mplayer->setPlaybackRate(rate);
    changePlaybackDisplay(rate);
}

void MapMainWindow::on_actionHfM_Style_triggered()
{
    setStyles(styleHfM);
}

void MapMainWindow::on_actionWindows_Style_triggered()
{
    setStyles(styleNormal);
}

void MapMainWindow::on_actionHfM_Stil_triggered()
{
    mZweitfensterStyles = styleHfM;
    int i = static_cast<int>(mZweitfensterStyles);
    zweitfenster->setStyles(sStyles[i], sTextBrowserStyles[i]);
}

void MapMainWindow::on_actionHauptfensterStil_triggered()
{
    mZweitfensterStyles = styleNormal;
    int i = static_cast<int>(mZweitfensterStyles);
    zweitfenster->setStyles(sStyles[i], sTextBrowserStyles[i]);
}

void MapMainWindow::on_actionZu_Cueingpoint_gehen_triggered()
{
    on_pushButton_clicked();
}

void MapMainWindow::onZweitfensterStyleChanged()
{
    ui->actionHauptfensterStil->setChecked(mZweitfensterStyles  == styleNormal);
    ui->actionHfM_Stil->setChecked(mZweitfensterStyles == styleHfM);
}

void MapMainWindow::on_actionStatusleiste_triggered()
{
    ui->statusBar->setVisible(!ui->statusBar->isVisible());
    ui->actionStatusleiste->setChecked(ui->statusBar->isVisible());
    resizeEvent(0);
    resizeEvent(0);
}



void MapMainWindow::on_actionDateiinformationen_zeigen_triggered()
{
    setShowFileInformations(!showFileInformations());
}

void MapMainWindow::on_actionMit_Zweitfenster_s_ynchronisieren_triggered()
{
    setMsynchWindows(!getMsynchWindows());
}

// HTML-Hilfe anzeigen
void MapMainWindow::on_action_Hilfe_triggered()
{
    if (use_internal_browser) showHelp(SFileHelpIndex);
    else
    // Wenn externer Browser sicht nicht aufrufen lässt, internen verwenden
        if (!ShowHelpExtern(SFileHelpIndex))
       showHelp(SFileHelpIndex);
}


// Handbuch (Help.pdf) im Standard-PDF-Reader anzeigen
// Fehlermeldung, wenn Fehlschlag
void MapMainWindow::on_actionHandbuch_triggered()
{
    if (!ShowHelpExtern(SHandbuch))
    {
       QMessageBox::warning(this, tr("MAP"), tr("Der externe Browser konnte nicht augerufen werden"));
    };
}
void MapMainWindow::showHelp(QString pageURL)
{
    mhelpb->showPage(pageURL);
    if (!mhelpb->isVisible()) mhelpb->show();
    else
    {
        mhelpb->activateWindow();
    }
}

void MapMainWindow::on_actionAufgabendateiinfos_zeigen_triggered()
{
    setShowAufgabenFileInformations(!showAufgabenFileInformations());
}

void MapMainWindow::on_lwAufgabenDetails_customContextMenuRequested
(const QPoint &pos)
{
    mPopupMenuLwAufgabenDetails->popup
            (ui->lwAufgabenDetails->viewport()->mapToGlobal(pos));
}

void MapMainWindow::setupPopupMenus()
{
    mPopupMenuLwAufgaben = new QMenu(this);
    mPopupMenuLwAufgaben->addAction(ui->actionAufgabe_l_schen);
    mPopupMenuLwAufgaben->addAction(ui->actionAufgabe_nach_oben);
    mPopupMenuLwAufgaben->addAction(ui->actionAufgabe_nach_unten);
    mPopupMenuLwAufgaben->addAction(ui->actionAufgabe_bearbeiten);
    mPopupMenuLwAufgaben->addAction(ui->actionAufgabe_hinzuf_gen);
    mPopupMenuLwAufgaben->addAction(ui->actionAufgabe_in_Editor_laden);
    mPopupMenuLwAufgaben->addSeparator();
    mPopupMenuLwAufgaben->addAction(ui->actionAufgabenliste_laden);
    mPopupMenuLwAufgaben->addAction(ui->actionAufgabenliste_speichern);
    mPopupMenuLwAufgaben->addAction(ui->actionAufgabenliste_neu_laden);
    mPopupMenuLwAufgaben->addAction(ui->actionAufgabenliste_bearbeiten);
    mPopupMenuLwAufgaben->addAction(ui->actionIn_Explorer_zeigen);
    mPopupMenuLwAufgaben->addSeparator();
    mPopupMenuLwAufgaben->addAction(ui->actionActionPause);
    mPopupMenuLwAufgaben->addAction(ui->actionStop);

    mPopupMenuLwAufgabenDetails = new QMenu(this);
    mPopupMenuLwAufgabenDetails->addAction(ui->actionAufgabe_bearbeiten);
    mPopupMenuLwAufgabenDetails->addAction(ui->actionIn_Explorer_zeigen);
    mPopupMenuLwAufgabenDetails->addAction(ui->actionAufgabe_in_Editor_laden);
    mPopupMenuLwAufgabenDetails->addSeparator();
    mPopupMenuLwAufgabenDetails->addAction(ui->actionActionPause);
    mPopupMenuLwAufgabenDetails->addAction(ui->actionStop);

    mPopupEditor = new QMenu(this);

    mPopupCuelist = new QMenu(this);
    mPopupCuelist->addAction(ui->actionListe_l_schen);
}

void MapMainWindow::changeAufgabensection(int increment)
{
    if (ui->lwAufgabenDetails->count() > 0)
    {
        int index = ui->lwAufgabenDetails->currentIndex().row();
        int a = ui->lwAufgabenDetails->count();
        index = (index+increment)% a;
        if (index <0) index = index + a;
        ui->lwAufgabenDetails->setCurrentRow(index);
        mplayer->setPosition(mcurrent_aufgabe->getMmilliseclist().value(index));
    }
}

void MapMainWindow::on_actionN_chste_Teilaufgabe_triggered()
{
    changeAufgabensection(1);
}

void MapMainWindow::on_action_Vorherige_Teilaufgabe_triggered()
{
    changeAufgabensection(-1);
}

void MapMainWindow::on_actionAufgabendetails_zeigen_triggered()
{
    setMaufgabendetailszeigen(!maufgabendetailszeigen);
}

void MapMainWindow::on_listWidget_doubleClicked(const QModelIndex &index)
{
    QListWidgetItem *it = ui->listWidget->item(index.row());

    double ms = Aufgabe::getMilliSeconds(it->text());
    if (ms < mplayer->duration())
    {
        ui->lineEditCurrentTime->setText(it->text());
    }
}

void MapMainWindow::on_pushButtonClear_2_clicked()
{
    ui->listWidget->clear();
}

void MapMainWindow::on_pushButtonAdd2History_clicked()
{
    if (ui->lineEditCurrentTime->hasAcceptableInput())
    {
        QString arg1 = ui->lineEditCurrentTime->text();
        double ms = Aufgabe::getMilliSeconds(arg1);
        if (ms < mplayer->duration())
        {
            QTime tm = QTime::fromMSecsSinceStartOfDay(ms);
            QString txt = tm.toString("mm:ss" % mDecimalPoint % "zzz");
            insertToCueHistory(txt);
        }
    }
}

void MapMainWindow::on_actionPickup_triggered()
{
    on_pushButtonPickup_clicked();
}

void MapMainWindow::on_actionListe_l_schen_triggered()
{
    on_pushButtonClear_2_clicked();
}

void MapMainWindow::on_actionEditor_anzeigen_triggered()
{
    setMeditorZeigen(!getMeditorZeigen());
}


void MapMainWindow::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
    mPopupCuelist->popup(ui->listWidget->viewport()->mapToGlobal(pos));
}

void MapMainWindow::on_actionAufgabe_in_Editor_laden_triggered()
{
    AufgabeInEditorLaden();
}

void MapMainWindow::on_actionAufgabe_speichern_triggered()
{
    saveAufgabeFromEditor(mEditierbareAufgabe.getFilename());
}

void MapMainWindow::saveAufgabeFromEditor(QString filename)
{
    if (filename == "")
    {
        QString sdir = maufgabenliste->getMcurrentpath();
        filename = QFileDialog::
                getSaveFileName(this,
                "Aufgabendatei speichern unter...",
                sdir, "Aufgabendateien (*.auf)");
    }
    if (filename != "")
    {
        QString Text = ui->Aufgabeneditor->toPlainText();
        mEditierbareAufgabe.saveToFile
                (Text , filename);
        ui->label_2->setText(mEditierbareAufgabe.title());
    }
    // Hinweis in der Statusbar
    QString msg = "Aufgabe gespeichert; für Sichtbarkeit der Änderungen";
    msg += "muss die Aufgabenliste neu geladen werden.";
    ui->statusBar->showMessage(msg, cMeldungsdauerStatusBar);
}

void MapMainWindow::on_Aufgabeneditor_textChanged()
{
    mEditierbareAufgabe.setHasChanged(true);
    ui->label_2->setText(mEditierbareAufgabe.title());
}

void MapMainWindow::on_actionAufgabe_speichern_unter_triggered()
{
    saveAufgabeFromEditor("");
}

void MapMainWindow::on_actionAufgabe_schlie_en_triggered()
{
    ui->Aufgabeneditor->clear();
    mEditierbareAufgabe.setFilename("");
    mEditierbareAufgabe.setHasChanged(true);
    ui->label_2->setText(mEditierbareAufgabe.title());
}

void MapMainWindow::on_actionKommentare_zeigen_triggered()
{
    setMkommentareZeigen(!getMkommentareZeigen());
}

void MapMainWindow::on_actionBei_Stop_Leinwand_l_schen_triggered()
{
    setClearSreenWhenStop(!clearSreenWhenStop());
}

void MapMainWindow::on_actionAuf_Stoppuhr_verkleinern_triggered()
{
    setmStoppuhrsize(!getmStoppuhrsize());
}

void MapMainWindow::fontsetzen(QFont font)
{
    ui->textBrowser->setFont(font);
    ui->textBrowser_2->setFont(font);
    zweitfenster->textbrowser->setFont(font);
}

void MapMainWindow::on_actionSchriftart_triggered()
{
    bool ok;
    QFont font = QFontDialog::getFont(
                &ok, QFont(ui->textBrowser->font()), this);
    if (ok) { fontsetzen(font);
    }
    else
    {}
}

bool MapMainWindow::getKeineStoppuhr()const

{
    return mKeineStoppuhr;
}

void MapMainWindow::setKeineStoppuhr(bool value)
{
    mKeineStoppuhr = value;
    ui->actionKeine_Stoppuhr->setChecked(mKeineStoppuhr);
    QRect geo = getlblTimeGeometry();
    if (mKeineStoppuhr)
    {
        geo.setWidth(0);
        ui->lblTime->setText("");
    }
    ui->lblTime->setGeometry(geo);
    zweitfenster->lbltime->setVisible(!mKeineStoppuhr);
    zweitfenster->lbltime->setGeometry(geo);
    setBrowserSize(0, 1);
    setBrowserSize(0, -1);
}

QRect MapMainWindow::getlblTimeGeometry()const
{
    return mlblTimeGeometry;
}

void MapMainWindow::setlblTimeGeometry(QRect value)
{
    mlblTimeGeometry = value;
}

// Schaltet Verwendung des internen Hilfebrowsers ein/aus
void MapMainWindow::actionInternen_Hilfebrowser_verwenden_triggerd()
{
    setuse_internal_browser(!use_internal_browser);
}

// Hilfedatei (html oder pdf-File) im Standard-Webbrowser des Betriebssystems anzeigen
bool MapMainWindow::ShowHelpExtern(QString SFilename)
{
    //  Hilfedatei im Standardbrowser anzeigen
    QUrl url(directoryOf(SHelpDirectory).absoluteFilePath(SFilename));
    url.setScheme(("file"));
    // Bei Gelingen Rückgabewert true
    return QDesktopServices::openUrl(url);
}

QDir MapMainWindow::directoryOf(const QString &subdir)
{
    QDir dir(QApplication::applicationDirPath());
    dir.cd(subdir);
    return dir;
}

void MapMainWindow::on_lwAufgabenDetails_doubleClicked()
{
    // Ruft einfach nur die Einfachklickroutine auf,
    // sonst gäbe es Fehlpositionierungen
    // (keine Ahnung, warum)
    on_actionStop_triggered();
    on_actionActionPause_triggered();
}

void MapMainWindow::on_lwAufgabenDetails_clicked(const QModelIndex &index)
{
    mplayer->setPosition(mcurrent_aufgabe->
                         getMmilliseclist().value(index.row()));
}

void MapMainWindow::on_hSlider_sliderPressed()
{
    // Player auf gemuted setzen, damit man
    // Bewegungen des Sliders nicht hört
    // Mute-Status speichern, damit er beim
    // Loslassen des Sliders wiederhergestellt werden kann
    mWasMutedBevoreSliderMoving = mAudioOutput->isMuted();
    // Nun Player muten (bis zum Loslassen des Sliders, siehe dort)
    mAudioOutput->setMuted(true);
}

void MapMainWindow::on_hSlider_sliderReleased()
{
    // Wiederherstellen des Mute-Status vor der Slider-Bewegung
    mAudioOutput->setMuted(mWasMutedBevoreSliderMoving);
}

void MapMainWindow::applyVolume(int volumeSliderValue)
{
    double vol = qreal(volumeSliderValue) / qreal(cVolumeRange);
    qreal linearVolume = QAudio::convertVolume(vol,
        QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
    mAudioOutput->setVolume(linearVolume);
}
