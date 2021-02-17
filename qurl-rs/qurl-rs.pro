QT += network widgets

LIBS += -L$$_PRO_FILE_PWD_ -lrsnet

HEADERS += \
    includes.h \
    rsnet.h
    netdll.h

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    widgets.cpp
