/********************************************************************************
** Form generated from reading UI file 'dlg_newbrush.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLG_NEWBRUSH_H
#define UI_DLG_NEWBRUSH_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QGroupBox *IDC_STATIC01;
    QRadioButton *RB_SOLID;
    QRadioButton *RB_CUT;
    QRadioButton *All;
    QCheckBox *CK_SPLITTER;
    QGroupBox *IDC_STATIC23;
    QCheckBox *CK_SETDETAIL;
    QCheckBox *CK_REVERSE;
    QCheckBox *CK_SKYDOM;
    QLabel *IDC_STATICVS;
    QSpinBox *IDC_SPIN2;
    QLabel *IDC_STATICRS;
    QSpinBox *IDC_SPIN3;
    QLineEdit *IDC_EDIT1;
    QSpinBox *IDC_SPIN1;
    QGroupBox *Mapping4;
    QRadioButton *RB_LINEAR;
    QRadioButton *RB_CYLINDRIC;
    QRadioButton *RB_SPHERIC;
    QRadioButton *RB_ENVI;
    QLabel *IDC_STATIC5;
    QLineEdit *EF_WIDTH;
    QLabel *IDC_STATIC6;
    QLineEdit *EF_HEIGHT;
    QLabel *IDC_STATIC7;
    QLineEdit *EF_DEPTH;
    QLabel *Group;
    QLabel *ST_GROUP;
    QPushButton *IDOK8;
    QPushButton *IDCANCEL9;
    QCheckBox *CK_SHELL;
    QCheckBox *CK_ZONE;
    QCheckBox *CK_DETACHED;
    QGroupBox *IDC_STATIC1011;
    QGroupBox *IDC_STATIC1213;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(764, 412);
        IDC_STATIC01 = new QGroupBox(Dialog);
        IDC_STATIC01->setObjectName(QString::fromUtf8("IDC_STATIC01"));
        IDC_STATIC01->setGeometry(QRect(12, 0, 137, 287));
        IDC_STATIC01->setFlat(true);
        RB_SOLID = new QRadioButton(Dialog);
        RB_SOLID->setObjectName(QString::fromUtf8("RB_SOLID"));
        RB_SOLID->setGeometry(QRect(27, 30, 77, 25));
        RB_CUT = new QRadioButton(Dialog);
        RB_CUT->setObjectName(QString::fromUtf8("RB_CUT"));
        RB_CUT->setGeometry(QRect(27, 62, 67, 25));
        All = new QRadioButton(Dialog);
        All->setObjectName(QString::fromUtf8("All"));
        All->setGeometry(QRect(27, 100, 90, 25));
        CK_SPLITTER = new QCheckBox(Dialog);
        CK_SPLITTER->setObjectName(QString::fromUtf8("CK_SPLITTER"));
        CK_SPLITTER->setGeometry(QRect(27, 137, 97, 25));
        IDC_STATIC23 = new QGroupBox(Dialog);
        IDC_STATIC23->setObjectName(QString::fromUtf8("IDC_STATIC23"));
        IDC_STATIC23->setGeometry(QRect(165, 2, 162, 285));
        IDC_STATIC23->setFlat(true);
        CK_SETDETAIL = new QCheckBox(Dialog);
        CK_SETDETAIL->setObjectName(QString::fromUtf8("CK_SETDETAIL"));
        CK_SETDETAIL->setGeometry(QRect(177, 37, 85, 25));
        CK_REVERSE = new QCheckBox(Dialog);
        CK_REVERSE->setObjectName(QString::fromUtf8("CK_REVERSE"));
        CK_REVERSE->setGeometry(QRect(190, 235, 107, 25));
        CK_SKYDOM = new QCheckBox(Dialog);
        CK_SKYDOM->setObjectName(QString::fromUtf8("CK_SKYDOM"));
        CK_SKYDOM->setGeometry(QRect(190, 200, 107, 25));
        IDC_STATICVS = new QLabel(Dialog);
        IDC_STATICVS->setObjectName(QString::fromUtf8("IDC_STATICVS"));
        IDC_STATICVS->setGeometry(QRect(545, 75, 65, 20));
        IDC_SPIN2 = new QSpinBox(Dialog);
        IDC_SPIN2->setObjectName(QString::fromUtf8("IDC_SPIN2"));
        IDC_SPIN2->setGeometry(QRect(628, 75, 91, 30));
        IDC_STATICRS = new QLabel(Dialog);
        IDC_STATICRS->setObjectName(QString::fromUtf8("IDC_STATICRS"));
        IDC_STATICRS->setGeometry(QRect(545, 150, 80, 20));
        IDC_SPIN3 = new QSpinBox(Dialog);
        IDC_SPIN3->setObjectName(QString::fromUtf8("IDC_SPIN3"));
        IDC_SPIN3->setGeometry(QRect(640, 140, 71, 30));
        IDC_EDIT1 = new QLineEdit(Dialog);
        IDC_EDIT1->setObjectName(QString::fromUtf8("IDC_EDIT1"));
        IDC_EDIT1->setGeometry(QRect(198, 142, 91, 31));
        IDC_SPIN1 = new QSpinBox(Dialog);
        IDC_SPIN1->setObjectName(QString::fromUtf8("IDC_SPIN1"));
        IDC_SPIN1->setGeometry(QRect(290, 142, 51, 31));
        Mapping4 = new QGroupBox(Dialog);
        Mapping4->setObjectName(QString::fromUtf8("Mapping4"));
        Mapping4->setGeometry(QRect(345, 5, 175, 282));
        Mapping4->setFlat(true);
        RB_LINEAR = new QRadioButton(Dialog);
        RB_LINEAR->setObjectName(QString::fromUtf8("RB_LINEAR"));
        RB_LINEAR->setGeometry(QRect(370, 50, 87, 25));
        RB_CYLINDRIC = new QRadioButton(Dialog);
        RB_CYLINDRIC->setObjectName(QString::fromUtf8("RB_CYLINDRIC"));
        RB_CYLINDRIC->setGeometry(QRect(370, 87, 100, 25));
        RB_SPHERIC = new QRadioButton(Dialog);
        RB_SPHERIC->setObjectName(QString::fromUtf8("RB_SPHERIC"));
        RB_SPHERIC->setGeometry(QRect(370, 125, 100, 25));
        RB_ENVI = new QRadioButton(Dialog);
        RB_ENVI->setObjectName(QString::fromUtf8("RB_ENVI"));
        RB_ENVI->setGeometry(QRect(370, 162, 137, 25));
        IDC_STATIC5 = new QLabel(Dialog);
        IDC_STATIC5->setObjectName(QString::fromUtf8("IDC_STATIC5"));
        IDC_STATIC5->setGeometry(QRect(37, 322, 50, 20));
        EF_WIDTH = new QLineEdit(Dialog);
        EF_WIDTH->setObjectName(QString::fromUtf8("EF_WIDTH"));
        EF_WIDTH->setGeometry(QRect(37, 347, 75, 27));
        IDC_STATIC6 = new QLabel(Dialog);
        IDC_STATIC6->setObjectName(QString::fromUtf8("IDC_STATIC6"));
        IDC_STATIC6->setGeometry(QRect(130, 322, 57, 20));
        EF_HEIGHT = new QLineEdit(Dialog);
        EF_HEIGHT->setObjectName(QString::fromUtf8("EF_HEIGHT"));
        EF_HEIGHT->setGeometry(QRect(122, 347, 75, 27));
        IDC_STATIC7 = new QLabel(Dialog);
        IDC_STATIC7->setObjectName(QString::fromUtf8("IDC_STATIC7"));
        IDC_STATIC7->setGeometry(QRect(212, 322, 60, 20));
        EF_DEPTH = new QLineEdit(Dialog);
        EF_DEPTH->setObjectName(QString::fromUtf8("EF_DEPTH"));
        EF_DEPTH->setGeometry(QRect(210, 347, 75, 27));
        Group = new QLabel(Dialog);
        Group->setObjectName(QString::fromUtf8("Group"));
        Group->setGeometry(QRect(432, 302, 100, 20));
        ST_GROUP = new QLabel(Dialog);
        ST_GROUP->setObjectName(QString::fromUtf8("ST_GROUP"));
        ST_GROUP->setGeometry(QRect(540, 302, 192, 22));
        IDOK8 = new QPushButton(Dialog);
        IDOK8->setObjectName(QString::fromUtf8("IDOK8"));
        IDOK8->setGeometry(QRect(462, 347, 125, 35));
        IDCANCEL9 = new QPushButton(Dialog);
        IDCANCEL9->setObjectName(QString::fromUtf8("IDCANCEL9"));
        IDCANCEL9->setGeometry(QRect(610, 345, 125, 35));
        CK_SHELL = new QCheckBox(Dialog);
        CK_SHELL->setObjectName(QString::fromUtf8("CK_SHELL"));
        CK_SHELL->setGeometry(QRect(177, 112, 82, 25));
        CK_ZONE = new QCheckBox(Dialog);
        CK_ZONE->setObjectName(QString::fromUtf8("CK_ZONE"));
        CK_ZONE->setGeometry(QRect(30, 175, 97, 25));
        CK_DETACHED = new QCheckBox(Dialog);
        CK_DETACHED->setObjectName(QString::fromUtf8("CK_DETACHED"));
        CK_DETACHED->setGeometry(QRect(202, 75, 117, 25));
        IDC_STATIC1011 = new QGroupBox(Dialog);
        IDC_STATIC1011->setObjectName(QString::fromUtf8("IDC_STATIC1011"));
        IDC_STATIC1011->setGeometry(QRect(12, 292, 325, 100));
        IDC_STATIC1011->setFlat(true);
        IDC_STATIC1213 = new QGroupBox(Dialog);
        IDC_STATIC1213->setObjectName(QString::fromUtf8("IDC_STATIC1213"));
        IDC_STATIC1213->setGeometry(QRect(535, 5, 200, 231));
        IDC_STATIC1213->setFlat(true);

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setProperty("Brush Inital Properties", QVariant(QApplication::translate("Dialog", "Dialog", 0, QApplication::UnicodeUTF8)));
        IDC_STATIC01->setTitle(QApplication::translate("Dialog", "&Content", 0, QApplication::UnicodeUTF8));
        RB_SOLID->setText(QApplication::translate("Dialog", "Solid", 0, QApplication::UnicodeUTF8));
        RB_CUT->setText(QApplication::translate("Dialog", "Cut", 0, QApplication::UnicodeUTF8));
        All->setText(QApplication::translate("Dialog", "Cut", 0, QApplication::UnicodeUTF8));
        CK_SPLITTER->setText(QApplication::translate("Dialog", "Splitter", 0, QApplication::UnicodeUTF8));
        IDC_STATIC23->setTitle(QApplication::translate("Dialog", "Type", 0, QApplication::UnicodeUTF8));
        CK_SETDETAIL->setText(QApplication::translate("Dialog", "&Detail", 0, QApplication::UnicodeUTF8));
        CK_REVERSE->setText(QApplication::translate("Dialog", "Re&verse", 0, QApplication::UnicodeUTF8));
        CK_SKYDOM->setText(QApplication::translate("Dialog", "SkyDom", 0, QApplication::UnicodeUTF8));
        IDC_STATICVS->setText(QApplication::translate("Dialog", "V&ertical", 0, QApplication::UnicodeUTF8));
        IDC_STATICRS->setText(QApplication::translate("Dialog", "&Horizontal", 0, QApplication::UnicodeUTF8));
        Mapping4->setTitle(QApplication::translate("Dialog", "Te&xture", 0, QApplication::UnicodeUTF8));
        RB_LINEAR->setText(QApplication::translate("Dialog", "Linear", 0, QApplication::UnicodeUTF8));
        RB_CYLINDRIC->setText(QApplication::translate("Dialog", "Cylindic", 0, QApplication::UnicodeUTF8));
        RB_SPHERIC->setText(QApplication::translate("Dialog", "Spheric", 0, QApplication::UnicodeUTF8));
        RB_ENVI->setText(QApplication::translate("Dialog", "Environment", 0, QApplication::UnicodeUTF8));
        IDC_STATIC5->setText(QApplication::translate("Dialog", "Width", 0, QApplication::UnicodeUTF8));
        IDC_STATIC6->setText(QApplication::translate("Dialog", "Height", 0, QApplication::UnicodeUTF8));
        IDC_STATIC7->setText(QApplication::translate("Dialog", "Depth", 0, QApplication::UnicodeUTF8));
        Group->setText(QApplication::translate("Dialog", "Brush", 0, QApplication::UnicodeUTF8));
        ST_GROUP->setText(QApplication::translate("Dialog", "X", 0, QApplication::UnicodeUTF8));
        IDOK8->setText(QApplication::translate("Dialog", "Create", 0, QApplication::UnicodeUTF8));
        IDCANCEL9->setText(QApplication::translate("Dialog", "Cancel", 0, QApplication::UnicodeUTF8));
        CK_SHELL->setText(QApplication::translate("Dialog", "&Shell", 0, QApplication::UnicodeUTF8));
        CK_ZONE->setText(QApplication::translate("Dialog", "Zone", 0, QApplication::UnicodeUTF8));
        CK_DETACHED->setText(QApplication::translate("Dialog", "Det&ached", 0, QApplication::UnicodeUTF8));
        IDC_STATIC1011->setTitle(QApplication::translate("Dialog", "Size", 0, QApplication::UnicodeUTF8));
        IDC_STATIC1213->setTitle(QApplication::translate("Dialog", "Strips", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLG_NEWBRUSH_H
