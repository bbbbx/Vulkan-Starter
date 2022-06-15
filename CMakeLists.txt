cmake_minimum_required( VERSION 3.0 )

set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin" )

project( Vulkan-Starter )

set( GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only" FORCE )
set( GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW lib only" FORCE )
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW lib only" FORCE )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" FORCE )
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/extern/glfw )

find_package( Vulkan 1.1 REQUIRED )

include_directories(
  "${CMAKE_SOURCE_DIR}/include"
  ${Vulkan_INCLUDE_DIR}
)
link_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/extern)

file( GLOB EXAMPLES RELATIVE "${CMAKE_SOURCE_DIR}" "examples/*" )
file( GLOB COMMON_SOURCES RELATIVE "${CMAKE_SOURCE_DIR}" "src/*" )

foreach( EXAMPLE_PATH ${EXAMPLES} )
  string( REPLACE "examples/" "" EXAMPLE_NAME ${EXAMPLE_PATH} )
  add_executable( ${EXAMPLE_NAME}
    "${CMAKE_SOURCE_DIR}/${COMMON_SOURCES}"
    "${CMAKE_SOURCE_DIR}/${EXAMPLE_PATH}/${EXAMPLE_NAME}.cpp" )
  target_link_libraries( ${EXAMPLE_NAME} ${Vulkan_LIBRARY} glfw )
  message( STATUS "Add target: ${EXAMPLE_NAME}" )
endforeach( EXAMPLE_PATH )
