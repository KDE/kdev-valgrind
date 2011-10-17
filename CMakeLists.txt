
project(valgrind)
add_definitions( -DKDE_DEFAULT_DEBUG_AREA=9002 )

find_package(KDE4 REQUIRED)
find_package(KDevPlatform 0.9.96 REQUIRED)

include_directories(
    ${KDEVPLATFORM_INCLUDE_DIR}
    ${KDE4_INCLUDES}
    ${valgrind_BINARY_DIR}
    ${valgrind_SOURCE_DIR}
    ${valgrind_SOURCE_DIR}/config
    ${valgrind_SOURCE_DIR}/models
)


########### next target ###############

set(kdevvalgrind_PART_SRCS

    launcher.cpp
    config.cpp
    job.cpp
    marks.cpp
    imodel.cpp
    parser.cpp
    plugin.cpp
    tree.cpp
    widget.cpp
    models/memcheckitems.cpp
    models/memcheckmodel.cpp
    models/memcheckitemsimpl.cpp
)

set(kdevvalgrind_CONFIG_SRCS
    config/cachegrindconfigpage.cpp
    config/callgrindconfigpage.cpp
    config/genericconfigpage.cpp
    config/helgrindconfigpage.cpp
    config/massifconfigpage.cpp
    config/memcheckconfigpage.cpp
)

set(kdevvalgrind_PART_UI
    config/ui/genericconfig.ui
    config/ui/memcheckconfig.ui
    config/ui/cachegrindconfig.ui
    config/ui/massifconfig.ui
    config/ui/helgrindconfig.ui
    config/ui/callgrindconfig.ui
)

kde4_add_ui_files(kdevvalgrind_PART_SRCS  ${kdevvalgrind_PART_UI})

kde4_add_plugin(kdevvalgrind ${kdevvalgrind_PART_SRCS} ${kdevvalgrind_CONFIG_SRCS})

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
