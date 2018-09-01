# g3log is a KjellKod Logger
# 2015 @author Kjell Hedström, hedstrom@kjellkod.cc 
# ==================================================================
# 2015 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own
#    risk and comes  with no warranties.
#
# This code is yours to share, use and modify with no strings attached
#   and no restrictions or obligations.
# ===================================================================



SET(LOG_SRC ${g3log_SOURCE_DIR})
include_directories(${LOG_SRC})

IF (${CMAKE_CXX_COMPILER_ID} MATCHES ".*Clang")
   SET(CMAKE_CXX_FLAGS "-Wall -std=c++11 -stdlib=libc++ -Wunused -D_GLIBCXX_USE_NANOSLEEP")
   IF (${CMAKE_SYSTEM} MATCHES "FreeBSD-([0-9]*)\\.(.*)")
       IF (${CMAKE_MATCH_1} GREATER 9)
           set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
           set(PLATFORM_LINK_LIBRIES execinfo)
       ENDIF()
   ELSEIF (APPLE)
       set(PLATFORM_LINK_LIBRIES c++abi)
   ELSE()
       set(PLATFORM_LINK_LIBRIES rt c++abi)
   ENDIF()



ELSEIF(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
   MESSAGE("cmake for GCC ")
   IF (APPLE)
       set(CMAKE_CXX_FLAGS "-Wall -Wunused -std=c++11  -pthread -D_GLIBCXX_USE_NANOSLEEP")
   ELSEIF (MINGW)
       set(CMAKE_CXX_FLAGS "-Wall -Wunused -std=c++11  -pthread -D_GLIBCXX_USE_NANOSLEEP -D_GLIBCXX_USE_SCHED_YIELD")
   ELSE()
       set(PLATFORM_LINK_LIBRIES rt)
       set(CMAKE_CXX_FLAGS "-Wall -rdynamic -Wunused -std=c++11 -pthread -D_GLIBCXX_USE_NANOSLEEP -D_GLIBCXX_USE_SCHED_YIELD")
   ENDIF()
ENDIF()


IF (MSVC OR MINGW)
  set(PLATFORM_LINK_LIBRIES dbghelp)
ENDIF()

   # GENERIC STEPS
   file(GLOB SRC_FILES ${LOG_SRC}/g3log/*.h ${LOG_SRC}/g3log/*.hpp ${LOG_SRC}/*.cpp ${LOG_SRC}/*.ipp)
   file(GLOB HEADER_FILES ${LOG_SRC}/g3log/*.hpp ${LOG_SRC}/*.hpp)
   #MESSAGE(" HEADER FILES ARE: ${HEADER_FILES}")

   IF (MSVC OR MINGW) 
         list(REMOVE_ITEM SRC_FILES  ${LOG_SRC}/crashhandler_unix.cpp)
   ELSE()
         list(REMOVE_ITEM SRC_FILES  ${LOG_SRC}/crashhandler_windows.cpp ${LOG_SRC}/g3log/stacktrace_windows.hpp ${LOG_SRC}/stacktrace_windows.cpp)
   ENDIF (MSVC OR MINGW)

   set(SRC_FILES ${SRC_FILES} ${SRC_PLATFORM_SPECIFIC})
 
   # Create the g3log library
   include_directories(${LOG_SRC})
   #MESSAGE("  g3logger files: [${SRC_FILES}]")
   add_library(g3logger ${SRC_FILES})
   set_target_properties(g3logger PROPERTIES LINKER_LANGUAGE CXX)
   target_link_libraries(g3logger ${PLATFORM_LINK_LIBRIES})
   SET(G3LOG_LIBRARY g3logger)

if(ADD_BUILD_WIN_SHARED OR NOT(MSVC OR MINGW))
   add_library(g3logger_shared SHARED ${SRC_FILES})
   set_target_properties(g3logger_shared PROPERTIES LINKER_LANGUAGE CXX)
   IF(APPLE)
      set_target_properties(g3logger_shared PROPERTIES MACOSX_RPATH TRUE)
   ENDIF(APPLE)
   target_link_libraries(g3logger_shared ${PLATFORM_LINK_LIBRIES})   

   SET(G3LOG_SHARED_LIBRARY g3logger_shared)
endif()


