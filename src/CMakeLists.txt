QT4_GENERATE_MOC( mttmainwin.h ${MTT_SOURCE_DIR}/src/moc_mttmainwin.cpp )
QT4_GENERATE_MOC( treemodel.h ${MTT_SOURCE_DIR}/src/moc_treemodel.cpp )
QT4_GENERATE_MOC( modeltest.h ${MTT_SOURCE_DIR}/src/moc_modeltest.cpp )
QT4_GENERATE_MOC( mtttreeview.h ${MTT_SOURCE_DIR}/src/moc_mtttreeview.cpp )


ADD_EXECUTABLE(../bin/mediatagtools.exe
mediatagtools.cpp
mttfile.cpp
mttmainwin.cpp
treeitem.cpp
treemodel.cpp
mtttreeview.cpp
moc_mtttreeview.cpp
moc_mttmainwin.cpp
moc_treemodel.cpp
modeltest.cpp
moc_modeltest.cpp
#mttaboutdialog.cpp
#mttcfdialog.cpp
#qclineedit.cpp
)

TARGET_LINK_LIBRARIES( ../bin/mediatagtools.exe ${TAGLIB_LIBRARIES}
${QT_QTCORE_LIBRARY_RELEASE}
${QT_QTGUI_LIBRARY_RELEASE})
      
