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
    ${CMAKE_CURRENT_SOURCE_DIR}/jobs
    ${CMAKE_CURRENT_SOURCE_DIR}/models
    ${CMAKE_CURRENT_SOURCE_DIR}/parsers
    ${CMAKE_CURRENT_SOURCE_DIR}/views
)

set(kdevvalgrind_PART_SRCS
    config.cpp
    debug.cpp
    imodel.cpp
    iparser.cpp
    job.cpp
    jobs/cachegrindjob.cpp
    jobs/callgrindjob.cpp
    jobs/massifjob.cpp
    jobs/memcheckjob.cpp
    launcher.cpp
    marks.cpp
    models/cachegrinditem.cpp
    models/cachegrinditem.cpp
    models/cachegrindmodel.cpp
    models/cachegrindmodel.cpp
    models/callgrinditem.cpp
    models/callgrindmodel.cpp
    models/icachegrinditem.cpp
    models/massifitem.cpp
    models/massifmodel.cpp
    models/memcheckfakemodel.cpp
    models/memcheckitemsimpl.cpp
    modelwrapper.cpp
    parsers/cachegrindparser.cpp
    parsers/callgrindparser.cpp
    parsers/massifparser.cpp
    parsers/memcheckparser.cpp
    plugin.cpp
    statjob.cpp
    views/cachegrindview.cpp
    views/cachegrindview.cpp
    views/callgrindview.cpp
    views/massifview.cpp
    widget.cpp
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
    config/ui/cachegrindconfig.ui
    config/ui/callgrindconfig.ui
    config/ui/callgrindview.ui
    config/ui/genericconfig.ui
    config/ui/helgrindconfig.ui
    config/ui/massifconfig.ui
    config/ui/memcheckconfig.ui
)

ki18n_wrap_ui(kdevvalgrind_PART_SRCS ${kdevvalgrind_PART_UI})
kdevplatform_add_plugin(kdevvalgrind
    JSON kdevvalgrind.json
    SOURCES ${kdevvalgrind_PART_SRCS} ${kdevvalgrind_CONFIG_SRCS}
)
target_link_libraries(kdevvalgrind
    KDev::Language
    KDev::Project
    KDev::Shell
    KF5::ItemViews
)
install(FILES kdevvalgrind.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/kdevvalgrind)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
