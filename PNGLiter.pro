# -------------------------------------------------
# Project created by QtCreator 2009-12-14T12:56:22
# -------------------------------------------------
QT += network
TARGET = PNGLiter
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    imagetable.cpp \
    imagedelegate.cpp \
    optimizer.cpp \
    dialogs/aboutdialog.cpp \
    dialogs/optionsdialog.cpp \
    tools/qtwin.cpp \
    tools/general.cpp
HEADERS += mainwindow.h \
    imagetable.h \
    imagedelegate.h \
    optimizer.h \
    dialogs/aboutdialog.h \
    dialogs/optionsdialog.h \
    tools/qtwin.h \
    tools/general.h
FORMS += mainwindow.ui \
    dialogs/aboutdialog.ui \
    dialogs/optionsdialog.ui
OTHER_FILES += TODO.txt
RESOURCES += pngliter.qrc
