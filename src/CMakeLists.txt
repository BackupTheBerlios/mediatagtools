QT4_GENERATE_MOC( mttmainwin.h ${MTT_SOURCE_DIR}/src/moc_mttmainwin.cpp )
QT4_GENERATE_MOC( modeltest.h ${MTT_SOURCE_DIR}/src/moc_modeltest.cpp )
QT4_GENERATE_MOC( mtttablewidget.h ${MTT_SOURCE_DIR}/src/moc_mtttablewidget.cpp )

set(mttSRCS 
mediatagtools.cpp
mttfile.cpp
mttmainwin.cpp
mtttreeview.cpp
#mttitemdelegate.cpp
moc_mttmainwin.cpp
#modeltest.cpp
#moc_modeltest.cpp
mttaboutdialog.cpp
#mttcfdialog.cpp
#qclineedit.cpp
mttadvtagdelegate.cpp
mtttablewidget.cpp
moc_mtttablewidget.cpp
)

set(mttRCCS
mttres.qrc
forms/about.qrc
)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

QT4_WRAP_UI( mttSRCS forms/about.ui )

QT4_ADD_RESOURCES(mtt_RCC_SRCS ${mttRCCS})

ADD_EXECUTABLE(mediatagtools.exe ${mttSRCS} ${mtt_RCC_SRCS})

TARGET_LINK_LIBRARIES( mediatagtools.exe ${TAGLIB_LIBRARIES}
${QT_QTCORE_LIBRARY_DEBUG}
${QT_QTGUI_LIBRARY_DEBUG}
${QT_QTUITOOLS_LIBRARY_DEBUG})
      
