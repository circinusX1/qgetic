/********************************************************************************
** Form generated from reading UI file 'dialognewbrush.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGNEWBRUSH_H
#define UI_DIALOGNEWBRUSH_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DialogNewBrush
{
public:
    QPushButton *IDCANCEL9;
    QPushButton *IDOK8;
    QWidget *layoutWidget;
    QFormLayout *formLayout;
    QRadioButton *RB_SOLID;
    QRadioButton *RB_CUT;
    QRadioButton *RB_CUTALL;
    QWidget *layoutWidget1;
    QFormLayout *formLayout_2;
    QRadioButton *RB_LINEAR;
    QRadioButton *RB_CYLINDRIC;
    QRadioButton *RB_SPHERIC;
    QRadioButton *RB_ENVI;
    QWidget *layoutWidget2;
    QFormLayout *formLayout_3;
    QCheckBox *CK_SPLITTER;
    QCheckBox *CK_ZONE;
    QWidget *layoutWidget3;
    QGridLayout *gridLayout;
    QLabel *IDC_STATIC5;
    QLabel *IDC_STATIC6;
    QLabel *IDC_STATIC7;
    QLineEdit *EF_WIDTH;
    QLineEdit *EF_HEIGHT;
    QLineEdit *EF_DEPTH;
    QWidget *layoutWidget4;
    QFormLayout *formLayout_4;
    QCheckBox *CK_SETDETAIL;
    QCheckBox *CK_DETACHED;
    QCheckBox *CK_SHELL;
    QSpinBox *IDC_SPIN1;
    QWidget *layoutWidget5;
    QFormLayout *formLayout_5;
    QCheckBox *CK_SKYDOM;
    QCheckBox *CK_REVERSE;
    QWidget *layoutWidget6;
    QGridLayout *gridLayout_2;
    QLabel *IDC_STATICVS;
    QSpinBox *IDC_SPIN2;
    QLabel *IDC_STATICRS;
    QSpinBox *IDC_SPIN3;
    QWidget *layoutWidget7;
    QFormLayout *formLayout_6;
    QLabel *Group;
    QLabel *ST_GROUP;

    void setupUi(QDialog *DialogNewBrush)
    {
        if (DialogNewBrush->objectName().isEmpty())
            DialogNewBrush->setObjectName(QString::fromUtf8("DialogNewBrush"));
        DialogNewBrush->resize(593, 329);
        IDCANCEL9 = new QPushButton(DialogNewBrush);
        IDCANCEL9->setObjectName(QString::fromUtf8("IDCANCEL9"));
        IDCANCEL9->setGeometry(QRect(440, 70, 125, 35));
        IDOK8 = new QPushButton(DialogNewBrush);
        IDOK8->setObjectName(QString::fromUtf8("IDOK8"));
        IDOK8->setGeometry(QRect(440, 20, 125, 35));
        layoutWidget = new QWidget(DialogNewBrush);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(20, 20, 121, 121));
        formLayout = new QFormLayout(layoutWidget);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        formLayout->setContentsMargins(0, 0, 0, 0);
        RB_SOLID = new QRadioButton(layoutWidget);
        RB_SOLID->setObjectName(QString::fromUtf8("RB_SOLID"));
        RB_SOLID->setEnabled(true);
        RB_SOLID->setChecked(true);

        formLayout->setWidget(0, QFormLayout::LabelRole, RB_SOLID);

        RB_CUT = new QRadioButton(layoutWidget);
        RB_CUT->setObjectName(QString::fromUtf8("RB_CUT"));

        formLayout->setWidget(2, QFormLayout::LabelRole, RB_CUT);

        RB_CUTALL = new QRadioButton(layoutWidget);
        RB_CUTALL->setObjectName(QString::fromUtf8("RB_CUTALL"));

        formLayout->setWidget(3, QFormLayout::LabelRole, RB_CUTALL);

        layoutWidget1 = new QWidget(DialogNewBrush);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(280, 20, 141, 121));
        formLayout_2 = new QFormLayout(layoutWidget1);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setContentsMargins(0, 0, 0, 0);
        RB_LINEAR = new QRadioButton(layoutWidget1);
        RB_LINEAR->setObjectName(QString::fromUtf8("RB_LINEAR"));
        RB_LINEAR->setChecked(true);

        formLayout_2->setWidget(0, QFormLayout::LabelRole, RB_LINEAR);

        RB_CYLINDRIC = new QRadioButton(layoutWidget1);
        RB_CYLINDRIC->setObjectName(QString::fromUtf8("RB_CYLINDRIC"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, RB_CYLINDRIC);

        RB_SPHERIC = new QRadioButton(layoutWidget1);
        RB_SPHERIC->setObjectName(QString::fromUtf8("RB_SPHERIC"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, RB_SPHERIC);

        RB_ENVI = new QRadioButton(layoutWidget1);
        RB_ENVI->setObjectName(QString::fromUtf8("RB_ENVI"));

        formLayout_2->setWidget(3, QFormLayout::LabelRole, RB_ENVI);

        layoutWidget2 = new QWidget(DialogNewBrush);
        layoutWidget2->setObjectName(QString::fromUtf8("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(20, 160, 121, 56));
        formLayout_3 = new QFormLayout(layoutWidget2);
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        formLayout_3->setContentsMargins(0, 0, 0, 0);
        CK_SPLITTER = new QCheckBox(layoutWidget2);
        CK_SPLITTER->setObjectName(QString::fromUtf8("CK_SPLITTER"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, CK_SPLITTER);

        CK_ZONE = new QCheckBox(layoutWidget2);
        CK_ZONE->setObjectName(QString::fromUtf8("CK_ZONE"));

        formLayout_3->setWidget(1, QFormLayout::LabelRole, CK_ZONE);

        layoutWidget3 = new QWidget(DialogNewBrush);
        layoutWidget3->setObjectName(QString::fromUtf8("layoutWidget3"));
        layoutWidget3->setGeometry(QRect(20, 260, 401, 56));
        gridLayout = new QGridLayout(layoutWidget3);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        IDC_STATIC5 = new QLabel(layoutWidget3);
        IDC_STATIC5->setObjectName(QString::fromUtf8("IDC_STATIC5"));

        gridLayout->addWidget(IDC_STATIC5, 0, 0, 1, 1);

        IDC_STATIC6 = new QLabel(layoutWidget3);
        IDC_STATIC6->setObjectName(QString::fromUtf8("IDC_STATIC6"));

        gridLayout->addWidget(IDC_STATIC6, 0, 1, 1, 1);

        IDC_STATIC7 = new QLabel(layoutWidget3);
        IDC_STATIC7->setObjectName(QString::fromUtf8("IDC_STATIC7"));

        gridLayout->addWidget(IDC_STATIC7, 0, 2, 1, 1);

        EF_WIDTH = new QLineEdit(layoutWidget3);
        EF_WIDTH->setObjectName(QString::fromUtf8("EF_WIDTH"));

        gridLayout->addWidget(EF_WIDTH, 1, 0, 1, 1);

        EF_HEIGHT = new QLineEdit(layoutWidget3);
        EF_HEIGHT->setObjectName(QString::fromUtf8("EF_HEIGHT"));

        gridLayout->addWidget(EF_HEIGHT, 1, 1, 1, 1);

        EF_DEPTH = new QLineEdit(layoutWidget3);
        EF_DEPTH->setObjectName(QString::fromUtf8("EF_DEPTH"));

        gridLayout->addWidget(EF_DEPTH, 1, 2, 1, 1);

        layoutWidget4 = new QWidget(DialogNewBrush);
        layoutWidget4->setObjectName(QString::fromUtf8("layoutWidget4"));
        layoutWidget4->setGeometry(QRect(150, 20, 111, 121));
        formLayout_4 = new QFormLayout(layoutWidget4);
        formLayout_4->setObjectName(QString::fromUtf8("formLayout_4"));
        formLayout_4->setContentsMargins(0, 0, 0, 0);
        CK_SETDETAIL = new QCheckBox(layoutWidget4);
        CK_SETDETAIL->setObjectName(QString::fromUtf8("CK_SETDETAIL"));

        formLayout_4->setWidget(0, QFormLayout::LabelRole, CK_SETDETAIL);

        CK_DETACHED = new QCheckBox(layoutWidget4);
        CK_DETACHED->setObjectName(QString::fromUtf8("CK_DETACHED"));
        CK_DETACHED->setEnabled(false);

        formLayout_4->setWidget(1, QFormLayout::LabelRole, CK_DETACHED);

        CK_SHELL = new QCheckBox(layoutWidget4);
        CK_SHELL->setObjectName(QString::fromUtf8("CK_SHELL"));

        formLayout_4->setWidget(2, QFormLayout::LabelRole, CK_SHELL);

        IDC_SPIN1 = new QSpinBox(layoutWidget4);
        IDC_SPIN1->setObjectName(QString::fromUtf8("IDC_SPIN1"));
        IDC_SPIN1->setEnabled(false);
        IDC_SPIN1->setMinimum(2);
        IDC_SPIN1->setMaximum(512);
        IDC_SPIN1->setValue(16);

        formLayout_4->setWidget(3, QFormLayout::LabelRole, IDC_SPIN1);

        layoutWidget5 = new QWidget(DialogNewBrush);
        layoutWidget5->setObjectName(QString::fromUtf8("layoutWidget5"));
        layoutWidget5->setGeometry(QRect(150, 160, 111, 56));
        formLayout_5 = new QFormLayout(layoutWidget5);
        formLayout_5->setObjectName(QString::fromUtf8("formLayout_5"));
        formLayout_5->setContentsMargins(0, 0, 0, 0);
        CK_SKYDOM = new QCheckBox(layoutWidget5);
        CK_SKYDOM->setObjectName(QString::fromUtf8("CK_SKYDOM"));

        formLayout_5->setWidget(0, QFormLayout::LabelRole, CK_SKYDOM);

        CK_REVERSE = new QCheckBox(layoutWidget5);
        CK_REVERSE->setObjectName(QString::fromUtf8("CK_REVERSE"));

        formLayout_5->setWidget(1, QFormLayout::LabelRole, CK_REVERSE);

        layoutWidget6 = new QWidget(DialogNewBrush);
        layoutWidget6->setObjectName(QString::fromUtf8("layoutWidget6"));
        layoutWidget6->setGeometry(QRect(280, 160, 145, 66));
        gridLayout_2 = new QGridLayout(layoutWidget6);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        IDC_STATICVS = new QLabel(layoutWidget6);
        IDC_STATICVS->setObjectName(QString::fromUtf8("IDC_STATICVS"));

        gridLayout_2->addWidget(IDC_STATICVS, 0, 0, 1, 1);

        IDC_SPIN2 = new QSpinBox(layoutWidget6);
        IDC_SPIN2->setObjectName(QString::fromUtf8("IDC_SPIN2"));
        IDC_SPIN2->setMinimum(1);
        IDC_SPIN2->setMaximum(64);
        IDC_SPIN2->setValue(16);

        gridLayout_2->addWidget(IDC_SPIN2, 0, 1, 1, 1);

        IDC_STATICRS = new QLabel(layoutWidget6);
        IDC_STATICRS->setObjectName(QString::fromUtf8("IDC_STATICRS"));

        gridLayout_2->addWidget(IDC_STATICRS, 1, 0, 1, 1);

        IDC_SPIN3 = new QSpinBox(layoutWidget6);
        IDC_SPIN3->setObjectName(QString::fromUtf8("IDC_SPIN3"));
        IDC_SPIN3->setMinimum(1);
        IDC_SPIN3->setMaximum(64);
        IDC_SPIN3->setValue(16);

        gridLayout_2->addWidget(IDC_SPIN3, 1, 1, 1, 1);

        layoutWidget7 = new QWidget(DialogNewBrush);
        layoutWidget7->setObjectName(QString::fromUtf8("layoutWidget7"));
        layoutWidget7->setGeometry(QRect(20, 220, 401, 31));
        formLayout_6 = new QFormLayout(layoutWidget7);
        formLayout_6->setObjectName(QString::fromUtf8("formLayout_6"));
        formLayout_6->setContentsMargins(0, 0, 0, 0);
        Group = new QLabel(layoutWidget7);
        Group->setObjectName(QString::fromUtf8("Group"));

        formLayout_6->setWidget(0, QFormLayout::LabelRole, Group);

        ST_GROUP = new QLabel(layoutWidget7);
        ST_GROUP->setObjectName(QString::fromUtf8("ST_GROUP"));

        formLayout_6->setWidget(0, QFormLayout::FieldRole, ST_GROUP);

        QWidget::setTabOrder(RB_SOLID, RB_CUT);
        QWidget::setTabOrder(RB_CUT, CK_SPLITTER);
        QWidget::setTabOrder(CK_SPLITTER, CK_ZONE);
        QWidget::setTabOrder(CK_ZONE, CK_SETDETAIL);
        QWidget::setTabOrder(CK_SETDETAIL, CK_DETACHED);
        QWidget::setTabOrder(CK_DETACHED, CK_SHELL);
        QWidget::setTabOrder(CK_SHELL, CK_SKYDOM);
        QWidget::setTabOrder(CK_SKYDOM, CK_REVERSE);
        QWidget::setTabOrder(CK_REVERSE, RB_LINEAR);
        QWidget::setTabOrder(RB_LINEAR, RB_CYLINDRIC);
        QWidget::setTabOrder(RB_CYLINDRIC, RB_SPHERIC);
        QWidget::setTabOrder(RB_SPHERIC, RB_ENVI);
        QWidget::setTabOrder(RB_ENVI, IDC_SPIN2);
        QWidget::setTabOrder(IDC_SPIN2, IDC_SPIN3);
        QWidget::setTabOrder(IDC_SPIN3, EF_WIDTH);
        QWidget::setTabOrder(EF_WIDTH, EF_HEIGHT);
        QWidget::setTabOrder(EF_HEIGHT, EF_DEPTH);
        QWidget::setTabOrder(EF_DEPTH, IDOK8);
        QWidget::setTabOrder(IDOK8, IDCANCEL9);

        retranslateUi(DialogNewBrush);

        QMetaObject::connectSlotsByName(DialogNewBrush);
    } // setupUi

    void retranslateUi(QDialog *DialogNewBrush)
    {
        DialogNewBrush->setWindowTitle(QApplication::translate("DialogNewBrush", "Dialog", 0, QApplication::UnicodeUTF8));
        IDCANCEL9->setText(QApplication::translate("DialogNewBrush", "Cancel", 0, QApplication::UnicodeUTF8));
        IDOK8->setText(QApplication::translate("DialogNewBrush", "Create", 0, QApplication::UnicodeUTF8));
        RB_SOLID->setText(QApplication::translate("DialogNewBrush", "Solid", 0, QApplication::UnicodeUTF8));
        RB_CUT->setText(QApplication::translate("DialogNewBrush", "Cut", 0, QApplication::UnicodeUTF8));
        RB_CUTALL->setText(QApplication::translate("DialogNewBrush", "Cut All", 0, QApplication::UnicodeUTF8));
        RB_LINEAR->setText(QApplication::translate("DialogNewBrush", "Linear", 0, QApplication::UnicodeUTF8));
        RB_CYLINDRIC->setText(QApplication::translate("DialogNewBrush", "Cylindic", 0, QApplication::UnicodeUTF8));
        RB_SPHERIC->setText(QApplication::translate("DialogNewBrush", "Spheric", 0, QApplication::UnicodeUTF8));
        RB_ENVI->setText(QApplication::translate("DialogNewBrush", "Environment", 0, QApplication::UnicodeUTF8));
        CK_SPLITTER->setText(QApplication::translate("DialogNewBrush", "Splitter", 0, QApplication::UnicodeUTF8));
        CK_ZONE->setText(QApplication::translate("DialogNewBrush", "Zone", 0, QApplication::UnicodeUTF8));
        IDC_STATIC5->setText(QApplication::translate("DialogNewBrush", "Width", 0, QApplication::UnicodeUTF8));
        IDC_STATIC6->setText(QApplication::translate("DialogNewBrush", "Height", 0, QApplication::UnicodeUTF8));
        IDC_STATIC7->setText(QApplication::translate("DialogNewBrush", "Depth", 0, QApplication::UnicodeUTF8));
        CK_SETDETAIL->setText(QApplication::translate("DialogNewBrush", "&Detail", 0, QApplication::UnicodeUTF8));
        CK_DETACHED->setText(QApplication::translate("DialogNewBrush", "Det&ached", 0, QApplication::UnicodeUTF8));
        CK_SHELL->setText(QApplication::translate("DialogNewBrush", "&Shell", 0, QApplication::UnicodeUTF8));
        CK_SKYDOM->setText(QApplication::translate("DialogNewBrush", "SkyDom", 0, QApplication::UnicodeUTF8));
        CK_REVERSE->setText(QApplication::translate("DialogNewBrush", "Re&verse", 0, QApplication::UnicodeUTF8));
        IDC_STATICVS->setText(QApplication::translate("DialogNewBrush", "V&ertical", 0, QApplication::UnicodeUTF8));
        IDC_STATICRS->setText(QApplication::translate("DialogNewBrush", "&Horizontal", 0, QApplication::UnicodeUTF8));
        Group->setText(QApplication::translate("DialogNewBrush", "Brush", 0, QApplication::UnicodeUTF8));
        ST_GROUP->setText(QApplication::translate("DialogNewBrush", "X", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DialogNewBrush: public Ui_DialogNewBrush {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGNEWBRUSH_H
