/*
 * Candle 2
 * Copyright (C) 2015-2016 Hayrullin Denis Ravilevich
 * Copyright (C) 2018-2019 Patrick F.

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QStringList>
#include <QTextBlock>
#include <QTextCursor>
#include <QMessageBox>
#include <QComboBox>
#include <QScrollBar>
#include <QShortcut>
#include <QAction>
#include <QLayout>
#include <QMimeData>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QStandardPaths>
#include <QThread>

#include "frmmain.h"
#include "ui_frmmain.h"

#include "interface/SerialInterface.h"
#include "GrIP/GrIP.h"


static const int ReceiveTimerInterval_ms = 10;
static const int SendTimerInterval_ms = 30;


frmMain::frmMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::frmMain)
{
    m_status << "Unknown"
             << "Idle"
             << "Alarm"
             << "Run"
             << "Home"
             << "Hold:0"
             << "Hold:1"
             << "Queue"
             << "Check"
             << "Door"                     // TODO: Update "Door" state
             << "Jog"
             << "Dwell"
             << "Tool";

    m_statusCaptions << tr("Unknown")
                     << tr("Idle")
                     << tr("Alarm")
                     << tr("Run")
                     << tr("Home")
                     << tr("Hold")
                     << tr("Hold")
                     << tr("Queue")
                     << tr("Check")
                     << tr("Door")
                     << tr("Jog")
                     << tr("Dwell")
                     << tr("Tool Change");

    m_statusBackColors << "red"
                       << "palette(button)"
                       << "red"
                       << "lime"
                       << "lime"
                       << "yellow"
                       << "yellow"
                       << "yellow"
                       << "palette(button)"
                       << "red"
                       << "lime"
                       << "yellow"
                       << "yellow";

    m_statusForeColors << "white"
                       << "palette(text)"
                       << "white"
                       << "black"
                       << "black"
                       << "black"
                       << "black"
                       << "black"
                       << "palette(text)"
                       << "white"
                       << "black"
                       << "black"
                       << "black";

    m_coord << "G54"
            << "G55"
            << "G56"
            << "G57"
            << "G58"
            << "G59";


    // Create settings path
    std::string path = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation).toStdString();
    auto pos = path.rfind('/');
    if (pos != std::string::npos) {
        path.erase(pos);
    }
    m_settingsFileName = QString::fromStdString(path) + "/Candle2"  + settings_file;

    // Loading settings
    preloadSettings();

    m_settings = new frmSettings(this);
    ui->setupUi(this);

    // Set protocols in combo box
    ui->comboProtocol->addItem(QString("GRBL 1.1"));
    ui->comboProtocol->addItem(QString("GRBL over IP"));

    // Get available Com Ports
    UpdateComPorts();

    if(ui->comboInterface->currentText() == "ETHERNET")
    {
        ui->comboBaud->setEnabled(false);
    }
    QString tt = ui->comboInterface->currentText();


#ifdef WINDOWS
    if(QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7)
    {
        m_taskBarButton = NULL;
        m_taskBarProgress = NULL;
    }
#endif
#ifndef UNIX
    ui->cboCommand->setStyleSheet("QComboBox {padding: 2;} QComboBox::drop-down {width: 0; border-style: none;} QComboBox::down-arrow {image: url(noimg);	border-width: 0;}");
#endif

    m_heightMapMode = false;
    m_lastDrawnLineIndex = 0;
    m_fileProcessedCommandIndex = 0;
    m_cellChanged = false;
    m_programLoading = false;
    m_currentModel = &m_programModel;
    m_transferCompleted = true;

    // Set colors for command buttons
    ui->cmdXMinus->setBackColor(QColor(153, 180, 209));
    ui->cmdXPlus->setBackColor(ui->cmdXMinus->backColor());
    ui->cmdYMinus->setBackColor(ui->cmdXMinus->backColor());
    ui->cmdYPlus->setBackColor(ui->cmdXMinus->backColor());
    ui->cmdZPlus->setBackColor(ui->cmdXMinus->backColor());
    ui->cmdZMinus->setBackColor(ui->cmdXMinus->backColor());
    ui->cmdStop->setBackColor(QColor(255, 140, 140));

    // Set up 3D view buttons
    ui->cmdFit->setParent(ui->glwVisualizer);
    ui->cmdIsometric->setParent(ui->glwVisualizer);
    ui->cmdTop->setParent(ui->glwVisualizer);
    ui->cmdFront->setParent(ui->glwVisualizer);
    ui->cmdLeft->setParent(ui->glwVisualizer);

    // Set up height map
    ui->cmdHeightMapBorderAuto->setMinimumHeight(ui->chkHeightMapBorderShow->sizeHint().height());
    ui->cmdHeightMapCreate->setMinimumHeight(ui->cmdFileOpen->sizeHint().height());
    ui->cmdHeightMapLoad->setMinimumHeight(ui->cmdFileOpen->sizeHint().height());
    ui->cmdHeightMapMode->setMinimumHeight(ui->cmdFileOpen->sizeHint().height());

    ui->cboJogStep->setValidator(new QDoubleValidator(0, 10000, 2));
    ui->cboJogFeed->setValidator(new QIntValidator(0, 100000));
    connect(ui->cboJogStep, &ComboBoxKey::currentTextChanged, [=] (QString) {updateJogTitle();});
    connect(ui->cboJogFeed, &ComboBoxKey::currentTextChanged, [=] (QString) {updateJogTitle();});

    // Prepare "Send"-button
    ui->cmdFileSend->setMinimumWidth(qMax(ui->cmdFileSend->width(), ui->cmdFileOpen->width()));
    QMenu *menuSend = new QMenu();
    menuSend->addAction(tr("Send from current line"), this, SLOT(onActSendFromLineTriggered()));
    ui->cmdFileSend->setMenu(menuSend);

    connect(ui->cboCommand, SIGNAL(returnPressed()), this, SLOT(onCboCommandReturnPressed()));

    foreach (StyledToolButton* button, this->findChildren<StyledToolButton*>(QRegExp("cmdUser\\d")))
    {
        connect(button, SIGNAL(clicked(bool)), this, SLOT(onCmdUserClicked(bool)));
    }

    // Setting up slider boxes
    ui->slbFeedOverride->setRatio(1);
    ui->slbFeedOverride->setMinimum(10);
    ui->slbFeedOverride->setMaximum(200);
    ui->slbFeedOverride->setCurrentValue(100);
    ui->slbFeedOverride->setTitle(tr("Feed rate:"));
    ui->slbFeedOverride->setSuffix("%");
    connect(ui->slbFeedOverride, SIGNAL(toggled(bool)), this, SLOT(onOverridingToggled(bool)));
    connect(ui->slbFeedOverride, &SliderBox::toggled, [=] {
        updateProgramEstimatedTime(m_currentDrawer->viewParser()->getLineSegmentList());
    });
    connect(ui->slbFeedOverride, &SliderBox::valueChanged, [=] {
        updateProgramEstimatedTime(m_currentDrawer->viewParser()->getLineSegmentList());
    });

    ui->slbRapidOverride->setRatio(50);
    ui->slbRapidOverride->setMinimum(25);
    ui->slbRapidOverride->setMaximum(100);
    ui->slbRapidOverride->setCurrentValue(100);
    ui->slbRapidOverride->setTitle(tr("Rapid speed:"));
    ui->slbRapidOverride->setSuffix("%");
    connect(ui->slbRapidOverride, SIGNAL(toggled(bool)), this, SLOT(onOverridingToggled(bool)));
    connect(ui->slbRapidOverride, &SliderBox::toggled, [=] {
        updateProgramEstimatedTime(m_currentDrawer->viewParser()->getLineSegmentList());
    });
    connect(ui->slbRapidOverride, &SliderBox::valueChanged, [=] {
        updateProgramEstimatedTime(m_currentDrawer->viewParser()->getLineSegmentList());
    });

    ui->slbSpindleOverride->setRatio(1);
    ui->slbSpindleOverride->setMinimum(50);
    ui->slbSpindleOverride->setMaximum(200);
    ui->slbSpindleOverride->setCurrentValue(100);
    ui->slbSpindleOverride->setTitle(tr("Spindle speed:"));
    ui->slbSpindleOverride->setSuffix("%");
    connect(ui->slbSpindleOverride, SIGNAL(toggled(bool)), this, SLOT(onOverridingToggled(bool)));

    m_originDrawer = new OriginDrawer();
    m_codeDrawer = new GcodeDrawer();
    m_codeDrawer->setViewParser(&m_viewParser);
    m_probeDrawer = new GcodeDrawer();
    m_probeDrawer->setViewParser(&m_probeParser);
    m_probeDrawer->setVisible(false);
    m_heightMapGridDrawer.setModel(&m_heightMapModel);
    m_currentDrawer = m_codeDrawer;
    m_toolDrawer.setToolPosition(QVector3D(0, 0, 0));

    QShortcut *insertShortcut = new QShortcut(QKeySequence(Qt::Key_Insert), ui->tblProgram);
    QShortcut *deleteShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), ui->tblProgram);

    connect(insertShortcut, SIGNAL(activated()), this, SLOT(onTableInsertLine()));
    connect(deleteShortcut, SIGNAL(activated()), this, SLOT(onTableDeleteLines()));

    m_tableMenu = new QMenu(this);
    m_tableMenu->addAction(tr("&Insert line"), this, SLOT(onTableInsertLine()), insertShortcut->key());
    m_tableMenu->addAction(tr("&Delete lines"), this, SLOT(onTableDeleteLines()), deleteShortcut->key());

    ui->glwVisualizer->addDrawable(m_originDrawer);
    ui->glwVisualizer->addDrawable(m_codeDrawer);
    ui->glwVisualizer->addDrawable(m_probeDrawer);
    ui->glwVisualizer->addDrawable(&m_toolDrawer);
    ui->glwVisualizer->addDrawable(&m_heightMapBorderDrawer);
    ui->glwVisualizer->addDrawable(&m_heightMapGridDrawer);
    ui->glwVisualizer->addDrawable(&m_heightMapInterpolationDrawer);
    ui->glwVisualizer->addDrawable(&m_selectionDrawer);
    ui->glwVisualizer->fitDrawable();

    connect(ui->glwVisualizer, SIGNAL(rotationChanged()), this, SLOT(onVisualizatorRotationChanged()));
    connect(ui->glwVisualizer, SIGNAL(resized()), this, SLOT(placeVisualizerButtons()));
    connect(&m_programModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCellChanged(QModelIndex,QModelIndex)));
    connect(&m_programHeightmapModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCellChanged(QModelIndex,QModelIndex)));
    connect(&m_probeModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCellChanged(QModelIndex,QModelIndex)));
    connect(&m_heightMapModel, SIGNAL(dataChangedByUserInput()), this, SLOT(updateHeightMapInterpolationDrawer()));

    ui->tblProgram->setModel(&m_programModel);
    ui->tblProgram->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    connect(ui->tblProgram->verticalScrollBar(), SIGNAL(actionTriggered(int)), this, SLOT(onScroolBarAction(int)));
    connect(ui->tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));    
    clearTable();

    // Console window handling
    connect(ui->grpConsole, SIGNAL(resized(QSize)), this, SLOT(onConsoleResized(QSize)));
    connect(ui->scrollAreaWidgetContents, SIGNAL(sizeChanged(QSize)), this, SLOT(onPanelsSizeChanged(QSize)));

    m_senderErrorBox = new QMessageBox(QMessageBox::Warning, qApp->applicationDisplayName(), QString(), QMessageBox::Ignore | QMessageBox::Abort, this);
    m_senderErrorBox->setCheckBox(new QCheckBox(tr("Don't show again")));

    // Loading settings
    loadSettings();
    ui->tblProgram->hideColumn(4);
    ui->tblProgram->hideColumn(5);
    updateControlsState();

    // Prepare jog buttons
    foreach (StyledToolButton* button, ui->grpJog->findChildren<StyledToolButton*>(QRegExp("cmdJogFeed\\d")))
    {
        connect(button, SIGNAL(clicked(bool)), this, SLOT(onCmdJogFeedClicked()));
    }

    // Prepare User Buttons
    ui->cmdUser0->setText(m_settings->userCommands(0));
    ui->cmdUser1->setText(m_settings->userCommands(1));
    ui->cmdUser2->setText(m_settings->userCommands(2));
    ui->cmdUser3->setText(m_settings->userCommands(3));

    // Setting up spindle slider box
    ui->slbSpindle->setTitle(tr("Speed:"));
    ui->slbSpindle->setCheckable(false);
    ui->slbSpindle->setChecked(true);
    connect(ui->slbSpindle, &SliderBox::valueUserChanged, [=] {m_updateSpindleSpeed = true;});
    connect(ui->slbSpindle, &SliderBox::valueChanged, [=] {
        if (!ui->grpSpindle->isChecked() && ui->cmdSpindle->isChecked())
            ui->grpSpindle->setTitle(tr("Spindle") + QString(tr(" (%1)")).arg(ui->slbSpindle->value()));
    });

    // Set up filters
    this->installEventFilter(this);
    ui->tblProgram->installEventFilter(this);
    ui->cboJogStep->installEventFilter(this);
    ui->cboJogFeed->installEventFilter(this);
    ui->splitPanels->handle(1)->installEventFilter(this);
    ui->splitPanels->installEventFilter(this);

    // Connect timers
    connect(&m_timerSpindleUpdate, SIGNAL(timeout()), this, SLOT(onTimerUpdateSpindleParser()));
    connect(&m_timerStateQuery, SIGNAL(timeout()), this, SLOT(onTimerStatusQuery()));
    connect(&m_timerRead, SIGNAL(timeout()), this, SLOT(onProcessData()));

    connect(&m_timerSend, SIGNAL(timeout()), this, SLOT(onSendSerial()));

    // Handle file drop
    if (qApp->arguments().count() > 1 && isGCodeFile(qApp->arguments().last()))
    {
        loadFile(qApp->arguments().last());
    }

    // Set white background for lcd panels
    ui->txtWPosX->setStyleSheet("* {background-color:rgb(255, 255, 255);}");
    ui->txtWPosY->setStyleSheet("* {background-color:rgb(255, 255, 255);}");
    ui->txtWPosZ->setStyleSheet("* {background-color:rgb(255, 255, 255);}");
    ui->txtWPosA->setStyleSheet("* {background-color:rgb(255, 255, 255);}");
    ui->txtWPosB->setStyleSheet("* {background-color:rgb(255, 255, 255);}");

    // Set green background for connect/send button
    ui->btnConnect->setPalette(QPalette(QColor(255, 140, 140)));
    ui->cmdFileSend->setPalette(QPalette(QColor(64, 255, 64)));

    // Set button background red
    ui->btnHandwheel->setPalette(QPalette(QColor(255, 140, 140)));

    // Set default protocol grbl 1.1
    m_Protocol = PROT_GRBL1_1;

    // Set isometric view
    ui->glwVisualizer->setIsometricView();

    if (m_settings->layoutCompact())
    {
        ui->workCoordsLabel->hide();
        ui->machineCoordslabel->hide();
        ui->grpState->setTitle("");
        ui->grpControl->setTitle("");
        ui->grpJog->setTitle("");
        ui->grpSpindle->setTitle("");
        ui->grpHeightMap->setTitle("");
    }
    
    // Start timers
    m_timerSpindleUpdate.start(500);
    m_timerStateQuery.start(200);
    m_timerSend.start(SendTimerInterval_ms);
}

void frmMain::UpdateComPorts()
{
    // Clear combobox
    ui->comboInterface->clear();
    ui->comboHandwheel->clear();

    // Add available ports to combobox
    foreach (QSerialPortInfo info ,QSerialPortInfo::availablePorts())
    {
        ui->comboInterface->insertItem(0, info.portName());
        ui->comboHandwheel->insertItem(0, info.portName());
    }
    // Add Ethernet option
    ui->comboInterface->addItem("ETHERNET");

    // Clear baud rates
    ui->comboBaud->clear();

    // Get baudrates for serial port
    foreach (int i, QSerialPortInfo::standardBaudRates())
    {
        // Only list between 9600 to 500k
        if(i < 9600 || i > 500000)
        {
            continue;
        }
        ui->comboBaud->addItem(QString::number(i));
    }

    // Default 115200 baud
    int idx = ui->comboBaud->findText("115200");
    if(idx != -1)
    {
        ui->comboBaud->setCurrentIndex(idx);
    }

    m_serialHandWheel.setParity(QSerialPort::NoParity);
    m_serialHandWheel.setDataBits(QSerialPort::Data8);
    m_serialHandWheel.setFlowControl(QSerialPort::NoFlowControl);
    m_serialHandWheel.setStopBits(QSerialPort::OneStop);
    m_serialHandWheel.setBaudRate(230400);
}

frmMain::~frmMain()
{    
    saveSettings();

    // Close interface if still open
    if(SerialIf_IsOpen())
    {
        // Disable steppers 0x17
        if(m_Protocol == PROT_GRBL1_1)
        {
            SerialIf_Write(QByteArray(1, (char)0x17));
        }
        else if(m_Protocol == PROT_GRIP)
        {
            QByteArray data(1, (char)0x17);
            //GrIP_Transmit(MSG_SYSTEM_CMD, 0, (const uint8_t*)data.constData(), data.length());
            Pdu_t p = {(uint8_t*)data.data(), (uint16_t)data.length()};
            GrIP_Transmit(MSG_REALTIME_CMD, 0, &p);
        }

        QThread::msleep(100);

        SerialIf_Close();
    }
    if(m_serialHandWheel.isOpen())
    {
        m_serialHandWheel.close();
    }

    delete m_senderErrorBox;
    delete ui;
}

bool frmMain::isGCodeFile(QString fileName)
{
    return fileName.endsWith(".txt", Qt::CaseInsensitive) || fileName.endsWith(".nc", Qt::CaseInsensitive) || fileName.endsWith(".ncc", Qt::CaseInsensitive)
          || fileName.endsWith(".ngc", Qt::CaseInsensitive) || fileName.endsWith(".tap", Qt::CaseInsensitive) || fileName.endsWith(".gcode", Qt::CaseInsensitive);
}

bool frmMain::isHeightmapFile(QString fileName)
{
    return fileName.endsWith(".map", Qt::CaseInsensitive);
}

double frmMain::toolZPosition()
{
    return m_toolDrawer.toolPosition().z();
}

void frmMain::preloadSettings()
{
    QSettings set(m_settingsFileName, QSettings::IniFormat);
    set.setIniCodec("UTF-8");

    qApp->setStyleSheet(QString(qApp->styleSheet()).replace(QRegExp("font-size:\\s*\\d+"), "font-size: " + set.value("fontSize", "8").toString()));

    // Update v-sync in glformat
    QGLFormat fmt = QGLFormat::defaultFormat();
    fmt.setSwapInterval(set.value("vsync", false).toBool() ? 1 : 0);
    QGLFormat::setDefaultFormat(fmt);
}

void frmMain::updateControlsState()
{
    bool portOpened = SerialIf_IsOpen();

    // Disable interface options (shouldn't be changed while open)
    ui->comboProtocol->setEnabled(!portOpened);
    ui->comboInterface->setEnabled(!portOpened);
    ui->comboBaud->setEnabled(!portOpened && ui->comboInterface->currentText() != "ETHERNET");
    ui->btnReload->setEnabled(!portOpened);

    // Enable rest of gui
    ui->grpState->setEnabled(portOpened);
    ui->grpControl->setEnabled(portOpened);
    ui->widgetUserCommands->setEnabled(portOpened && !m_processingFile);
    ui->widgetSpindle->setEnabled(portOpened);
    ui->widgetJog->setEnabled(portOpened && !m_processingFile);
    ui->cboCommand->setEnabled(portOpened && (!ui->chkKeyboardControl->isChecked()));
    ui->cmdCommandSend->setEnabled(portOpened);

    ui->tabWidget->setTabEnabled(1, portOpened);
    ui->tabWidget->setTabEnabled(2, portOpened);
    ui->tabWidget->setTabEnabled(3, portOpened);

    ui->chkTestMode->setEnabled(portOpened && !m_processingFile);
    ui->cmdHome->setEnabled(!m_processingFile);
    ui->cmdTouch->setEnabled(!m_processingFile);
    ui->cmdZeroX->setEnabled(!m_processingFile);
    ui->cmdZeroY->setEnabled(!m_processingFile);
    ui->cmdZeroZ->setEnabled(!m_processingFile);
    ui->cmdRestoreOrigin->setEnabled(!m_processingFile);
    ui->cmdSafePosition->setEnabled(!m_processingFile);
    ui->cmdUnlock->setEnabled(!m_processingFile);
    ui->cmdSpindle->setEnabled(!m_processingFile);

    ui->actFileNew->setEnabled(!m_processingFile);
    ui->actFileOpen->setEnabled(!m_processingFile);
    ui->cmdFileOpen->setEnabled(!m_processingFile);
    ui->cmdFileReset->setEnabled(!m_processingFile && m_programModel.rowCount() > 1);
    ui->cmdFileSend->setEnabled(portOpened && !m_processingFile && m_programModel.rowCount() > 1);
    ui->cmdFilePause->setEnabled(m_processingFile && !ui->chkTestMode->isChecked());
    ui->cmdFileAbort->setEnabled(m_processingFile);
    ui->actFileOpen->setEnabled(!m_processingFile);
    ui->mnuRecent->setEnabled(!m_processingFile && ((m_recentFiles.count() > 0 && !m_heightMapMode)
                                                      || (m_recentHeightmaps.count() > 0 && m_heightMapMode)));
    ui->actFileSave->setEnabled(m_programModel.rowCount() > 1);
    ui->actFileSaveAs->setEnabled(m_programModel.rowCount() > 1);

    ui->tblProgram->setEditTriggers(m_processingFile ? QAbstractItemView::NoEditTriggers :
                                                         QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked
                                                         | QAbstractItemView::EditKeyPressed | QAbstractItemView::AnyKeyPressed);

    if (!portOpened)
    {
        ui->txtStatus->setText(tr("Not connected"));
        ui->txtStatus->setStyleSheet(QString("background-color: palette(button); color: palette(text);"));
    }

    this->setWindowTitle(m_programFileName.isEmpty() ? qApp->applicationDisplayName() : m_programFileName.mid(m_programFileName.lastIndexOf("/") + 1) + " - " + qApp->applicationDisplayName());

    if (!m_processingFile)
    {
        ui->chkKeyboardControl->setChecked(m_storedKeyboardControl);
    }

#ifdef WINDOWS
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7)
    {
        if (!m_processingFile && m_taskBarProgress) m_taskBarProgress->hide();
    }
#endif

    style()->unpolish(ui->cmdFileOpen);
    style()->unpolish(ui->cmdFileReset);
    style()->unpolish(ui->cmdFileSend);
    style()->unpolish(ui->cmdFilePause);
    style()->unpolish(ui->cmdFileAbort);
    ui->cmdFileOpen->ensurePolished();
    ui->cmdFileReset->ensurePolished();
    ui->cmdFileSend->ensurePolished();
    ui->cmdFilePause->ensurePolished();
    ui->cmdFileAbort->ensurePolished();

    // Heightmap
    m_heightMapBorderDrawer.setVisible(ui->chkHeightMapBorderShow->isChecked() && m_heightMapMode);
    m_heightMapGridDrawer.setVisible(ui->chkHeightMapGridShow->isChecked() && m_heightMapMode);
    m_heightMapInterpolationDrawer.setVisible(ui->chkHeightMapInterpolationShow->isChecked() && m_heightMapMode);

    ui->grpProgram->setTitle(m_heightMapMode ? tr("Heightmap") : tr("G-Code Program"));
    ui->grpProgram->setProperty("overrided", m_heightMapMode);
    style()->unpolish(ui->grpProgram);
    ui->grpProgram->ensurePolished();

    ui->grpHeightMapSettings->setVisible(m_heightMapMode);
    ui->grpHeightMapSettings->setEnabled(!m_processingFile && !ui->chkKeyboardControl->isChecked());

    ui->cboJogStep->setEditable(!ui->chkKeyboardControl->isChecked());
    ui->cboJogFeed->setEditable(!ui->chkKeyboardControl->isChecked());
    ui->cboJogStep->setStyleSheet(QString("font-size: %1").arg(m_settings->fontSize()));
    ui->cboJogFeed->setStyleSheet(ui->cboJogStep->styleSheet());

    ui->chkTestMode->setVisible(!m_heightMapMode);
    ui->chkAutoScroll->setVisible(ui->splitter->sizes()[1] && !m_heightMapMode);

    ui->tblHeightMap->setVisible(m_heightMapMode);
    ui->tblProgram->setVisible(!m_heightMapMode);

    ui->widgetHeightMap->setEnabled(!m_processingFile && m_programModel.rowCount() > 1);
    //ui->widgetHeightMap->setEnabled(false);
    ui->cmdHeightMapMode->setEnabled(!ui->txtHeightMap->text().isEmpty());

    ui->cmdFileSend->setText(m_heightMapMode ? tr("Probe") : tr("Send"));

    ui->chkHeightMapUse->setEnabled(!m_heightMapMode && !ui->txtHeightMap->text().isEmpty());

    ui->actFileSaveTransformedAs->setVisible(ui->chkHeightMapUse->isChecked());

    ui->cmdFileSend->menu()->actions().first()->setEnabled(!ui->cmdHeightMapMode->isChecked());

    m_selectionDrawer.setVisible(!ui->cmdHeightMapMode->isChecked());    
}

void frmMain::sendCommand(QString command, int tableIndex, bool showInConsole)
{
    if (!SerialIf_IsOpen() || !m_resetCompleted)
    {
        // Not connected or still resetting
        return;
    }

    CommandQueue2 cq;

    cq.command = command.toUpper();
    cq.tableIndex = tableIndex;
    cq.length = command.length() + 1;

    if (showInConsole)
    {
        ui->txtConsole->appendPlainText(command);
        cq.consoleIndex = ui->txtConsole->blockCount() - 1;
    }
    else
    {
        cq.consoleIndex = -1;
    }

    mCommandsWait.push_back(cq);
}

void frmMain::GrblReset()
{
    qDebug() << "GRBL Reset";

    ui->txtConsole->clear();

    // Reset: 0x18
    if(m_Protocol == PROT_GRBL1_1)
    {
        SerialIf_Write(QByteArray(1, (char)0x18));
    }
    else if(m_Protocol == PROT_GRIP)
    {
        //QByteArray data(1, (char)0x18);
        //GrIP_Transmit(MSG_SYSTEM_CMD, 0, (const uint8_t*)data.constData(), data.length());
        uint8_t c = 0x18;
        Pdu_t p = {&c, 1};
        GrIP_Transmit(MSG_SYSTEM_CMD, 0, &p);
    }

    m_processingFile = false;
    m_transferCompleted = true;
    m_fileCommandIndex = 0;

    m_reseting = true;
    m_homing = false;
    m_resetCompleted = false;
    // TODO: Don't set spindle speed at startup. Add option for it
    //m_updateSpindleSpeed = true;
    m_lastGrblStatus = -1;
    m_statusReceived = true;

    // Drop all remaining commands in buffer
    m_CommandAttributesList.clear();
    m_CommandQueueList.clear();

    mCommandsWait.clear();
    mCommandsSent.clear();

    // Prepare reset response catch
    /*CommandAttributes ca;

    ca.command = "[CTRL+X]";

    if (m_settings->showUICommands())
    {
        ui->txtConsole->appendPlainText(ca.command);
    }

    ca.consoleIndex = m_settings->showUICommands() ? ui->txtConsole->blockCount() - 1 : -1;
    ca.tableIndex = -1;
    ca.length = ca.command.length() + 1;
    m_CommandAttributesList.append(ca);*/

    CommandQueue2 ca;

    ca.command = "[CTRL+X]";

    if (m_settings->showUICommands())
    {
        ui->txtConsole->appendPlainText(ca.command);
    }

    ca.consoleIndex = m_settings->showUICommands() ? ui->txtConsole->blockCount() - 1 : -1;
    ca.tableIndex = -1;
    ca.length = ca.command.length() + 1;

    mCommandsSent.push_back(ca);

    updateControlsState();
}

