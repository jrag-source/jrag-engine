add_library(jrag-app_state
   base_state.cpp
   command.cpp
   graphics_tooling.cpp
   logic_tooling.cpp
   state_manager.cpp
)

add_library(jrag::app_state ALIAS jrag-app_state)

target_compile_features(jrag-app_state PRIVATE cxx_std_23)

target_link_libraries(jrag-app_state
   PRIVATE
      sfml-graphics
)

target_include_directories(jrag-app_state
   PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/include/public
)
