ki18n_wrap_ui(helgrind_UI_SRCS
)

add_library(helgrind_tool STATIC
    job.cpp

    ${memcheck_UI_SRCS}
)

target_link_libraries(helgrind_tool
    KDev::Language
)
