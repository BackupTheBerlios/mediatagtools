# ?? ?????? ????????????? ??? ?? ??????????? qmake ??? kdevelop. 
# ------------------------------------------- 
# ???????? ???????????? ?? ?????? ???????? ?????: ./src
# ? ?????????? ????? ??? ????????:  ../bin/mediatagtools

LANGUAGE = C++
INSTALLS += mtt_el.qm \
            icons \
            target 
target.path = bin 
icons.files += ../icons/* 
icons.path = share/mediatagtools/icons 
mtt_el.qm.files += ../translations/mtt_el.qm 
mtt_el.qm.path = share/mediatagtools 
QMAKE_LFLAGS_RELEASE = `taglib-config \
                       --libs`
QMAKE_LFLAGS_DEBUG = `taglib-config --libs`
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
          debug
TEMPLATE = app 
FORMS += form1.ui \
         form2.ui \
         about.ui 
TRANSLATIONS += ../translations/mtt_el.ts 
HEADERS += alistviewitem.h \
           mttcfdialog.h \
           mttmainwin.h \
           mttaboutdialog.h \
           config.h \
           qclineedit.h 
SOURCES += alistviewitem.cpp \
           mediatagtools.cpp \
           mttcfdialog.cpp \
           mttmainwin.cpp \
           mttaboutdialog.cpp \
           qclineedit.cpp 
