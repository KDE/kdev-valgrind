ki18n_wrap_ui(memcheck_UI_SRCS
    configpage.ui
)

add_library(memcheck_tool STATIC
    configpage.cpp
    job.cpp
    error.cpp
    parser.cpp
    settings.cpp

    ${memcheck_UI_SRCS}
)

target_link_libraries(memcheck_tool
    KDev::Language
)
