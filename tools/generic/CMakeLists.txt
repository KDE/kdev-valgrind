ki18n_wrap_ui(generic_UI_SRCS
    configpage.ui
)

add_library(generic_tool STATIC
    core/ijob.cpp
    core/ilauncher.cpp
    core/isettings.cpp
    core/utils.cpp

    core/xml/ixmljob.cpp
    core/xml/ixmlsettings.cpp
    core/xml/error.cpp
    core/xml/parser.cpp

    configpage.cpp
    settings.cpp

    ${generic_UI_SRCS}
)

target_link_libraries(generic_tool
    KDev::Shell
)
