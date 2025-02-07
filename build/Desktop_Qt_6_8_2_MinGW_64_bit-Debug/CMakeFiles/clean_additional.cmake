# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\LumePad_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\LumePad_autogen.dir\\ParseCache.txt"
  "LumePad_autogen"
  )
endif()
