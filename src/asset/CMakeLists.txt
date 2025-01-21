add_library(jrag-asset
   manager.cpp
   sprite_sheet.cpp
)

add_library(jrag::asset ALIAS jrag-asset)

target_compile_features(jrag-asset PRIVATE cxx_std_23)

target_link_libraries(jrag-asset
   PUBLIC
      jrag::math

   PRIVATE
      sfml-graphics
)

target_include_directories(jrag-asset
   PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/include/public
)