int frmMain::BufferLength()
{
    int length = 0;

    for(int i = 0; i < mCommandsSent.size(); ++i)
    {
        auto it = mCommandsSent.get_at(i);
        length += (*it).length;
    }

    return length;
}

void frmMain::onProcessData()
{
    RX_Packet_t dat;

    switch (m_Protocol)
    {
    case PROT_GRIP:
        // GrIP
        GrIP_Update();
        if(GrIP_Receive(&dat))
            ProcessGRBL_ETH(QString(QByteArray((const char*)dat.Data, dat.RX_Header.Length)));
        break;

    case PROT_GRBL1_1:
        // GRBL 1.1
        ProcessGRBL1_1();
        break;

    case PROT_GRBL2:
        // GRBL HAL
        ProcessGRBL2();
        break;

    default:
        break;
    }

    if(m_serialHandWheel.isOpen() && SerialIf_IsOpen() && m_serialHandWheel.canReadLine() && m_transferCompleted)
    {
        QString msg = m_serialHandWheel.readLine();
        //qDebug() << "HW: " << msg;
        if(msg.size() < 3)
        {
            // RT message
            on_cmdStop_clicked();
            return;
        }
        sendCommand(msg, -1, false);
    }
    if(m_transferCompleted == false && m_serialHandWheel.isOpen())
    {
        // Drop all data received while sending file
        m_serialHandWheel.clear();
    }
}

