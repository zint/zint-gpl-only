# (c) 2008 by BogDan Vatra < bogdan@licentia.eu >

project(QZint_frontend)

include_directories(BEFORE "${CMAKE_SOURCE_DIR}/backend"  "${CMAKE_SOURCE_DIR}/backend_qt4")

set(QZint_frontend_SRCS barcodeitem.cpp  main.cpp  mainwindow.cpp)
QT4_WRAP_CPP(QZint_frontend_SRCS mainwindow.h)
QT4_WRAP_UI(QZint_frontend_SRCS mainWindow.ui)

add_executable(QZint_frontend ${QZint_frontend_SRCS})
add_dependencies(QZint_frontend QZint zint)

link_directories( "${CMAKE_BINARY_DIR}/backend" "${CMAKE_BINARY_DIR}/backend_qt4" )

target_link_libraries(QZint_frontend zint QZint ${QT_QTGUI_LIBRARY} 
     ${QT_QTCORE_LIBRARY} )
