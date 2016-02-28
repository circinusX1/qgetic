/****************************************************************************
** Meta object code from reading C++ file 'dialognewbrush.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../getic/dialognewbrush.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dialognewbrush.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DialogNewBrush[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x08,
      43,   35,   15,   15, 0x08,
      73,   35,   15,   15, 0x08,
      99,   15,   15,   15, 0x08,
     122,   15,   15,   15, 0x08,
     145,   35,   15,   15, 0x08,
     174,   35,   15,   15, 0x08,
     199,   35,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DialogNewBrush[] = {
    "DialogNewBrush\0\0on_IDOK8_clicked()\0"
    "checked\0on_CK_SETDETAIL_clicked(bool)\0"
    "on_CK_SHELL_clicked(bool)\0"
    "on_IDCANCEL9_clicked()\0showEvent(QShowEvent*)\0"
    "on_CK_SPLITTER_clicked(bool)\0"
    "on_CK_ZONE_clicked(bool)\0"
    "on_CK_SKYDOM_clicked(bool)\0"
};

void DialogNewBrush::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DialogNewBrush *_t = static_cast<DialogNewBrush *>(_o);
        switch (_id) {
        case 0: _t->on_IDOK8_clicked(); break;
        case 1: _t->on_CK_SETDETAIL_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->on_CK_SHELL_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->on_IDCANCEL9_clicked(); break;
        case 4: _t->showEvent((*reinterpret_cast< QShowEvent*(*)>(_a[1]))); break;
        case 5: _t->on_CK_SPLITTER_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->on_CK_ZONE_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->on_CK_SKYDOM_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DialogNewBrush::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DialogNewBrush::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DialogNewBrush,
      qt_meta_data_DialogNewBrush, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DialogNewBrush::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DialogNewBrush::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DialogNewBrush::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DialogNewBrush))
        return static_cast<void*>(const_cast< DialogNewBrush*>(this));
    return QDialog::qt_metacast(_clname);
}

int DialogNewBrush::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
