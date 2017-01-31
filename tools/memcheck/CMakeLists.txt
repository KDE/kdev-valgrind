ki18n_wrap_ui(memcheck_UI_SRCS
    configpage.ui
)

add_library(memcheck_tool STATIC
    configpage.cpp
    job.cpp
    modelitem.cpp
    parser.cpp

    ${memcheck_UI_SRCS}
)

target_link_libraries(memcheck_tool
    KDev::Language
)
