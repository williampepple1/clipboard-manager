# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\clipboard-manager_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\clipboard-manager_autogen.dir\\ParseCache.txt"
  "clipboard-manager_autogen"
  )
endif()
