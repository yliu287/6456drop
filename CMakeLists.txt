cmake_minimum_required(VERSION 2.8.9)

project(exp2)
## set glibc params
find_package(PkgConfig QUIET)
pkg_check_modules(PC_GLIB QUIET glib-2.0)

find_library(GLIB_LIBRARIES
	    NAMES glib-2.0
	        HINTS ${PC_GLIB_LIBDIR}
		          ${PC_GLIB_LIBRARY_DIRS}
			  )

		  # Files in glib's main include path may include glibconfig.h, which,
		  # for some odd reason, is normally in $LIBDIR/glib-2.0/include.
		  get_filename_component(_GLIB_LIBRARY_DIR ${GLIB_LIBRARIES} PATH)
		  find_path(GLIBCONFIG_INCLUDE_DIR
			      NAMES glibconfig.h
			          HINTS ${PC_LIBDIR} ${PC_LIBRARY_DIRS} ${_GLIB_LIBRARY_DIR}
				            ${PC_GLIB_INCLUDEDIR} ${PC_GLIB_INCLUDE_DIRS}
					        PATH_SUFFIXES glib-2.0/include
						)

					find_path(GLIB_INCLUDE_DIR
						    NAMES glib.h
						        HINTS ${PC_GLIB_INCLUDEDIR}
							          ${PC_GLIB_INCLUDE_DIRS}
								      PATH_SUFFIXES glib-2.0
								      )

							      set(GLIB_INCLUDE_DIRS ${GLIB_INCLUDE_DIR} ${GLIBCONFIG_INCLUDE_DIR})

							      # Version detection
							      if (EXISTS "${GLIBCONFIG_INCLUDE_DIR}/glibconfig.h")
								          file(READ "${GLIBCONFIG_INCLUDE_DIR}/glibconfig.h" GLIBCONFIG_H_CONTENTS)
									      string(REGEX MATCH "#define GLIB_MAJOR_VERSION ([0-9]+)" _dummy "${GLIBCONFIG_H_CONTENTS}")
									          set(GLIB_VERSION_MAJOR "${CMAKE_MATCH_1}")
										      string(REGEX MATCH "#define GLIB_MINOR_VERSION ([0-9]+)" _dummy "${GLIBCONFIG_H_CONTENTS}")
										          set(GLIB_VERSION_MINOR "${CMAKE_MATCH_1}")
											      string(REGEX MATCH "#define GLIB_MICRO_VERSION ([0-9]+)" _dummy "${GLIBCONFIG_H_CONTENTS}")
											          set(GLIB_VERSION_MICRO "${CMAKE_MATCH_1}")
												      set(GLIB_VERSION "${GLIB_VERSION_MAJOR}.${GLIB_VERSION_MINOR}.${GLIB_VERSION_MICRO}")
											      endif ()

											      # Additional Glib components.  We only look for libraries, as not all of them
											      # have corresponding headers and all headers are installed alongside the main
											      # glib ones.
											      foreach (_component ${GLIB_FIND_COMPONENTS})
												          if (${_component} STREQUAL "gio")
														          find_library(GLIB_GIO_LIBRARIES NAMES gio-2.0 HINTS ${_GLIB_LIBRARY_DIR})
															          set(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GLIB_GIO_LIBRARIES)
																      elseif (${_component} STREQUAL "gobject")
																	              find_library(GLIB_GOBJECT_LIBRARIES NAMES gobject-2.0 HINTS ${_GLIB_LIBRARY_DIR})
																		              set(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GLIB_GOBJECT_LIBRARIES)
																			          elseif (${_component} STREQUAL "gmodule")
																					          find_library(GLIB_GMODULE_LIBRARIES NAMES gmodule-2.0 HINTS ${_GLIB_LIBRARY_DIR})
																						          set(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GLIB_GMODULE_LIBRARIES)
																							      elseif (${_component} STREQUAL "gthread")
																								              find_library(GLIB_GTHREAD_LIBRARIES NAMES gthread-2.0 HINTS ${_GLIB_LIBRARY_DIR})
																									              set(ADDITIONAL_REQUIRED_VARS ${ADDITIONAL_REQUIRED_VARS} GLIB_GTHREAD_LIBRARIES)
																										          elseif (${_component} STREQUAL "gio-unix")
																												          # gio-unix is compiled as part of the gio library, but the include paths
																													          # are separate from the shared glib ones. Since this is currently only used
																														          # by WebKitGTK we don't go to extraordinary measures beyond pkg-config.
																															          pkg_check_modules(GIO_UNIX QUIET gio-unix-2.0)
																																      endif ()
																															      endforeach ()

																															      include(FindPackageHandleStandardArgs)
																															      FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLIB REQUIRED_VARS GLIB_INCLUDE_DIRS GLIB_LIBRARIES ${ADDITIONAL_REQUIRED_VARS}
																																                                             VERSION_VAR   GLIB_VERSION)

																																				     mark_as_advanced(
																																					         GLIBCONFIG_INCLUDE_DIR
																																						     GLIB_GIO_LIBRARIES
																																						         GLIB_GIO_UNIX_LIBRARIES
																																							     GLIB_GMODULE_LIBRARIES
																																							         GLIB_GOBJECT_LIBRARIES
																																								     GLIB_GTHREAD_LIBRARIES
																																								         GLIB_INCLUDE_DIR
																																									     GLIB_INCLUDE_DIRS
																																									         GLIB_LIBRARIES
																																										 )
## toggle this to use vtune itt notify instrumentation
SET(USE_VTUNE "1") 
SET(USE_INIT_ELEMENTS "0")

# -- apply to all configurations --- #
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -g -Wall -Wextra -Werror -pthread -O2")
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
SET(CMAKE_C_COMPILER "gcc")

include_directories(
  ./
  ${GLIB_INCLUDE_DIRS}
)
LINK_DIRECTORIES(${GLIB_INCLUDE_DIRS})
set(COMMON_LIB
    dl
    )
  
if (USE_VTUNE)
    include_directories(
            /data/intel/vtune_profiler/include   # for vtune instrumentation
    )
    
    ADD_LIBRARY(ittnotify STATIC IMPORTED)
    SET_TARGET_PROPERTIES(ittnotify PROPERTIES IMPORTED_LOCATION /data/intel/vtune_profiler/lib64/libittnotify.a)
    SET (COMMON_LIB ittnotify ${COMMON_LIB})
    
    add_definitions(-DUSE_VTUNE)    
endif()

if (USE_INIT_ELEMENTS)
  add_definitions(-DUSE_INIT_ELEMENTS)
endif()

add_definitions(-DDEBUG)

file(GLOB HEADERS
"./*.h"
)

file(GLOB COMMON_SOURCES
  ${HEADERS}
  SortedList.c 
  measure.c 
  common.c)

# ------------------------------------- #
# all executables  
# ------------------------------------- #

# biglock
add_executable(list
  main.c    
  ${COMMON_SOURCES}
)

TARGET_LINK_LIBRARIES(list
        ${COMMON_LIB}
        )      