void frmMain::onSendSerial()
{
    for(int i = 0; i < 4; i++)
    {
        if(mCommandsWait.size() > 0)
        {
            if (!SerialIf_IsOpen())
            {
                return;
            }

            CommandQueue2 q = mCommandsWait.front();
            QString command(q.command);

            if((BufferLength() + command.length() + 1) < GRBL_BUFFERLENGTH && !m_toolChangeActive)
            {
                mCommandsSent.push_back(q);
                mCommandsWait.pop_front();

                // Processing spindle speed only from g-code program
                /*QRegExp s("[Ss]0*(\\d+)");
                if (s.indexIn(command) != -1 && ca.tableIndex > -2)
                {
                    int speed = s.cap(1).toInt();
                    if (ui->slbSpindle->value() != speed)
                    {
                        ui->slbSpindle->setValue(speed);
                    }
                }*/

                //qDebug() << "Send: " + command;

                // Set M2 & M30 commands sent flag
                if (command.contains(QRegExp("M0*2|M30")))
                {
                    m_fileEndSent = true;
                }

                if(m_settings->UseM6() && (command.contains("M6") || command.contains("M06")) && command[0] != '(' && command[0] != ';')
                {
                    qDebug() << "Tool change command";
                    m_toolChangeActive = true;
                }

                if(m_Protocol == PROT_GRBL1_1)
                {
                    SerialIf_Write((command + "\r").toLatin1());
                }
                else if(m_Protocol == PROT_GRIP)
                {
                    QByteArray data((command + "\r").toLatin1());
                    //GrIP_Transmit(MSG_DATA_NO_RESPONSE, 0, (const uint8_t*)data.constData(), data.length());
                    Pdu_t p = {(uint8_t*)data.data(), (uint16_t)data.length()};
                    GrIP_Transmit(MSG_DATA_NO_RESPONSE, 0, &p);
                }
                //qDebug() << "Sent: " << command;

                m_currentModel->setData(m_currentModel->index(q.tableIndex, 2), GCodeItem::Sent);
            }
        }
    }
}

