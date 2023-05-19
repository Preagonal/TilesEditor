add_subdirectory(mingw64)
add_subdirectory(${PROJECT_SOURCE_DIR}/dependencies/gs2lib ${PROJECT_BINARY_DIR}/dependencies/gs2lib)

target_link_libraries(TilesEditor PRIVATE gs2lib)
target_include_directories(TilesEditor PRIVATE ${GS2LIB_INCLUDE_DIRECTORY})
add_dependencies(TilesEditor gs2lib)
