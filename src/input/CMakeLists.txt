add_library(jrag-input
   dispatcher.cpp
   keyboard.cpp
   loader.cpp
   mouse.cpp
   mouse_wheel.cpp
   parser.cpp
   parser.hpp
   print.cpp
   variant.cpp
)

add_library(jrag::input ALIAS jrag-input)

target_compile_features(jrag-input PRIVATE cxx_std_23)

target_link_libraries(jrag-input
   PRIVATE
      jrag::paths
      jrag::util
      
      sfml-window
)

target_include_directories(jrag-input
   PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/include/public
)
