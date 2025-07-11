# generated from ament/cmake/core/templates/nameConfig.cmake.in

# prevent multiple inclusion
if(_myArm3_CONFIG_INCLUDED)
  # ensure to keep the found flag the same
  if(NOT DEFINED myArm3_FOUND)
    # explicitly set it to FALSE, otherwise CMake will set it to TRUE
    set(myArm3_FOUND FALSE)
  elseif(NOT myArm3_FOUND)
    # use separate condition to avoid uninitialized variable warning
    set(myArm3_FOUND FALSE)
  endif()
  return()
endif()
set(_myArm3_CONFIG_INCLUDED TRUE)

# output package information
if(NOT myArm3_FIND_QUIETLY)
  message(STATUS "Found myArm3: 0.0.0 (${myArm3_DIR})")
endif()

# warn when using a deprecated package
if(NOT "" STREQUAL "")
  set(_msg "Package 'myArm3' is deprecated")
  # append custom deprecation text if available
  if(NOT "" STREQUAL "TRUE")
    set(_msg "${_msg} ()")
  endif()
  # optionally quiet the deprecation message
  if(NOT ${myArm3_DEPRECATED_QUIET})
    message(DEPRECATION "${_msg}")
  endif()
endif()

# flag package as ament-based to distinguish it after being find_package()-ed
set(myArm3_FOUND_AMENT_PACKAGE TRUE)

# include all config extra files
set(_extras "")
foreach(_extra ${_extras})
  include("${myArm3_DIR}/${_extra}")
endforeach()
