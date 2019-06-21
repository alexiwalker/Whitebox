#-------------------------------------------------
#
# Project created by QtCreator 2019-03-02T13:10:47
#
#-------------------------------------------------

QT       += core gui sql multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = whitebox
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
	main.cpp \
	classes/data/databasehander.cpp \
	classes/ui/flowlayout.cpp \
	classes/ui/episode_widget.cpp \
	classes/ui/show_widget.cpp \
	classes/ui/player_playlist.cpp \
	classes/process/http_scrape.cpp \
	classes/process/linked_mem_buffer.cpp \
	classes/process/microtimer.cpp \
	classes/ui/flow_grid_layout.cpp \
	classes/ui/tools/reconcile.cpp \
    classes/process/settings.cpp \
    playback.cpp \
    settingswindow.cpp \
    toolswindow.cpp \
    whitebox.cpp \
    classes/ui/tools/replace.cpp \
    classes/process/util.cpp \
    classes/ui/tools/replace_folder.cpp \
    classes/ui/tools/rename.cpp \
    classes/ui/tools/locate.cpp

HEADERS += \
	classes/data/databasehandler.h \
	classes/ui/flowlayout.h \
	classes/process/http_scrape.h \
	classes/process/linked_mem_buffer.h \
	classes/process/microtimer.h \
	classes/ui/player_playlist.h \
	classes/ui/episode_widget.h \
	classes/ui/show_widget.h \
	classes/ui/flow_grid_layout.h \
	classes/ui/tools/reconcile.h \
    classes/process/settings.h \
    playback.h \
    settingswindow.h \
    toolswindow.h \
    whitebox.h \
    classes/ui/tools/replace.h \
    classes/process/util.h \
    classes/ui/tools/replace_folder.h \
    classes/ui/tools/rename.h \
    classes/ui/tools/locate.h

FORMS += \
    playback.ui \
    settingswindow.ui \
    toolswindow.ui \
    whitebox.ui

win32:RC_ICONS += box2.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    icons.qrc
