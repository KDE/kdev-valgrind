cmake_minimum_required(VERSION 2.8.12)
cmake_policy(SET CMP0028 NEW)

project(kdevvalgrind)

find_package(ECM 0.0.9 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddTests)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(FeatureSummary)

find_package(Qt5 REQUIRED Core Widgets)
find_package(KF5 REQUIRED COMPONENTS ItemModels)
find_package(KDevPlatform 5.0.0 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/config
    ${CMAKE_CURRENT_SOURCE_DIR}/tools
)

set(kdevvalgrind_SRCS
    config.cpp
    debug.cpp
    ijob.cpp
    imodel.cpp
    iparser.cpp
    launcher.cpp
    modelwrapper.cpp
    plugin.cpp
    statjob.cpp
    widget.cpp

    config/genericconfigpage.cpp

    tools/cachegrind/cachegrindconfigpage.cpp
    tools/cachegrind/cachegrinditem.cpp
    tools/cachegrind/cachegrinditem.cpp
    tools/cachegrind/cachegrindjob.cpp
    tools/cachegrind/cachegrindmodel.cpp
    tools/cachegrind/cachegrindmodel.cpp
    tools/cachegrind/cachegrindparser.cpp
    tools/cachegrind/cachegrindview.cpp
    tools/cachegrind/icachegrinditem.cpp

    tools/callgrind/callgrindconfigpage.cpp
    tools/callgrind/callgrinditem.cpp
    tools/callgrind/callgrindjob.cpp
    tools/callgrind/callgrindmodel.cpp
    tools/callgrind/callgrindparser.cpp
    tools/callgrind/callgrindview.cpp

    tools/massif/massifconfigpage.cpp
    tools/massif/massifitem.cpp
    tools/massif/massifjob.cpp
    tools/massif/massifmodel.cpp
    tools/massif/massifparser.cpp
    tools/massif/massifview.cpp

    tools/memcheck/memcheckconfigpage.cpp
    tools/memcheck/memcheckfakemodel.cpp
    tools/memcheck/memcheckitemsimpl.cpp
    tools/memcheck/memcheckjob.cpp
    tools/memcheck/memcheckparser.cpp
)

ki18n_wrap_ui(kdevvalgrind_UI_SRCS
    config/ui/genericconfig.ui

    tools/cachegrind/cachegrindconfig.ui
    tools/callgrind/callgrindconfig.ui
    tools/callgrind/callgrindview.ui
    tools/massif/massifconfig.ui
    tools/memcheck/memcheckconfig.ui
)

kdevplatform_add_plugin(kdevvalgrind
    JSON kdevvalgrind.json
    SOURCES ${kdevvalgrind_SRCS} ${kdevvalgrind_UI_SRCS}
)

target_link_libraries(kdevvalgrind
    KDev::Language
    KDev::Project
    KDev::Shell
    KF5::ItemViews
)

install(FILES kdevvalgrind.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/kdevvalgrind)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