void frmMain::onTimerUpdateSpindleParser()
{
    static bool updateSpinde = false;

    if (SerialIf_IsOpen() && !m_homing && !ui->cmdFilePause->isChecked() && mCommandsWait.size() == 0)
    {
        if (m_updateSpindleSpeed)
        {
            m_updateSpindleSpeed = false;
            sendCommand(QString("S%1").arg(ui->slbSpindle->value()), -2, m_settings->showUICommands());
        }
        updateSpinde = !updateSpinde;
        if (m_updateParserStatus || updateSpinde)
        {
            m_updateParserStatus = false;
            sendCommand("$G", -3, false);
        }
    }
}

void frmMain::onTimerStatusQuery()
{
    if (SerialIf_IsOpen() && m_resetCompleted && m_statusReceived)
    {
        // Status report: ?
        if(m_Protocol == PROT_GRBL1_1)
        {
            SerialIf_Write("?");
        }
        else if(m_Protocol == PROT_GRIP)
        {
            QByteArray data("?");
            //GrIP_Transmit(MSG_SYSTEM_CMD, 0, (const uint8_t*)data.constData(), data.length());
            Pdu_t p = {(uint8_t*)data.data(), (uint16_t)data.length()};
            GrIP_Transmit(MSG_SYSTEM_CMD, 0, &p);
        }

        m_statusReceived = false;
    }

    ui->glwVisualizer->setBufferState(QString(tr("Buffer: %1 / %2 / %3")).arg(BufferLength()).arg(mCommandsSent.size()).arg(mCommandsWait.size()));
}

void frmMain::onVisualizatorRotationChanged()
{
    ui->cmdIsometric->setChecked(false);
}

void frmMain::onScroolBarAction(int action)
{
    Q_UNUSED(action)

    if (m_processingFile)
    {
        ui->chkAutoScroll->setChecked(false);
    }
}

void frmMain::onJogTimer()
{
    m_jogBlock = false;
}

void frmMain::placeVisualizerButtons()
{
    ui->cmdIsometric->move(ui->glwVisualizer->width() - ui->cmdIsometric->width() - 8, 8);
    ui->cmdTop->move(ui->cmdIsometric->geometry().left() - ui->cmdTop->width() - 8, 8);
    ui->cmdLeft->move(ui->glwVisualizer->width() - ui->cmdLeft->width() - 8, ui->cmdIsometric->geometry().bottom() + 8);
    ui->cmdFront->move(ui->cmdLeft->geometry().left() - ui->cmdFront->width() - 8, ui->cmdIsometric->geometry().bottom() + 8);
    ui->cmdFit->move(ui->glwVisualizer->width() - ui->cmdFit->width() - 8, ui->cmdLeft->geometry().bottom() + 8);
}

void frmMain::clearTable()
{
    m_programModel.clear();
    m_programModel.insertRow(0);
}

void frmMain::on_cmdFit_clicked()
{
    ui->glwVisualizer->fitDrawable(m_currentDrawer);
}

void frmMain::on_cmdFileSend_clicked()
{
    if (m_currentModel->rowCount() == 1)
    {
        return;
    }

    on_cmdFileReset_clicked();

    m_startTime.start();

    m_transferCompleted = false;
    m_processingFile = true;
    m_fileEndSent = false;
    m_storedKeyboardControl = ui->chkKeyboardControl->isChecked();
    ui->chkKeyboardControl->setChecked(false);

    if (!ui->chkTestMode->isChecked())
    {
        storeOffsets(); // Allready stored on check
    }
    storeParserState();

#ifdef WINDOWS
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7)
    {
        if (m_taskBarProgress)
        {
            m_taskBarProgress->setMaximum(m_currentModel->rowCount() - 2);
            m_taskBarProgress->setValue(0);
            m_taskBarProgress->show();
        }
    }
#endif

    updateControlsState();
    ui->cmdFilePause->setFocus();

    sendNextFileCommands();
}

