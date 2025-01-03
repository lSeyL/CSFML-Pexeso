
add_executable(Client client.c
        ../../UI/window.c
        ../../UI/Components/button.c
        ../../UI/Components/header.c
        ../../UI/Components/Setters.c
        ../../GameLogic/Pexeso.c
        ../../Rules/Rules.c
)

include_directories(${CSFML_INCLUDE_DIR})
link_directories(${CSFML_LIB_DIR})

find_library(CSFML_SYSTEM csfml-system PATHS ${CSFML_LIB_DIR})
find_library(CSFML_WINDOW csfml-window PATHS ${CSFML_LIB_DIR})
find_library(CSFML_GRAPHICS csfml-graphics PATHS ${CSFML_LIB_DIR})
find_library(CSFML_NETWORK csfml-network PATHS ${CSFML_LIB_DIR})

target_link_libraries(Client
        ${CSFML_SYSTEM}
        ${CSFML_WINDOW}
        ${CSFML_GRAPHICS}
        ${CSFML_NETWORK}
        pthread
)
target_compile_options(Client PRIVATE -Wno-deprecated-declarations)
add_custom_command(TARGET Client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CSFML_BIN_DIR} $<TARGET_FILE_DIR:Client>
)