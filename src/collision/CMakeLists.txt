add_library(jrag-collision
   collider.cpp
   map.cpp
)

add_library(jrag::collision ALIAS jrag-collision)

target_compile_features(jrag-collision PRIVATE cxx_std_23)

target_link_libraries(jrag-collision
   PUBLIC
      jrag::math
)

target_include_directories(jrag-collision
   PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/include/public
)
