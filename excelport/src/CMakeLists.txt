# set build type before Project
if(NOT DEFINED CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
	message(STATUS "Default build type set to ${CMAKE_BUILD_TYPE}")
else()
	message(STATUS "Build type is ${CMAKE_BUILD_TYPE}")
endif()

if(COMMAND cmake_policy)
	if(${CMAKE_VERSION} VERSION_GREATER "3.0.0")
		cmake_policy(SET CMP0048 NEW)
	endif()
endif(COMMAND cmake_policy)
set(CMAKE_VERBOSE_MAKEFILE OFF)

project(excelport_srv)
cmake_minimum_required(VERSION 3.12.0)

if( ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	message(FATAL_ERROR "Windows build not supported")
endif()

find_package(Qt4 REQUIRED)
find_package(FSQt REQUIRED)

fs_use_cxx11()

######### END #########
######### script to include subprojects #########
macro(INCLUDE_SUBDIR SUBDIR CMAKENAME)
	    set(current_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR})
	    set(current_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${SUBDIR})
	    include(${SUBDIR}/${CMAKENAME}.cmake)
endmacro(INCLUDE_SUBDIR SUBDIR)

######### set referent srv version #########
if (NOT excelport_srv_VERSION_MAJOR)
	set(excelport_srv_VERSION_MAJOR 0)
endif (NOT excelport_srv_VERSION_MAJOR)

if (NOT excelport_srv_VERSION_MINOR)
	set(excelport_srv_VERSION_MINOR 1)
endif (NOT excelport_srv_VERSION_MINOR)

if (NOT excelport_srv_VERSION_PATCH)
	set(excelport_srv_VERSION_PATCH 0)
endif (NOT excelport_srv_VERSION_PATCH)

if (NOT excelport_srv_VERSION_BUILD)
	set(excelport_srv_VERSION_BUILD 0)
endif (NOT excelport_srv_VERSION_BUILD)

if(NOT excelport_srv_SVN_REVISION)
		set(excelport_srv_SVN_REVISION 0)
endif(NOT excelport_srv_SVN_REVISION)
######### END #########

set(excelport_srv_SOURCES
	main.cpp

	excelport_srv_ctl.cpp
	excel2xml_handler.cpp

	config/excelport_srv_config.cpp

	core/people.cpp
	core/group.cpp

	xlnt/xlntparser.cpp
)

set(excelport_srv_HEADERS

	excelport_srv_ctl.h
	excel2xml_handler.h

	config/excelport_srv_config.h
)

set (NOT_QT_SHARED_HEADERS ${NOT_QT_SHARED_HEADERS}
	core/i_excel_parser.h
	)
#INCLUDE_SUBDIR(qtsingleapplication qtsingleapplication)
#INCLUDE_SUBDIR(rest rest)
#INCLUDE_SUBDIR(qhttpserver qhttpserver)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(excelport_srv_EXTRALIBS)

######### include subprojects sources to main source list #########
set(excelport_srv_SOURCES
		${excelport_srv_SOURCES}
		${SHARED_SOURCES})

set(excelport_srv_HEADERS
		${excelport_srv_HEADERS}
		${SHARED_HEADERS})

set(excelport_srv_LINKS
		${excelport_srv_LINKS}
		${SHARED_LINKS})
######### END #########

######### include libfsqt  ################
include_directories(${FSQt_INCLUDES})
set(SHARED_EXTRALIBS ${SHARED_EXTRALIBS} ${FSQt_LIBRARIES})
add_definitions(${FSQt_DEFINITIONS})

######### include xint  ################
include_directories("/usr/local/include/xlnt")
link_directories("/usr/local/lib/")
set(SHARED_EXTRALIBS ${SHARED_EXTRALIBS} "-lxlnt")

if(${CMAKE_BUILD_TYPE} MATCHES "Debug" AND NOT VERBOSE_COMPILER)
	set(VERBOSE_COMPILER "-Wall -Wextra -g -O2")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${VERBOSE_COMPILER}")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${VERBOSE_COMPILER}")
endif()

# generate qrc_*.cpp binaries
#QT4_ADD_RESOURCES(excelport_srv_RESOURCES ${excelport_srv_RESOURCES})

######### src #########
set(excelport_srv_SOURCES ${excelport_srv_SOURCES} ${SHARED_SOURCES})
set(excelport_srv_HEADERS ${excelport_srv_HEADERS} ${SHARED_HEADERS})
set(excelport_srv_EXTRALIBS ${excelport_srv_EXTRALIBS} ${SHARED_EXTRALIBS})

######### QT #########
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTSQL TRUE)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTXMLPATTERNS TRUE)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})
set(excelport_srv_EXTRALIBS ${excelport_srv_EXTRALIBS} ${QT_LIBRARIES})
#########

######### generate qt files #########
# generate moc_*.cpp files
QT4_WRAP_CPP(excelport_srv_HEADERS_MOC ${excelport_srv_HEADERS})
######### END #########

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake_config.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/cmake_config.h)

# copy settings files
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/excelport_srv.xml
   ${CMAKE_CURRENT_BINARY_DIR}/excelport_srv.xml COPYONLY)

######### END #########

message(STATUS "excelport_srv_EXTRALIBS = ${excelport_srv_EXTRALIBS}")

add_executable(excelport_srv ${excelport_srv_SOURCES} ${excelport_srv_HEADERS_MOC}
						${excelport_srv_RESOURCES} ${NOT_QT_SHARED_HEADERS})
target_link_libraries(excelport_srv ${excelport_srv_LINKS} ${excelport_srv_EXTRALIBS}
	${SHARED_EXTRALIBS})

