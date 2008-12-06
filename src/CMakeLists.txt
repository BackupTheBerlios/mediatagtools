QT4_GENERATE_MOC( mttmainwin.h ${MTT_SOURCE_DIR}/src/moc_mttmainwin.cpp )
QT4_GENERATE_MOC( modeltest.h ${MTT_SOURCE_DIR}/src/moc_modeltest.cpp )
QT4_GENERATE_MOC( mtttreeview.h ${MTT_SOURCE_DIR}/src/moc_mtttreeview.cpp )

set(mttSRCS 
mediatagtools.cpp
mttfile.cpp
mttmainwin.cpp
mtttreeview.cpp
moc_mtttreeview.cpp
moc_mttmainwin.cpp
modeltest.cpp
moc_modeltest.cpp
#mttaboutdialog.cpp
#mttcfdialog.cpp
#qclineedit.cpp
renummodel.cpp
)

QT4_WRAP_UI( mttSRCS mainform.ui )

ADD_EXECUTABLE(mediatagtools.exe ${mttSRCS})

TARGET_LINK_LIBRARIES( mediatagtools.exe ${TAGLIB_LIBRARIES}
${QT_QTCORE_LIBRARY_DEBUG}
${QT_QTGUI_LIBRARY_DEBUG})
      
