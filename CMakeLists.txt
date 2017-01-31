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

    launcher.cpp
    plugin.cpp
    widget.cpp

    config/globalconfigpage.cpp

    interfaces/ijob.cpp
    interfaces/imodel.cpp
    interfaces/iview.cpp

    tools/statjob.cpp
)

ki18n_wrap_ui(kdevvalgrind_UI_SRCS
    config/globalconfigpage.ui
)

kconfig_add_kcfg_files(kdevvalgrind_CONFIG_SRCS
    config/globalsettings.kcfgc
)

add_library(kdevvalgrind_config STATIC
    ${kdevvalgrind_CONFIG_SRCS}
)

target_link_libraries(kdevvalgrind_config
    KDev::Shell
)

kdevplatform_add_plugin(kdevvalgrind
    JSON kdevvalgrind.json
    SOURCES ${kdevvalgrind_SRCS} ${kdevvalgrind_UI_SRCS}
)

add_subdirectory(tools/cachegrind)
add_subdirectory(tools/callgrind)
add_subdirectory(tools/generic)
add_subdirectory(tools/massif)
add_subdirectory(tools/memcheck)

target_link_libraries(kdevvalgrind
    KDev::Language
    KDev::Project
    KDev::Shell
    KF5::ItemViews

    kdevvalgrind_config

    cachegrind_tool
    callgrind_tool
    generic_tool
    massif_tool
    memcheck_tool
)

install(FILES kdevvalgrind.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/kdevvalgrind)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
