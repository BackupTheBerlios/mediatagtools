# ?? ?????? ????????????? ??? ?? ??????????? qmake ??? kdevelop. 
# ------------------------------------------- 
# ???????? ???????????? ?? ?????? ???????? ?????: ./src
# ? ?????????? ????? ??? ????????:  ../bin/mediatagtools

LANGUAGE = C++
INSTALLS += mtt_el.qm \
            icons \
            target  \
            desktop_icon \
            desktop_menu_entry
target.path = bin 
icons.files += ../icons/* 
icons.path = share/mediatagtools/icons 
mtt_el.qm.files += ../translations/mtt_el.qm 
mtt_el.qm.path = share/mediatagtools 
QMAKE_CXXFLAGS_RELEASE += `taglib-config \
                          --cflags` \
                          -ansi \
                          -pedantic \
                          -Wno-long-long 
QMAKE_CXXFLAGS_DEBUG += `taglib-config \
                        --cflags` \
                        -static \
                        -ansi \
                        -pedantic \
                        -Wno-long-long \
                        -pg 
TARGET = ../bin/mediatagtools 
CONFIG += warn_on \
          x11 \
 build_all
TEMPLATE = app 
#The following line was changed from FORMS to FORMS3 by qt3to4
TRANSLATIONS += ../translations/mtt_el.ts 
HEADERS += mttmainwin.h \
           config.h \
 tools.h \
 mttfile.h \
 treeitem.h \
 treemodel.h
SOURCES += mediatagtools.cpp \
           mttmainwin.cpp \
 mttfile.cpp \
 treeitem.cpp \
 treemodel.cpp

desktop_icon.files += ../icons/mediatagtools.svg

desktop_menu_entry.files += ../mediatagtools.desktop

desktop_menu_entry.path = share/applications

desktop_icon.path = share/icons/hicolor/scalable/apps

#The following line was inserted by qt3to4
#The following line was inserted by qt3to4

FORMS += mainform.ui


DESTDIR = .

LIBS += `taglib-config \
--libs`
QT += core \
gui
