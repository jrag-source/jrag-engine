add_executable(example
   main.cpp
   states/root.cpp
   states/root.hpp
   states/game/game.cpp
   states/game/game.hpp
   states/game/pause/pause.cpp
   states/game/pause/pause.hpp
   states/intro/intro.cpp
   states/intro/intro.hpp
   states/main_menu/main_menu.cpp
   states/main_menu/main_menu.hpp
)

target_compile_features(example PRIVATE cxx_std_23)

target_link_libraries(example
   PRIVATE
      jrag
   
      sfml-graphics
      sfml-window
      sfml-system
)

add_custom_target(symlink_assets ALL
   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/assets/
)

add_custom_command(
   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/assets/
   COMMAND ${CMAKE_COMMAND} -E create_symlink
      ${CMAKE_CURRENT_SOURCE_DIR}/assets
      ${CMAKE_CURRENT_BINARY_DIR}/assets
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/assets/
   COMMENT "Creating assets symlink in build directory"
)
