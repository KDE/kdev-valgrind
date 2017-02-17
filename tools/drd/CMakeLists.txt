ki18n_wrap_ui(drd_UI_SRCS
    configpage.ui
)

add_library(drd_tool STATIC
    configpage.cpp
    job.cpp
    launcher.cpp
    settings.cpp

    ${drd_UI_SRCS}
)

target_link_libraries(drd_tool
    generic_tool
)
