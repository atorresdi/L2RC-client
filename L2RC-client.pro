TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.cpp \
    usb_vcp.cpp \
    timing.cpp \
    protocol.cpp

HEADERS += \
    usb_vcp.h \
    Time_Module.h \
    timing.h \
    protocol.h \
    l2rc_constants.h

LIBS += -lrt \
    -lboost_system \
    -lboost_filesystem

OTHER_FILES += \
    notes.txt
