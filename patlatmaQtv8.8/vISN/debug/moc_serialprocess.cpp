/****************************************************************************
** Meta object code from reading C++ file 'serialprocess.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../serialprocess.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'serialprocess.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SerialProcess_t {
    QByteArrayData data[13];
    char stringdata0[127];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SerialProcess_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SerialProcess_t qt_meta_stringdata_SerialProcess = {
    {
QT_MOC_LITERAL(0, 0, 13), // "SerialProcess"
QT_MOC_LITERAL(1, 14, 5), // "write"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 4), // "data"
QT_MOC_LITERAL(4, 26, 12), // "profileReady"
QT_MOC_LITERAL(5, 39, 10), // "commStatus"
QT_MOC_LITERAL(6, 50, 6), // "status"
QT_MOC_LITERAL(7, 57, 10), // "testStatus"
QT_MOC_LITERAL(8, 68, 24), // "SerialProcess::testState"
QT_MOC_LITERAL(9, 93, 1), // "s"
QT_MOC_LITERAL(10, 95, 8), // "PLCReply"
QT_MOC_LITERAL(11, 104, 7), // "command"
QT_MOC_LITERAL(12, 112, 14) // "processHandler"

    },
    "SerialProcess\0write\0\0data\0profileReady\0"
    "commStatus\0status\0testStatus\0"
    "SerialProcess::testState\0s\0PLCReply\0"
    "command\0processHandler"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SerialProcess[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       4,    0,   47,    2, 0x06 /* Public */,
       5,    1,   48,    2, 0x06 /* Public */,
       7,    1,   51,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    1,   54,    2, 0x0a /* Public */,
      12,    0,   57,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QByteArray,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, 0x80000000 | 8,    9,

 // slots: parameters
    QMetaType::Void, QMetaType::UChar,   11,
    QMetaType::Void,

       0        // eod
};

void SerialProcess::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SerialProcess *_t = static_cast<SerialProcess *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->write((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 1: _t->profileReady(); break;
        case 2: _t->commStatus((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->testStatus((*reinterpret_cast< SerialProcess::testState(*)>(_a[1]))); break;
        case 4: _t->PLCReply((*reinterpret_cast< quint8(*)>(_a[1]))); break;
        case 5: _t->processHandler(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SerialProcess::*_t)(QByteArray );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialProcess::write)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (SerialProcess::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialProcess::profileReady)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (SerialProcess::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialProcess::commStatus)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (SerialProcess::*_t)(SerialProcess::testState );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialProcess::testStatus)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject SerialProcess::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SerialProcess.data,
      qt_meta_data_SerialProcess,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *SerialProcess::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SerialProcess::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_SerialProcess.stringdata0))
        return static_cast<void*>(const_cast< SerialProcess*>(this));
    return QObject::qt_metacast(_clname);
}

int SerialProcess::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void SerialProcess::write(QByteArray _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SerialProcess::profileReady()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void SerialProcess::commStatus(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SerialProcess::testStatus(SerialProcess::testState _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
