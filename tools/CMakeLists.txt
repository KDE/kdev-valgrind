add_subdirectory(cachegrind)
add_subdirectory(callgrind)
add_subdirectory(drd)
add_subdirectory(generic)
add_subdirectory(helgrind)
add_subdirectory(massif)
add_subdirectory(memcheck)

add_library(kdevvalgrind_tools STATIC
)

target_link_libraries(kdevvalgrind_tools
    cachegrind_tool
    callgrind_tool
    drd_tool
    helgrind_tool
    massif_tool
    memcheck_tool
)
