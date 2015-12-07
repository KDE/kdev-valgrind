cmake_minimum_required(VERSION 2.8.12)
cmake_policy(SET CMP0028 NEW)

project(valgrind)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

find_package(ECM 0.0.9 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(ECMAddTests)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)

find_package(Qt5 REQUIRED Core Widgets)
find_package(KF5 REQUIRED COMPONENTS KDELibs4Support TextEditor ThreadWeaver)
find_package(KDevPlatform 1.90.90 REQUIRED)

include_directories(
    ${KDEVPLATFORM_INCLUDE_DIR}
    ${valgrind_BINARY_DIR}
    ${valgrind_SOURCE_DIR}
    ${valgrind_SOURCE_DIR}/config
    ${valgrind_SOURCE_DIR}/models
    ${valgrind_SOURCE_DIR}/parsers
    ${valgrind_SOURCE_DIR}/jobs
    ${valgrind_SOURCE_DIR}/views
)


########### next target ###############

set(kdevvalgrind_PART_SRCS
    launcher.cpp
    config.cpp
    debug.cpp
    job.cpp
    marks.cpp
    imodel.cpp
    modelwrapper.cpp
    iparser.cpp
    plugin.cpp
    statjob.cpp
    widget.cpp
    models/massifitem.cpp
    models/massifmodel.cpp
    models/memcheckfakemodel.cpp
    models/memcheckitemsimpl.cpp
    models/icachegrinditem.cpp
    models/cachegrindmodel.cpp
    models/cachegrinditem.cpp
    models/callgrindmodel.cpp
    models/callgrinditem.cpp
    models/cachegrindmodel.cpp
    models/cachegrinditem.cpp
    parsers/massifparser.cpp
    parsers/memcheckparser.cpp
    parsers/callgrindparser.cpp
    parsers/cachegrindparser.cpp
    jobs/memcheckjob.cpp
    jobs/massifjob.cpp
    jobs/cachegrindjob.cpp
    jobs/callgrindjob.cpp
    views/massifview.cpp
    views/cachegrindview.cpp
    views/callgrindview.cpp
    views/cachegrindview.cpp
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
    config/ui/callgrindview.ui
)

ki18n_wrap_ui(kdevvalgrind_PART_SRCS  ${kdevvalgrind_PART_UI})

kdevplatform_add_plugin(kdevvalgrind JSON kdevvalgrind.json SOURCES ${kdevvalgrind_PART_SRCS} ${kdevvalgrind_CONFIG_SRCS})

target_link_libraries(kdevvalgrind
    KDev::Interfaces
    KDev::Project
    KDev::Language
    KDev::OutputView
    KDev::Shell
    KF5::ConfigCore
    KF5::IconThemes
    KF5::KIOCore
    KF5::WidgetsAddons
    KF5::TextEditor
    KF5::KDELibs4Support
    Qt5::Network
    )



install(TARGETS kdevvalgrind DESTINATION ${PLUGIN_INSTALL_DIR} )

########### install files ###############

install( FILES kdevvalgrind.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/kdevvalgrind )

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
