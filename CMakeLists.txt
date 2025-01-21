cmake_minimum_required(VERSION 3.31)

project(
   "JRAG Engine"
   VERSION 1.0
   DESCRIPTION "Game engine made while following SFML Game Development By Example"
   LANGUAGES C CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_custom_target(symlink_compile_commands ALL
   DEPENDS ${CMAKE_SOURCE_DIR}/build/compile_commands.json 
)

add_custom_command(
   OUTPUT ${CMAKE_SOURCE_DIR}/build/compile_commands.json

   COMMAND ${CMAKE_COMMAND} -E create_symlink
      ${CMAKE_BINARY_DIR}/compile_commands.json
      ${CMAKE_SOURCE_DIR}/build/compile_commands.json
   
   DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json

   COMMENT "Creating symlink for compile_commands.json\n\t* Only relevant for IDE code-completion/checking"
)

find_package(SFML COMPONENTS audio graphics network window system REQUIRED)

add_subdirectory(src)
add_subdirectory(example)