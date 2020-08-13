QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# include(./ui/table/table.pri)
include(./ui/login/login.pri)
include(./services/services.pri)
include(./ui/tableform/tableform.pri)

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES +=                      \
    main.cpp                    \
    mainwindow.cpp              \
    bc/bcmath.cpp               \
    crypto/AES/AES.cpp          \
    crypto/BASE64/base64.cpp    \
    crypto/DES/des.cpp          \
    crypto/MD5/md5.cpp          \
    crypto/RC4/RC4.cpp          \
    crypto/RSA/RSA.cpp          \
    crypto/SHA1/sha1.cpp        \
    crypto/SHA256/sha256.cpp


HEADERS +=                      \
    mainwindow.h                \
    db/db.h                     \
    bc/bcmath.h                 \
    crypto/AES/AES.h            \
    crypto/BASE64/base64.h      \
    crypto/DES/des.h            \
    crypto/MD5/md5.h            \
    crypto/RC4/RC4.h            \
    crypto/RSA/RSA.h            \
    crypto/SHA1/sha1.hpp        \
    crypto/SHA256/sha256.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
