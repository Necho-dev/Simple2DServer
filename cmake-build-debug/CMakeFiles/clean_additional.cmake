# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Simple2DServer_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Simple2DServer_autogen.dir\\ParseCache.txt"
  "Simple2DServer_autogen"
  )
endif()
