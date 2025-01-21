add_library(jrag-paths
   paths.cpp
   whereami.c
   whereami.h
)

add_library(jrag::paths ALIAS jrag-paths)

target_compile_features(jrag-paths PRIVATE cxx_std_23)

target_link_libraries(jrag-paths
   PRIVATE
      sfml-window
)

target_include_directories(jrag-paths
   PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/include/public
)
