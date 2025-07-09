# generated from ament/cmake/core/templates/nameConfig.cmake.in

# prevent multiple inclusion
if(_shit_CONFIG_INCLUDED)
  # ensure to keep the found flag the same
  if(NOT DEFINED shit_FOUND)
    # explicitly set it to FALSE, otherwise CMake will set it to TRUE
    set(shit_FOUND FALSE)
  elseif(NOT shit_FOUND)
    # use separate condition to avoid uninitialized variable warning
    set(shit_FOUND FALSE)
  endif()
  return()
endif()
set(_shit_CONFIG_INCLUDED TRUE)

# output package information
if(NOT shit_FIND_QUIETLY)
  message(STATUS "Found shit: 0.3.0 (${shit_DIR})")
endif()

# warn when using a deprecated package
if(NOT "" STREQUAL "")
  set(_msg "Package 'shit' is deprecated")
  # append custom deprecation text if available
  if(NOT "" STREQUAL "TRUE")
    set(_msg "${_msg} ()")
  endif()
  # optionally quiet the deprecation message
  if(NOT ${shit_DEPRECATED_QUIET})
    message(DEPRECATION "${_msg}")
  endif()
endif()

# flag package as ament-based to distinguish it after being find_package()-ed
set(shit_FOUND_AMENT_PACKAGE TRUE)

# include all config extra files
set(_extras "")
foreach(_extra ${_extras})
  include("${shit_DIR}/${_extra}")
endforeach()
