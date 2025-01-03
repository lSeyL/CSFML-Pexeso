add_executable(Server server.c)

set(CSFML_LIB_DIR "C:/pos_sem/CSFML/lib/gcc")
set(CSFML_INCLUDE_DIR "C:/pos_sem/CSFML/include")
set(CSFML_BIN_DIR "C:/pos_sem/CSFML/bin")

include_directories(${CSFML_INCLUDE_DIR})
link_directories(${CSFML_LIB_DIR})

find_library(CSFML_SYSTEM csfml-system PATHS ${CSFML_LIB_DIR})
find_library(CSFML_WINDOW csfml-window PATHS ${CSFML_LIB_DIR})
find_library(CSFML_GRAPHICS csfml-graphics PATHS ${CSFML_LIB_DIR})
find_library(CSFML_NETWORK csfml-network PATHS ${CSFML_LIB_DIR})

target_link_libraries(Server
        ${CSFML_SYSTEM}
        ${CSFML_WINDOW}
        ${CSFML_GRAPHICS}
        ${CSFML_NETWORK}
        pthread
)
target_compile_options(Server PRIVATE -Wno-deprecated-declarations)
add_custom_command(TARGET Server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CSFML_BIN_DIR} $<TARGET_FILE_DIR:Server>
)