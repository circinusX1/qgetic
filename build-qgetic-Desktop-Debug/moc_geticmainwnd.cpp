/****************************************************************************
** Meta object code from reading C++ file 'geticmainwnd.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../getic/geticmainwnd.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'geticmainwnd.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GeticMainWnd[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      18,   14,   13,   13, 0x0a,
      48,   13,   13,   13, 0x08,
      82,   13,   13,   13, 0x08,
     116,   13,   13,   13, 0x08,
     150,   13,   13,   13, 0x08,
     186,   13,   13,   13, 0x08,
     222,   13,   13,   13, 0x08,
     255,   13,   13,   13, 0x08,
     289,   13,   13,   13, 0x08,
     324,   13,   13,   13, 0x08,
     359,   13,   13,   13, 0x08,
     386,   13,   13,   13, 0x08,
     419,   13,   13,   13, 0x08,
     456,   13,   13,   13, 0x08,
     488,   13,   13,   13, 0x08,
     522,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_GeticMainWnd[] = {
    "GeticMainWnd\0\0m,,\0onUpdate(int32_t,void*,void*)\0"
    "on_action_New_tCtrl_N_triggered()\0"
    "on_action_Open_Ctrl_0_triggered()\0"
    "on_action_Save_Ctrl_S_triggered()\0"
    "on_actionSave_As_Ctrl_A_triggered()\0"
    "on_action_Export_Ctrl_E_triggered()\0"
    "on_actionMove_Ctrl_M_triggered()\0"
    "on_actionScale_Ctrl_L_triggered()\0"
    "on_actionVertex_Ctrl_T_triggered()\0"
    "on_actionRotate_Ctrl_R_triggered()\0"
    "on_actionBRUSH_triggered()\0"
    "on_actionCone_Ctrl_N_triggered()\0"
    "on_actionCylinder_Ctrl_Y_triggered()\0"
    "on_actionBox_Ctrl_O_triggered()\0"
    "on_actionBrush_Ctrl_B_triggered()\0"
    "on_actionSlide_Ctrl_S_triggered()\0"
};

void GeticMainWnd::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        GeticMainWnd *_t = static_cast<GeticMainWnd *>(_o);
        switch (_id) {
        case 0: _t->onUpdate((*reinterpret_cast< int32_t(*)>(_a[1])),(*reinterpret_cast< void*(*)>(_a[2])),(*reinterpret_cast< void*(*)>(_a[3]))); break;
        case 1: _t->on_action_New_tCtrl_N_triggered(); break;
        case 2: _t->on_action_Open_Ctrl_0_triggered(); break;
        case 3: _t->on_action_Save_Ctrl_S_triggered(); break;
        case 4: _t->on_actionSave_As_Ctrl_A_triggered(); break;
        case 5: _t->on_action_Export_Ctrl_E_triggered(); break;
        case 6: _t->on_actionMove_Ctrl_M_triggered(); break;
        case 7: _t->on_actionScale_Ctrl_L_triggered(); break;
        case 8: _t->on_actionVertex_Ctrl_T_triggered(); break;
        case 9: _t->on_actionRotate_Ctrl_R_triggered(); break;
        case 10: _t->on_actionBRUSH_triggered(); break;
        case 11: _t->on_actionCone_Ctrl_N_triggered(); break;
        case 12: _t->on_actionCylinder_Ctrl_Y_triggered(); break;
        case 13: _t->on_actionBox_Ctrl_O_triggered(); break;
        case 14: _t->on_actionBrush_Ctrl_B_triggered(); break;
        case 15: _t->on_actionSlide_Ctrl_S_triggered(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GeticMainWnd::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GeticMainWnd::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_GeticMainWnd,
      qt_meta_data_GeticMainWnd, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GeticMainWnd::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GeticMainWnd::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GeticMainWnd::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GeticMainWnd))
        return static_cast<void*>(const_cast< GeticMainWnd*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int GeticMainWnd::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
