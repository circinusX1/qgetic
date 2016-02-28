#-------------------------------------------------
#
# Project created by QtCreator 2013-10-16T10:30:38
#
#-------------------------------------------------
# sudo apt-get install freeglut3-dev

# sudo apt-get install freeglut3-dev

QT       += core gui opengl
INCLUDEPATH += ../gtcbase
INCLUDEPATH += ../gtcbase/GNU_LINUX
INCLUDEPATH += ../include
DEFINES += _CFG_GNU_LINUX Q_WS_QWS__NONO

QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-unused-value
QMAKE_CXXFLAGS += -Wno-int-to-pointer-cast
QMAKE_CXXFLAGS += -Wno-reorder

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qgetic
TEMPLATE = app


SOURCES = main.cpp\
        geticmainwnd.cpp \
    leftdock.cpp \
    view2d.cpp \
    view3d.cpp \
    safereleases.cpp \
    camera.cpp \
    frustrum.cpp \
    poly.cpp \
    texadapter.cpp \
    texsys.cpp \
    baseentity.cpp \
    scene.cpp \
    document.cpp \
    texref.cpp \
    brush.cpp \
    minibsp.cpp \
    compiler.cpp \
    bsptree.cpp \
    bigterrain.cpp \
    bsphook.cpp \
    crck.cpp \
    curve_class.cpp \
    custitemcat.cpp \
    gameitem.cpp \
    lightbulb.cpp \
    lmproc.cpp \
    motion.cpp \
    portalprc.cpp \
    pvsprc.cpp \
    sceitem.cpp \
    skydom.cpp \
    scriptitem.cpp \
    soudcarrier.cpp \
    sounditem.cpp \
    startplayitem.cpp \
    triggeritem.cpp \
    geticapp.cpp \
    winwrap.cpp \
    docdraw.cpp \
    docmk.cpp \
    docalign.cpp \
    docproc.cpp \
    docsave.cpp \
    undo.cpp \
    tertree.cpp \
    carchive.cpp \
    cfont.cpp \
    mfcdlgbuilder.cpp \
    cdialogevaldets.cpp \
    cdlgcompprogress.cpp \
    cwaitcursor.cpp \
    geoutils.cpp \
    cfiledialog.cpp \
    progressdlg.cpp \
    docmfccmd.cpp \
    glwindow.cpp \
    mmove.cpp \
    stdafx.cpp \
    dialognewbrush.cpp \
    ../gtcbase/baseutils.cpp

HEADERS  = geticmainwnd.h \
    leftdock.h \
    view2d.h \
    view3d.h \
    safereleases.h \
    qgttypes.h \
    camera.h \
    frustrum.h \
    stdafx.h \
    poly.h \
    texadapter.h \
    texref.h \
    texsys.h \
    baselibhdrs.h \
    ../include/_isystem.h \
    ../include/_isound.h \
    ../include/_irender.h \
    ../include/_iinput.h \
    ../include/glui.h \
    ../include/geticplugbsp.h \
    ../include/geticplug.h \
    ../include/bspfilestr.h \
    baseentity.h \
    ../gtcbase/GNU_LINUX/os.h \
    scene.h \
    document.h \
    brush.h \
    minibsp.h \
    compiler.h \
    bsptree.h \
    bigterrain.h \
    bsphook.h \
    crck.h \
    curve_class.h \
    custitemcat.h \
    gameitem.h \
    lightbulb.h \
    lmproc.h \
    motion.h \
    portalprc.h \
    pvsprc.h \
    sceitem.h \
    skydom.h \
    flowpath.h \
    scriptitem.h \
    soudcarrier.h \
    sounditem.h \
    startplayitem.h \
    triggeritem.h \
    geticapp.h \
    winwrap.h \
    undo.h \
    tertree.h \
    ../gtcbase/baseutils.h \
    ../gtcbase/basecont.h \
    carchive.h \
    cfont.h \
    mfcdlgbuilder.h \
    cdialogevaldets.h \
    cdlgcompprogress.h \
    cwaitcursor.h \
    geoutils.h \
    cfiledialog.h \
    progressdlg.h \
    docmfccmd.h \
    glwindow.h \
    mmove.h \
    dialognewbrush.h \
    qgemenu.h

FORMS    += geticmainwnd.ui \
    dialognewbrush.ui



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../_lib/release/ -lgtcbase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../_lib/debug/ -lgtcbase
else:unix:!macx: LIBS += -L$$PWD/../_lib/ -lgtcbase

INCLUDEPATH += $$PWD/../_lib
DEPENDPATH += $$PWD/../_lib

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../_lib/release/gtcbase.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../_lib/debug/gtcbase.lib
else:unix:!macx: PRE_TARGETDEPS += $$PWD/../_lib/libgtcbase.a


## sudo apt-get install freeglut3 freeglut3-dev
unix|win32: LIBS += -lX11  -lglut -lGLU

unix:!macx|win32: LIBS += -ljpeg

OTHER_FILES += \
    resources/rc2ui.php \
    resources/geticgui.rc \
    resources/MAIN_MENU
