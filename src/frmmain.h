// This file is a part of "Candle" application.
// Copyright 2015-2016 Hayrullin Denis Ravilevich
#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QSettings>
#include <QTimer>
#include <QBasicTimer>
#include <QStringList>
#include <QList>
#include <QTime>
#include <QMenu>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QProgressDialog>
#include <exception>
#include <atomic>

#include "parser/gcodeviewparse.h"

#include "drawers/origindrawer.h"
#include "drawers/gcodedrawer.h"
#include "drawers/tooldrawer.h"
#include "drawers/heightmapborderdrawer.h"
#include "drawers/heightmapgriddrawer.h"
#include "drawers/heightmapinterpolationdrawer.h"
#include "drawers/shaderdrawable.h"
#include "drawers/selectiondrawer.h"

#include "tables/gcodetablemodel.h"
#include "tables/heightmaptablemodel.h"

#include "utils/interpolation.h"

#include "widgets/styledtoolbutton.h"
#include "widgets/sliderbox.h"

#include "frmsettings.h"
#include "frmabout.h"

#include "utils/safequeue.h"


// GRBL Status
#define UNKNOWN             0
#define IDLE                1
#define ALARM               2
#define RUN                 3
#define HOME                4
#define HOLD0               5
#define HOLD1               6
#define QUEUE               7
#define CHECK               8
#define DOOR                9
#define JOG                 10
#define DWELL               11
#define TOOLCHANGE          12

#define PROGRESSMINLINES    10000

#define PROGRESSSTEP        1000


#ifdef WINDOWS
    #include <QtWinExtras/QtWinExtras>
    #include "shobjidl.h"
#endif


namespace Ui
{
    class frmMain;
}


struct CommandAttributes
{
    int length;
    int consoleIndex;
    int tableIndex;
    QString command;
};


struct CommandQueue
{
    QString command;
    int tableIndex;
    bool showInConsole;
};

struct CommandQueue2
{
    QString command;
    int tableIndex;
    int consoleIndex;
    int length;
};


typedef enum
{
    PROT_GRBL1_1 = 0, PROT_GRBL2, PROT_GRIP
} Protocol_e;


class CancelException : public std::exception
{
public:
    const char *what() const _GLIBCXX_USE_NOEXCEPT
    {
        return "Operation was cancelled by user";
    }
};


class frmMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();
    
