cmake_minimum_required(VERSION 3.16)

project(kdevvalgrind)

set(QT_MIN_VERSION "6.5.0")
set(KF_MIN_VERSION "6.0.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddTests)
include(ECMQtDeclareLoggingCategory)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(FeatureSummary)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Widgets)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    I18n
    ItemViews
    KIO
)

find_package(KDevPlatform 6.0 REQUIRED)

include_directories(config core)

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

    core/config.cpp
    core/configpage.cpp
    core/job.cpp
    core/tool.cpp
    core/xmlconfig.cpp
    core/xmljob.cpp
    core/utils.cpp
    core/widgets/combobox.cpp
    core/widgets/menubutton.cpp
    core/private/common_config.cpp
    core/private/common_configpage.cpp
    core/private/launcher.cpp
    core/private/xmlerror.cpp
    core/private/xmlparser.cpp

    tools/cachegrind/cachegrind_config.cpp
    tools/cachegrind/cachegrind_configpage.cpp
    tools/cachegrind/cachegrind_job.cpp
    tools/cachegrind/cachegrind_model.cpp
    tools/cachegrind/cachegrind_parser.cpp
    tools/cachegrind/cachegrind_tool.cpp
    tools/cachegrind/cachegrind_view.cpp

    tools/callgrind/callgrind_config.cpp
    tools/callgrind/callgrind_configpage.cpp
    tools/callgrind/callgrind_job.cpp
    tools/callgrind/callgrind_model.cpp
    tools/callgrind/callgrind_parser.cpp
    tools/callgrind/callgrind_tool.cpp
    tools/callgrind/callgrind_view.cpp

    tools/drd/drd_config.cpp
    tools/drd/drd_configpage.cpp
    tools/drd/drd_configpage.ui
    tools/drd/drd_job.cpp
    tools/drd/drd_tool.cpp

    tools/helgrind/helgrind_config.cpp
    tools/helgrind/helgrind_configpage.cpp
    tools/helgrind/helgrind_job.cpp
    tools/helgrind/helgrind_tool.cpp

    tools/massif/massif_config.cpp
    tools/massif/massif_configpage.cpp
    tools/massif/massif_job.cpp
    tools/massif/massif_model.cpp
    tools/massif/massif_parser.cpp
    tools/massif/massif_snapshot.cpp
    tools/massif/massif_tool.cpp
    tools/massif/massif_view.cpp

    tools/memcheck/memcheck_config.cpp
    tools/memcheck/memcheck_configpage.cpp
    tools/memcheck/memcheck_job.cpp
    tools/memcheck/memcheck_tool.cpp
)
ki18n_wrap_ui(kdevvalgrind_SRCS
    config/globalconfigpage.ui

    core/private/common_configpage.ui

    tools/cachegrind/cachegrind_configpage.ui
    tools/cachegrind/cachegrind_view.ui

    tools/callgrind/callgrind_configpage.ui
    tools/callgrind/callgrind_view.ui

    tools/drd/drd_configpage.ui

    tools/helgrind/helgrind_configpage.ui

    tools/massif/massif_configpage.ui
    tools/massif/massif_view.ui

    tools/memcheck/memcheck_configpage.ui
)
kconfig_add_kcfg_files(kdevvalgrind_SRCS
    config/globalsettings.kcfgc
)
ecm_qt_declare_logging_category(kdevvalgrind_SRCS
    HEADER debug.h
    IDENTIFIER KDEV_VALGRIND
    CATEGORY_NAME "kdevelop.analyzers.valgrind"
)

qt_add_resources(kdevvalgrind_SRCS
    kdevvalgrind.qrc
)

kdevplatform_add_plugin(kdevvalgrind
    JSON kdevvalgrind.json
    SOURCES ${kdevvalgrind_SRCS}
)
target_link_libraries(kdevvalgrind
    KDev::Language
    KDev::Project
    KDev::Shell
    KF6::ItemViews
    KF6::KIOWidgets
)

# kdebugsettings file
install(FILES kdevvalgrind.categories DESTINATION ${KDE_INSTALL_CONFDIR})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
