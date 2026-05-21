HEADERS += \
    $$PWD/loggermanager.h \
    $$PWD/ilogger.h

SOURCES += \
    $$PWD/loggermanager.cpp



# spdlog
include($$PWD/spdlog/spdlog.pri)
