// Copyright 2016 Manfred Dings

#ifndef MAPMAINWINDOW_H
#define MAPMAINWINDOW_H

#include <QMainWindow>
#include <QtMultimedia/QAudioOutput>
#include <QTimer>
#include <QModelIndexList>
#include "aufgabenliste.h"
#include "aufgabe.h"
#include "dialog.h"
#include "helpbrowser.h"
#include "editierbareaufgabe.h"

/**************************************************
    *Dies regelmäßig anpassen
    */
const QString CVERSIONSNUMMER ="5.1.1 2024-02-13";
/* **********************************************/

const QString CVERSIONSTEXT = "MAP "+
        CVERSIONSNUMMER+QObject::tr("\nCreated with Qt 6.1.6 under LPGL3. "
        "You received\n a copy of LGPL3 with this program.");

const QString SFileHelpIndex = "Help.html";
const QString SHelpDirectory = "";
const QString SHandbuch      = "Help.pdf";
const QString SBeispielAufgabenliste = "MAP/Beispieldateien/Sample.afl";
class QSettings;

// Use background-color instead of background
const QString sMENUSTYLE =
        "QMenu::item{background-color: "
        "rgb(255,250,199); color: rgb(50,50,50)}"
        "QMenu::item:selected{ background-color: "
        "rgb(150,120,100);color: rgb(253,247,195)}"
        "QMenuBar::item { background-color: "
        "rgb(255,250,199); color: rgb(50,50,50) } "
        "QMenuBar::item:selected{ background-color: "
        "rgb(150,120,100);color: rgb(253,247,195)}";


enum entempoState {ts_min2 = 0, ts_min1, ts_norm, ts_plus1, ts_plus2};

const QString sTempoStates[5] = {"<-2>", "<-1>", " <0>", "<+1>", "<+2>"};

const int NSTYLES = 2;

enum enStyles {styleHfM = 0, styleNormal};

const QString sStyles[NSTYLES] =
  {"background-color:rgb(255,250,199)", ""};
const QString sTextBrowserStylesMainWin[2] =
  {"background-color:rgb(245,245,197());\nborder-style: none",
  "border-style: none"};
const QString sTextBrowserStylesMainWin2[2] =
  {"background-color:rgb(255,235,195);\nborder-style: none",
  "background-color:#e0e0e0;border-style: none"};
const QString sTextBrowserStyles[2] =
  {"background-color:rgb(255,250,199);\nborder-style: none",
  "border-style: none"};
const QString sMenuStyles[NSTYLES] = {sMENUSTYLE, ""};
const QString sEditStyles[NSTYLES] =
{"background-color: rgb(235,240,189);", ""};


const int HNORMALANSICHT = 500;

const int MILLISEC = 1000;
const int CMPLAYERNOTIFY = 20;  // millisec
// Einheiten des Erhöhes bzw. Erniedrigens der Lautstärke beim Fading
const int FADINGINTERVAL = 1;
const int CFADINGTIMERINTERVAL = 20;  // millisec
// millisec; Dauer der Meldungen in der Statusbar
const int cMeldungsdauerStatusBar = 3000;

namespace Ui {
class MapMainWindow;
}

class QMediaPlayer;

class MapMainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MapMainWindow(QWidget *parent = 0);
    ~MapMainWindow();

    void closeEvent(QCloseEvent *event) override;

    QString getmname_currentmp3file()const {return mname_currentmp3file;}
    bool set_mname_currentmp3file(QString s, bool open_file = true);

    void resizeEvent(QResizeEvent *event) override;

    void showEvent(QShowEvent *event) override;

    int getMsampleRate() const;
    void setMsampleRate(int value);

    int getBitRate() const;
    void setBitRate(int bitRate);

    int volume() const;

    QLabel *statuslabel, *statuslabel2, *statuslabel3;
    void setVolume(int volume);

    int isFading() const;
    void setIsFading(int isFading);

    int volumeStandard() const;
    void setVolumeStandard(int volumeStandard);

    bool stoppuhronly() const;
    void setStoppuhronly(bool stoppuhronly);

    enStyles styles() const;
    void setStyles(const enStyles &styles);

    bool showFileInformations() const;
    void setShowFileInformations(bool showFileInformations);

    bool getMsynchWindows() const;
    void setMsynchWindows(bool value);

    bool showAufgabenFileInformations() const;
    void setShowAufgabenFileInformations(bool showAufgabenFileInformations);

    bool getMaufgabendetailszeigen() const;
    void setMaufgabendetailszeigen(bool value);

    bool getShowZweitfenster() const;
    void setShowZweitfenster(bool value);

    bool getuse_internal_browser();
    void setuse_internal_browser(bool value);

    bool getMeditorZeigen() const;
    void setMeditorZeigen(bool value);

    bool getMkommentareZeigen() const;
    void setMkommentareZeigen(bool value);

    bool clearSreenWhenStop() const;
    void setClearSreenWhenStop(bool clearSreenWhenStop);

    bool getmStoppuhrsize() const;
    void setmStoppuhrsize(bool value);

    bool getKeineStoppuhr() const;
    void setKeineStoppuhr(bool value);

    QRect getlblTimeGeometry()const;
    void setlblTimeGeometry(QRect value);