private slots:
    void updateHeightMapInterpolationDrawer(bool reset = false);
    void placeVisualizerButtons();

    void onProcessData();
    void onSendSerial();

    void onTimerUpdateSpindleParser();
    void onTimerStatusQuery();
    void onVisualizatorRotationChanged();
    void onScroolBarAction(int action);
    void onJogTimer();
    void onTableInsertLine();
    void onTableDeleteLines();
    void onActRecentFileTriggered();
    void onCboCommandReturnPressed();
    void onTableCurrentChanged(QModelIndex idx1, QModelIndex idx2);
    void onConsoleResized(QSize size);
    void onPanelsSizeChanged(QSize size);
    void onCmdUserClicked(bool checked);
    void onOverridingToggled(bool checked);
    void onActSendFromLineTriggered();

    void on_actFileExit_triggered();
    void on_cmdFileOpen_clicked();
    void on_cmdFit_clicked();
    void on_cmdFileSend_clicked();
    void onTableCellChanged(QModelIndex i1, QModelIndex i2);
    void on_actServiceSettings_triggered();
    void on_actFileOpen_triggered();
    void on_cmdCommandSend_clicked();
    void on_cmdHome_clicked();
    void on_cmdTouch_clicked();
    void on_cmdZeroX_clicked();
    void on_cmdZeroY_clicked();
    void on_cmdZeroZ_clicked();
    void on_cmdRestoreOrigin_clicked();
    void on_cmdReset_clicked();
    void on_cmdUnlock_clicked();
    void on_cmdSafePosition_clicked();
    void on_cmdSpindle_toggled(bool checked);
    void on_chkTestMode_clicked(bool checked);
    void on_cmdFilePause_clicked(bool checked);
    void on_cmdFileReset_clicked();
    void on_actFileNew_triggered();
    void on_cmdClearConsole_clicked();
    void on_actFileSaveAs_triggered();
    void on_actFileSave_triggered();
    void on_actFileSaveTransformedAs_triggered();
    void on_cmdTop_clicked();
    void on_cmdFront_clicked();
    void on_cmdLeft_clicked();
    void on_cmdIsometric_clicked();
    void on_actAbout_triggered();
    void on_grpOverriding_toggled(bool checked);
    void on_grpSpindle_toggled(bool checked);
    void on_grpJog_toggled(bool checked);
    void on_grpUserCommands_toggled(bool checked);
    void on_chkKeyboardControl_toggled(bool checked);
    void on_tblProgram_customContextMenuRequested(const QPoint &pos);
    void on_splitter_splitterMoved(int pos, int index);
    void on_actRecentClear_triggered();
    void on_grpHeightMap_toggled(bool arg1);
    void on_chkHeightMapBorderShow_toggled(bool checked);
    void on_txtHeightMapBorderX_valueChanged(double arg1);
    void on_txtHeightMapBorderWidth_valueChanged(double arg1);
    void on_txtHeightMapBorderY_valueChanged(double arg1);
    void on_txtHeightMapBorderHeight_valueChanged(double arg1);
    void on_chkHeightMapGridShow_toggled(bool checked);
    void on_txtHeightMapGridX_valueChanged(double arg1);
    void on_txtHeightMapGridY_valueChanged(double arg1);
    void on_txtHeightMapGridZBottom_valueChanged(double arg1);
    void on_txtHeightMapGridZTop_valueChanged(double arg1);
    void on_cmdHeightMapMode_toggled(bool checked);
    void on_chkHeightMapInterpolationShow_toggled(bool checked);
    void on_cmdHeightMapLoad_clicked();
    void on_txtHeightMapInterpolationStepX_valueChanged(double arg1);
    void on_txtHeightMapInterpolationStepY_valueChanged(double arg1);
    void on_chkHeightMapUse_clicked(bool checked);
    void on_cmdHeightMapCreate_clicked();
    void on_cmdHeightMapBorderAuto_clicked();
    void on_cmdFileAbort_clicked();
    void on_cmdSpindle_clicked(bool checked);   

    void on_cmdYPlus_pressed();
    void on_cmdYPlus_released();
    void on_cmdYMinus_pressed();
    void on_cmdYMinus_released();
    void on_cmdXPlus_pressed();
    void on_cmdXPlus_released();
    void on_cmdXMinus_pressed();
    void on_cmdXMinus_released();
    void on_cmdZPlus_pressed();
    void on_cmdZPlus_released();
    void on_cmdZMinus_pressed();
    void on_cmdZMinus_released();
    void on_cmdStop_clicked();
    void on_cmdSpindle_triggered(QAction *arg1);

    void on_btnReload_clicked();
    void on_btnConnect_clicked();

    void on_comboInterface_currentTextChanged(const QString &arg1);

    void on_actionHard_Reset_triggered();

    void on_actionSet_TLS_Position_triggered();

    void on_actionDisable_Stepper_triggered();

    void on_btnHandwheel_clicked();

protected:
    void showEvent(QShowEvent *se);
    void hideEvent(QHideEvent *he);
    void resizeEvent(QResizeEvent *re);
    void timerEvent(QTimerEvent *);
    void closeEvent(QCloseEvent *ce);
    void dragEnterEvent(QDragEnterEvent *dee);
    void dropEvent(QDropEvent *de);

