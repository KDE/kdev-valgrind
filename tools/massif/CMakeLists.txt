ki18n_wrap_ui(massif_UI_SRCS
    configpage.ui
    view.ui
)

add_library(massif_tool STATIC
    configpage.cpp
    job.cpp
    launcher.cpp
    model.cpp
    parser.cpp
    settings.cpp
    snapshot.cpp
    view.cpp

    ${massif_UI_SRCS}
)

target_link_libraries(massif_tool
    generic_tool
)