void frmMain::onActSendFromLineTriggered()
{
    if (m_currentModel->rowCount() == 1)
        return;

    //Line to start from
    int commandIndex = ui->tblProgram->currentIndex().row();

    // Set parser state
    if (m_settings->autoLine())
    {
        GcodeViewParse *parser = m_currentDrawer->viewParser();
        QList<LineSegment*> list = parser->getLineSegmentList();
        QVector<QList<int>> lineIndexes = parser->getLinesIndexes();

        int lineNumber = m_currentModel->data(m_currentModel->index(commandIndex, 4)).toInt();
        LineSegment* firstSegment = list.at(lineIndexes.at(lineNumber).first());
        LineSegment* lastSegment = list.at(lineIndexes.at(lineNumber).last());
        LineSegment* feedSegment = lastSegment;
        int segmentIndex = list.indexOf(feedSegment);

        while (feedSegment->isFastTraverse() && segmentIndex > 0)
            feedSegment = list.at(--segmentIndex);

        QStringList commands;

        commands.append(QString("M3 S%1").arg(qMax<double>(lastSegment->getSpindleSpeed(), ui->slbSpindle->value())));

        commands.append(QString("G21 G90 G0 X%1 Y%2")
                        .arg(firstSegment->getStart().x())
                        .arg(firstSegment->getStart().y()));

        commands.append(QString("G1 Z%1 F%2")
                        .arg(firstSegment->getStart().z())
                        .arg(feedSegment->getSpeed()));

        commands.append(QString("%1 %2 %3 F%4")
                        .arg(lastSegment->isMetric() ? "G21" : "G20")
                        .arg(lastSegment->isAbsolute() ? "G90" : "G91")
                        .arg(lastSegment->isFastTraverse() ? "G0" : "G1")
                        .arg(lastSegment->isMetric() ? feedSegment->getSpeed() : feedSegment->getSpeed() / 25.4));

        if (lastSegment->isArc())
        {
            commands.append(lastSegment->plane() == PointSegment::XY ? "G17" : lastSegment->plane() == PointSegment::ZX ? "G18" : "G19");
        }

        QMessageBox box(this);
        box.setIcon(QMessageBox::Information);
        box.setText(tr("Following commands will be sent before selected line:\n") + commands.join('\n'));
        box.setWindowTitle(qApp->applicationDisplayName());
        box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        box.addButton(tr("Skip"), QMessageBox::DestructiveRole);

        int res = box.exec();
        if (res == QMessageBox::Cancel)
        {
            return;
        }
        else if (res == QMessageBox::Ok)
        {
            foreach (QString command, commands)
            {
                sendCommand(command, -1, m_settings->showUICommands());
            }
        }
    }

    m_fileCommandIndex = commandIndex;
    m_fileProcessedCommandIndex = commandIndex;
    m_lastDrawnLineIndex = 0;
    m_probeIndex = -1;

    QList<LineSegment*> list = m_viewParser.getLineSegmentList();

    QList<int> indexes;
    for (int i = 0; i < list.count(); i++)
    {
        list[i]->setDrawn(list.at(i)->getLineNumber() < m_currentModel->data().at(commandIndex).line);
        indexes.append(i);
    }

    m_codeDrawer->update(indexes);

    ui->tblProgram->setUpdatesEnabled(false);

    for (int i = 0; i < m_currentModel->data().count() - 1; i++)
    {
        m_currentModel->data()[i].state = i < commandIndex ? GCodeItem::Skipped : GCodeItem::InQueue;
        m_currentModel->data()[i].response = QString();
    }

    ui->tblProgram->setUpdatesEnabled(true);
    ui->glwVisualizer->setSpendTime(QTime(0, 0, 0));

    m_startTime.start();

    m_transferCompleted = false;
    m_processingFile = true;
    m_fileEndSent = false;
    m_storedKeyboardControl = ui->chkKeyboardControl->isChecked();
    ui->chkKeyboardControl->setChecked(false);

    if (!ui->chkTestMode->isChecked())
    {
        storeOffsets(); // Allready stored on check
    }

    storeParserState();

#ifdef WINDOWS
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7)
    {
        if (m_taskBarProgress)
        {
            m_taskBarProgress->setMaximum(m_currentModel->rowCount() - 2);
            m_taskBarProgress->setValue(commandIndex);
            m_taskBarProgress->show();
        }
    }
#endif

    updateControlsState();
    ui->cmdFilePause->setFocus();

    m_fileCommandIndex = commandIndex;
    m_fileProcessedCommandIndex = commandIndex;

    sendNextFileCommands();
}

void frmMain::on_cmdFileAbort_clicked()
{
    m_aborting = true;

    if (!ui->chkTestMode->isChecked())
    {
        // Feed hold: !
        if(m_Protocol == PROT_GRBL1_1)
        {
            SerialIf_Write("!");
        }
        else if(m_Protocol == PROT_GRIP)
        {
            QByteArray data("!");
            //GrIP_Transmit(MSG_REALTIME_CMD, 0, (const uint8_t*)data.constData(), data.length());
            Pdu_t p = {(uint8_t*)data.data(), (uint16_t)data.length()};
            GrIP_Transmit(MSG_REALTIME_CMD, 0, &p);
        }

    }
    else
    {
        GrblReset();
    }
}

void frmMain::storeParserState()
{    
    m_storedParserStatus = ui->glwVisualizer->parserStatus().remove(QRegExp("GC:|\\[|\\]|G[01234]\\s|M[0345]+\\s|\\sF[\\d\\.]+|\\sS[\\d\\.]+"));
}

void frmMain::restoreParserState()
{
    if (!m_storedParserStatus.isEmpty())
    {
        sendCommand(m_storedParserStatus, -1, m_settings->showUICommands());
    }
}

void frmMain::storeOffsets()
{
//    sendCommand2("$#", -2, m_settings->showUICommands());
}

void frmMain::restoreOffsets()
{
    // Still have pre-reset working position
    sendCommand(QString("G21G53G90X%1Y%2Z%3").arg(toMetric(ui->txtMPosX->text().toDouble()))
                                       .arg(toMetric(ui->txtMPosY->text().toDouble()))
                                       .arg(toMetric(ui->txtMPosZ->text().toDouble())), -1, m_settings->showUICommands());

    sendCommand(QString("G21G92X%1Y%2Z%3").arg(toMetric(ui->txtWPosX->value()))
                                       .arg(toMetric(ui->txtWPosY->value()))
                                       .arg(toMetric(ui->txtWPosZ->value())), -1, m_settings->showUICommands());
}

void frmMain::sendNextFileCommands()
{
    if (m_CommandQueueList.length() > 0)
        return;

    QString command = FeedOverride(m_currentModel->data(m_currentModel->index(m_fileCommandIndex, 1)).toString());

    while (m_fileCommandIndex < m_currentModel->rowCount() - 1 && !(!m_CommandAttributesList.isEmpty() && m_CommandAttributesList.last().command.contains(QRegExp("M0*2|M30"))))
    {
        //m_currentModel->setData(m_currentModel->index(m_fileCommandIndex, 2), GCodeItem::Sent);
        sendCommand(command, m_fileCommandIndex, m_settings->showProgramCommands());
        m_fileCommandIndex++;

        command = FeedOverride(m_currentModel->data(m_currentModel->index(m_fileCommandIndex, 1)).toString());
    }
}

void frmMain::on_actServiceSettings_triggered()
{
    if (m_settings->exec())
    {
        qDebug() << "Applying settings";

        updateControlsState();
        applySettings();
    }
    else
    {
        m_settings->undo();
    }
}

bool buttonLessThan(StyledToolButton *b1, StyledToolButton *b2)
{
    return b1->objectName().right(1).toDouble() < b2->objectName().right(1).toDouble();
}

