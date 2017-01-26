ki18n_wrap_ui(massif_UI_SRCS
    configpage.ui
)

add_library(massif_tool STATIC
    configpage.cpp
    job.cpp
    model.cpp
    modelitem.cpp
    parser.cpp
    view.cpp

    ${massif_UI_SRCS}
)

target_link_libraries(massif_tool
    KDev::Language
)
