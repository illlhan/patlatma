/****************************************************************************
** Meta object code from reading C++ file 'numpad.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../keyboard/numpad.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'numpad.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Numpad_t {
    QByteArrayData data[12];
    char stringdata0[184];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Numpad_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Numpad_t qt_meta_stringdata_Numpad = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Numpad"
QT_MOC_LITERAL(1, 7, 15), // "keyboardHandler"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 16), // "on_shift_clicked"
QT_MOC_LITERAL(4, 41, 17), // "on_char_2_toggled"
QT_MOC_LITERAL(5, 59, 7), // "checked"
QT_MOC_LITERAL(6, 67, 16), // "on_clear_clicked"
QT_MOC_LITERAL(7, 84, 22), // "on_enterButton_clicked"
QT_MOC_LITERAL(8, 107, 23), // "on_lineEdit_textChanged"
QT_MOC_LITERAL(9, 131, 4), // "arg1"
QT_MOC_LITERAL(10, 136, 25), // "on_lineEdit_returnPressed"
QT_MOC_LITERAL(11, 162, 21) // "on_backButton_clicked"

    },
    "Numpad\0keyboardHandler\0\0on_shift_clicked\0"
    "on_char_2_toggled\0checked\0on_clear_clicked\0"
    "on_enterButton_clicked\0on_lineEdit_textChanged\0"
    "arg1\0on_lineEdit_returnPressed\0"
    "on_backButton_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Numpad[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x08 /* Private */,
       3,    0,   55,    2, 0x08 /* Private */,
       4,    1,   56,    2, 0x08 /* Private */,
       6,    0,   59,    2, 0x08 /* Private */,
       7,    0,   60,    2, 0x08 /* Private */,
       8,    1,   61,    2, 0x08 /* Private */,
      10,    0,   64,    2, 0x08 /* Private */,
      11,    0,   65,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Numpad::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Numpad *_t = static_cast<Numpad *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->keyboardHandler(); break;
        case 1: _t->on_shift_clicked(); break;
        case 2: _t->on_char_2_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->on_clear_clicked(); break;
        case 4: _t->on_enterButton_clicked(); break;
        case 5: _t->on_lineEdit_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->on_lineEdit_returnPressed(); break;
        case 7: _t->on_backButton_clicked(); break;
        default: ;
        }
    }
}

const QMetaObject Numpad::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Numpad.data,
      qt_meta_data_Numpad,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Numpad::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Numpad::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Numpad.stringdata0))
        return static_cast<void*>(const_cast< Numpad*>(this));
    return QWidget::qt_metacast(_clname);
}

int Numpad::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
