/********************************************************************************
** Form generated from reading UI file 'sliderbox.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SLIDERBOX_H
#define UI_SLIDERBOX_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "widgets/slider.h"

QT_BEGIN_NAMESPACE

class Ui_SliderBox
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_15;
    QCheckBox *chkTitle;
    QLabel *lblTitle;
    QSpinBox *txtValue;
    Slider *sliValue;

    void setupUi(QWidget *SliderBox)
    {
        if (SliderBox->objectName().isEmpty())
            SliderBox->setObjectName(QString::fromUtf8("SliderBox"));
        SliderBox->resize(185, 50);
        verticalLayout = new QVBoxLayout(SliderBox);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setObjectName(QString::fromUtf8("horizontalLayout_15"));
        chkTitle = new QCheckBox(SliderBox);
        chkTitle->setObjectName(QString::fromUtf8("chkTitle"));

        horizontalLayout_15->addWidget(chkTitle);

        lblTitle = new QLabel(SliderBox);
        lblTitle->setObjectName(QString::fromUtf8("lblTitle"));

        horizontalLayout_15->addWidget(lblTitle);

        txtValue = new QSpinBox(SliderBox);
        txtValue->setObjectName(QString::fromUtf8("txtValue"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(txtValue->sizePolicy().hasHeightForWidth());
        txtValue->setSizePolicy(sizePolicy);
        txtValue->setAlignment(Qt::AlignCenter);
        txtValue->setButtonSymbols(QAbstractSpinBox::NoButtons);
        txtValue->setMinimum(10);
        txtValue->setMaximum(200);

        horizontalLayout_15->addWidget(txtValue);

        horizontalLayout_15->setStretch(2, 1);

        verticalLayout->addLayout(horizontalLayout_15);

        sliValue = new Slider(SliderBox);
        sliValue->setObjectName(QString::fromUtf8("sliValue"));
        sliValue->setMinimum(10);
        sliValue->setMaximum(200);
        sliValue->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(sliValue);


        retranslateUi(SliderBox);

        QMetaObject::connectSlotsByName(SliderBox);
    } // setupUi

    void retranslateUi(QWidget *SliderBox)
    {
        SliderBox->setWindowTitle(QCoreApplication::translate("SliderBox", "Form", nullptr));
        chkTitle->setText(QCoreApplication::translate("SliderBox", "Title:", nullptr));
        lblTitle->setText(QCoreApplication::translate("SliderBox", "Title:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SliderBox: public Ui_SliderBox {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SLIDERBOX_H
