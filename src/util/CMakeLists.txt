add_library(jrag-util INTERFACE)
add_library(jrag::util ALIAS jrag-util)

target_include_directories(jrag-util
   INTERFACE
      ${CMAKE_CURRENT_SOURCE_DIR}/include/public
)