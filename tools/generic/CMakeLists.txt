ki18n_wrap_ui(generic_UI_SRCS
    configpage.ui
)

add_library(generic_tool STATIC
    configpage.cpp
    events.cpp
    isettings.cpp
    job.cpp
    launcher.cpp
    model.cpp
    settings.cpp
    utils.cpp

    ${generic_UI_SRCS}
)

target_link_libraries(generic_tool
    KDev::Shell
)