void frmMain::updateParser()
{
    QTime time;

    qDebug() << "Updating parser:" << m_currentModel << m_currentDrawer;
    time.start();

    GcodeViewParse *parser = m_currentDrawer->viewParser();

    GcodeParser gp;
    gp.setTraverseSpeed(m_settings->rapidSpeed());
    if (m_codeDrawer->getIgnoreZ()) gp.reset(QVector3D(qQNaN(), qQNaN(), 0));

    ui->tblProgram->setUpdatesEnabled(false);

    QString stripped;
    QList<QString> args;

    QProgressDialog progress(tr("Updating..."), tr("Abort"), 0, m_currentModel->rowCount() - 2, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setFixedSize(progress.sizeHint());

    if (m_currentModel->rowCount() > PROGRESSMINLINES)
    {
        progress.show();
        progress.setStyleSheet("QProgressBar {text-align: center; qproperty-format: \"\"}");
    }

    for (int i = 0; i < m_currentModel->rowCount() - 1; i++)
    {
        // Get stored args
        args = m_currentModel->data().at(i).args;

        // Store args if none
        if (args.isEmpty())
        {
            stripped = GcodePreprocessorUtils::removeComment(m_currentModel->data().at(i).command);
            args = GcodePreprocessorUtils::splitCommand(stripped);
            m_currentModel->data()[i].args = args;
        }

        // Add command to parser
        gp.addCommand(args);

        // Update table model
        m_currentModel->data()[i].state = GCodeItem::InQueue;
        m_currentModel->data()[i].response = QString();
        m_currentModel->data()[i].line = gp.getCommandNumber();

        if (progress.isVisible() && (i % PROGRESSSTEP == 0))
        {
            progress.setValue(i);
            qApp->processEvents();

            if (progress.wasCanceled())
                break;
        }
    }

    progress.close();

    ui->tblProgram->setUpdatesEnabled(true);

    parser->reset();

    updateProgramEstimatedTime(parser->getLinesFromParser(&gp, m_settings->arcPrecision(), m_settings->arcDegreeMode()));
    m_currentDrawer->update();
    ui->glwVisualizer->updateExtremes(m_currentDrawer);
    updateControlsState();

    if (m_currentModel == &m_programModel)
    {
        m_fileChanged = true;
    }

    qDebug() << "Update parser time: " << time.elapsed();
}

void frmMain::on_cmdCommandSend_clicked()
{
    QString command = ui->cboCommand->currentText();

    if (command.isEmpty())
        return;

    command += "\n";

    ui->cboCommand->storeText();
    ui->cboCommand->setCurrentText("");

    sendCommand(command, -1);
}

void frmMain::on_actFileOpen_triggered()
{
    on_cmdFileOpen_clicked();
}

void frmMain::on_cmdHome_clicked()
{
    m_homing = true;
    m_updateSpindleSpeed = true;

    sendCommand("$H", -1, m_settings->showUICommands());
}

void frmMain::on_cmdTouch_clicked()
{
//    m_homing = true;

    QStringList list = m_settings->touchCommand().split(";");

    foreach (QString cmd, list)
    {
        sendCommand(cmd.trimmed(), -1, m_settings->showUICommands());
    }
}

void frmMain::on_cmdZeroX_clicked()
{
    m_settingZeroX = true;

    sendCommand("G92X0", -1, m_settings->showUICommands());
    sendCommand("$#", -2, m_settings->showUICommands());
}

void frmMain::on_cmdZeroY_clicked()
{
    m_settingZeroXY = true;

    sendCommand("G92Y0", -1, m_settings->showUICommands());
    sendCommand("$#", -2, m_settings->showUICommands());
}

void frmMain::on_cmdZeroZ_clicked()
{
    m_settingZeroZ = true;

    sendCommand("G92Z0", -1, m_settings->showUICommands());
    sendCommand("$#", -2, m_settings->showUICommands());
}

void frmMain::on_cmdRestoreOrigin_clicked()
{
    // Restore offset
    // G21: unit mm
    sendCommand(QString("G21"), -1, m_settings->showUICommands());

    // G53: Move in machine coordinates
    // G90: Absolut distance mode
    // G0: Rapid move
    sendCommand(QString("G53G90G0X%1Y%2Z%3").arg(toMetric(ui->txtMPosX->text().toDouble()))
                                            .arg(toMetric(ui->txtMPosY->text().toDouble()))
                                            .arg(toMetric(ui->txtMPosZ->text().toDouble())), -1, m_settings->showUICommands());

    // G92: Makes the current position have the coordiantes you want (without motion)
    sendCommand(QString("G92X%1Y%2Z%3").arg(toMetric(ui->txtMPosX->text().toDouble()) - m_storedX)
                                        .arg(toMetric(ui->txtMPosY->text().toDouble()) - m_storedY)
                                        .arg(toMetric(ui->txtMPosZ->text().toDouble()) - m_storedZ), -1, m_settings->showUICommands());

    // Move tool
    if (m_settings->moveOnRestore())
    {
        switch (m_settings->restoreMode())
        {
        case 0:
            sendCommand("G0X0Y0", -1, m_settings->showUICommands());
            break;

        case 1:
            sendCommand("G0X0Y0Z0", -1, m_settings->showUICommands());
            break;
        default:
            break;
        }
    }
}

void frmMain::on_cmdReset_clicked()
{
    GrblReset();
}

void frmMain::on_cmdUnlock_clicked()
{
    m_updateSpindleSpeed = true;

    sendCommand("$X", -1, m_settings->showUICommands());
}

void frmMain::on_cmdSafePosition_clicked()
{
    QStringList list = m_settings->safePositionCommand().split(";");

    foreach (QString cmd, list)
    {
        sendCommand(cmd.trimmed(), -1, m_settings->showUICommands());
    }
}

void frmMain::on_cmdSpindle_toggled(bool checked)
{
    ui->grpSpindle->setProperty("overrided", checked);
    style()->unpolish(ui->grpSpindle);
    ui->grpSpindle->ensurePolished();

    if (checked)
    {
        if (!ui->grpSpindle->isChecked())
        {
            ui->grpSpindle->setTitle(tr("Spindle") + QString(tr(" (%1)")).arg(ui->slbSpindle->value()));
        }
    }
    else
    {
        ui->grpSpindle->setTitle(tr("Spindle"));
    }
}

void frmMain::on_cmdSpindle_clicked(bool checked)
{
    if (ui->cmdFilePause->isChecked())
    {
        // Toggle spindle stop
        if(m_Protocol == PROT_GRBL1_1)
        {
            SerialIf_Write(QByteArray(1, char(0x9E)));
        }
        else if(m_Protocol == PROT_GRIP)
        {
            QByteArray data(1, char(0x9E));
            //GrIP_Transmit(MSG_REALTIME_CMD, 0, (const uint8_t*)data.constData(), data.length());
            Pdu_t p = {(uint8_t*)data.data(), (uint16_t)data.length()};
            GrIP_Transmit(MSG_REALTIME_CMD, 0, &p);
        }
    }
    else
    {
        sendCommand(checked ? QString("M3 S%1").arg(ui->slbSpindle->value()) : "M5", -1, m_settings->showUICommands());
    }
}

void frmMain::on_chkTestMode_clicked(bool checked)
{
    if (checked)
    {
        storeOffsets();
        storeParserState();

        sendCommand("$C", -1, m_settings->showUICommands());
    }
    else
    {
        m_aborting = true;
        GrblReset();
    }
}

void frmMain::on_cmdFilePause_clicked(bool checked)
{
    // Feed hold / Cycle Resume
    if(m_Protocol == PROT_GRBL1_1)
    {
        SerialIf_Write(checked ? "!" : "~");
    }
    else if(m_Protocol == PROT_GRIP)
    {
        QByteArray data(1, char(checked ? '!' : '~'));
        //GrIP_Transmit(MSG_REALTIME_CMD, 0, (const uint8_t*)data.constData(), data.length());
        Pdu_t p = {(uint8_t*)data.data(), (uint16_t)data.length()};
        GrIP_Transmit(MSG_REALTIME_CMD, 0, &p);
    }
}

void frmMain::on_cmdFileReset_clicked()
{
    m_fileCommandIndex = 0;
    m_fileProcessedCommandIndex = 0;
    m_lastDrawnLineIndex = 0;
    m_probeIndex = -1;

    if (!m_heightMapMode)
    {
        QTime time;

        time.start();

        QList<LineSegment*> list = m_viewParser.getLineSegmentList();

        QList<int> indexes;
        for (int i = 0; i < list.count(); i++)
        {
            list[i]->setDrawn(false);
            indexes.append(i);
        }

        m_codeDrawer->update(indexes);

        qDebug() << "Drawn false:" << time.elapsed();

        time.start();

        ui->tblProgram->setUpdatesEnabled(false);

        for (int i = 0; i < m_currentModel->data().count() - 1; i++)
        {
            m_currentModel->data()[i].state = GCodeItem::InQueue;
            m_currentModel->data()[i].response = QString();
        }

        ui->tblProgram->setUpdatesEnabled(true);

        qDebug() << "Table updated:" << time.elapsed();

        ui->tblProgram->scrollTo(m_currentModel->index(0, 0));
        ui->tblProgram->clearSelection();
        ui->tblProgram->selectRow(0);

        ui->glwVisualizer->setSpendTime(QTime(0, 0, 0));
    }
    else
    {
        ui->txtHeightMapGridX->setEnabled(true);
        ui->txtHeightMapGridY->setEnabled(true);
        ui->txtHeightMapGridZBottom->setEnabled(true);
        ui->txtHeightMapGridZTop->setEnabled(true);

        delete m_heightMapInterpolationDrawer.data();
        m_heightMapInterpolationDrawer.setData(NULL);

        m_heightMapModel.clear();
        updateHeightMapGrid();
    }
}

void frmMain::on_actFileNew_triggered()
{
    qDebug() << "changes:" << m_fileChanged << m_heightMapChanged;

    if (!saveChanges(m_heightMapMode))
        return;

    if (!m_heightMapMode)
    {
        // Reset tables
        clearTable();
        m_probeModel.clear();
        m_programHeightmapModel.clear();
        m_currentModel = &m_programModel;

        // Reset parsers
        m_viewParser.reset();
        m_probeParser.reset();

        // Reset code drawer
        m_codeDrawer->update();
        m_currentDrawer = m_codeDrawer;
        ui->glwVisualizer->fitDrawable();
        updateProgramEstimatedTime(QList<LineSegment*>());

        m_programFileName = "";
        ui->chkHeightMapUse->setChecked(false);
        ui->grpHeightMap->setProperty("overrided", false);
        style()->unpolish(ui->grpHeightMap);
        ui->grpHeightMap->ensurePolished();

        // Reset tableview
        QByteArray headerState = ui->tblProgram->horizontalHeader()->saveState();
        ui->tblProgram->setModel(NULL);

        // Set table model
        ui->tblProgram->setModel(&m_programModel);
        ui->tblProgram->horizontalHeader()->restoreState(headerState);

        // Update tableview
        connect(ui->tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));
        ui->tblProgram->selectRow(0);

        // Clear selection marker
        m_selectionDrawer.setEndPosition(QVector3D(sNan, sNan, sNan));
        m_selectionDrawer.update();

        resetHeightmap();
    }
    else
    {
        m_heightMapModel.clear();
        on_cmdFileReset_clicked();
        ui->txtHeightMap->setText(tr("Untitled"));
        m_heightMapFileName.clear();

        updateHeightMapBorderDrawer();
        updateHeightMapGrid();

        m_heightMapChanged = false;
    }

    updateControlsState();
}

void frmMain::on_cmdClearConsole_clicked()
{
    ui->txtConsole->clear();
}

bool frmMain::saveProgramToFile(QString fileName, GCodeTableModel *model)
{
    QFile file(fileName);
    QDir dir;

    qDebug() << "Saving program";

    if (file.exists())
        dir.remove(file.fileName());

    if (!file.open(QIODevice::WriteOnly))
        return false;

    QTextStream textStream(&file);

    for (int i = 0; i < model->rowCount() - 1; i++)
    {
        textStream << model->data(model->index(i, 1)).toString() << "\r\n";
    }

    file.close();

    return true;
}

