/****************************************************************************
** Meta object code from reading C++ file 'myserial.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../myserial.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'myserial.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_mySerial_t {
    QByteArrayData data[10];
    char stringdata0[107];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_mySerial_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_mySerial_t qt_meta_stringdata_mySerial = {
    {
QT_MOC_LITERAL(0, 0, 8), // "mySerial"
QT_MOC_LITERAL(1, 9, 13), // "messageSerial"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 7), // "command"
QT_MOC_LITERAL(4, 32, 4), // "data"
QT_MOC_LITERAL(5, 37, 10), // "readSerial"
QT_MOC_LITERAL(6, 48, 11), // "errorSerial"
QT_MOC_LITERAL(7, 60, 28), // "QSerialPort::SerialPortError"
QT_MOC_LITERAL(8, 89, 5), // "error"
QT_MOC_LITERAL(9, 95, 11) // "writeSerial"

    },
    "mySerial\0messageSerial\0\0command\0data\0"
    "readSerial\0errorSerial\0"
    "QSerialPort::SerialPortError\0error\0"
    "writeSerial"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_mySerial[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   39,    2, 0x08 /* Private */,
       6,    1,   40,    2, 0x08 /* Private */,
       9,    1,   43,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::UInt, QMetaType::QByteArray,    3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Bool, QMetaType::QByteArray,    4,

       0        // eod
};

void mySerial::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        mySerial *_t = static_cast<mySerial *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->messageSerial((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< QByteArray(*)>(_a[2]))); break;
        case 1: _t->readSerial(); break;
        case 2: _t->errorSerial((*reinterpret_cast< QSerialPort::SerialPortError(*)>(_a[1]))); break;
        case 3: { bool _r = _t->writeSerial((*reinterpret_cast< const QByteArray(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (mySerial::*_t)(uint , QByteArray );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&mySerial::messageSerial)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject mySerial::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_mySerial.data,
      qt_meta_data_mySerial,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *mySerial::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *mySerial::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_mySerial.stringdata0))
        return static_cast<void*>(const_cast< mySerial*>(this));
    return QObject::qt_metacast(_clname);
}

int mySerial::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void mySerial::messageSerial(uint _t1, QByteArray _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