private:
    void loadFile(QString fileName);
    void loadFile(QList<QString> data);
    void clearTable();
    void preloadSettings();
    void loadSettings();
    void saveSettings();
    bool saveChanges(bool heightMapMode);
    void updateControlsState();

    void sendCommand(QString command, int tableIndex = -1, bool showInConsole = true);

    void GrblReset();
    int BufferLength();
    void sendNextFileCommands();
    void applySettings();
    void updateParser();
    bool DataIsFloating(QString data);
    bool DataIsEnd(QString data);
    bool DataIsReset(QString data);

    void ProcessGRBL1_1();
    void ProcessGRBL2();

    void ProcessGRBL_ETH(QString data);

    double toolZPosition();

    void UpdateComPorts();

    QString GetErrorMsg(int err_code);

    QTime updateProgramEstimatedTime(QList<LineSegment *> lines);
    bool saveProgramToFile(QString fileName, GCodeTableModel *model);
    QString FeedOverride(QString command);

    bool eventFilter(QObject *obj, QEvent *event);
    bool keyIsMovement(int key);
    void resizeCheckBoxes();
    void updateLayouts();
    void updateRecentFilesMenu();
    void addRecentFile(QString fileName);
    void addRecentHeightmap(QString fileName);
    double toMetric(double value);

    QRectF borderRectFromTextboxes();
    QRectF borderRectFromExtremes();
    void updateHeightMapBorderDrawer();
    bool updateHeightMapGrid();
    void loadHeightMap(QString fileName);
    bool saveHeightMap(QString fileName);

    GCodeTableModel *m_currentModel;
    QList<LineSegment *> subdivideSegment(LineSegment *segment);
    void resizeTableHeightMapSections();
    void updateHeightMapGrid(double arg1);
    void resetHeightmap();
    void storeParserState();
    void restoreParserState();
    void storeOffsets();
    void restoreOffsets();
    bool isGCodeFile(QString fileName);
    bool isHeightmapFile(QString fileName);
    bool compareCoordinates(double x, double y, double z);
    int getConsoleMinHeight();
    void UpdateOverride(SliderBox *slider, int value, char command);
    void jogStep();
    void updateJogTitle();


    Ui::frmMain *ui;
    GcodeViewParse m_viewParser;
    GcodeViewParse m_probeParser;

    OriginDrawer *m_originDrawer;

    GcodeDrawer *m_codeDrawer;
    GcodeDrawer *m_probeDrawer;
    GcodeDrawer *m_currentDrawer;

    ToolDrawer m_toolDrawer;
    HeightMapBorderDrawer m_heightMapBorderDrawer;
    HeightMapGridDrawer m_heightMapGridDrawer;
    HeightMapInterpolationDrawer m_heightMapInterpolationDrawer;

    SelectionDrawer m_selectionDrawer;

    GCodeTableModel m_programModel;
    GCodeTableModel m_probeModel;
    GCodeTableModel m_programHeightmapModel;

    HeightMapTableModel m_heightMapModel;

    bool m_programLoading;
    bool m_settingsLoading;

    Protocol_e m_Protocol;

    frmSettings *m_settings;
    frmAbout m_frmAbout;

    QString m_settingsFileName;
    QString m_programFileName;
    QString m_heightMapFileName;
    QString m_lastFolder;

    bool m_fileChanged = false;
    bool m_heightMapChanged = false;

    bool m_toolChangeActive = false;

    QTimer m_timerSpindleUpdate;
    QTimer m_timerStateQuery;
    QBasicTimer m_timerToolAnimation;

    QTimer m_timerRead;
    QTimer m_timerSend;

    QStringList m_status;
    QStringList m_statusCaptions;
    QStringList m_statusBackColors;
    QStringList m_statusForeColors;

#ifdef WINDOWS
    QWinTaskbarButton *m_taskBarButton;
    QWinTaskbarProgress *m_taskBarProgress;
#endif

    QMenu *m_tableMenu;
    QList<CommandAttributes> m_CommandAttributesList;
    QList<CommandQueue> m_CommandQueueList;
    QTime m_startTime;

    SafeQueue<CommandQueue2> mCommandsWait;
    SafeQueue<CommandQueue2> mCommandsSent;

    QMessageBox *m_senderErrorBox;

    // Stored origin
    double m_storedX = 0;
    double m_storedY = 0;
    double m_storedZ = 0;
    QString m_storedParserStatus;

    // Console window
    int m_storedConsoleMinimumHeight;
    int m_storedConsoleHeight;
    int m_consolePureHeight;

    // Flags
    bool m_settingZeroXY = false;
    bool m_settingZeroX = false;
    bool m_settingZeroZ = false;
    bool m_homing = false;
    bool m_updateSpindleSpeed = false;
    bool m_updateParserStatus = false;
    bool m_updateFeed = false;

    bool m_reseting = false;
    bool m_resetCompleted = true;
    bool m_aborting = false;
    bool m_statusReceived = false;

    bool m_processingFile = false;
    bool m_transferCompleted = false;
    bool m_fileEndSent = false;

    bool m_heightMapMode;
    bool m_cellChanged;

    // Indices
    int m_fileCommandIndex;
    int m_fileProcessedCommandIndex;
    int m_probeIndex;

    // Current values
    int m_lastDrawnLineIndex;
    int m_lastGrblStatus;
    double m_originalFeed;

    // Keyboard
    bool m_keyPressed = false;
    bool m_jogBlock = false;
    bool m_absoluteCoordinates;
    bool m_storedKeyboardControl;

    // Spindle
    bool m_spindleCW = true;
    bool m_spindleCommandSpeed = false;

    // Jog
    QVector3D m_jogVector;

    QStringList m_recentFiles;
    QStringList m_recentHeightmaps;

    // Size of internal GRBL buffer
    static const int GRBL_BUFFERLENGTH = 127;

    // Name of setting file
    const QString settings_file = "/settings.ini";

    QSerialPort m_serialHandWheel;
};


#endif // FRMMAIN_H
