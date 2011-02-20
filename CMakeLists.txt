project(valgrind)
add_definitions( -DKDE_DEFAULT_DEBUG_AREA=9002 )

find_package(KDE4 REQUIRED)
find_package(KDevPlatform 0.9.96 REQUIRED)

include_directories(
    ${KDEVPLATFORM_INCLUDE_DIR}
    ${KDE4_INCLUDES}
    ${valgrind_BINARY_DIR}
    ${valgrind_SOURCE_DIR}
)


########### next target ###############

set(kdevvalgrind_PART_SRCS
  valgrindconfigpage.cpp
  valgrindlauncher.cpp
    valgrindconfig.cpp
    valgrinditem.cpp
    valgrindjob.cpp
    valgrindmodel.cpp
    valgrindparser.cpp
    valgrindplugin.cpp
    valgrindtree.cpp
    valgrindwidget.cpp
)

set(kdevvalgrind_PART_UI valgrindconfig.ui)

kde4_add_ui_files(kdevvalgrind_PART_SRCS ${kdevvalgrind_PART_UI})

kde4_add_plugin(kdevvalgrind ${kdevvalgrind_PART_SRCS})
target_link_libraries(kdevvalgrind
    ${KDEVPLATFORM_INTERFACES_LIBRARIES}
    ${KDEVPLATFORM_PROJECT_LIBRARIES}
    ${KDEVPLATFORM_LANGUAGE_LIBRARIES}
    ${KDEVPLATFORM_OUTPUTVIEW_LIBRARIES}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_KUTILS_LIBS}
    ${KDEVPLATFORM_UTIL_LIBRARIES}
    ${KDE4_KTEXTEDITOR_LIBS}
    ${QT_QTNETWORK_LIBRARY})

install(TARGETS kdevvalgrind DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kdevvalgrind.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
install( FILES kdevvalgrind.rc DESTINATION ${DATA_INSTALL_DIR}/kdevvalgrind )