void frmMain::on_actFileSaveTransformedAs_triggered()
{
    QString fileName = (QFileDialog::getSaveFileName(this, tr("Save file as"), m_lastFolder, tr("G-Code files (*.nc *.ncc *.ngc *.tap *.txt *.gcode)")));

    if (!fileName.isEmpty())
    {
        saveProgramToFile(fileName, &m_programHeightmapModel);
    }
}

void frmMain::on_actFileSaveAs_triggered()
{
    if (!m_heightMapMode)
    {
        QString fileName = (QFileDialog::getSaveFileName(this, tr("Save file as"), m_lastFolder, tr("G-Code files (*.nc *.ncc *.ngc *.tap *.txt *.gcode)")));

        if (!fileName.isEmpty()) if (saveProgramToFile(fileName, &m_programModel))
        {
            m_programFileName = fileName;
            m_fileChanged = false;

            addRecentFile(fileName);
            updateRecentFilesMenu();

            updateControlsState();
        }
    }
    else
    {
        QString fileName = (QFileDialog::getSaveFileName(this, tr("Save file as"), m_lastFolder, tr("Heightmap files (*.map)")));

        if (!fileName.isEmpty()) if (saveHeightMap(fileName))
        {
            ui->txtHeightMap->setText(fileName.mid(fileName.lastIndexOf("/") + 1));
            m_heightMapFileName = fileName;
            m_heightMapChanged = false;

            addRecentHeightmap(fileName);
            updateRecentFilesMenu();

            updateControlsState();
        }
    }
}

void frmMain::on_actFileSave_triggered()
{
    if (!m_heightMapMode)
    {
        // G-code saving
        if (m_programFileName.isEmpty())
        {
            on_actFileSaveAs_triggered();
        }
        else
        {
            saveProgramToFile(m_programFileName, &m_programModel);
            m_fileChanged = false;
        }
    }
    else
    {
        // Height map saving
        if (m_heightMapFileName.isEmpty())
        {
            on_actFileSaveAs_triggered();
        }
        else
        {
            saveHeightMap(m_heightMapFileName);
        }
    }
}

void frmMain::on_cmdTop_clicked()
{
    ui->glwVisualizer->setTopView();
}

void frmMain::on_cmdFront_clicked()
{
    ui->glwVisualizer->setFrontView();
}

void frmMain::on_cmdLeft_clicked()
{
    ui->glwVisualizer->setLeftView();
}

void frmMain::on_cmdIsometric_clicked()
{
    ui->glwVisualizer->setIsometricView();
}

void frmMain::on_actAbout_triggered()
{
    m_frmAbout.exec();
}

bool frmMain::DataIsEnd(QString data)
{
    QStringList ends;

    ends << "ok";
    ends << "error";
//    ends << "Reset to continue";
//    ends << "'$' for help";
//    ends << "'$H'|'$X' to unlock";
//    ends << "Caution: Unlocked";
//    ends << "Enabled";
//    ends << "Disabled";
//    ends << "Check Door";
//    ends << "Pgm End";

    foreach (QString str, ends)
    {
        if (data.contains(str)) return true;
    }

    return false;
}

bool frmMain::DataIsFloating(QString data)
{
    QStringList ends;

    ends << "Reset to continue";
    ends << "'$H'|'$X' to unlock";
    ends << "ALARM: Soft limit";
    ends << "ALARM: Hard limit";
    ends << "Check Door";

    foreach (QString str, ends)
    {
        if (data.contains(str))
        {
            return true;
        }
    }

    return false;
}

bool frmMain::DataIsReset(QString data)
{
    //return QRegExp("^GRBL\\s\\d\\.\\d").indexIn(data.toUpper()) != -1;
    QString g("GRBL 1.1");

    return data.toUpper().contains(g);
}

QString frmMain::FeedOverride(QString command)
{
    // Feed override if not in heightmap probing mode
//    if (!ui->cmdHeightMapMode->isChecked()) command = GcodePreprocessorUtils::overrideSpeed(command, ui->chkFeedOverride->isChecked() ?
//        ui->txtFeed->value() : 100, &m_originalFeed);

    return command;
}

void frmMain::on_grpOverriding_toggled(bool checked)
{
    if (checked)
    {
        ui->grpOverriding->setTitle(tr("Overriding"));
    }
    else if (ui->slbFeedOverride->isChecked() | ui->slbRapidOverride->isChecked() | ui->slbSpindleOverride->isChecked())
    {
        ui->grpOverriding->setTitle(tr("Overriding") + QString(tr(" (%1/%2/%3)"))
                                    .arg(ui->slbFeedOverride->isChecked() ? QString::number(ui->slbFeedOverride->value()) : "-")
                                    .arg(ui->slbRapidOverride->isChecked() ? QString::number(ui->slbRapidOverride->value()) : "-")
                                    .arg(ui->slbSpindleOverride->isChecked() ? QString::number(ui->slbSpindleOverride->value()) : "-"));
    }
    updateLayouts();

    ui->widgetFeed->setVisible(checked);
}

void frmMain::on_grpSpindle_toggled(bool checked)
{
    if (checked)
    {
        ui->grpSpindle->setTitle(tr("Spindle"));
    }
    else if (ui->cmdSpindle->isChecked())
    {
        ui->grpSpindle->setTitle(tr("Spindle") + QString(tr(" (%1)")).arg(ui->slbSpindle->value()));
    }

    updateLayouts();

    ui->widgetSpindle->setVisible(checked);
}

void frmMain::on_grpUserCommands_toggled(bool checked)
{
    ui->widgetUserCommands->setVisible(checked);
}

int frmMain::getConsoleMinHeight()
{
    return ui->grpConsole->height() - ui->grpConsole->contentsRect().height() + ui->spacerConsole->geometry().height() + ui->grpConsole->layout()->margin() * 2 + ui->cboCommand->height();
}

void frmMain::onConsoleResized(QSize size)
{
    Q_UNUSED(size)

    int minHeight = getConsoleMinHeight();
    bool visible = ui->grpConsole->height() > minHeight;

    if (ui->txtConsole->isVisible() != visible)
    {
        ui->txtConsole->setVisible(visible);
    }
}

void frmMain::onPanelsSizeChanged(QSize size)
{
    ui->splitPanels->setSizes(QList<int>() << size.height() + 4  << ui->splitPanels->height() - size.height() - 4 - ui->splitPanels->handleWidth());
}

void frmMain::on_tblProgram_customContextMenuRequested(const QPoint &pos)
{
    if (m_processingFile)
    {
        return;
    }

    if (ui->tblProgram->selectionModel()->selectedRows().count() > 0)
    {
        m_tableMenu->actions().at(0)->setEnabled(true);
        m_tableMenu->actions().at(1)->setEnabled(ui->tblProgram->selectionModel()->selectedRows()[0].row() != m_currentModel->rowCount() - 1);
    }
    else
    {
        m_tableMenu->actions().at(0)->setEnabled(false);
        m_tableMenu->actions().at(1)->setEnabled(false);
    }

    m_tableMenu->popup(ui->tblProgram->viewport()->mapToGlobal(pos));
}

void frmMain::on_splitter_splitterMoved(int pos, int index)
{
    Q_UNUSED(pos)
    Q_UNUSED(index)

    static bool tableCollapsed = ui->splitter->sizes()[1] == 0;

    if ((ui->splitter->sizes()[1] == 0) != tableCollapsed)
    {
        this->setUpdatesEnabled(false);
        ui->chkAutoScroll->setVisible(ui->splitter->sizes()[1] && !m_heightMapMode);
        updateLayouts();
        resizeCheckBoxes();

        this->setUpdatesEnabled(true);
        ui->chkAutoScroll->repaint();

        // Store collapsed state
        tableCollapsed = ui->splitter->sizes()[1] == 0;
    }
}

void frmMain::updateLayouts()
{
    this->update();
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
}

void frmMain::addRecentFile(QString fileName)
{
    m_recentFiles.removeAll(fileName);
    m_recentFiles.append(fileName);

    if (m_recentFiles.count() > 5)
    {
        m_recentFiles.takeFirst();
    }
}

void frmMain::onActRecentFileTriggered()
{
    QAction *action = static_cast<QAction*>(sender());
    QString fileName = action->text();

    if (action != NULL)
    {
        if (!saveChanges(m_heightMapMode))
            return;

        if (!m_heightMapMode)
            loadFile(fileName);
        else
            loadHeightMap(fileName);
    }
}

void frmMain::onCboCommandReturnPressed()
{
    QString command = ui->cboCommand->currentText();

    if (command.isEmpty())
        return;

    ui->cboCommand->setCurrentText("");
    sendCommand(command, -1);
}

void frmMain::updateRecentFilesMenu()
{
    foreach (QAction * action, ui->mnuRecent->actions())
    {
        if (action->text() == "")
            break;
        else
        {
            ui->mnuRecent->removeAction(action);
            delete action;
        }
    }

    foreach (QString file, !m_heightMapMode ? m_recentFiles : m_recentHeightmaps)
    {
        QAction *action = new QAction(file, this);
        connect(action, SIGNAL(triggered()), this, SLOT(onActRecentFileTriggered()));
        ui->mnuRecent->insertAction(ui->mnuRecent->actions()[0], action);
    }

    updateControlsState();
}

void frmMain::on_actRecentClear_triggered()
{
    if (!m_heightMapMode)
        m_recentFiles.clear();
    else
        m_recentHeightmaps.clear();

    updateRecentFilesMenu();
}

