# (c) 2008 by BogDan Vatra < bogdan@licentia.eu >

project(QZint)

include_directories(BEFORE ../backend )

set(QZint_SRCS qzint.cpp)

QT4_WRAP_CPP(QZint_SRCS qzint.h)

#add_definitions (-DNO_QT_KEYWORDS)
# Uncomment the above line if you want to compile qzint without qt keywords


add_library(QZint SHARED ${QZint_SRCS})

set_target_properties(QZint PROPERTIES	VERSION ${ZINT_VERSION}
                                        SOVERSION ${ZINT_VERSION})

add_dependencies(QZint zint)

link_directories( "../backend" )

target_link_libraries(QZint zint ${QT_QTGUI_LIBRARY} 
        ${QT_QTCORE_LIBRARY} )

install(TARGETS QZint  ${INSTALL_TARGETS_DEFAULT_ARGS} )
install(FILES qzint.h DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel)
