# -------------------------------------------------
# Project created by QtCreator 2009-05-01T21:51:35
# -------------------------------------------------
QT += sql \
    testlib
TARGET = DBLite
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    sqlhighlighter.cpp
HEADERS += mainwindow.hpp \
    sqlhighlighter.hpp
FORMS += mainwindow.ui
RESOURCES += resources.qrc
OTHER_FILES += highlighter/sql_keywords.txt
