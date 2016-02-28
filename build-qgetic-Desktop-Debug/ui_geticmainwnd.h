/********************************************************************************
** Form generated from reading UI file 'geticmainwnd.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GETICMAINWND_H
#define UI_GETICMAINWND_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDockWidget>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GeticMainWnd
{
public:
    QAction *action_New_tCtrl_N;
    QAction *action_Open_Ctrl_0;
    QAction *action_Save_Ctrl_S;
    QAction *actionSave_As_Ctrl_A;
    QAction *actionImport;
    QAction *action_Export_Ctrl_E;
    QAction *action_Undo_Ctrl_Z;
    QAction *action_Redo_Ctrl_X;
    QAction *actionCut_Ctrl_x;
    QAction *action_Copy_Ctrl_C;
    QAction *action_Paste_Ctrl_P;
    QAction *actionBRUSH;
    QAction *actionMove_Ctrl_M;
    QAction *actionScale_Ctrl_L;
    QAction *actionRotate_Ctrl_R;
    QAction *actionBrush_Ctrl_B;
    QAction *actionFace_Ctrl_E;
    QAction *actionVertex_Ctrl_T;
    QAction *actionEdge_Ctrl_G;
    QAction *actionBox_Ctrl_O;
    QAction *actionCylinder_Ctrl_Y;
    QAction *actionCone_Ctrl_N;
    QAction *actionStairs_Ctrl_I;
    QAction *actionSheet_Ctrl_H;
    QAction *actionSphere_Ctrl_R;
    QAction *actionTerrain_Ctrl_1;
    QAction *actionTerrian_Triangulated_Ctrl_8;
    QAction *action2D_Shape_Ctrl_9;
    QAction *actionPrefabricate_Ctrl_Q;
    QAction *actionSlide_Ctrl_S;
    QWidget *centralWidget;
    QStatusBar *statusBar;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menu_Recent;
    QMenu *menu_Edit;
    QMenu *menuAction;
    QMenu *menuInsert;
    QMenu *menuOperations;
    QToolBar *toolBar;
    QDockWidget *bardlg_2;
    QWidget *dockWidgetContents_2;

    void setupUi(QMainWindow *GeticMainWnd)
    {
        if (GeticMainWnd->objectName().isEmpty())
            GeticMainWnd->setObjectName(QString::fromUtf8("GeticMainWnd"));
        GeticMainWnd->resize(984, 685);
        action_New_tCtrl_N = new QAction(GeticMainWnd);
        action_New_tCtrl_N->setObjectName(QString::fromUtf8("action_New_tCtrl_N"));
        action_Open_Ctrl_0 = new QAction(GeticMainWnd);
        action_Open_Ctrl_0->setObjectName(QString::fromUtf8("action_Open_Ctrl_0"));
        action_Save_Ctrl_S = new QAction(GeticMainWnd);
        action_Save_Ctrl_S->setObjectName(QString::fromUtf8("action_Save_Ctrl_S"));
        actionSave_As_Ctrl_A = new QAction(GeticMainWnd);
        actionSave_As_Ctrl_A->setObjectName(QString::fromUtf8("actionSave_As_Ctrl_A"));
        actionImport = new QAction(GeticMainWnd);
        actionImport->setObjectName(QString::fromUtf8("actionImport"));
        action_Export_Ctrl_E = new QAction(GeticMainWnd);
        action_Export_Ctrl_E->setObjectName(QString::fromUtf8("action_Export_Ctrl_E"));
        action_Undo_Ctrl_Z = new QAction(GeticMainWnd);
        action_Undo_Ctrl_Z->setObjectName(QString::fromUtf8("action_Undo_Ctrl_Z"));
        action_Redo_Ctrl_X = new QAction(GeticMainWnd);
        action_Redo_Ctrl_X->setObjectName(QString::fromUtf8("action_Redo_Ctrl_X"));
        actionCut_Ctrl_x = new QAction(GeticMainWnd);
        actionCut_Ctrl_x->setObjectName(QString::fromUtf8("actionCut_Ctrl_x"));
        action_Copy_Ctrl_C = new QAction(GeticMainWnd);
        action_Copy_Ctrl_C->setObjectName(QString::fromUtf8("action_Copy_Ctrl_C"));
        action_Paste_Ctrl_P = new QAction(GeticMainWnd);
        action_Paste_Ctrl_P->setObjectName(QString::fromUtf8("action_Paste_Ctrl_P"));
        actionBRUSH = new QAction(GeticMainWnd);
        actionBRUSH->setObjectName(QString::fromUtf8("actionBRUSH"));
        actionMove_Ctrl_M = new QAction(GeticMainWnd);
        actionMove_Ctrl_M->setObjectName(QString::fromUtf8("actionMove_Ctrl_M"));
        actionScale_Ctrl_L = new QAction(GeticMainWnd);
        actionScale_Ctrl_L->setObjectName(QString::fromUtf8("actionScale_Ctrl_L"));
        actionRotate_Ctrl_R = new QAction(GeticMainWnd);
        actionRotate_Ctrl_R->setObjectName(QString::fromUtf8("actionRotate_Ctrl_R"));
        actionBrush_Ctrl_B = new QAction(GeticMainWnd);
        actionBrush_Ctrl_B->setObjectName(QString::fromUtf8("actionBrush_Ctrl_B"));
        actionBrush_Ctrl_B->setCheckable(true);
        actionFace_Ctrl_E = new QAction(GeticMainWnd);
        actionFace_Ctrl_E->setObjectName(QString::fromUtf8("actionFace_Ctrl_E"));
        actionVertex_Ctrl_T = new QAction(GeticMainWnd);
        actionVertex_Ctrl_T->setObjectName(QString::fromUtf8("actionVertex_Ctrl_T"));
        actionEdge_Ctrl_G = new QAction(GeticMainWnd);
        actionEdge_Ctrl_G->setObjectName(QString::fromUtf8("actionEdge_Ctrl_G"));
        actionBox_Ctrl_O = new QAction(GeticMainWnd);
        actionBox_Ctrl_O->setObjectName(QString::fromUtf8("actionBox_Ctrl_O"));
        actionCylinder_Ctrl_Y = new QAction(GeticMainWnd);
        actionCylinder_Ctrl_Y->setObjectName(QString::fromUtf8("actionCylinder_Ctrl_Y"));
        actionCone_Ctrl_N = new QAction(GeticMainWnd);
        actionCone_Ctrl_N->setObjectName(QString::fromUtf8("actionCone_Ctrl_N"));
        actionStairs_Ctrl_I = new QAction(GeticMainWnd);
        actionStairs_Ctrl_I->setObjectName(QString::fromUtf8("actionStairs_Ctrl_I"));
        actionSheet_Ctrl_H = new QAction(GeticMainWnd);
        actionSheet_Ctrl_H->setObjectName(QString::fromUtf8("actionSheet_Ctrl_H"));
        actionSphere_Ctrl_R = new QAction(GeticMainWnd);
        actionSphere_Ctrl_R->setObjectName(QString::fromUtf8("actionSphere_Ctrl_R"));
        actionTerrain_Ctrl_1 = new QAction(GeticMainWnd);
        actionTerrain_Ctrl_1->setObjectName(QString::fromUtf8("actionTerrain_Ctrl_1"));
        actionTerrian_Triangulated_Ctrl_8 = new QAction(GeticMainWnd);
        actionTerrian_Triangulated_Ctrl_8->setObjectName(QString::fromUtf8("actionTerrian_Triangulated_Ctrl_8"));
        action2D_Shape_Ctrl_9 = new QAction(GeticMainWnd);
        action2D_Shape_Ctrl_9->setObjectName(QString::fromUtf8("action2D_Shape_Ctrl_9"));
        actionPrefabricate_Ctrl_Q = new QAction(GeticMainWnd);
        actionPrefabricate_Ctrl_Q->setObjectName(QString::fromUtf8("actionPrefabricate_Ctrl_Q"));
        actionSlide_Ctrl_S = new QAction(GeticMainWnd);
        actionSlide_Ctrl_S->setObjectName(QString::fromUtf8("actionSlide_Ctrl_S"));
        centralWidget = new QWidget(GeticMainWnd);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        GeticMainWnd->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(GeticMainWnd);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        GeticMainWnd->setStatusBar(statusBar);
        menuBar = new QMenuBar(GeticMainWnd);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 984, 27));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menu_Recent = new QMenu(menuFile);
        menu_Recent->setObjectName(QString::fromUtf8("menu_Recent"));
        menu_Edit = new QMenu(menuBar);
        menu_Edit->setObjectName(QString::fromUtf8("menu_Edit"));
        menuAction = new QMenu(menuBar);
        menuAction->setObjectName(QString::fromUtf8("menuAction"));
        menuInsert = new QMenu(menuBar);
        menuInsert->setObjectName(QString::fromUtf8("menuInsert"));
        menuOperations = new QMenu(menuBar);
        menuOperations->setObjectName(QString::fromUtf8("menuOperations"));
        GeticMainWnd->setMenuBar(menuBar);
        toolBar = new QToolBar(GeticMainWnd);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        GeticMainWnd->addToolBar(Qt::TopToolBarArea, toolBar);
        bardlg_2 = new QDockWidget(GeticMainWnd);
        bardlg_2->setObjectName(QString::fromUtf8("bardlg_2"));
        dockWidgetContents_2 = new QWidget();
        dockWidgetContents_2->setObjectName(QString::fromUtf8("dockWidgetContents_2"));
        bardlg_2->setWidget(dockWidgetContents_2);
        GeticMainWnd->addDockWidget(static_cast<Qt::DockWidgetArea>(2), bardlg_2);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menu_Edit->menuAction());
        menuBar->addAction(menuInsert->menuAction());
        menuBar->addAction(menuAction->menuAction());
        menuBar->addAction(menuOperations->menuAction());
        menuFile->addAction(action_New_tCtrl_N);
        menuFile->addAction(action_Open_Ctrl_0);
        menuFile->addSeparator();
        menuFile->addAction(action_Save_Ctrl_S);
        menuFile->addAction(actionSave_As_Ctrl_A);
        menuFile->addSeparator();
        menuFile->addAction(actionImport);
        menuFile->addAction(action_Export_Ctrl_E);
        menuFile->addSeparator();
        menuFile->addAction(menu_Recent->menuAction());
        menu_Recent->addSeparator();
        menu_Edit->addAction(action_Undo_Ctrl_Z);
        menu_Edit->addAction(action_Redo_Ctrl_X);
        menu_Edit->addSeparator();
        menu_Edit->addAction(actionCut_Ctrl_x);
        menu_Edit->addAction(action_Copy_Ctrl_C);
        menu_Edit->addAction(action_Paste_Ctrl_P);
        menuAction->addSeparator();
        menuAction->addAction(actionBrush_Ctrl_B);
        menuAction->addAction(actionFace_Ctrl_E);
        menuAction->addAction(actionVertex_Ctrl_T);
        menuAction->addAction(actionEdge_Ctrl_G);
        menuInsert->addSeparator();
        menuInsert->addAction(actionBox_Ctrl_O);
        menuInsert->addAction(actionCylinder_Ctrl_Y);
        menuInsert->addAction(actionCone_Ctrl_N);
        menuInsert->addAction(actionStairs_Ctrl_I);
        menuInsert->addAction(actionSheet_Ctrl_H);
        menuInsert->addAction(actionSphere_Ctrl_R);
        menuInsert->addAction(actionTerrain_Ctrl_1);
        menuInsert->addAction(actionTerrian_Triangulated_Ctrl_8);
        menuInsert->addAction(action2D_Shape_Ctrl_9);
        menuInsert->addAction(actionPrefabricate_Ctrl_Q);
        menuOperations->addAction(actionMove_Ctrl_M);
        menuOperations->addAction(actionScale_Ctrl_L);
        menuOperations->addAction(actionRotate_Ctrl_R);
        menuOperations->addSeparator();
        menuOperations->addAction(actionSlide_Ctrl_S);
        toolBar->addSeparator();

        retranslateUi(GeticMainWnd);

        QMetaObject::connectSlotsByName(GeticMainWnd);
    } // setupUi

    void retranslateUi(QMainWindow *GeticMainWnd)
    {
        GeticMainWnd->setWindowTitle(QApplication::translate("GeticMainWnd", "GeticMainWnd", 0, QApplication::UnicodeUTF8));
        action_New_tCtrl_N->setText(QApplication::translate("GeticMainWnd", "&New Ctrl+N", 0, QApplication::UnicodeUTF8));
        action_Open_Ctrl_0->setText(QApplication::translate("GeticMainWnd", "&Open... Ctrl+O", 0, QApplication::UnicodeUTF8));
        action_Save_Ctrl_S->setText(QApplication::translate("GeticMainWnd", "&Save Ctrl+S", 0, QApplication::UnicodeUTF8));
        actionSave_As_Ctrl_A->setText(QApplication::translate("GeticMainWnd", "Save &As... Ctrl+A", 0, QApplication::UnicodeUTF8));
        actionImport->setText(QApplication::translate("GeticMainWnd", "&Import... Ctrl+I", 0, QApplication::UnicodeUTF8));
        action_Export_Ctrl_E->setText(QApplication::translate("GeticMainWnd", "&Export... Ctrl+E", 0, QApplication::UnicodeUTF8));
        action_Undo_Ctrl_Z->setText(QApplication::translate("GeticMainWnd", "&Undo Ctrl+Z", 0, QApplication::UnicodeUTF8));
        action_Redo_Ctrl_X->setText(QApplication::translate("GeticMainWnd", "&Redo Ctrl+Shift+Z", 0, QApplication::UnicodeUTF8));
        actionCut_Ctrl_x->setText(QApplication::translate("GeticMainWnd", "Cut Ctrl+x", 0, QApplication::UnicodeUTF8));
        action_Copy_Ctrl_C->setText(QApplication::translate("GeticMainWnd", "&Copy Ctrl+C", 0, QApplication::UnicodeUTF8));
        action_Paste_Ctrl_P->setText(QApplication::translate("GeticMainWnd", "&Paste Ctrl+P", 0, QApplication::UnicodeUTF8));
        actionBRUSH->setText(QApplication::translate("GeticMainWnd", "BRUSH", 0, QApplication::UnicodeUTF8));
        actionMove_Ctrl_M->setText(QApplication::translate("GeticMainWnd", "Move Ctrl+M", 0, QApplication::UnicodeUTF8));
        actionScale_Ctrl_L->setText(QApplication::translate("GeticMainWnd", "Scale Ctrl+L", 0, QApplication::UnicodeUTF8));
        actionRotate_Ctrl_R->setText(QApplication::translate("GeticMainWnd", "Rotate Ctrl+R", 0, QApplication::UnicodeUTF8));
        actionBrush_Ctrl_B->setText(QApplication::translate("GeticMainWnd", "Brush Ctrl+B", 0, QApplication::UnicodeUTF8));
        actionFace_Ctrl_E->setText(QApplication::translate("GeticMainWnd", "Face Ctrl+E", 0, QApplication::UnicodeUTF8));
        actionVertex_Ctrl_T->setText(QApplication::translate("GeticMainWnd", "Vertex Ctrl+T", 0, QApplication::UnicodeUTF8));
        actionEdge_Ctrl_G->setText(QApplication::translate("GeticMainWnd", "Edge Ctrl+G", 0, QApplication::UnicodeUTF8));
        actionBox_Ctrl_O->setText(QApplication::translate("GeticMainWnd", "Box Ctrl+1", 0, QApplication::UnicodeUTF8));
        actionCylinder_Ctrl_Y->setText(QApplication::translate("GeticMainWnd", "Cylinder Ctrl+2", 0, QApplication::UnicodeUTF8));
        actionCone_Ctrl_N->setText(QApplication::translate("GeticMainWnd", "Cone Ctrl+3", 0, QApplication::UnicodeUTF8));
        actionStairs_Ctrl_I->setText(QApplication::translate("GeticMainWnd", "Stairs Ctrl+4", 0, QApplication::UnicodeUTF8));
        actionSheet_Ctrl_H->setText(QApplication::translate("GeticMainWnd", "Sheet Ctrl+5", 0, QApplication::UnicodeUTF8));
        actionSphere_Ctrl_R->setText(QApplication::translate("GeticMainWnd", "Sphere Ctrl+6", 0, QApplication::UnicodeUTF8));
        actionTerrain_Ctrl_1->setText(QApplication::translate("GeticMainWnd", "Terrain Ctrl+7", 0, QApplication::UnicodeUTF8));
        actionTerrian_Triangulated_Ctrl_8->setText(QApplication::translate("GeticMainWnd", "Terrian Triangulated Ctrl+8", 0, QApplication::UnicodeUTF8));
        action2D_Shape_Ctrl_9->setText(QApplication::translate("GeticMainWnd", "2D Shape Ctrl+9", 0, QApplication::UnicodeUTF8));
        actionPrefabricate_Ctrl_Q->setText(QApplication::translate("GeticMainWnd", "Prefabricate Ctrl+Q", 0, QApplication::UnicodeUTF8));
        actionSlide_Ctrl_S->setText(QApplication::translate("GeticMainWnd", "Slide Ctrl+S", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("GeticMainWnd", "&File", 0, QApplication::UnicodeUTF8));
        menu_Recent->setTitle(QApplication::translate("GeticMainWnd", "&Recent", 0, QApplication::UnicodeUTF8));
        menu_Edit->setTitle(QApplication::translate("GeticMainWnd", "&Edit", 0, QApplication::UnicodeUTF8));
        menuAction->setTitle(QApplication::translate("GeticMainWnd", "Select", 0, QApplication::UnicodeUTF8));
        menuInsert->setTitle(QApplication::translate("GeticMainWnd", "Insert", 0, QApplication::UnicodeUTF8));
        menuOperations->setTitle(QApplication::translate("GeticMainWnd", "Operations", 0, QApplication::UnicodeUTF8));
        toolBar->setWindowTitle(QApplication::translate("GeticMainWnd", "toolBar", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class GeticMainWnd: public Ui_GeticMainWnd {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GETICMAINWND_H
