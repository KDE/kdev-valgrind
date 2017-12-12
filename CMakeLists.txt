cmake_minimum_required(VERSION 3.0)

project(kdevvalgrind)

find_package(ECM "5.14.0" REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddTests)
include(ECMQtDeclareLoggingCategory)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(FeatureSummary)

set(QT_MIN_VERSION "5.5.0")
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Core Widgets)
set(KF5_DEP_VERSION "5.15.0")
find_package(KF5 ${KF5_DEP_VERSION} REQUIRED COMPONENTS ItemModels)
find_package(KDevPlatform 5.1 CONFIG)
set_package_properties(KDevPlatform PROPERTIES
    TYPE REQUIRED
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/config
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/tools
)

add_definitions(
    -DQT_DEPRECATED_WARNINGS
    -DQT_DISABLE_DEPRECATED_BEFORE=0x050500
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_STRICT_ITERATORS
    -DQT_USE_QSTRINGBUILDER
)

add_definitions(-DTRANSLATION_DOMAIN=\"kdevvalgrind\")

set(kdevvalgrind_SRCS
    launchmode.cpp
    plugin.cpp
    problemmodel.cpp
    toolviewfactory.cpp

    config/globalconfigpage.cpp
)

ecm_qt_declare_logging_category(kdevvalgrind_SRCS
    HEADER debug.h
    IDENTIFIER KDEV_VALGRIND
    CATEGORY_NAME "kdevelop.analyzers.valgrind"
)

ki18n_wrap_ui(kdevvalgrind_SRCS
    config/globalconfigpage.ui
)

qt5_add_resources(kdevvalgrind_SRCS
    kdevvalgrind.qrc
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

add_subdirectory(tools)

kdevplatform_add_plugin(kdevvalgrind
    JSON kdevvalgrind.json
    SOURCES ${kdevvalgrind_SRCS}
)

target_link_libraries(kdevvalgrind
    kdevvalgrind_tools

    KDev::Language
    KDev::Project
    KF5::ItemViews
)

# kdebugsettings file
install(FILES kdevvalgrind.categories DESTINATION ${KDE_INSTALL_CONFDIR})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
