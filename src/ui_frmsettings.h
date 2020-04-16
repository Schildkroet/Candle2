/********************************************************************************
** Form generated from reading UI file 'frmsettings.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRMSETTINGS_H
#define UI_FRMSETTINGS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <widgets/groupbox.h>
#include <widgets/scrollarea.h>
#include <widgets/widget.h>
#include "widgets/colorpicker.h"

QT_BEGIN_NAMESPACE

class Ui_frmSettings
{
public:
    QVBoxLayout *verticalLayout_17;
    QHBoxLayout *horizontalLayout_2;
    QListWidget *listCategories;
    ScrollArea *scrollSettings;
    Widget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_4;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QLineEdit *txtIP;
    QPushButton *btnTestNetwork;
    QSpacerItem *horizontalSpacer_7;
    GroupBox *grpConnection_2;
    QVBoxLayout *verticalLayout_6;
    QCheckBox *chkIgnoreErrors;
    QCheckBox *chkAutoLine;
    QCheckBox *chkUseM6;
    QCheckBox *chkEnableRotaryAxis;
    GroupBox *grpGRBL;
    QVBoxLayout *verticalLayout_5;
    QGridLayout *gridLayout_2;
    QSpinBox *txtSpindleSpeedMin;
    QLabel *label_11;
    QSpinBox *txtQueryStateTime;
    QComboBox *cboUnits;
    QSpacerItem *horizontalSpacer_6;
    QLabel *label_15;
    QLabel *label_10;
    QSpinBox *txtRapidSpeed;
    QLabel *label_14;
    QSpinBox *txtAcceleration;
    QLabel *label_13;
    QLabel *label_12;
    QSpinBox *txtSpindleSpeedMax;
    QLabel *label_18;
    QSpinBox *txtLaserPowerMin;
    QLabel *label_35;
    QSpinBox *txtLaserPowerMax;
    GroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_9;
    QLabel *label_17;
    QLineEdit *txtTouchCommand;
    QLabel *label_33;
    QLineEdit *txtSafeCommand;
    QGridLayout *gridLayout_4;
    QHBoxLayout *horizontalLayout_8;
    QCheckBox *chkMoveOnRestore;
    QComboBox *cboRestoreMode;
    QSpacerItem *horizontalSpacer_16;
    GroupBox *groupBox_5;
    QVBoxLayout *verticalLayout_12;
    QLabel *label_34;
    QLineEdit *txtUserCommand0;
    QLabel *label_36;
    QLineEdit *txtUserCommand1;
    QLabel *label_37;
    QLineEdit *txtUserCommand2;
    QLabel *label_39;
    QLineEdit *txtUserCommand3;
    GroupBox *groupBox_4;
    QVBoxLayout *verticalLayout_11;
    QGridLayout *gridLayout_9;
    QLabel *label_38;
    QSpacerItem *horizontalSpacer_8;
    QSpinBox *txtHeightMapProbingFeed;
    GroupBox *grpParser;
    QVBoxLayout *verticalLayout_8;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_32;
    QSpacerItem *horizontalSpacer_14;
    QGridLayout *gridLayout_7;
    QRadioButton *radArcLengthMode;
    QSpacerItem *horizontalSpacer_15;
    QDoubleSpinBox *txtArcLength;
    QRadioButton *radArcDegreeMode;
    QDoubleSpinBox *txtArcDegree;
    GroupBox *grpVisualizer;
    QVBoxLayout *verticalLayout_2;
    QGridLayout *gridLayout_5;
    QCheckBox *chkZBuffer;
    QCheckBox *radMSAA;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_5;
    QDoubleSpinBox *txtLineWidth;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_7;
    QComboBox *cboFps;
    QLabel *label_6;
    QRadioButton *radDrawModeVectors;
    QRadioButton *radDrawModeRaster;
    QSpacerItem *horizontalSpacer_5;
    QRadioButton *radGrayscaleZ;
    QRadioButton *radGrayscaleS;
    QCheckBox *chkGrayscale;
    QDoubleSpinBox *txtSimplifyPrecision;
    QCheckBox *chkSimplify;
    QLabel *lblSimpilyPrecision;
    QCheckBox *chkVSync;
    QCheckBox *chkAntialiasing;
    GroupBox *grpTool;
    QGridLayout *gridLayout;
    QLabel *label_3;
    QLabel *label_4;
    QComboBox *cboToolType;
    QLabel *lblToolAngle;
    QLabel *lblToolLength;
    QSpacerItem *horizontalSpacer_4;
    QDoubleSpinBox *txtToolDiameter;
    QDoubleSpinBox *txtToolLength;
    QDoubleSpinBox *txtToolAngle;
    GroupBox *grpConsole;
    QVBoxLayout *verticalLayout_3;
    QCheckBox *chkShowProgramCommands;
    QCheckBox *chkShowUICommands;
    QCheckBox *chkAutocompletion;
    GroupBox *grpPanels;
    QGridLayout *gridLayout_3;
    QCheckBox *chkPanelUserCommands;
    QCheckBox *chkPanelHeightmap;
    QCheckBox *chkPanelJog;
    QCheckBox *chkPanelSpindle;
    QCheckBox *chkPanelOverriding;
    GroupBox *grpColors;
    QVBoxLayout *verticalLayout_7;
    QGridLayout *gridLayout_6;
    QLabel *label_20;
    QLabel *label_21;
    ColorPicker *clpToolpathZMovement;
    QLabel *label_24;
    ColorPicker *clpToolpathHighlight;
    ColorPicker *clpToolpathStart;
    QLabel *label_25;
    QLabel *label_26;
    ColorPicker *clpToolpathEnd;
    QLabel *label_27;
    ColorPicker *clpVisualizerBackground;
    ColorPicker *clpVisualizerText;
    QLabel *label_22;
    QLabel *label_28;
    QLabel *label_19;
    ColorPicker *clpTool;
    QSpacerItem *verticalSpacer;
    QLabel *label_16;
    ColorPicker *clpToolpathDrawn;
    ColorPicker *clpToolpathNormal;
    QLabel *label_23;
    GroupBox *groupBox;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_29;
    QComboBox *cboFontSize;
    QLabel *label_30;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_31;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout;
    QPushButton *cmdDefaults;
    QSpacerItem *horizontalSpacer;
    QPushButton *cmdOK;
    QPushButton *cmdCancel;

    void setupUi(QDialog *frmSettings)
    {
        if (frmSettings->objectName().isEmpty())
            frmSettings->setObjectName(QString::fromUtf8("frmSettings"));
        frmSettings->resize(607, 715);
        frmSettings->setStyleSheet(QString::fromUtf8("/*QWidget {\n"
"	font-size: 9pt;\n"
"}*/\n"
"\n"
"QSpinBox, QDoubleSpinBox {\n"
"	padding-top: 1px;\n"
"	padding-bottom: 1px;\n"
"}\n"
"\n"
"QGroupBox {\n"
"	border: none;\n"
"	padding-top: 16;\n"
"	font-weight: bold;\n"
"}"));
        frmSettings->setModal(true);
        verticalLayout_17 = new QVBoxLayout(frmSettings);
        verticalLayout_17->setObjectName(QString::fromUtf8("verticalLayout_17"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        listCategories = new QListWidget(frmSettings);
        listCategories->setObjectName(QString::fromUtf8("listCategories"));
        listCategories->setStyleSheet(QString::fromUtf8("QListWidget::item {\n"
"	height: 1.75em;\n"
"}"));
        listCategories->setSpacing(1);

        horizontalLayout_2->addWidget(listCategories);

        scrollSettings = new ScrollArea(frmSettings);
        scrollSettings->setObjectName(QString::fromUtf8("scrollSettings"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(scrollSettings->sizePolicy().hasHeightForWidth());
        scrollSettings->setSizePolicy(sizePolicy);
        scrollSettings->setFrameShape(QFrame::NoFrame);
        scrollSettings->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        scrollSettings->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollSettings->setWidgetResizable(true);
        scrollAreaWidgetContents = new Widget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 497, 1798));
        verticalLayout_4 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(scrollAreaWidgetContents);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);

        verticalLayout->addWidget(label);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_2 = new QLabel(scrollAreaWidgetContents);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setMinimumSize(QSize(62, 0));
        label_2->setFont(font);

        horizontalLayout_3->addWidget(label_2);

        txtIP = new QLineEdit(scrollAreaWidgetContents);
        txtIP->setObjectName(QString::fromUtf8("txtIP"));
        txtIP->setMinimumSize(QSize(160, 0));

        horizontalLayout_3->addWidget(txtIP);

        btnTestNetwork = new QPushButton(scrollAreaWidgetContents);
        btnTestNetwork->setObjectName(QString::fromUtf8("btnTestNetwork"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(btnTestNetwork->sizePolicy().hasHeightForWidth());
        btnTestNetwork->setSizePolicy(sizePolicy1);
        btnTestNetwork->setMinimumSize(QSize(65, 25));

        horizontalLayout_3->addWidget(btnTestNetwork);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_7);


        verticalLayout->addLayout(horizontalLayout_3);


        verticalLayout_4->addLayout(verticalLayout);

        grpConnection_2 = new GroupBox(scrollAreaWidgetContents);
        grpConnection_2->setObjectName(QString::fromUtf8("grpConnection_2"));
        verticalLayout_6 = new QVBoxLayout(grpConnection_2);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        chkIgnoreErrors = new QCheckBox(grpConnection_2);
        chkIgnoreErrors->setObjectName(QString::fromUtf8("chkIgnoreErrors"));

        verticalLayout_6->addWidget(chkIgnoreErrors);

        chkAutoLine = new QCheckBox(grpConnection_2);
        chkAutoLine->setObjectName(QString::fromUtf8("chkAutoLine"));

        verticalLayout_6->addWidget(chkAutoLine);

        chkUseM6 = new QCheckBox(grpConnection_2);
        chkUseM6->setObjectName(QString::fromUtf8("chkUseM6"));

        verticalLayout_6->addWidget(chkUseM6);

        chkEnableRotaryAxis = new QCheckBox(grpConnection_2);
        chkEnableRotaryAxis->setObjectName(QString::fromUtf8("chkEnableRotaryAxis"));

        verticalLayout_6->addWidget(chkEnableRotaryAxis);


        verticalLayout_4->addWidget(grpConnection_2);

        grpGRBL = new GroupBox(scrollAreaWidgetContents);
        grpGRBL->setObjectName(QString::fromUtf8("grpGRBL"));
        verticalLayout_5 = new QVBoxLayout(grpGRBL);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        txtSpindleSpeedMin = new QSpinBox(grpGRBL);
        txtSpindleSpeedMin->setObjectName(QString::fromUtf8("txtSpindleSpeedMin"));
        QFont font1;
        font1.setPointSize(9);
        txtSpindleSpeedMin->setFont(font1);
        txtSpindleSpeedMin->setAlignment(Qt::AlignCenter);
        txtSpindleSpeedMin->setButtonSymbols(QAbstractSpinBox::NoButtons);
        txtSpindleSpeedMin->setMaximum(99999);

        gridLayout_2->addWidget(txtSpindleSpeedMin, 2, 1, 1, 1);

        label_11 = new QLabel(grpGRBL);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        gridLayout_2->addWidget(label_11, 0, 0, 1, 1);

        txtQueryStateTime = new QSpinBox(grpGRBL);
        txtQueryStateTime->setObjectName(QString::fromUtf8("txtQueryStateTime"));
        txtQueryStateTime->setFont(font1);
        txtQueryStateTime->setAlignment(Qt::AlignCenter);
        txtQueryStateTime->setButtonSymbols(QAbstractSpinBox::NoButtons);
        txtQueryStateTime->setMaximum(9999);

        gridLayout_2->addWidget(txtQueryStateTime, 0, 1, 1, 1);

        cboUnits = new QComboBox(grpGRBL);
        cboUnits->addItem(QString());
        cboUnits->addItem(QString());
        cboUnits->setObjectName(QString::fromUtf8("cboUnits"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(cboUnits->sizePolicy().hasHeightForWidth());
        cboUnits->setSizePolicy(sizePolicy2);

        gridLayout_2->addWidget(cboUnits, 0, 4, 1, 1);

        horizontalSpacer_6 = new QSpacerItem(6, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_6, 0, 2, 1, 1);

        label_15 = new QLabel(grpGRBL);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        gridLayout_2->addWidget(label_15, 0, 3, 1, 1);

        label_10 = new QLabel(grpGRBL);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout_2->addWidget(label_10, 1, 0, 1, 1);

        txtRapidSpeed = new QSpinBox(grpGRBL);
        txtRapidSpeed->setObjectName(QString::fromUtf8("txtRapidSpeed"));
        txtRapidSpeed->setFont(font1);
        txtRapidSpeed->setAlignment(Qt::AlignCenter);
        txtRapidSpeed->setButtonSymbols(QAbstractSpinBox::NoButtons);
        txtRapidSpeed->setMaximum(99999);

        gridLayout_2->addWidget(txtRapidSpeed, 1, 1, 1, 1);

        label_14 = new QLabel(grpGRBL);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        gridLayout_2->addWidget(label_14, 1, 3, 1, 1);

        txtAcceleration = new QSpinBox(grpGRBL);
        txtAcceleration->setObjectName(QString::fromUtf8("txtAcceleration"));
        txtAcceleration->setFont(font1);
        txtAcceleration->setAlignment(Qt::AlignCenter);
        txtAcceleration->setButtonSymbols(QAbstractSpinBox::NoButtons);
        txtAcceleration->setMaximum(99999);

        gridLayout_2->addWidget(txtAcceleration, 1, 4, 1, 1);

        label_13 = new QLabel(grpGRBL);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        gridLayout_2->addWidget(label_13, 2, 3, 1, 1);

        label_12 = new QLabel(grpGRBL);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        gridLayout_2->addWidget(label_12, 2, 0, 1, 1);

        txtSpindleSpeedMax = new QSpinBox(grpGRBL);
        txtSpindleSpeedMax->setObjectName(QString::fromUtf8("txtSpindleSpeedMax"));
        txtSpindleSpeedMax->setFont(font1);
        txtSpindleSpeedMax->setAlignment(Qt::AlignCenter);
        txtSpindleSpeedMax->setButtonSymbols(QAbstractSpinBox::NoButtons);
        txtSpindleSpeedMax->setMaximum(99999);

        gridLayout_2->addWidget(txtSpindleSpeedMax, 2, 4, 1, 1);

        label_18 = new QLabel(grpGRBL);
        label_18->setObjectName(QString::fromUtf8("label_18"));

        gridLayout_2->addWidget(label_18, 3, 0, 1, 1);

        txtLaserPowerMin = new QSpinBox(grpGRBL);
        txtLaserPowerMin->setObjectName(QString::fromUtf8("txtLaserPowerMin"));
        txtLaserPowerMin->setFont(font1);
        txtLaserPowerMin->setAlignment(Qt::AlignCenter);
        txtLaserPowerMin->setButtonSymbols(QAbstractSpinBox::NoButtons);
        txtLaserPowerMin->setMaximum(99999);

        gridLayout_2->addWidget(txtLaserPowerMin, 3, 1, 1, 1);

        label_35 = new QLabel(grpGRBL);
        label_35->setObjectName(QString::fromUtf8("label_35"));

        gridLayout_2->addWidget(label_35, 3, 3, 1, 1);

        txtLaserPowerMax = new QSpinBox(grpGRBL);
        txtLaserPowerMax->setObjectName(QString::fromUtf8("txtLaserPowerMax"));
        txtLaserPowerMax->setFont(font1);
        txtLaserPowerMax->setAlignment(Qt::AlignCenter);
        txtLaserPowerMax->setButtonSymbols(QAbstractSpinBox::NoButtons);
        txtLaserPowerMax->setMaximum(99999);

        gridLayout_2->addWidget(txtLaserPowerMax, 3, 4, 1, 1);

        gridLayout_2->setColumnStretch(1, 1);
        gridLayout_2->setColumnStretch(4, 1);

        verticalLayout_5->addLayout(gridLayout_2);


        verticalLayout_4->addWidget(grpGRBL);

        groupBox_2 = new GroupBox(scrollAreaWidgetContents);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        verticalLayout_9 = new QVBoxLayout(groupBox_2);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        label_17 = new QLabel(groupBox_2);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setStyleSheet(QString::fromUtf8("padding-top: 3;"));
        label_17->setIndent(0);

        verticalLayout_9->addWidget(label_17);

        txtTouchCommand = new QLineEdit(groupBox_2);
        txtTouchCommand->setObjectName(QString::fromUtf8("txtTouchCommand"));
        txtTouchCommand->setAlignment(Qt::AlignCenter);

        verticalLayout_9->addWidget(txtTouchCommand);

        label_33 = new QLabel(groupBox_2);
        label_33->setObjectName(QString::fromUtf8("label_33"));
        label_33->setStyleSheet(QString::fromUtf8("padding-top: 3;"));
        label_33->setIndent(0);

        verticalLayout_9->addWidget(label_33);

        txtSafeCommand = new QLineEdit(groupBox_2);
        txtSafeCommand->setObjectName(QString::fromUtf8("txtSafeCommand"));
        txtSafeCommand->setAlignment(Qt::AlignCenter);

        verticalLayout_9->addWidget(txtSafeCommand);

        gridLayout_4 = new QGridLayout();
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        chkMoveOnRestore = new QCheckBox(groupBox_2);
        chkMoveOnRestore->setObjectName(QString::fromUtf8("chkMoveOnRestore"));
        QSizePolicy sizePolicy3(QSizePolicy::Maximum, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(chkMoveOnRestore->sizePolicy().hasHeightForWidth());
        chkMoveOnRestore->setSizePolicy(sizePolicy3);

        horizontalLayout_8->addWidget(chkMoveOnRestore);


        gridLayout_4->addLayout(horizontalLayout_8, 3, 0, 1, 1);

        cboRestoreMode = new QComboBox(groupBox_2);
        cboRestoreMode->addItem(QString());
        cboRestoreMode->addItem(QString());
        cboRestoreMode->setObjectName(QString::fromUtf8("cboRestoreMode"));
        cboRestoreMode->setEnabled(false);

        gridLayout_4->addWidget(cboRestoreMode, 3, 2, 1, 1);

        horizontalSpacer_16 = new QSpacerItem(9, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer_16, 3, 1, 1, 1);

        gridLayout_4->setColumnStretch(1, 1);
        gridLayout_4->setColumnStretch(2, 2);

        verticalLayout_9->addLayout(gridLayout_4);


        verticalLayout_4->addWidget(groupBox_2);

        groupBox_5 = new GroupBox(scrollAreaWidgetContents);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        verticalLayout_12 = new QVBoxLayout(groupBox_5);
        verticalLayout_12->setObjectName(QString::fromUtf8("verticalLayout_12"));
        label_34 = new QLabel(groupBox_5);
        label_34->setObjectName(QString::fromUtf8("label_34"));
        label_34->setStyleSheet(QString::fromUtf8("padding-top: 3;"));
        label_34->setIndent(0);

        verticalLayout_12->addWidget(label_34);

        txtUserCommand0 = new QLineEdit(groupBox_5);
        txtUserCommand0->setObjectName(QString::fromUtf8("txtUserCommand0"));
        txtUserCommand0->setAlignment(Qt::AlignCenter);

        verticalLayout_12->addWidget(txtUserCommand0);

        label_36 = new QLabel(groupBox_5);
        label_36->setObjectName(QString::fromUtf8("label_36"));
        label_36->setStyleSheet(QString::fromUtf8("padding-top: 3;"));
        label_36->setIndent(0);

        verticalLayout_12->addWidget(label_36);

        txtUserCommand1 = new QLineEdit(groupBox_5);
        txtUserCommand1->setObjectName(QString::fromUtf8("txtUserCommand1"));
        txtUserCommand1->setAlignment(Qt::AlignCenter);

        verticalLayout_12->addWidget(txtUserCommand1);

        label_37 = new QLabel(groupBox_5);
        label_37->setObjectName(QString::fromUtf8("label_37"));
        label_37->setStyleSheet(QString::fromUtf8("padding-top: 3;"));
        label_37->setIndent(0);

        verticalLayout_12->addWidget(label_37);

        txtUserCommand2 = new QLineEdit(groupBox_5);
        txtUserCommand2->setObjectName(QString::fromUtf8("txtUserCommand2"));
        txtUserCommand2->setAlignment(Qt::AlignCenter);

        verticalLayout_12->addWidget(txtUserCommand2);

        label_39 = new QLabel(groupBox_5);
        label_39->setObjectName(QString::fromUtf8("label_39"));
        label_39->setStyleSheet(QString::fromUtf8("padding-top: 3;"));
        label_39->setIndent(0);

        verticalLayout_12->addWidget(label_39);

        txtUserCommand3 = new QLineEdit(groupBox_5);
        txtUserCommand3->setObjectName(QString::fromUtf8("txtUserCommand3"));
        txtUserCommand3->setAlignment(Qt::AlignCenter);

        verticalLayout_12->addWidget(txtUserCommand3);


        verticalLayout_4->addWidget(groupBox_5);

        groupBox_4 = new GroupBox(scrollAreaWidgetContents);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        verticalLayout_11 = new QVBoxLayout(groupBox_4);
        verticalLayout_11->setObjectName(QString::fromUtf8("verticalLayout_11"));
        gridLayout_9 = new QGridLayout();
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        label_38 = new QLabel(groupBox_4);
        label_38->setObjectName(QString::fromUtf8("label_38"));

        gridLayout_9->addWidget(label_38, 3, 0, 1, 1);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_9->addItem(horizontalSpacer_8, 3, 1, 1, 1);

        txtHeightMapProbingFeed = new QSpinBox(groupBox_4);
        txtHeightMapProbingFeed->setObjectName(QString::fromUtf8("txtHeightMapProbingFeed"));
        txtHeightMapProbingFeed->setFont(font1);
        txtHeightMapProbingFeed->setAlignment(Qt::AlignCenter);
        txtHeightMapProbingFeed->setButtonSymbols(QAbstractSpinBox::NoButtons);
        txtHeightMapProbingFeed->setMaximum(99999);

        gridLayout_9->addWidget(txtHeightMapProbingFeed, 3, 2, 1, 1);

        gridLayout_9->setColumnStretch(1, 1);
        gridLayout_9->setColumnStretch(2, 1);

        verticalLayout_11->addLayout(gridLayout_9);


        verticalLayout_4->addWidget(groupBox_4);

        grpParser = new GroupBox(scrollAreaWidgetContents);
        grpParser->setObjectName(QString::fromUtf8("grpParser"));
        verticalLayout_8 = new QVBoxLayout(grpParser);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        label_32 = new QLabel(grpParser);
        label_32->setObjectName(QString::fromUtf8("label_32"));

        horizontalLayout_11->addWidget(label_32);

        horizontalSpacer_14 = new QSpacerItem(0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_11->addItem(horizontalSpacer_14);


        verticalLayout_8->addLayout(horizontalLayout_11);

        gridLayout_7 = new QGridLayout();
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        radArcLengthMode = new QRadioButton(grpParser);
        radArcLengthMode->setObjectName(QString::fromUtf8("radArcLengthMode"));
        radArcLengthMode->setChecked(true);

        gridLayout_7->addWidget(radArcLengthMode, 0, 0, 1, 1);

        horizontalSpacer_15 = new QSpacerItem(6, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_7->addItem(horizontalSpacer_15, 0, 2, 1, 1);

        txtArcLength = new QDoubleSpinBox(grpParser);
        txtArcLength->setObjectName(QString::fromUtf8("txtArcLength"));
        txtArcLength->setEnabled(true);
        txtArcLength->setAlignment(Qt::AlignCenter);
        txtArcLength->setButtonSymbols(QAbstractSpinBox::NoButtons);
        txtArcLength->setDecimals(2);
        txtArcLength->setMaximum(99.000000000000000);

        gridLayout_7->addWidget(txtArcLength, 0, 1, 1, 1);

        radArcDegreeMode = new QRadioButton(grpParser);
        radArcDegreeMode->setObjectName(QString::fromUtf8("radArcDegreeMode"));

        gridLayout_7->addWidget(radArcDegreeMode, 0, 3, 1, 1);

        txtArcDegree = new QDoubleSpinBox(grpParser);
        txtArcDegree->setObjectName(QString::fromUtf8("txtArcDegree"));
        txtArcDegree->setEnabled(false);
        txtArcDegree->setAlignment(Qt::AlignCenter);
        txtArcDegree->setButtonSymbols(QAbstractSpinBox::NoButtons);
        txtArcDegree->setDecimals(2);
        txtArcDegree->setMaximum(180.000000000000000);

        gridLayout_7->addWidget(txtArcDegree, 0, 4, 1, 1);


        verticalLayout_8->addLayout(gridLayout_7);


        verticalLayout_4->addWidget(grpParser);

        grpVisualizer = new GroupBox(scrollAreaWidgetContents);
        grpVisualizer->setObjectName(QString::fromUtf8("grpVisualizer"));
        verticalLayout_2 = new QVBoxLayout(grpVisualizer);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        gridLayout_5 = new QGridLayout();
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        chkZBuffer = new QCheckBox(grpVisualizer);
        chkZBuffer->setObjectName(QString::fromUtf8("chkZBuffer"));

        gridLayout_5->addWidget(chkZBuffer, 1, 4, 1, 1);

        radMSAA = new QCheckBox(grpVisualizer);
        radMSAA->setObjectName(QString::fromUtf8("radMSAA"));
        radMSAA->setEnabled(false);

        gridLayout_5->addWidget(radMSAA, 0, 4, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_5 = new QLabel(grpVisualizer);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_4->addWidget(label_5);

        txtLineWidth = new QDoubleSpinBox(grpVisualizer);
        txtLineWidth->setObjectName(QString::fromUtf8("txtLineWidth"));
        txtLineWidth->setAlignment(Qt::AlignCenter);
        txtLineWidth->setButtonSymbols(QAbstractSpinBox::NoButtons);
        txtLineWidth->setDecimals(1);
        txtLineWidth->setMaximum(9.000000000000000);

        horizontalLayout_4->addWidget(txtLineWidth);


        gridLayout_5->addLayout(horizontalLayout_4, 0, 0, 1, 1);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        label_7 = new QLabel(grpVisualizer);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        horizontalLayout_7->addWidget(label_7);

        cboFps = new QComboBox(grpVisualizer);
        cboFps->addItem(QString());
        cboFps->addItem(QString());
        cboFps->addItem(QString());
        cboFps->setObjectName(QString::fromUtf8("cboFps"));
        cboFps->setInputMethodHints(Qt::ImhDigitsOnly|Qt::ImhFormattedNumbersOnly);
        cboFps->setEditable(true);

        horizontalLayout_7->addWidget(cboFps);


        gridLayout_5->addLayout(horizontalLayout_7, 1, 0, 1, 1);

        label_6 = new QLabel(grpVisualizer);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout_5->addWidget(label_6, 2, 0, 1, 1);

        radDrawModeVectors = new QRadioButton(grpVisualizer);
        radDrawModeVectors->setObjectName(QString::fromUtf8("radDrawModeVectors"));
        radDrawModeVectors->setChecked(true);
        radDrawModeVectors->setAutoExclusive(false);

        gridLayout_5->addWidget(radDrawModeVectors, 2, 3, 1, 1);

        radDrawModeRaster = new QRadioButton(grpVisualizer);
        radDrawModeRaster->setObjectName(QString::fromUtf8("radDrawModeRaster"));
        radDrawModeRaster->setAutoExclusive(false);

        gridLayout_5->addWidget(radDrawModeRaster, 2, 4, 1, 1);

        horizontalSpacer_5 = new QSpacerItem(6, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_5->addItem(horizontalSpacer_5, 0, 2, 1, 1);

        radGrayscaleZ = new QRadioButton(grpVisualizer);
        radGrayscaleZ->setObjectName(QString::fromUtf8("radGrayscaleZ"));
        radGrayscaleZ->setEnabled(false);
        radGrayscaleZ->setAutoExclusive(false);

        gridLayout_5->addWidget(radGrayscaleZ, 4, 4, 1, 1);

        radGrayscaleS = new QRadioButton(grpVisualizer);
        radGrayscaleS->setObjectName(QString::fromUtf8("radGrayscaleS"));
        radGrayscaleS->setEnabled(false);
        radGrayscaleS->setAutoExclusive(false);

        gridLayout_5->addWidget(radGrayscaleS, 4, 3, 1, 1);

        chkGrayscale = new QCheckBox(grpVisualizer);
        chkGrayscale->setObjectName(QString::fromUtf8("chkGrayscale"));

        gridLayout_5->addWidget(chkGrayscale, 4, 0, 1, 1);

        txtSimplifyPrecision = new QDoubleSpinBox(grpVisualizer);
        txtSimplifyPrecision->setObjectName(QString::fromUtf8("txtSimplifyPrecision"));
        txtSimplifyPrecision->setEnabled(false);
        txtSimplifyPrecision->setAlignment(Qt::AlignCenter);
        txtSimplifyPrecision->setButtonSymbols(QAbstractSpinBox::NoButtons);
        txtSimplifyPrecision->setDecimals(2);
        txtSimplifyPrecision->setMaximum(99.000000000000000);

        gridLayout_5->addWidget(txtSimplifyPrecision, 3, 4, 1, 1);

        chkSimplify = new QCheckBox(grpVisualizer);
        chkSimplify->setObjectName(QString::fromUtf8("chkSimplify"));

        gridLayout_5->addWidget(chkSimplify, 3, 0, 1, 1);

        lblSimpilyPrecision = new QLabel(grpVisualizer);
        lblSimpilyPrecision->setObjectName(QString::fromUtf8("lblSimpilyPrecision"));
        lblSimpilyPrecision->setEnabled(false);

        gridLayout_5->addWidget(lblSimpilyPrecision, 3, 3, 1, 1);

        chkVSync = new QCheckBox(grpVisualizer);
        chkVSync->setObjectName(QString::fromUtf8("chkVSync"));

        gridLayout_5->addWidget(chkVSync, 1, 3, 1, 1);

        chkAntialiasing = new QCheckBox(grpVisualizer);
        chkAntialiasing->setObjectName(QString::fromUtf8("chkAntialiasing"));

        gridLayout_5->addWidget(chkAntialiasing, 0, 3, 1, 1);


        verticalLayout_2->addLayout(gridLayout_5);


        verticalLayout_4->addWidget(grpVisualizer);

        grpTool = new GroupBox(scrollAreaWidgetContents);
        grpTool->setObjectName(QString::fromUtf8("grpTool"));
        gridLayout = new QGridLayout(grpTool);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_3 = new QLabel(grpTool);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 0, 0, 1, 1);

        label_4 = new QLabel(grpTool);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 1, 0, 1, 1);

        cboToolType = new QComboBox(grpTool);
        cboToolType->addItem(QString());
        cboToolType->addItem(QString());
        cboToolType->setObjectName(QString::fromUtf8("cboToolType"));
        sizePolicy2.setHeightForWidth(cboToolType->sizePolicy().hasHeightForWidth());
        cboToolType->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(cboToolType, 0, 1, 1, 1);

        lblToolAngle = new QLabel(grpTool);
        lblToolAngle->setObjectName(QString::fromUtf8("lblToolAngle"));
        lblToolAngle->setEnabled(false);

        gridLayout->addWidget(lblToolAngle, 0, 4, 1, 1);

        lblToolLength = new QLabel(grpTool);
        lblToolLength->setObjectName(QString::fromUtf8("lblToolLength"));

        gridLayout->addWidget(lblToolLength, 1, 4, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(6, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_4, 0, 2, 1, 1);

        txtToolDiameter = new QDoubleSpinBox(grpTool);
        txtToolDiameter->setObjectName(QString::fromUtf8("txtToolDiameter"));
        sizePolicy2.setHeightForWidth(txtToolDiameter->sizePolicy().hasHeightForWidth());
        txtToolDiameter->setSizePolicy(sizePolicy2);
        txtToolDiameter->setAlignment(Qt::AlignCenter);
        txtToolDiameter->setButtonSymbols(QAbstractSpinBox::NoButtons);
        txtToolDiameter->setDecimals(3);
        txtToolDiameter->setMaximum(99.000000000000000);

        gridLayout->addWidget(txtToolDiameter, 1, 1, 1, 1);

        txtToolLength = new QDoubleSpinBox(grpTool);
        txtToolLength->setObjectName(QString::fromUtf8("txtToolLength"));
        sizePolicy2.setHeightForWidth(txtToolLength->sizePolicy().hasHeightForWidth());
        txtToolLength->setSizePolicy(sizePolicy2);
        txtToolLength->setAlignment(Qt::AlignCenter);
        txtToolLength->setButtonSymbols(QAbstractSpinBox::NoButtons);
        txtToolLength->setDecimals(3);
        txtToolLength->setMaximum(999.000000000000000);

        gridLayout->addWidget(txtToolLength, 1, 5, 1, 1);

        txtToolAngle = new QDoubleSpinBox(grpTool);
        txtToolAngle->setObjectName(QString::fromUtf8("txtToolAngle"));
        sizePolicy2.setHeightForWidth(txtToolAngle->sizePolicy().hasHeightForWidth());
        txtToolAngle->setSizePolicy(sizePolicy2);
        txtToolAngle->setAlignment(Qt::AlignCenter);
        txtToolAngle->setButtonSymbols(QAbstractSpinBox::NoButtons);
        txtToolAngle->setDecimals(3);
        txtToolAngle->setMaximum(180.000000000000000);

        gridLayout->addWidget(txtToolAngle, 0, 5, 1, 1);


        verticalLayout_4->addWidget(grpTool);

        grpConsole = new GroupBox(scrollAreaWidgetContents);
        grpConsole->setObjectName(QString::fromUtf8("grpConsole"));
        verticalLayout_3 = new QVBoxLayout(grpConsole);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        chkShowProgramCommands = new QCheckBox(grpConsole);
        chkShowProgramCommands->setObjectName(QString::fromUtf8("chkShowProgramCommands"));

        verticalLayout_3->addWidget(chkShowProgramCommands);

        chkShowUICommands = new QCheckBox(grpConsole);
        chkShowUICommands->setObjectName(QString::fromUtf8("chkShowUICommands"));

        verticalLayout_3->addWidget(chkShowUICommands);

        chkAutocompletion = new QCheckBox(grpConsole);
        chkAutocompletion->setObjectName(QString::fromUtf8("chkAutocompletion"));

        verticalLayout_3->addWidget(chkAutocompletion);


        verticalLayout_4->addWidget(grpConsole);

        grpPanels = new GroupBox(scrollAreaWidgetContents);
        grpPanels->setObjectName(QString::fromUtf8("grpPanels"));
        gridLayout_3 = new QGridLayout(grpPanels);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        chkPanelUserCommands = new QCheckBox(grpPanels);
        chkPanelUserCommands->setObjectName(QString::fromUtf8("chkPanelUserCommands"));

        gridLayout_3->addWidget(chkPanelUserCommands, 0, 0, 1, 1);

        chkPanelHeightmap = new QCheckBox(grpPanels);
        chkPanelHeightmap->setObjectName(QString::fromUtf8("chkPanelHeightmap"));

        gridLayout_3->addWidget(chkPanelHeightmap, 0, 1, 1, 1);

        chkPanelJog = new QCheckBox(grpPanels);
        chkPanelJog->setObjectName(QString::fromUtf8("chkPanelJog"));

        gridLayout_3->addWidget(chkPanelJog, 3, 0, 1, 1);

        chkPanelSpindle = new QCheckBox(grpPanels);
        chkPanelSpindle->setObjectName(QString::fromUtf8("chkPanelSpindle"));

        gridLayout_3->addWidget(chkPanelSpindle, 1, 0, 1, 1);

        chkPanelOverriding = new QCheckBox(grpPanels);
        chkPanelOverriding->setObjectName(QString::fromUtf8("chkPanelOverriding"));

        gridLayout_3->addWidget(chkPanelOverriding, 1, 1, 1, 1);


        verticalLayout_4->addWidget(grpPanels);

        grpColors = new GroupBox(scrollAreaWidgetContents);
        grpColors->setObjectName(QString::fromUtf8("grpColors"));
        verticalLayout_7 = new QVBoxLayout(grpColors);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        gridLayout_6 = new QGridLayout();
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        label_20 = new QLabel(grpColors);
        label_20->setObjectName(QString::fromUtf8("label_20"));

        gridLayout_6->addWidget(label_20, 7, 2, 1, 1);

        label_21 = new QLabel(grpColors);
        label_21->setObjectName(QString::fromUtf8("label_21"));
        label_21->setWordWrap(true);

        gridLayout_6->addWidget(label_21, 5, 0, 1, 1);

        clpToolpathZMovement = new ColorPicker(grpColors);
        clpToolpathZMovement->setObjectName(QString::fromUtf8("clpToolpathZMovement"));

        gridLayout_6->addWidget(clpToolpathZMovement, 7, 3, 1, 1);

        label_24 = new QLabel(grpColors);
        label_24->setObjectName(QString::fromUtf8("label_24"));

        gridLayout_6->addWidget(label_24, 2, 0, 1, 1);

        clpToolpathHighlight = new ColorPicker(grpColors);
        clpToolpathHighlight->setObjectName(QString::fromUtf8("clpToolpathHighlight"));

        gridLayout_6->addWidget(clpToolpathHighlight, 7, 1, 1, 1);

        clpToolpathStart = new ColorPicker(grpColors);
        clpToolpathStart->setObjectName(QString::fromUtf8("clpToolpathStart"));

        gridLayout_6->addWidget(clpToolpathStart, 8, 1, 1, 1);

        label_25 = new QLabel(grpColors);
        label_25->setObjectName(QString::fromUtf8("label_25"));

        gridLayout_6->addWidget(label_25, 7, 0, 1, 1);

        label_26 = new QLabel(grpColors);
        label_26->setObjectName(QString::fromUtf8("label_26"));

        gridLayout_6->addWidget(label_26, 8, 0, 1, 1);

        clpToolpathEnd = new ColorPicker(grpColors);
        clpToolpathEnd->setObjectName(QString::fromUtf8("clpToolpathEnd"));

        gridLayout_6->addWidget(clpToolpathEnd, 8, 3, 1, 1);

        label_27 = new QLabel(grpColors);
        label_27->setObjectName(QString::fromUtf8("label_27"));

        gridLayout_6->addWidget(label_27, 8, 2, 1, 1);

        clpVisualizerBackground = new ColorPicker(grpColors);
        clpVisualizerBackground->setObjectName(QString::fromUtf8("clpVisualizerBackground"));

        gridLayout_6->addWidget(clpVisualizerBackground, 2, 1, 1, 1);

        clpVisualizerText = new ColorPicker(grpColors);
        clpVisualizerText->setObjectName(QString::fromUtf8("clpVisualizerText"));

        gridLayout_6->addWidget(clpVisualizerText, 2, 3, 1, 1);

        label_22 = new QLabel(grpColors);
        label_22->setObjectName(QString::fromUtf8("label_22"));

        gridLayout_6->addWidget(label_22, 1, 0, 1, 1);

        label_28 = new QLabel(grpColors);
        label_28->setObjectName(QString::fromUtf8("label_28"));

        gridLayout_6->addWidget(label_28, 2, 2, 1, 1);

        label_19 = new QLabel(grpColors);
        label_19->setObjectName(QString::fromUtf8("label_19"));

        gridLayout_6->addWidget(label_19, 3, 0, 1, 1);

        clpTool = new ColorPicker(grpColors);
        clpTool->setObjectName(QString::fromUtf8("clpTool"));

        gridLayout_6->addWidget(clpTool, 3, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 6, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout_6->addItem(verticalSpacer, 4, 0, 1, 1);

        label_16 = new QLabel(grpColors);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        gridLayout_6->addWidget(label_16, 6, 2, 1, 1);

        clpToolpathDrawn = new ColorPicker(grpColors);
        clpToolpathDrawn->setObjectName(QString::fromUtf8("clpToolpathDrawn"));

        gridLayout_6->addWidget(clpToolpathDrawn, 6, 3, 1, 1);

        clpToolpathNormal = new ColorPicker(grpColors);
        clpToolpathNormal->setObjectName(QString::fromUtf8("clpToolpathNormal"));

        gridLayout_6->addWidget(clpToolpathNormal, 6, 1, 1, 1);

        label_23 = new QLabel(grpColors);
        label_23->setObjectName(QString::fromUtf8("label_23"));

        gridLayout_6->addWidget(label_23, 6, 0, 1, 1);


        verticalLayout_7->addLayout(gridLayout_6);


        verticalLayout_4->addWidget(grpColors);

        groupBox = new GroupBox(scrollAreaWidgetContents);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        horizontalLayout_10 = new QHBoxLayout(groupBox);
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        label_29 = new QLabel(groupBox);
        label_29->setObjectName(QString::fromUtf8("label_29"));

        horizontalLayout_10->addWidget(label_29);

        cboFontSize = new QComboBox(groupBox);
        cboFontSize->addItem(QString());
        cboFontSize->addItem(QString());
        cboFontSize->addItem(QString());
        cboFontSize->addItem(QString());
        cboFontSize->addItem(QString());
        cboFontSize->setObjectName(QString::fromUtf8("cboFontSize"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(cboFontSize->sizePolicy().hasHeightForWidth());
        cboFontSize->setSizePolicy(sizePolicy4);
        cboFontSize->setEditable(true);
        cboFontSize->setMaxCount(10);
        cboFontSize->setMinimumContentsLength(1);

        horizontalLayout_10->addWidget(cboFontSize);

        label_30 = new QLabel(groupBox);
        label_30->setObjectName(QString::fromUtf8("label_30"));

        horizontalLayout_10->addWidget(label_30);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_2);

        label_31 = new QLabel(groupBox);
        label_31->setObjectName(QString::fromUtf8("label_31"));
        label_31->setStyleSheet(QString::fromUtf8("color: blue;"));

        horizontalLayout_10->addWidget(label_31);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_3);


        verticalLayout_4->addWidget(groupBox);

        scrollSettings->setWidget(scrollAreaWidgetContents);

        horizontalLayout_2->addWidget(scrollSettings);


        verticalLayout_17->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        cmdDefaults = new QPushButton(frmSettings);
        cmdDefaults->setObjectName(QString::fromUtf8("cmdDefaults"));
        cmdDefaults->setAutoDefault(false);

        horizontalLayout->addWidget(cmdDefaults);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        cmdOK = new QPushButton(frmSettings);
        cmdOK->setObjectName(QString::fromUtf8("cmdOK"));
        cmdOK->setAutoDefault(false);

        horizontalLayout->addWidget(cmdOK);

        cmdCancel = new QPushButton(frmSettings);
        cmdCancel->setObjectName(QString::fromUtf8("cmdCancel"));
        cmdCancel->setAutoDefault(false);

        horizontalLayout->addWidget(cmdCancel);


        verticalLayout_17->addLayout(horizontalLayout);

        QWidget::setTabOrder(listCategories, scrollSettings);
        QWidget::setTabOrder(scrollSettings, cboToolType);
        QWidget::setTabOrder(cboToolType, txtToolAngle);
        QWidget::setTabOrder(txtToolAngle, txtToolDiameter);
        QWidget::setTabOrder(txtToolDiameter, txtToolLength);
        QWidget::setTabOrder(txtToolLength, chkShowProgramCommands);
        QWidget::setTabOrder(chkShowProgramCommands, chkShowUICommands);
        QWidget::setTabOrder(chkShowUICommands, chkAutocompletion);
        QWidget::setTabOrder(chkAutocompletion, cboFontSize);
        QWidget::setTabOrder(cboFontSize, cmdDefaults);
        QWidget::setTabOrder(cmdDefaults, cmdOK);
        QWidget::setTabOrder(cmdOK, cmdCancel);

        retranslateUi(frmSettings);
        QObject::connect(chkAntialiasing, SIGNAL(toggled(bool)), radMSAA, SLOT(setEnabled(bool)));
        QObject::connect(chkGrayscale, SIGNAL(toggled(bool)), radGrayscaleS, SLOT(setEnabled(bool)));
        QObject::connect(chkGrayscale, SIGNAL(toggled(bool)), radGrayscaleZ, SLOT(setEnabled(bool)));
        QObject::connect(chkVSync, SIGNAL(toggled(bool)), label_7, SLOT(setDisabled(bool)));
        QObject::connect(chkVSync, SIGNAL(toggled(bool)), cboFps, SLOT(setDisabled(bool)));
        QObject::connect(chkMoveOnRestore, SIGNAL(toggled(bool)), cboRestoreMode, SLOT(setEnabled(bool)));
        QObject::connect(radArcDegreeMode, SIGNAL(toggled(bool)), txtArcDegree, SLOT(setEnabled(bool)));
        QObject::connect(chkSimplify, SIGNAL(toggled(bool)), lblSimpilyPrecision, SLOT(setEnabled(bool)));
        QObject::connect(chkSimplify, SIGNAL(toggled(bool)), txtSimplifyPrecision, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(frmSettings);
    } // setupUi

    void retranslateUi(QDialog *frmSettings)
    {
        frmSettings->setWindowTitle(QCoreApplication::translate("frmSettings", "Settings", nullptr));
        label->setText(QCoreApplication::translate("frmSettings", "Ethernet", nullptr));
        label_2->setText(QCoreApplication::translate("frmSettings", "GRBL IP:", nullptr));
        btnTestNetwork->setText(QCoreApplication::translate("frmSettings", "Test", nullptr));
        grpConnection_2->setTitle(QCoreApplication::translate("frmSettings", "Sender", nullptr));
        chkIgnoreErrors->setText(QCoreApplication::translate("frmSettings", "Ignore error responses", nullptr));
        chkAutoLine->setText(QCoreApplication::translate("frmSettings", "Automatically set parser state before sending from selected line", nullptr));
        chkUseM6->setText(QCoreApplication::translate("frmSettings", "Process M6 Tool Change (GRBL-Advanced)", nullptr));
        chkEnableRotaryAxis->setText(QCoreApplication::translate("frmSettings", "Use rotary axis (A and B)", nullptr));
        grpGRBL->setTitle(QCoreApplication::translate("frmSettings", "Machine information", nullptr));
        label_11->setText(QCoreApplication::translate("frmSettings", "<html><head/><body><p>Status Query Time (ms):</p></body></html>", nullptr));
        cboUnits->setItemText(0, QCoreApplication::translate("frmSettings", "mm", nullptr));
        cboUnits->setItemText(1, QCoreApplication::translate("frmSettings", "inch", nullptr));

        label_15->setText(QCoreApplication::translate("frmSettings", "Units:", nullptr));
        label_10->setText(QCoreApplication::translate("frmSettings", "Rapid Speed:", nullptr));
        label_14->setText(QCoreApplication::translate("frmSettings", "Acceleration:", nullptr));
        label_13->setText(QCoreApplication::translate("frmSettings", "max.:", nullptr));
        label_12->setText(QCoreApplication::translate("frmSettings", "Spindle speed min.:", nullptr));
        label_18->setText(QCoreApplication::translate("frmSettings", "Laser power min.:", nullptr));
        label_35->setText(QCoreApplication::translate("frmSettings", "max.:", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("frmSettings", "Control", nullptr));
        label_17->setText(QCoreApplication::translate("frmSettings", "Probe commands:", nullptr));
        txtTouchCommand->setPlaceholderText(QCoreApplication::translate("frmSettings", "Command 1; command 2; ...", nullptr));
        label_33->setText(QCoreApplication::translate("frmSettings", "Safe position commands:", nullptr));
        txtSafeCommand->setPlaceholderText(QCoreApplication::translate("frmSettings", "Command 1; command 2; ...", nullptr));
        chkMoveOnRestore->setText(QCoreApplication::translate("frmSettings", "\"Restore origin\" moves tool in:", nullptr));
        cboRestoreMode->setItemText(0, QCoreApplication::translate("frmSettings", "Plane (XY)", nullptr));
        cboRestoreMode->setItemText(1, QCoreApplication::translate("frmSettings", "Space (XYZ)", nullptr));

        cboRestoreMode->setCurrentText(QCoreApplication::translate("frmSettings", "Plane (XY)", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("frmSettings", "User commands", nullptr));
        label_34->setText(QCoreApplication::translate("frmSettings", "Button 1:", nullptr));
        txtUserCommand0->setPlaceholderText(QCoreApplication::translate("frmSettings", "Command 1; command 2; ...", nullptr));
        label_36->setText(QCoreApplication::translate("frmSettings", "Button 2:", nullptr));
        txtUserCommand1->setPlaceholderText(QCoreApplication::translate("frmSettings", "Command 1; command 2; ...", nullptr));
        label_37->setText(QCoreApplication::translate("frmSettings", "Button 3:", nullptr));
        txtUserCommand2->setPlaceholderText(QCoreApplication::translate("frmSettings", "Command 1; command 2; ...", nullptr));
        label_39->setText(QCoreApplication::translate("frmSettings", "Button 4:", nullptr));
        txtUserCommand3->setPlaceholderText(QCoreApplication::translate("frmSettings", "Command 1; command 2; ...", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("frmSettings", "Heightmap", nullptr));
        label_38->setText(QCoreApplication::translate("frmSettings", "Heightmap probing feed:", nullptr));
        grpParser->setTitle(QCoreApplication::translate("frmSettings", "Parser", nullptr));
        label_32->setText(QCoreApplication::translate("frmSettings", "Arc approximation segment size:", nullptr));
        radArcLengthMode->setText(QCoreApplication::translate("frmSettings", "by length", nullptr));
        radArcDegreeMode->setText(QCoreApplication::translate("frmSettings", "by angle (deg.)", nullptr));
        grpVisualizer->setTitle(QCoreApplication::translate("frmSettings", "Visualizer", nullptr));
        chkZBuffer->setText(QCoreApplication::translate("frmSettings", "Z-buffer", nullptr));
        radMSAA->setText(QCoreApplication::translate("frmSettings", "MSAA", nullptr));
        label_5->setText(QCoreApplication::translate("frmSettings", "Line width:", nullptr));
        label_7->setText(QCoreApplication::translate("frmSettings", "FPS lock:", nullptr));
        cboFps->setItemText(0, QCoreApplication::translate("frmSettings", "30", nullptr));
        cboFps->setItemText(1, QCoreApplication::translate("frmSettings", "60", nullptr));
        cboFps->setItemText(2, QCoreApplication::translate("frmSettings", "120", nullptr));

        label_6->setText(QCoreApplication::translate("frmSettings", "Program draw mode:", nullptr));
        radDrawModeVectors->setText(QCoreApplication::translate("frmSettings", "Vectors", nullptr));
        radDrawModeRaster->setText(QCoreApplication::translate("frmSettings", "Raster", nullptr));
        radGrayscaleZ->setText(QCoreApplication::translate("frmSettings", "By 'Z'-code", nullptr));
        radGrayscaleS->setText(QCoreApplication::translate("frmSettings", "By 'S'-code", nullptr));
        chkGrayscale->setText(QCoreApplication::translate("frmSettings", "Grayscale segments", nullptr));
        chkSimplify->setText(QCoreApplication::translate("frmSettings", "Simplify geometry", nullptr));
        lblSimpilyPrecision->setText(QCoreApplication::translate("frmSettings", "Precision:", nullptr));
        chkVSync->setText(QCoreApplication::translate("frmSettings", "V-sync", nullptr));
        chkAntialiasing->setText(QCoreApplication::translate("frmSettings", "Smoothing", nullptr));
        grpTool->setTitle(QCoreApplication::translate("frmSettings", "Tool model", nullptr));
        label_3->setText(QCoreApplication::translate("frmSettings", "Type:", nullptr));
        label_4->setText(QCoreApplication::translate("frmSettings", "Diameter:", nullptr));
        cboToolType->setItemText(0, QCoreApplication::translate("frmSettings", "Flat", nullptr));
        cboToolType->setItemText(1, QCoreApplication::translate("frmSettings", "Conic", nullptr));

        lblToolAngle->setText(QCoreApplication::translate("frmSettings", "Angle:", nullptr));
        lblToolLength->setText(QCoreApplication::translate("frmSettings", "Length:", nullptr));
        grpConsole->setTitle(QCoreApplication::translate("frmSettings", "Console", nullptr));
        chkShowProgramCommands->setText(QCoreApplication::translate("frmSettings", "Show G-code program commands", nullptr));
        chkShowUICommands->setText(QCoreApplication::translate("frmSettings", "Show UI commands", nullptr));
        chkAutocompletion->setText(QCoreApplication::translate("frmSettings", "Command auto-completion", nullptr));
        grpPanels->setTitle(QCoreApplication::translate("frmSettings", "Panels", nullptr));
        chkPanelUserCommands->setText(QCoreApplication::translate("frmSettings", "User commands", nullptr));
        chkPanelHeightmap->setText(QCoreApplication::translate("frmSettings", "Heightmap", nullptr));
        chkPanelJog->setText(QCoreApplication::translate("frmSettings", "Jog", nullptr));
        chkPanelSpindle->setText(QCoreApplication::translate("frmSettings", "Spindle", nullptr));
        chkPanelOverriding->setText(QCoreApplication::translate("frmSettings", "Overriding", nullptr));
        grpColors->setTitle(QCoreApplication::translate("frmSettings", "Colors", nullptr));
        label_20->setText(QCoreApplication::translate("frmSettings", "Z-movement:", nullptr));
        label_21->setText(QCoreApplication::translate("frmSettings", "Toolpath", nullptr));
        label_24->setText(QCoreApplication::translate("frmSettings", "Background:", nullptr));
        label_25->setText(QCoreApplication::translate("frmSettings", "Highlight:", nullptr));
        label_26->setText(QCoreApplication::translate("frmSettings", "Start point:", nullptr));
        label_27->setText(QCoreApplication::translate("frmSettings", "End point:", nullptr));
        label_22->setText(QCoreApplication::translate("frmSettings", "Visualizer", nullptr));
        label_28->setText(QCoreApplication::translate("frmSettings", "Text:", nullptr));
        label_19->setText(QCoreApplication::translate("frmSettings", "Tool:", nullptr));
        label_16->setText(QCoreApplication::translate("frmSettings", "Drawn:", nullptr));
        label_23->setText(QCoreApplication::translate("frmSettings", "Normal:", nullptr));
        groupBox->setTitle(QCoreApplication::translate("frmSettings", "Font", nullptr));
        label_29->setText(QCoreApplication::translate("frmSettings", "Size:", nullptr));
        cboFontSize->setItemText(0, QCoreApplication::translate("frmSettings", "8", nullptr));
        cboFontSize->setItemText(1, QCoreApplication::translate("frmSettings", "9", nullptr));
        cboFontSize->setItemText(2, QCoreApplication::translate("frmSettings", "10", nullptr));
        cboFontSize->setItemText(3, QCoreApplication::translate("frmSettings", "11", nullptr));
        cboFontSize->setItemText(4, QCoreApplication::translate("frmSettings", "12", nullptr));

        cboFontSize->setCurrentText(QCoreApplication::translate("frmSettings", "8", nullptr));
        label_30->setText(QCoreApplication::translate("frmSettings", "pt", nullptr));
        label_31->setText(QCoreApplication::translate("frmSettings", "(restart needed)", nullptr));
        cmdDefaults->setText(QCoreApplication::translate("frmSettings", "Set to defaults", nullptr));
        cmdOK->setText(QCoreApplication::translate("frmSettings", "OK", nullptr));
        cmdCancel->setText(QCoreApplication::translate("frmSettings", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class frmSettings: public Ui_frmSettings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRMSETTINGS_H
