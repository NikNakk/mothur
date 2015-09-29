cmake_minimum_required (VERSION 2.8)
ENABLE_LANGUAGE(CXX)

project (g3log)

# Detect 64 or 32 bit
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
   # 64-bit project
   SET(64_BIT_OS TRUE)
     MESSAGE("A 64-bit OS detected")
 else()
   SET(64_BIT_OS FALSE)
   MESSAGE("A 32-bit OS detected")
endif()

include(${g3log_SOURCE_DIR}/Build.cmake)