protected:
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;


private:
    Ui::MapMainWindow *ui;

    QMediaPlayer *mplayer;
    QAudioOutput *mAudioOutput;

    QString mname_currentmp3file;

    Aufgabenliste *maufgabenliste;

    Aufgabe *mcurrent_aufgabe;

    bool mStoppuhronly, mKeineStoppuhr, mStoppuhrsize;

    QRect mlblTimeGeometry;

    QString millisecs2QTime(quint64 msec, QString &withmillisec);

    QTimer *mtimer, *mFadeTimer;

    QSettings *msettings;

    HelpBrowser *mhelpb;

    QPalette mpalette, mpaletteTime;

    QString mDecimalPoint;

    QString maufgabefilename;

    QString mlabeltext;

    entempoState mtempoState;

    QString mPlaybackRateText;

    QString mDocPath;

    bool showZweitfenster, maufgabendetailszeigen,
    mEditorZeigen, mkommentareZeigen,
        mClearSreenWhenStop;

    enum { MaxRecentFiles = 12};

    enStyles mStyles, mZweitfensterStyles;

    QStringList recentFiles;
    QAction *recentFileActions[MaxRecentFiles];
    QAction *separatorAction;

    void saveSettings();
    void restoreSettings();

    Dialog *zweitfenster;

    QMenu *mPopupMenuLwAufgaben, *mPopupMenuLwAufgabenDetails, *mPopupMain,
            *mPopupEditor, *mPopupCuelist;

    bool mShowFileInformations, mShowAufgabenFileInformations,
    msynchWindows, use_internal_browser, first_start;

    int mVolumeStandard;

    int mVolume;

    int mIsFading;  // mit 0, -1 oder 1 für Volumenänderung setzen

    EditierbareAufgabe mEditierbareAufgabe;

    // Wenn Slider bewegt: muten, Status hier speichern
    bool mWasMutedBevoreSliderMoving;

    void openMp3File(QString filename);
    void UpdateRecentFileActions();
    QString strippedName(const QString &fullFileName);
    void changeAufgabe(int increment);
    void setInfo();
    double CurrentVolumeToDecibels();
    void Fensterhoehe_setzen(int h);
    bool checkWindowStandardHeight(int currentheight, int targetheight);
    void moveAufgabe(int direc);
    void raiseFileWithExternalSoftware(QString s);
    void changePlaybackDisplay(qreal rate, bool setMediapos = true);
    entempoState getTempoState(qreal rate);
    void enableMenuesZweitfenster(bool enable);
    void showHelp(QString pageURL);
    void setupPopupMenus();
    void changeAufgabensection(int increment);
    void enableNavigationTeilaufgaben();
    void insertToCueHistory(QString insertText);
    void AufgabeInEditorLaden();
    void saveAufgabeFromEditor(QString filename);
    void setBrowserSize(int x, int y);
    void clearScreen();
    void fontsetzen(QFont font);
    bool ShowHelpExtern(QString SFilename);
    QDir directoryOf(const QString &subdir);
    void PauseState();
    void switch_PlayButton();
    void AufgabenlisteLaden(QString s);
    void applyVolume(int volumeSliderValue);
    int volume2Faderposition();
public slots:
    void openRecentFile();
    void on_aufgabepositionchanges(int index);

