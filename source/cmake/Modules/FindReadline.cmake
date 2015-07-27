# - Find the readline library
# This module defines
#  READLINE_INCLUDE_DIR, path to readline/readline.h, etc.
#  READLINE_LIBRARIES, the libraries required to use READLINE.
#  READLINE_FOUND, If false, do not try to use READLINE.
# also defined, but not for general use are
# READLINE_readline_LIBRARY, where to find the READLINE library.
# READLINE_ncurses_LIBRARY, where to find the ncurses library [might not be defined]

# Taken from taskwarrior - a command line task list manager. (http://thelackthereof.org/projects/cpp/task/cmake/Modules/FindReadline.cmake)

# Copyright 2006-2013, Paul Beckingham, Federico Hernandez.

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# http://www.opensource.org/licenses/mit-license.php


# Apple readline does not support readline hooks
# So we look for another one by default
IF (APPLE)
  FIND_PATH (READLINE_INCLUDE_DIR NAMES readline/readline.h PATHS
    /sw/include
    /opt/local/include
    /opt/include
    /usr/local/include
    /usr/include/
    NO_DEFAULT_PATH
    )
ENDIF (APPLE)
FIND_PATH (READLINE_INCLUDE_DIR NAMES readline/readline.h)


# Apple readline does not support readline hooks
# So we look for another one by default
IF (APPLE)
  FIND_LIBRARY (READLINE_readline_LIBRARY NAMES readline PATHS
    /sw/lib
    /opt/local/lib
    /opt/lib
    /usr/local/lib
    /usr/lib
    NO_DEFAULT_PATH
    )
ENDIF (APPLE)
FIND_LIBRARY (READLINE_readline_LIBRARY NAMES readline)

# Sometimes readline really needs ncurses
IF (APPLE)
  FIND_LIBRARY (READLINE_ncurses_LIBRARY NAMES ncurses PATHS
    /sw/lib
    /opt/local/lib
    /opt/lib
    /usr/local/lib
    /usr/lib
    NO_DEFAULT_PATH
    )
ENDIF (APPLE)
FIND_LIBRARY (READLINE_ncurses_LIBRARY NAMES ncurses)

MARK_AS_ADVANCED (
  READLINE_INCLUDE_DIR
  READLINE_readline_LIBRARY
  READLINE_ncurses_LIBRARY
  )

SET (READLINE_FOUND "NO" )
IF (READLINE_INCLUDE_DIR)
  IF (READLINE_readline_LIBRARY)
    SET (READLINE_FOUND "YES" )
    SET (READLINE_LIBRARIES
      ${READLINE_readline_LIBRARY} 
      )

    # some readline libraries depend on ncurses
    IF (READLINE_ncurses_LIBRARY)
      SET (READLINE_LIBRARIES ${READLINE_LIBRARIES} ${READLINE_ncurses_LIBRARY})
    ENDIF (READLINE_ncurses_LIBRARY)

  ENDIF (READLINE_readline_LIBRARY)
ENDIF (READLINE_INCLUDE_DIR)

IF (READLINE_FOUND)
  MESSAGE (STATUS "Found readline library")
ELSE (READLINE_FOUND)
  IF (READLINE_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find readline -- please give some paths to CMake")
  ENDIF (READLINE_FIND_REQUIRED)
ENDIF (READLINE_FOUND)