double frmMain::toMetric(double value)
{
    return m_settings->units() == 0 ? value : value * 25.4;
}

bool frmMain::compareCoordinates(double x, double y, double z)
{
    return ui->txtMPosX->text().toDouble() == x && ui->txtMPosY->text().toDouble() == y && ui->txtMPosZ->text().toDouble() == z;
}

void frmMain::onCmdUserClicked(bool checked)
{
    Q_UNUSED(checked);

    int i = sender()->objectName().right(1).toInt();

    QStringList list = m_settings->userCommands(i).split(";");

    foreach (QString cmd, list)
    {
        sendCommand(cmd.trimmed(), -1, m_settings->showUICommands());
    }
}

void frmMain::onOverridingToggled(bool checked)
{
    Q_UNUSED(checked);

    ui->grpOverriding->setProperty("overrided", ui->slbFeedOverride->isChecked() || ui->slbRapidOverride->isChecked() || ui->slbSpindleOverride->isChecked());

    style()->unpolish(ui->grpOverriding);
    ui->grpOverriding->ensurePolished();
}

void frmMain::UpdateOverride(SliderBox *slider, int value, char command)
{
    slider->setCurrentValue(value);

    int target = slider->isChecked() ? slider->value() : 100;
    bool smallStep = abs(target - slider->currentValue()) < 10 || m_settings->queryStateTime() < 100;

    if (slider->currentValue() < target)
    {

        if(m_Protocol == PROT_GRBL1_1)
        {
            SerialIf_Write(QByteArray(1, char(smallStep ? command + 2 : command)));
        }
        else if(m_Protocol == PROT_GRIP)
        {
            QByteArray data(1, char(smallStep ? command + 2 : command));
            //GrIP_Transmit(MSG_REALTIME_CMD, 0, (const uint8_t*)data.constData(), data.length());
            Pdu_t p = {(uint8_t*)data.data(), (uint16_t)data.length()};
            GrIP_Transmit(MSG_REALTIME_CMD, 0, &p);
        }
    }
    else if (slider->currentValue() > target)
    {
        if(m_Protocol == PROT_GRBL1_1)
        {
            SerialIf_Write(QByteArray(1, char(smallStep ? command + 3 : command + 1)));
        }
        else if(m_Protocol == PROT_GRIP)
        {
            QByteArray data(1, char(smallStep ? command + 3 : command + 1));
            //GrIP_Transmit(MSG_REALTIME_CMD, 0, (const uint8_t*)data.constData(), data.length());
            Pdu_t p = {(uint8_t*)data.data(), (uint16_t)data.length()};
            GrIP_Transmit(MSG_REALTIME_CMD, 0, &p);
        }
    }
}

void frmMain::on_cmdSpindle_triggered(QAction *arg1)
{
    Q_UNUSED(arg1);
}

void frmMain::on_btnReload_clicked()
{
    this->UpdateComPorts();
}

void frmMain::on_btnConnect_clicked()
{
    if(!SerialIf_IsOpen())
    {
        // Get used protocol
        int idx = ui->comboProtocol->currentIndex();
        switch (idx)
        {
        case 0:
            qDebug() << "GRBL 1.1";
            m_Protocol = PROT_GRBL1_1;
            break;

        case 1:
            qDebug() << "GrIP";
            m_Protocol = PROT_GRIP;
            GrIP_Init();
            break;

        default:
            qDebug() << "Default GRBL 1.1";
            m_Protocol = PROT_GRBL1_1;
            break;
        }

        QString serial_interface = ui->comboInterface->currentText();
        if(serial_interface != "ETHERNET")
        {
            // Open serial interface
            if(SerialIf_OpenSerial(0, ui->comboInterface->currentText(), ui->comboBaud->currentText().toInt()))
            {
                ui->txtStatus->setText(tr("Port opened"));
                ui->txtStatus->setStyleSheet(QString("background-color: palette(button); color: palette(text);"));
#ifndef WINDOWS
                SerialIf_Clear();
#endif
                qDebug() << "Serial OK";

                m_timerRead.start(ReceiveTimerInterval_ms);

                GrblReset();
            }
            else
            {
                ui->txtConsole->appendPlainText(tr("Serial port error: ") + SerialIf_GetError());
                qDebug() << "Coultdn't open serial port";
            }
        }
        else
        {
            // Ethernet
            if(SerialIf_OpenEth(m_settings->IPAddress(), 30501))
            {
                qDebug() << "Ethernet OK";
                // ETH only with GrIP!
                m_Protocol = PROT_GRIP;

                m_timerRead.start(ReceiveTimerInterval_ms);

                //m_statusReceived = true;

                GrblReset();

                // Test
                /*uint8_t c[] = "Hello!";
                Pdu_t p = {c, 6};
                GrIP_Transmit(1, 2, &p);*/
            }
            else
            {
                qDebug() << "Ethernet failed";
                ui->txtConsole->appendPlainText(tr("Ethernet error!") + SerialIf_GetError());
            }
        }
    }
    else
    {
        m_timerRead.stop();
        m_timerToolAnimation.stop();

        SerialIf_Close();
    }

    if(SerialIf_IsOpen())
    {
        ui->btnConnect->setText("Disconnect");
        // Set button background green
        ui->btnConnect->setPalette(QPalette(QColor(100, 255, 100)));

        ui->comboProtocol->setEnabled(false);
        ui->comboBaud->setEnabled(false);
        ui->comboInterface->setEnabled(false);
    }
    else
    {
        ui->btnConnect->setText("Connect");
        // Set button background red
        ui->btnConnect->setPalette(QPalette(QColor(255, 140, 140)));

        ui->comboProtocol->setEnabled(true);
        ui->comboBaud->setEnabled(true);
        ui->comboInterface->setEnabled(true);
    }

    this->updateControlsState();

    if(ui->comboInterface->currentText() == "ETHERNET")
    {
        // Only GrIP
        ui->comboProtocol->setCurrentIndex(1);
    }
}

void frmMain::on_comboInterface_currentTextChanged(const QString &arg1)
{
    if(arg1 == "ETHERNET")
    {
        ui->comboProtocol->setEnabled(false);
        ui->comboBaud->setEnabled(false);
        // Only GrIP
        ui->comboProtocol->setCurrentIndex(1);
    }
    else    // Serial port
    {
        ui->comboProtocol->setEnabled(true);
        ui->comboBaud->setEnabled(true);
    }
}

void frmMain::on_actionHard_Reset_triggered()
{
    if(SerialIf_IsOpen())
    {
        // Reset: 0x18
        if(m_Protocol == PROT_GRBL1_1)
        {
            SerialIf_Write(QByteArray(1, (char)0x19));
        }
        else if(m_Protocol == PROT_GRIP)
        {
            QByteArray data(1, (char)0x19);
            //GrIP_Transmit(MSG_SYSTEM_CMD, 0, (const uint8_t*)data.constData(), data.length());
            Pdu_t p = {(uint8_t*)data.data(), (uint16_t)data.length()};
            GrIP_Transmit(MSG_REALTIME_CMD, 0, &p);
        }

        //GrblReset();
        this->on_btnConnect_clicked();
    }
}

void frmMain::on_actionSet_TLS_Position_triggered()
{
    sendCommand("$P", -1, m_settings->showUICommands());
}

void frmMain::on_actionDisable_Stepper_triggered()
{
    if(SerialIf_IsOpen())
    {
        // Disable steppers 0x17
        if(m_Protocol == PROT_GRBL1_1)
        {
            SerialIf_Write(QByteArray(1, (char)0x17));
        }
        else if(m_Protocol == PROT_GRIP)
        {
            QByteArray data(1, (char)0x17);
            //GrIP_Transmit(MSG_SYSTEM_CMD, 0, (const uint8_t*)data.constData(), data.length());
            Pdu_t p = {(uint8_t*)data.data(), (uint16_t)data.length()};
            GrIP_Transmit(MSG_REALTIME_CMD, 0, &p);
        }
    }
}

void frmMain::on_btnHandwheel_clicked()
{
    if(!m_serialHandWheel.isOpen())
    {
        if(ui->comboHandwheel->currentText().size())
        {
            m_serialHandWheel.setPortName(ui->comboHandwheel->currentText());
            m_serialHandWheel.open(QIODevice::ReadWrite);
        }
    }
    else
    {
        m_serialHandWheel.close();
    }

    if(m_serialHandWheel.isOpen())
    {
        // Set button background green
        ui->btnHandwheel->setPalette(QPalette(QColor(100, 255, 100)));
        ui->comboHandwheel->setEnabled(false);
    }
    else
    {
        // Set button background red
        ui->btnHandwheel->setPalette(QPalette(QColor(255, 140, 140)));
        ui->comboHandwheel->setEnabled(true);
    }
}

void frmMain::on_btnSetCoord_clicked()
{
    sendCommand(m_coord[ui->cboCoord1->currentIndex()], -1, m_settings->showUICommands());
    m_updateParserStatus = true;
}

void frmMain::on_btnSaveCoord_clicked()
{
    QString cmd = "G10L20P";

    cmd += QString::number(ui->cboCoord2->currentIndex() + 1);
    cmd += "X" + ui->txtCoordX->text();
    cmd += "Y" + ui->txtCoordY->text();
    cmd += "Z" + ui->txtCoordZ->text();

    cmd += "\n";

    sendCommand(cmd, -1, m_settings->showUICommands());
}