private slots:
    void on_mediapositionchanges();
    void on_mediadurationchanges();
    void on_mediavolumechanges();
    void on_audioavailablechanges(bool available);
    void on_actionMen_leiste_triggered();
    void on_hSlider_sliderMoved(int position);
    void on_lineEditCurrentTime_textChanged();
    void on_actionActionPause_triggered();
    void on_actionStop_triggered();
    void on_actionAufgabenliste_laden_triggered();
    void onAufgabenlisteFilenamechanged(QString s);
    void onAufgabenlisteChanged(QStringList titelliste);
    void onAufgabechanged();
    void on_lwAufgaben_currentRowChanged(int currentRow);
    void on_lwAufgaben_doubleClicked();
    void on_actionPrevious_triggered();
    void on_actionNext_triggered();
    void onTimer();
    void onFadeTimer();
    void on_actionZweitfenster_triggered();
    void on_action_ber_MAP_triggered();
    void on_actionLauter_triggered();
    void on_actionLeiser_triggered();
    void on_actionFade_out_triggered();
    void on_actionFade_in_triggered();
    void on_actionFading_anhalten_triggered();
    void on_vslider_valueChanged(int value);
    void on_actionStandardvolumen_festlegen_triggered();
    void on_pushButton_clicked();
    void on_pushButtonClear_clicked();
    void on_pushButtonPickup_clicked();
    void on_btnMute_clicked();
    void on_btnToStandardvolume_clicked();
    void on_actionNormalansicht_triggered();
    void on_actionLektionsansicht_triggered();
    void on_actionEnger_triggered();
    void on_actionWeiter_triggered();
    void on_actionKleiner_triggered();
    void on_actionGr_er_triggered();
    void on_actionStandardgr_e_triggered();
    void on_actionNach_rechts_triggered();
    void on_actionNach_links_triggered();
    void on_actionNach_oben_triggered();
    void on_actionNach_untern_triggered();
    void on_actionSchnell_rechts_triggered();
    void on_actionSchnell_links_triggered();
    void on_actionSchnell_oben_triggered();
    void on_actionSchnell_unten_triggered();
    void on_actionBeenden_triggered();
    void on_actionLeinwand_l_schen_triggered();
    void on_actionNur_Stoppuhr_2_triggered();
    void on_actionKeine_Stoppuhr_triggered();
    void on_actionAufgabe_l_schen_triggered();
    void on_actionAufgabenliste_speichern_triggered();
    void on_actionAufgabenliste_neu_laden_triggered();
    void on_actionAufgabenliste_speichern_als_triggered();
    void on_actionAufgabe_nach_oben_triggered();
    void on_actionAufgabe_nach_unten_triggered();
    void on_actionLeere_Aufgabenliste_triggered();
    void on_actionAufgabe_hinzuf_gen_triggered();
    void on_lwAufgaben_customContextMenuRequested(const QPoint &pos);
    void on_actionAufgabenliste_bearbeiten_triggered();
    void on_actionAufgabe_bearbeiten_triggered();
    void on_actionIn_Explorer_zeigen_triggered();
    void on_actionZum_Hauptfenster_bewegen_triggered();
    void on_MapMainWindow_customContextMenuRequested(const QPoint &pos);
    void on_actionHalbton_tiefer_triggered();
    void on_actionNormales_Tempo_triggered();
    void on_actionHaltbon_h_her_triggered();
    void on_actionGanzton_h_her_triggered();
    void onPlaybackRateChanged(qreal rate);
    void onPlaybackstateChanged(QMediaPlayer::PlaybackState state);
    void on_actionGanztonTiefer_triggered();
    void on_actionHfM_Style_triggered();
    void on_actionWindows_Style_triggered();
    void on_actionHfM_Stil_triggered();
    void on_actionHauptfensterStil_triggered();
    void on_actionZu_Cueingpoint_gehen_triggered();
    void onZweitfensterStyleChanged();
    void on_actionStatusleiste_triggered();
    void on_actionDateiinformationen_zeigen_triggered();
    void on_actionMit_Zweitfenster_s_ynchronisieren_triggered();
    void on_action_Hilfe_triggered();
    void on_actionHandbuch_triggered();
    void on_actionAufgabendateiinfos_zeigen_triggered();
    void on_lwAufgabenDetails_customContextMenuRequested(const QPoint &pos);
    void on_actionN_chste_Teilaufgabe_triggered();
    void on_action_Vorherige_Teilaufgabe_triggered();
    void on_actionAufgabendetails_zeigen_triggered();
    void on_listWidget_doubleClicked(const QModelIndex &index);
    void on_pushButtonClear_2_clicked();
    void on_pushButtonAdd2History_clicked();
    void on_actionPickup_triggered();
    void on_actionListe_l_schen_triggered();
    void on_actionEditor_anzeigen_triggered();
    void on_listWidget_customContextMenuRequested(const QPoint &pos);
    void on_actionAufgabe_in_Editor_laden_triggered();
    void on_actionAufgabe_speichern_triggered();
    void on_Aufgabeneditor_textChanged();
    void on_actionAufgabe_speichern_unter_triggered();
    void on_actionAufgabe_schlie_en_triggered();
    void on_actionKommentare_zeigen_triggered();
    void on_actionBei_Stop_Leinwand_l_schen_triggered();
    void on_actionAuf_Stoppuhr_verkleinern_triggered();
    void on_actionSchriftart_triggered();
    void actionInternen_Hilfebrowser_verwenden_triggerd();
    void on_lwAufgabenDetails_doubleClicked(/*const QModelIndex &index*/);
    void on_lwAufgabenDetails_clicked(const QModelIndex &index);
    void on_hSlider_sliderPressed();
    void on_hSlider_sliderReleased();
};

#endif  // MAPMAINWINDOW_H
