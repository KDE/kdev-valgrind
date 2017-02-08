ki18n_wrap_ui(cachegrind_UI_SRCS
    configpage.ui
    view.ui
)

add_library(cachegrind_tool STATIC
    configpage.cpp
    job.cpp
    model.cpp
    parser.cpp
    settings.cpp
    view.cpp

    ${cachegrind_UI_SRCS}
)

target_link_libraries(cachegrind_tool
    KDev::Language
)
