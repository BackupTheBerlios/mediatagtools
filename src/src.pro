# ?? ?????? ????????????? ??? ?? ??????????? qmake ??? kdevelop. 
# ------------------------------------------- 
# ???????? ???????????? ?? ?????? ???????? ?????: ./src
# ? ?????????? ????? ??? ????????:  ../bin/mediatagtools

LANGUAGE = C++ 
INSTALLS += target 
target.path = bin 
QMAKE_LFLAGS_RELEASE = `taglib-config \
                       --libs` 
QMAKE_LFLAGS_DEBUG = `taglib-config \
                     --libs` 
QMAKE_CXXFLAGS_RELEASE += `taglib-config \
                          --cflags` 
QMAKE_CXXFLAGS_DEBUG += `taglib-config \
                        --cflags` \
                        -static \
                        -ansi \
                        -pedantic \
                        -Wno-long-long 
TARGET = ../bin/mediatagtools 
CONFIG += debug \
          warn_on 
TEMPLATE = app 
FORMS += form1.ui \
         form2.ui \
         about.ui 
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
