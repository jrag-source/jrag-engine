add_library(jrag-math INTERFACE)
add_library(jrag::math ALIAS jrag-math)

target_include_directories(jrag-math
   INTERFACE
      ${CMAKE_CURRENT_SOURCE_DIR}/include/public
)