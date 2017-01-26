ki18n_wrap_ui(callgrind_UI_SRCS
    configpage.ui
    view.ui
)

add_library(callgrind_tool STATIC
    configpage.cpp
    job.cpp
    model.cpp
    modelitem.cpp
    parser.cpp
    view.cpp

    ${callgrind_UI_SRCS}
)

target_link_libraries(callgrind_tool
    KDev::Language

    cachegrind_tool # FIXME iCachegrindItem
)
