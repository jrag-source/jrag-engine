add_library(jrag-engine
   engine.cpp
)

add_library(jrag::engine ALIAS jrag-engine)

target_compile_features(jrag-engine PRIVATE cxx_std_23)

target_link_libraries(jrag-engine
   PRIVATE
      jrag::app_state
      jrag::input
      jrag::paths
   
      sfml-graphics
      sfml-window
      sfml-system
)

target_include_directories(jrag-engine
   PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/include/public
)
