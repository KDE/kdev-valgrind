ki18n_wrap_ui(helgrind_UI_SRCS
    configpage.ui
)

add_library(helgrind_tool STATIC
    configpage.cpp
    job.cpp
    settings.cpp
    tool.cpp

    ${helgrind_UI_SRCS}
)

target_link_libraries(helgrind_tool
    generic_tool
)
