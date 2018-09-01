#pragma once

/*
 *  mothur.h
 *  Mothur
 *
 *  Created by Sarah Westcott on 2/19/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

 /* This file contains all the standard incudes we use in the project as well as some common utilities. */

 //#include <cstddef>

#ifndef NOMINMAX
#define NOMINMAX // Required on Windows because of macros for min and max
#endif


//config
#include "mothurConfig.h"


//boost libraries
#ifdef USE_BOOST
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#endif

//io libraries
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <signal.h>


//exception
#include <stdexcept>
#include <exception>
#include <cstdlib> 


//containers
#include <vector>
#include <set>
#include <map>
#include <string>
#include <list>
#include <string>

//math
#include <cmath>
#include <math.h>
#include <algorithm>
#include <numeric>

//misc
#include <cerrno>
#include <ctime>
#include <limits>
#include <atomic>

/***********************************************************************/

#if defined (UNIX)
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#else
#include <conio.h> //allows unbuffered screen capture from stdin
#include <direct.h> //get cwd
#include <windows.h>
#include <psapi.h>
#include <direct.h>
#include <tchar.h>

#endif

#ifdef USE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#elif USE_EDITLINE
#include <editline/readline.h>
#endif

#include "mothurdefs.h"

using namespace std;

