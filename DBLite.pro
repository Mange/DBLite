# -------------------------------------------------
# Project created by QtCreator 2009-05-01T21:51:35
# -------------------------------------------------
QT += sql \
    testlib
TARGET = DBLite
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    sqlhighlighter.cpp \
    databasefile.cpp \
    settingswindow.cpp \
    settingsproxy.cpp
HEADERS += mainwindow.hpp \
    sqlhighlighter.hpp \
    databasefile.hpp \
    settingswindow.hpp \
    settingsproxy.hpp
FORMS += mainwindow.ui \
    settingswindow.ui
RESOURCES += resources.qrc
OTHER_FILES += highlighter/sql_keywords.txt \
    highlighter/sql_functions.txt

# Compile .o files to a separate dir
OBJECTS_DIR = objs/
DESTDIR = bin/
