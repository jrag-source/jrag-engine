add_library(jrag INTERFACE)

target_link_libraries(jrag
   INTERFACE
      jrag::app_state
      jrag::asset
      jrag::engine
      jrag::input
      jrag::math
      jrag::paths
      jrag::util
)

add_subdirectory(app_state)
add_subdirectory(asset)
add_subdirectory(engine)
add_subdirectory(input)
add_subdirectory(math)
add_subdirectory(paths)
add_subdirectory(util)